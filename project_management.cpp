#include "project_management.h"
#include "QAESEncryption/qaesencryption.h"
#include "logic_view.h"
#include "lua.h"
#include "mainwindow.h"
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
    ui->action_save_project->setEnabled(false);
    connect(ui->checkBox_advanced_program, &QCheckBox::stateChanged, this, project_advanced_program_slot);
    ui->tabWidget_logic->removeTab(2);
    /* 添加快捷键 */
    ui->action_new_project->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    ui->action_save_project->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    ui->action_import_project->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));

    /* 屏蔽原lua下载 */
    ui->lua_filename_lineEdit->setVisible(false);
    ui->lua_select_file_pushButton->setVisible(false);
    ui->lua_download_pushButton->setVisible(false);

    QRegExp           regExp("[A-Za-z][A-Za-z0-9_]*");
    QRegExpValidator* validator = new QRegExpValidator(regExp);
    ui->lineEdit_projectname->setValidator(validator);
    ui->lineEdit_objectname->setValidator(validator);
    QRegExp           regExp1("[vV0-9][vV0-9.]*");
    QRegExpValidator* validator1 = new QRegExpValidator(regExp1);
    ui->lineEdit_objectverson->setValidator(validator1);
}

QByteArray project_management::project_lua_code_creat()
{
    QString lua_code;
    /* creat lua code */

    mainwindow->logic_view_class->update_timer->stop();  //停止刷新数据
    lua_code.clear();
    /* 获取sf block list */
    QList<logic_block*>   sf_list;
    QList<QGraphicsItem*> allBlocks = mainwindow->logic_view_class->my_scene->items();
    foreach (QGraphicsItem* item, allBlocks) {
        if (item->type() == QGraphicsItem::UserType + BLOCK_TYPE_LOGIC) {
            logic_block* logic = dynamic_cast<logic_block*>(item);
            if (logic->block_attribute.block_info.tool_type == TOOL_TYPE_LOGIC_SF) {
                sf_list.append(logic);
            }
        }
    }
    for (uint8_t i = 0; i < sf_type_str.count(); i++) {
        lua_code.append(sf_type_str[i] + " = " + QString::number(i) + "\r\n");
    }
    lua_code.append("\r\nnot_relevant = 0");
    lua_code.append("\r\nrelevant = 1");
    lua_code.append("\r\nset_lua_version(\"" + ui->lineEdit_objectname->text() + "-" + ui->lineEdit_objectverson->text()
                    + "\")");

    /* 函数生成 */
    for (uint8_t i = 0; i < sf_list.count(); i++) {
        lua_code.append("\r\nfunction " + sf_list[i]->sf_param.name + "_func() return "
                        + sf_list[i]->block_attribute.logic_string + " end");
    }
    /* 通用函数生成 */
    /* delay ms */
    //    lua_code.append("\r\nfunction lua_delay_ms(delay_time)\r\n  local start_time = sys_tick()\r\n  while "
    //                    "true do\r\n    if ((start_time + delay_time) < sys_tick()) or (get_module_state() == 3) "
    //                    " then\r\n break\r\n end\r\n coroutine.yield()\r\n end\r\nend ");
    lua_code.append("\r\nfunction lua_delay_ms(delay_time)\r\n  local start_time = sys_tick()\r\n  while "
                    "true do\r\n    if ((start_time + delay_time) < sys_tick()) "
                    " then\r\n break\r\n end\r\n coroutine.yield()\r\n end\r\nend ");

    /* 线程生成 */
    QStringList coroutine_name;
    for (uint8_t i = 0; i < ui->listWidget_coroutine->count(); i++) {
        QListWidgetItem* item1 = ui->listWidget_coroutine->item(i);
        if (item1->checkState() == Qt::Unchecked) {
            continue;
        }
        coroutine_name.append(item1->text());
        lua_code.append("\r\n" + item1->text() + " = coroutine.create(function()" + "\r\n\t while true do");
        lua_code.append("\r\n" + mainwindow->coroutine_lua_class->coroutine_code[i]);
        lua_code.append("\t\t coroutine.yield()\r\n\t end\r\nend)");
    }

    /* main */
    lua_code.append("\r\nfunction main()");
    /* set ss */
    QStringList ss_relevan;
    ss_relevan << ", not_relevant"
               << ", relevant";
    for (uint8_t i = 0; i < mainwindow->condition_view_class->ss_info_list.count(); i++) {
        uint8_t code           = mainwindow->condition_view_class->ss_info_list[i].ss_code;
        uint8_t relevant_value = mainwindow->condition_view_class->ss_info_list[i].relevant_state;
        lua_code.append("\t set_ss(0x" + QString::number(code, 16) + ss_relevan[((relevant_value >> 0)) & 0x01]
                        + ss_relevan[((relevant_value >> 1)) & 0x01] + ss_relevan[((relevant_value >> 2)) & 0x01]
                        + ss_relevan[((relevant_value >> 3)) & 0x01] + ss_relevan[((relevant_value >> 4)) & 0x01]
                        + ss_relevan[((relevant_value >> 5)) & 0x01] + ")");
    }
    lua_code.append("\t while true do");

    /* set sf */
    for (uint8_t i = 0; i < sf_list.count(); i++) {
        lua_code.append("\r\n\t\t sf(\"" + sf_list[i]->sf_param.name + "\", 0x"
                        + QString::number(sf_list[i]->sf_param.sf_code, 16) + ", " + sf_list[i]->sf_param.name
                        + "_func(), " + sf_type_str[sf_list[i]->sf_param.sf_type] + ", 0x"
                        + QString::number(sf_list[i]->sf_param.ss_code, 16) + ", "
                        + QString::number(sf_list[i]->sf_param.delay_time) + ", "
                        + QString::number(sf_list[i]->sf_param.option_time) + ")");
    }
    lua_code.append("\t\t exit_ss(true,0)");
    /* set coroutine */
    for (uint8_t i = 0; i < coroutine_name.count(); i++) {
        lua_code.append("\t\t local success, errorMessage = coroutine.resume(" + coroutine_name[i] + ")"
                        + "\r\n\t\t if not success then" + "\r\n\t\t\t sf(\"" + coroutine_name[i] + " error \""
                        + ", 0xff" + ", true, " + sf_type_str[0] + ", 0xff" + ", 0" + ", nil" + ")" + "\r\n\t\t end");
    }
    lua_code.append("\t end\r\nend");
    lua_code.append("\r\nmain()");
    // ui->lua_log_textBrowser->append(lua_code);
    mainwindow->logic_view_class->update_timer->start(LOGIC_VIEW_DATA_REFRESH_TIME);
    return lua_code.toUtf8();
}

