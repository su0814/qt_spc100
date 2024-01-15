#include "project_management.h"
#include "QAESEncryption/qaesencryption.h"
#include "logic_view.h"
#include "lua.h"
#include "mainwindow.h"
#include "md5.h"
#include "param.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QShortcut>
#include <QTemporaryFile>
project_management::project_management(QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )parent;
    // ui->menu->setIcon(QIcon(":/new/photo/photo/logo1.png"));
    ui->menulogo->setIcon(QIcon(":/new/photo/photo/logo1.png"));
    connect(ui->action_new_project, &QAction::triggered, this, project_new_slot);
    connect(ui->action_save_project, &QAction::triggered, this, project_save_slot);
    connect(ui->action_import_project, &QAction::triggered, this, project_import_slot);
    connect(ui->actiona_transmit_todevice, &QAction::triggered, this, project_transmit_to_device_slot);
    connect(ui->action_read_from_device, &QAction::triggered, this, project_readback_from_device_slot);
    ui->action_save_project->setEnabled(false);
    connect(ui->checkBox_advanced_program, &QCheckBox::stateChanged, this, project_advanced_program_slot);
    ui->tabWidget_logic->removeTab(3);
    /* 添加快捷键 */
    ui->action_new_project->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    ui->action_save_project->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    ui->action_import_project->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));

    ui->menu_name->setDisabled(true);
    connect(ui->pushButton_creat_usercode, &QPushButton::clicked, this, lua_debug_creat_slot);
}

QByteArray project_management::project_lua_code_creat()
{
    QString lua_code;
    QString exit_ss_code;
    /* creat lua code */
    lua_code.clear();
    /* 获取sf block list */
    QList<logic_block*>   sf_list;
    QList<logic_block*>   exit_list;
    QList<QGraphicsItem*> allBlocks = mainwindow->logic_view_class->my_scene->items();
    foreach (QGraphicsItem* item, allBlocks) {
        if (item->type() == QGraphicsItem::UserType + BLOCK_TYPE_LOGIC) {
            logic_block* logic = dynamic_cast<logic_block*>(item);
            if (logic->block_attribute.block_info.tool_type == TOOL_TYPE_LOGIC_SF) {
                sf_list.append(logic);
            } else if (logic->block_attribute.block_info.tool_type == TOOL_TYPE_LOGIC_EXIT) {
                exit_list.append(logic);
            }
        }
    }

    QString code_title = "--" + ui->lineEdit_projectname->text() + "-" + ui->lineEdit_author_name->text() + "\r\n";
    if (code_title.isEmpty()) {
        code_title = "--User code\r\n";
    }
    lua_code.append(code_title);
    for (uint8_t i = 0; i < sf_type_str.count(); i++) {
        lua_code.append(sf_type_str[i] + " = " + QString::number(i) + "\r\n");
    }

    lua_code.append("\r\nnot_relevant = 0");
    lua_code.append("\r\nrelevant = 1");
    lua_code.append("\r\n\r\nset_lua_version(\"" + ui->lineEdit_projectname->text() + "\")");

    /* 函数生成 */
    for (uint8_t i = 0; i < sf_list.count(); i++) {
        lua_code.append("\r\n\r\nfunction " + sf_list[i]->block_attribute.other_name + "_func() return "
                        + sf_list[i]->block_attribute.logic_string + " end");
    }

    if (exit_list.count() > 0 && exit_list[0]->block_attribute.logic_string.size() > 0) {
        lua_code.append("\r\n\r\nfunction exit_func() return " + exit_list[0]->block_attribute.logic_string + " end");
        exit_ss_code = "\r\n\t\t exit_ss(exit_func()," + QString::number(exit_list[0]->exit_delay_time) + ")";
    } else {
        exit_ss_code = "\r\n\t\t exit_ss(true,0)";
    }

    /* 通用函数生成 */
    /* delay ms */
    lua_code.append("\r\nfunction lua_delay_ms(delay_time)\r\n  local start_time = sys_tick()\r\n  while "
                    "true do\r\n    if ((start_time + delay_time) < sys_tick()) or (get_module_state() == 3) "
                    " then\r\n break\r\n end\r\n coroutine.yield()\r\n end\r\nend ");
    //    lua_code.append("\r\n\r\nfunction lua_delay_ms(delay_time)\r\n  local start_time = sys_tick()\r\n  while "
    //                    "true do\r\n    if ((start_time + delay_time) < sys_tick()) "
    //                    " then\r\n \t\tbreak\r\n \tend\r\n coroutine.yield()\r\n end\r\nend ");

    /* 线程生成 */
    QStringList coroutine_name;
    for (uint8_t i = 0; i < ui->listWidget_coroutine->count(); i++) {
        QListWidgetItem* item1 = ui->listWidget_coroutine->item(i);
        if (item1->checkState() == Qt::Unchecked) {
            continue;
        }
        coroutine_name.append(item1->text());
        lua_code.append("\r\n\r\n" + item1->text() + " = coroutine.create(function()" + "\r\n\t while true do");
        lua_code.append("\r\n" + mainwindow->coroutine_lua_class->coroutine_code[i]);
        lua_code.append("\r\n\t coroutine.yield()\r\n\t end\r\nend)");
    }

    /* main */
    lua_code.append("\r\n\r\nfunction main()");
    /* set ss */
    QStringList ss_relevan;
    ss_relevan << ", not_relevant"
               << ", relevant";
    for (uint8_t i = 0; i < mainwindow->condition_view_class->ss_info_list.count(); i++) {
        uint8_t code           = mainwindow->condition_view_class->ss_info_list[i].ss_code;
        uint8_t relevant_value = mainwindow->condition_view_class->ss_info_list[i].relevant_state;
        lua_code.append("\r\n\t set_ss(0x" + QString::number(code, 16) + ss_relevan[((relevant_value >> 0)) & 0x01]
                        + ss_relevan[((relevant_value >> 1)) & 0x01] + ss_relevan[((relevant_value >> 2)) & 0x01]
                        + ss_relevan[((relevant_value >> 3)) & 0x01] + ss_relevan[((relevant_value >> 4)) & 0x01]
                        + ss_relevan[((relevant_value >> 5)) & 0x01] + ")");
    }
    lua_code.append("\r\n\t while true do");

    /* set sf */
    for (uint8_t i = 0; i < sf_list.count(); i++) {
        lua_code.append(
            "\r\n\t\t sf(\"" + sf_list[i]->sf_param.name + "\", 0x" + QString::number(sf_list[i]->sf_param.sf_code, 16)
            + ", " + sf_list[i]->block_attribute.other_name + "_func(), " + sf_type_str[sf_list[i]->sf_param.sf_type]
            + ", 0x" + QString::number(sf_list[i]->sf_param.ss_code, 16) + ", "
            + QString::number(sf_list[i]->sf_param.delay_time) + ", "
            + QString::number(sf_list[i]->sf_param.option_time) + ")");
    }
    lua_code.append(exit_ss_code);
    /* set coroutine */
    for (uint8_t i = 0; i < coroutine_name.count(); i++) {
        lua_code.append("\r\n\t\t local success, errorMessage = coroutine.resume(" + coroutine_name[i] + ")"
                        + "\r\n\t\t if not success then" + "\r\n\t\t\t sf(\"" + coroutine_name[i] + " error \""
                        + ", 0xff" + ", true, " + sf_type_str[0] + ", 0xff" + ", 0" + ", nil" + ")" + "\r\n\t\t end");
    }
    lua_code.append("\r\n\t end\r\nend");
    lua_code.append("\r\nmain()");
    ui->plainTextEdit_usercode->setPlainText(lua_code);
    return lua_code.toUtf8();
}