void project_management::project_management_reset()
{
    ui->lineEdit_projectname->setEnabled(true);
    ui->lineEdit_objectname->setEnabled(true);
    ui->lineEdit_objectverson->setEnabled(true);
    ui->lineEdit_project_path->clear();
    ui->lineEdit_project_reftime->clear();
    ui->lineEdit_projectname->clear();
    ui->lineEdit_objectname->clear();
    ui->lineEdit_objectverson->clear();
    mainwindow->condition_view_class->condition_view_reset();
    mainwindow->logic_view_class->logic_view_reset();
    mainwindow->logic_tools_class->logic_tools_reset();
    mainwindow->coroutine_lua_class->coroutine_lua_reset();
    project_management_info.is_new   = false;
    project_management_info.is_valid = false;
    project_management_info.filename.clear();
    project_management_info.filepath.clear();
    ui->checkBox_advanced_program->setChecked(false);
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
    ui->tabWidget->setCurrentIndex(5);
    ui->tabWidget_logic->setCurrentIndex(0);
    ui->action_save_project->setEnabled(true);
    mainwindow->logic_view_class->update_timer->start(LOGIC_VIEW_DATA_REFRESH_TIME);
}

int project_management::project_save_slot()
{
    if (project_management_info.is_valid = false) {
        return -1;
    }
    QString folderPath;
    if (ui->lineEdit_projectname->text().isEmpty() || ui->lineEdit_objectname->text().isEmpty()
        || ui->lineEdit_objectverson->text().isEmpty()) {
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

    // 获取当前日期和时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    // 格式化日期和时间
    QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    ui->lineEdit_project_reftime->setText(formattedDateTime);
    QJsonObject rootObject;
    rootObject[project_name]     = ui->lineEdit_projectname->text();
    rootObject[project_lua_name] = ui->lineEdit_objectname->text();
    rootObject[project_lua_ver]  = ui->lineEdit_objectverson->text();
    rootObject[project_ref_time] = ui->lineEdit_project_reftime->text();
    rootObject[project_path]     = ui->lineEdit_project_path->text();
    if (ui->checkBox_advanced_program->isChecked()) {
        rootObject[project_advanced_program] = 1;
    } else {
        rootObject[project_advanced_program] = 0;
    }
    rootObject[project_object_device]         = mainwindow->condition_view_class->condition_view_project_info();
    rootObject[project_object_logic_programe] = mainwindow->logic_view_class->logic_view_project_info();
    rootObject[project_object_coroutine]      = mainwindow->coroutine_lua_class->coroutine_lua_project_info();
    QJsonDocument jsonDoc(rootObject);
    QByteArray    jsonString  = jsonDoc.toJson();
    QString       projectname = ui->lineEdit_projectname->text();
    QString       path        = ui->lineEdit_project_path->text();
    QFile         outputFile(path + "/" + projectname + "~.spc100");
    if (outputFile.open(QIODevice::WriteOnly)) {
        QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray     encrydata = encryption.encode(jsonString, PROJECT_ENCRY_AES_KEY, PROJECT_ENCRY_AES_IV);
        for (int i = 0; i < encrydata.size(); i++) {
            char byte = encrydata[i];
            byte ^= PROJECT_ENCRY_XOR_KEY;
            encrydata[i] = byte;
        }
        outputFile.write(encrydata);
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

void project_management::project_import_slot()
{
    if (project_management_info.is_new && !project_management_info.is_valid) {
        if (mainwindow->my_message_box("新工程保存", "当前有新工程未保存，是否保存?", true) == QMessageBox::Ok) {
            project_save_slot();
        }
    }
    if (project_management_info.is_valid) {
        project_save_slot();
    }
    QString curPath  = QDir::currentPath();     //获取系统当前目录
    QString dlgTitle = "打开一个*.spc100文件";  //对话框标题
    QString filter   = "lua文件(*.spc100)";     //文件过滤器
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
        for (int i = 0; i < jsonData.size(); i++) {
            char byte = jsonData[i];
            byte ^= PROJECT_ENCRY_XOR_KEY;
            jsonData[i] = byte;
        }
        QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray     deencrydata = encryption.decode(jsonData, PROJECT_ENCRY_AES_KEY, PROJECT_ENCRY_AES_IV);
        deencrydata                = encryption.removePadding(deencrydata);
        inputFile.close();
        // 将JSON字符串转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(deencrydata);
        // 从QJsonDocument中获取QJsonObject
        QJsonObject jsonObject = jsonDoc.object();
        mainwindow->condition_view_class->condition_view_project_parse(jsonObject[project_object_device].toObject());
        mainwindow->logic_view_class->logic_view_project_parse(jsonObject[project_object_logic_programe].toObject());
        mainwindow->coroutine_lua_class->coroutine_lua_project_parse(jsonObject[project_object_coroutine].toObject());
        project_management_info.is_new   = false;
        project_management_info.is_valid = true;
        project_management_info.filename = filename.mid(ret + 1);
        project_management_info.filepath = filename;
        project_management_info.filepath.remove(ret, project_management_info.filename.length() + 1);
        ui->lineEdit_project_path->setText(project_management_info.filepath);
        ui->lineEdit_projectname->setText(jsonObject[project_name].toString());
        ui->lineEdit_objectname->setText(jsonObject[project_lua_name].toString());
        ui->lineEdit_objectverson->setText(jsonObject[project_lua_ver].toString());
        ui->lineEdit_project_reftime->setText(jsonObject[project_ref_time].toString());
        if (jsonObject[project_advanced_program].toInt() == 0) {
            ui->checkBox_advanced_program->setChecked(false);
        } else {
            ui->checkBox_advanced_program->setChecked(true);
        }
        ui->tabWidget->setCurrentIndex(5);
        ui->tabWidget_logic->setCurrentIndex(0);
        ui->tabWidget_logic->setEnabled(true);
        ui->action_save_project->setEnabled(true);
        ui->lineEdit_projectname->setEnabled(false);
        ui->lineEdit_objectname->setEnabled(false);
        mainwindow->logic_view_class->update_timer->start(LOGIC_VIEW_DATA_REFRESH_TIME);
    }
}

void project_management::project_transmit_to_device_slot()
{
    if (ui->serial_switch_pushButton->text() == "打开串口") {
        mainwindow->my_message_box("设备未连接", "请检查连接线束并查看端口是否打开", false);
        return;
    }
    if (mainwindow->user_permissions != USER_AUTHORIZED) {
        mainwindow->my_message_box("操作失败", "普通用户无升级权限,请授权后重试", false);
        return;
    }
    if (mainwindow->logic_view_class->blocks_error_detect()) {
        mainwindow->my_message_box("传输失败", "逻辑编程有错误，请检查", false);
        return;
    }

    if (project_save_slot() != 0) {
        return;
    }
    QByteArray file = project_lua_code_creat();
    if (file.size() > 0x4000) {
        mainwindow->my_message_box("脚本文件较大", "逻辑过多或线程过大，请删减", false);
        return;
    }
    ui->tabWidget->setCurrentIndex(3);
    mainwindow->lua_class->lua_download_from_project(&file, project_management_info.filename);
}

void project_management::project_advanced_program_slot(int state)
{
    if (state == Qt::Checked) {
        // 复选框被选中
        ui->tabWidget_logic->insertTab(2, ui->tab_coroutine_lua, "协程编程");
    } else {
        // 复选框被取消选中
        ui->tabWidget_logic->removeTab(2);
    }
}