QByteArray project_management::project_file_creat()
{

    QJsonObject rootObject;
    rootObject[project_name]         = ui->lineEdit_projectname->text();
    rootObject[project_company_name] = ui->lineEdit_company_name->text();
    rootObject[project_author_ver]   = ui->lineEdit_author_name->text();
    rootObject[project_version]      = ui->lineEdit_project_version->text();
    if (ui->checkBox_advanced_program->isChecked()) {
        rootObject[project_advanced_program] = 1;
    } else {
        rootObject[project_advanced_program] = 0;
    }
    rootObject[project_object_device]         = mainwindow->condition_view_class->condition_view_project_info();
    rootObject[project_object_logic_programe] = mainwindow->logic_view_class->logic_view_project_info();
    rootObject[project_object_coroutine]      = mainwindow->coroutine_lua_class->coroutine_lua_project_info();
    rootObject[project_safety_param]          = mainwindow->param_class->param_project_info();
    QJsonDocument  jsonDoc(rootObject);
    QByteArray     jsonsource = jsonDoc.toJson();
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
    QByteArray     encrydata = encryption.encode(jsonsource, PROJECT_ENCRY_AES_KEY, PROJECT_ENCRY_AES_IV);
    for (int i = 0; i < encrydata.size(); i++) {
        char byte = encrydata[i];
        byte ^= PROJECT_ENCRY_XOR_KEY;
        encrydata[i] = byte;
    }
    return encrydata;
}

void project_management::project_management_reset()
{
    ui->lineEdit_projectname->setEnabled(true);
    ui->lineEdit_company_name->setEnabled(true);
    ui->lineEdit_author_name->setEnabled(true);
    ui->lineEdit_project_version->setEnabled(true);
    ui->lineEdit_project_path->clear();
    ui->lineEdit_project_version->clear();
    ui->lineEdit_projectname->clear();
    ui->lineEdit_author_name->clear();
    ui->lineEdit_company_name->clear();
    mainwindow->condition_view_class->condition_view_reset();
    mainwindow->logic_view_class->logic_view_reset();
    mainwindow->logic_tools_class->logic_tools_reset();
    mainwindow->coroutine_lua_class->coroutine_lua_reset();
    project_management_info.is_new   = false;
    project_management_info.is_valid = false;
    project_management_info.filename.clear();
    project_management_info.filepath.clear();
    ui->checkBox_advanced_program->setChecked(false);
    mainwindow->param_class->param_ui_clear();
}

/* user slots */
void project_management::project_new_slot()
{
    if (project_management_info.is_new && !project_management_info.is_valid) {
        if (mainwindow->my_message_box("新工程保存", "当前有新工程未保存，是否保存?", true) == QMessageBox::Ok) {
            project_save_slot();
        }
    }
    /* reset */
    project_management_reset();
    ui->tabWidget_logic->setEnabled(true);
    project_management_info.is_new = true;
    ui->tabWidget->setCurrentIndex(4);
    ui->tabWidget_logic->setCurrentIndex(0);
    ui->action_save_project->setEnabled(true);
    project_management_info.is_valid = true;
    if (mainwindow->serial_is_connect) {
        ui->actiona_transmit_todevice->setEnabled(true);
    }
}

int project_management::project_save_slot()
{
    if (project_management_info.is_valid == false) {
        return -1;
    }
    QString folderPath;
    if (ui->lineEdit_projectname->text().isEmpty()) {
        mainwindow->my_message_box("保存失败", "项目信息填写不完整，请完善项目信息", false);
        return -2;
    }
    if (project_management_info.is_new) {
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::Directory);   // 设置对话框模式为选择文件夹
        dialog.setOption(QFileDialog::ShowDirsOnly);  // 只显示文件夹
        folderPath = dialog.getExistingDirectory(this, tr("选择保存路径"), QDir::homePath());
        if (folderPath.isEmpty()) {
            mainwindow->my_message_box("保存失败", "保存路径为空", false);
            return -3;
        }
        ui->lineEdit_project_path->setText(folderPath);
    }

    QByteArray project_file = project_file_creat();
    QString    projectname  = ui->lineEdit_projectname->text();
    QString    path         = ui->lineEdit_project_path->text();
    QFile      outputFile(path + "/" + projectname + "~.spc100");
    if (outputFile.open(QIODevice::WriteOnly)) {
        outputFile.write(project_file);
        outputFile.close();
        QFile oldFile(path + "/" + projectname + ".spc100");
        oldFile.remove();
        outputFile.rename(path + "/" + projectname + ".spc100");
        project_management_info.is_new   = false;
        project_management_info.is_valid = true;
        project_management_info.filepath = path;
        project_management_info.filename = projectname;
    } else {
        mainwindow->my_message_box("项目保存失败", "项目文件生成或打开失败，请检查文件权限或文件是否被占用", false);
    }
    return 0;
}

void project_management::project_file_prase(QByteArray file)
{
    QByteArray project_file = file;
    for (int i = 0; i < project_file.size(); i++) {
        char byte = project_file[i];
        byte ^= PROJECT_ENCRY_XOR_KEY;
        project_file[i] = byte;
    }
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
    QByteArray     deencrydata = encryption.decode(project_file, PROJECT_ENCRY_AES_KEY, PROJECT_ENCRY_AES_IV);
    deencrydata                = encryption.removePadding(deencrydata);
    QJsonDocument jsonDoc      = QJsonDocument::fromJson(deencrydata);
    // 从QJsonDocument中获取QJsonObject
    QJsonObject jsonObject = jsonDoc.object();
    mainwindow->condition_view_class->condition_view_project_parse(jsonObject[project_object_device].toObject());
    mainwindow->logic_view_class->logic_view_project_parse(jsonObject[project_object_logic_programe].toObject());
    mainwindow->coroutine_lua_class->coroutine_lua_project_parse(jsonObject[project_object_coroutine].toObject());
    mainwindow->param_class->param_project_parse(jsonObject[project_safety_param].toObject());

    ui->lineEdit_projectname->setText(jsonObject[project_name].toString());
    ui->lineEdit_company_name->setText(jsonObject[project_company_name].toString());
    ui->lineEdit_author_name->setText(jsonObject[project_author_ver].toString());
    ui->lineEdit_project_version->setText(jsonObject[project_version].toString());
    if (jsonObject[project_advanced_program].toInt() == 0) {
        ui->checkBox_advanced_program->setChecked(false);
    } else {
        ui->checkBox_advanced_program->setChecked(true);
    }
}

void project_management::project_import_slot()
{
    if (project_management_info.is_valid) {
        if (mainwindow->my_message_box("工程保存", "是否保存当前工程？", true) == QMessageBox::Ok) {
            project_save_slot();
        }
    }
    if (project_management_info.is_new && !project_management_info.is_valid) {
        if (mainwindow->my_message_box("新工程保存", "当前有新工程未保存，是否保存?", true) == QMessageBox::Ok) {
            project_save_slot();
        }
    }

    QString curPath  = QDir::currentPath();     //获取系统当前目录
    QString dlgTitle = "打开一个*.spc100文件";  //对话框标题
    QString filter   = "工程文件(*.spc100)";    //文件过滤器
    QString filename = QFileDialog::getOpenFileName(this, dlgTitle, curPath, filter);
    if (filename == "") {
        return;
    }
    int ret = 0;
    while (1) {
        int a = filename.indexOf("/", ret + 1);
        if (a >= 0)
            ret = a;
        else
            break;
    }

    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly)) {
        project_management_reset();
        QByteArray jsonData = inputFile.readAll();
        inputFile.close();
        project_management_info.is_new   = false;
        project_management_info.is_valid = true;
        project_management_info.filename = filename.mid(ret + 1);
        project_management_info.filepath = filename;
        project_management_info.filepath.remove(ret, project_management_info.filename.length() + 1);
        project_file_prase(jsonData);
        ui->tabWidget->setCurrentIndex(4);
        ui->tabWidget_logic->setCurrentIndex(0);
        ui->tabWidget_logic->setEnabled(true);
        ui->action_save_project->setEnabled(true);
        ui->lineEdit_projectname->setEnabled(false);
        ui->lineEdit_project_path->setText(project_management_info.filepath);
        if (mainwindow->serial_is_connect) {
            ui->actiona_transmit_todevice->setEnabled(true);
        }
    }
}

bool project_management::projec_info_creat()
{
    QByteArray file = project_file_creat();
    QByteArray code = project_lua_code_creat();
    if (code.size() > 0x4000 || file.size() > 0x1B800) {
        return false;
    }
    total_file_data            = file + code;
    project_info.project_size  = file.size();
    project_info.usercode_size = code.size();
    project_info.param_size    = sizeof(mainwindow->param_class->module_param);
    total_file_data.append(( char* )(&mainwindow->param_class->module_param),
                           sizeof(mainwindow->param_class->module_param));
    mbedtls_md5(( unsigned char* )total_file_data.data(), total_file_data.size(), project_info.md5);
    return true;
}

void project_management::project_transmit_to_device_slot()
{
    if (ui->serial_switch_pushButton->text() == "打开串口") {
        mainwindow->my_message_box("设备未连接", "请检查连接线束并查看端口是否打开", false);
        return;
    }
    if (mainwindow->user_permissions != USER_AUTHORIZED) {
        mainwindow->my_message_box("操作失败", "普通用户无权限,请授权后重试", false);
        return;
    }
    if (mainwindow->logic_view_class->blocks_error_detect()) {
        mainwindow->my_message_box("传输失败", "逻辑编程有错误，请检查", false);
        return;
    }
    if (projec_info_creat() == false) {
        mainwindow->my_message_box("传输失败", "工程过大，请删减", false);
        return;
    }
    ui->tabWidget->setCurrentIndex(2);
    mainwindow->lua_class->lua_download_from_project(&total_file_data, project_info);
}

void project_management::project_readback_from_device_slot()
{
    if (ui->serial_switch_pushButton->text() == "打开串口") {
        mainwindow->my_message_box("设备未连接", "请检查连接线束并查看端口是否打开", false);
        return;
    }
    if (mainwindow->user_permissions != USER_AUTHORIZED) {
        mainwindow->my_message_box("操作失败", "普通用户无权限,请授权后重试", false);
        return;
    }
    if (project_management_info.is_valid) {
        if (mainwindow->my_message_box("警告", "从设备读取工程会覆盖当前工程，是否继续读取？", true)
            != QMessageBox::Ok) {
            return;
        }
    }
    ui->tabWidget->setCurrentIndex(2);
    projec_info_creat();
    if (mainwindow->lua_class->readback_project_file(project_info)) {
        project_management_reset();
        QByteArray project_file = mainwindow->lua_class->readback_info.project_file.mid(
            0, mainwindow->lua_class->read_project_info.project_size);
        project_file_prase(project_file);
        project_management_info.is_new   = true;
        project_management_info.is_valid = true;
        ui->tabWidget->setCurrentIndex(4);
        ui->tabWidget_logic->setCurrentIndex(0);
        ui->tabWidget_logic->setEnabled(true);
        ui->action_save_project->setEnabled(true);
        ui->lineEdit_projectname->setEnabled(true);
        QByteArray usercode = mainwindow->lua_class->readback_info.project_file.mid(
            mainwindow->lua_class->read_project_info.project_size,
            mainwindow->lua_class->read_project_info.usercode_size);
        ui->plainTextEdit_usercode->setPlainText(QString::fromUtf8(usercode.data()));
        //        qDebug() << mainwindow->lua_class->readback_info.project_file.size()
        //                 << QString::number(mainwindow->lua_class->read_project_info.check_sum, 16);
        if (mainwindow->serial_is_connect) {
            ui->actiona_transmit_todevice->setEnabled(true);
        }
    }
}

void project_management::project_advanced_program_slot(int state)
{
    if (state == Qt::Checked) {
        // 复选框被选中
        ui->tabWidget_logic->insertTab(3, ui->tab_coroutine_lua, "高级编程");
    } else {
        // 复选框被取消选中
        ui->tabWidget_logic->removeTab(3);
    }
}

void project_management::lua_debug_creat_slot()
{
    if (mainwindow->logic_view_class->blocks_error_detect()) {
        mainwindow->my_message_box("生成失败", "逻辑编程有错误，请检查", false);
        return;
    }
    project_lua_code_creat();
}
