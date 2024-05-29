#include "project_management.h"
#include "algorithm/MD5/md5.h"
#include "algorithm/QAESEncryption/qaesencryption.h"
#include "logic_view/logic_view.h"
#include "mainwindow.h"
#include "project_transmit.h"
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
project_management::project_management(QWidget* mparent, QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )mparent;
    connect(ui->action_new_project, &QAction::triggered, this, project_new_slot);
    connect(ui->action_save_project, &QAction::triggered, this, project_save_slot);
    connect(ui->action_import_project, &QAction::triggered, this, project_import_slot);
    connect(ui->actiona_transmit_todevice, &QAction::triggered, this, project_transmit_to_device_slot);
    connect(ui->action_read_from_device, &QAction::triggered, this, project_readback_from_device_slot);
    ui->action_save_project->setEnabled(false);
    /* 添加快捷键 */
    ui->action_new_project->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    ui->action_save_project->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    ui->action_import_project->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    connect(ui->action_usercode, &QAction::triggered, this, lua_debug_creat_slot);

    project_verify_timer.setSingleShot(true);
    connect(&project_verify_timer, &QTimer::timeout, this, project_verify_enter_slot);
}

void project_management::project_verify_send_cmd()
{
    uint8_t  frame[256] = { 0 };
    uint16_t data_len   = sizeof(project_info_t);
    memcpy(frame, ( char* )&project_info, data_len);
    mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_PROJECT, CMD_PROJECT_INFO, SUB_PROJECT_INFO_VERIFY, frame,
                                              data_len);
}

bool project_management::project_verify()
{
    project_verify_ack.ack_info[0].ack_code  = 0;
    project_verify_ack.ack_info[0].responsed = false;
    project_verify_ack.ack_info[1].ack_code  = 0;
    project_verify_ack.ack_info[1].responsed = false;
    project_verify_ack.retry                 = 0;
    project_verify_ack.ack_status            = ACK_STATUS_WAITING;
    project_verify_send_cmd();
    project_verify_timer.start(500);
    while (project_verify_ack.ack_status == ACK_STATUS_WAITING) {
        QApplication::processEvents();
    }
    if (project_verify_ack.ack_status == ACK_STATUS_SUCCESS) {
        return true;
    }
    return false;
}

QByteArray project_management::project_lua_code_creat()
{
    QString lua_code;
    /* creat lua code */
    lua_code.clear();
    int emu_size = mainwindow->logic_view_class->input_block_list.size()
                   + mainwindow->logic_view_class->base_logic_block_list.size()
                   + mainwindow->logic_view_class->apply_logic_block_list.size()
                   + mainwindow->logic_view_class->delay_counter_block_list.size()
                   + mainwindow->logic_view_class->speed_logic_block_list.size()
                   + mainwindow->logic_view_class->output_block_list.size();
    lua_code.append("\r\nset_emu_size(" + QString::number(emu_size) + ")");
    foreach (input_block* block, mainwindow->logic_view_class->input_block_list) {
        lua_code.append("\r\nfunction " + block->get_attribute()->function_name + block->get_attribute()->logic_function
                        + " end");
    }
    foreach (base_logic_block* block, mainwindow->logic_view_class->base_logic_block_list) {
        lua_code.append("\r\nfunction " + block->get_attribute()->function_name + " "
                        + block->get_attribute()->logic_function + " end");
    }
    foreach (apply_logic_block* block, mainwindow->logic_view_class->apply_logic_block_list) {
        lua_code.append("\r\nfunction " + block->get_attribute()->function_name + " "
                        + block->get_attribute()->logic_function + " end");
    }
    foreach (delay_counter_logic_block* block, mainwindow->logic_view_class->delay_counter_block_list) {
        lua_code.append("\r\nfunction " + block->get_attribute()->function_name + " "
                        + block->get_attribute()->logic_function + " end");
    }
    foreach (speed_logic_block* block, mainwindow->logic_view_class->speed_logic_block_list) {
        lua_code.append("\r\nfunction " + block->get_attribute()->function_name + " "
                        + block->get_attribute()->logic_function + " end");
    }
    foreach (output_block* block, mainwindow->logic_view_class->output_block_list) {
        lua_code.append("\r\nfunction " + block->get_attribute()->function_name + block->get_attribute()->logic_function
                        + " end");
    }
    /* main */
    lua_code.append("\r\nfunction main()");
    /* set ss */
    lua_code.append("\r\nwhile true do");
    foreach (output_block* block, mainwindow->logic_view_class->output_block_list) {
        if (block->get_config_block_data()->config_param_data.model_type == MODEL_OUTPUT_RELAY_MOS) {
            lua_code.append("\r\n " + block->get_attribute()->function_name);
        }
    }
    foreach (delay_counter_logic_block* block, mainwindow->logic_view_class->counter_logging_list) {
        lua_code.append("\r\n " + block->get_attribute()->function_name);
    }
    lua_code.append("\r\nend\r\nend");
    lua_code.append("\r\nmain()");
    usercode_dialog.set_usercode(lua_code);
    return lua_code.toUtf8();
}

QByteArray project_management::project_file_creat()
{
    QJsonObject rootObject;
    rootObject[project_name]                  = ui->lineEdit_project_name->text();
    rootObject[project_company_name]          = ui->lineEdit_company_name->text();
    rootObject[project_author_ver]            = ui->lineEdit_author_name->text();
    rootObject[project_object_logic_programe] = mainwindow->logic_view_class->logic_view_project_info();
    rootObject[project_safety_param]          = mainwindow->config_view_class->param_project_info();
    rootObject[project_config_photo] = mainwindow->config_view_class->config_photo_svg->config_photo_project_info();
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

void project_management::update_project_date()
{
    QString date = QString::fromUtf8(project_info.date);
    ui->lineEdit_save_time->setText(date);
}

void project_management::update_project_md5()
{
    QString md5;
    for (int i = 0; i < 16; i++) {
        md5 += QString::number(project_info.md5[i], 16);
    }
    ui->lineEdit_current_project_checknum->setText(md5);
}

bool project_management::projec_info_creat()
{
    QByteArray file = project_file_creat();
    QByteArray code = project_lua_code_creat();
    if (code.size() > 0x4000 || file.size() > 0x1B800) {
        return false;
    }
    project_info_t temp_project_info;
    memset(( uint8_t* )&temp_project_info, 0, sizeof(project_info_t));
    total_file_data                 = file + code;
    temp_project_info.project_size  = file.size();
    temp_project_info.usercode_size = code.size();
    module_param_t module_param;
    module_param                 = *mainwindow->config_view_class->get_module_param();
    temp_project_info.param_size = sizeof(module_param);
    total_file_data.append(( char* )(&module_param), sizeof(module_param));
    mbedtls_md5(( unsigned char* )total_file_data.data(), total_file_data.size(), temp_project_info.md5);
    if (memcmp(temp_project_info.md5, project_info.md5, 16) != 0) {
        QDateTime   currentDateTime   = QDateTime::currentDateTime();
        QString     formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
        QByteArray  byteArray         = formattedDateTime.toUtf8();
        const char* charData          = byteArray.constData();
        strncpy(temp_project_info.date, charData, byteArray.size());
        temp_project_info.date[byteArray.size()] = '\0';
        memcpy(( uint8_t* )&project_info, ( uint8_t* )&temp_project_info, sizeof(project_info_t));
        update_project_date();
        update_project_md5();
    }
    return true;
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
    mainwindow->config_view_class->param_project_parse(jsonObject[project_safety_param].toObject());
    mainwindow->config_view_class->config_photo_svg->config_photo_project_parse(
        jsonObject[project_config_photo].toObject());
    mainwindow->logic_view_class->logic_view_project_parse(jsonObject[project_object_logic_programe].toObject());

    ui->lineEdit_project_name->setText(jsonObject[project_name].toString());
    ui->lineEdit_company_name->setText(jsonObject[project_company_name].toString());
    ui->lineEdit_author_name->setText(jsonObject[project_author_ver].toString());
}

void project_management::project_management_reset()
{
    ui->lineEdit_author_name->clear();
    ui->lineEdit_company_name->clear();
    ui->lineEdit_current_project_checknum->clear();
    ui->lineEdit_device_project_checknum->clear();
    ui->lineEdit_path_name->clear();
    ui->lineEdit_project_name->clear();
    ui->lineEdit_save_time->clear();
    mainwindow->logic_view_class->logic_view_reset();
    project_management_info.is_new   = false;
    project_management_info.is_valid = false;
    project_management_info.filename.clear();
    project_management_info.filepath.clear();
    mainwindow->config_view_class->config_view_reset();
}

void project_management::project_cmd_response(uint8_t* frame, int32_t length)
{
    uint8_t cmd     = frame[6];
    uint8_t sub     = frame[7];
    uint8_t sync_id = frame[0];
    if (sync_id >= SYNC_ID_MAX) {
        return;
    }
    mainwindow->project_debug_class->project_debug_cmd_prase(frame, length);
    switch (cmd) {
    case CMD_PUBLIC_FILE_DOWNLOAD:
    case CMD_PUBLIC_FILE_READBACK:
        mainwindow->project_transmit_class->transmit_cmd_response(frame, length);
        break;
    case CMD_PROJECT_INFO:
        switch (sub) {
        case SUB_PROJECT_INFO_VERIFY_ACK:
            if (sync_id == SYNC_ID_A) {
                project_verify_ack.ack_info[0].responsed = true;
                project_verify_ack.ack_info[0].ack_code  = frame[10];
            } else if (sync_id == SYNC_ID_B) {
                project_verify_ack.ack_info[1].responsed = true;
                project_verify_ack.ack_info[1].ack_code  = frame[10];
            }
            break;
        default:
            break;
        }
        break;
    }
}

void project_management::project_transmit_to_device()
{
    if (mainwindow->serial_is_connect == false) {
        mainwindow->my_message_box("请检查连接线束并查看端口是否打开", MESSAGE_TYPE_WARNING);
        return;
    }
    if (mainwindow->user_permissions != USER_AUTHORIZED) {
        mainwindow->my_message_box("普通用户无权限,请授权后重试", MESSAGE_TYPE_WARNING);
        return;
    }
    if (mainwindow->logic_view_class->blocks_error_detect()) {
        mainwindow->my_message_box("图形化编程逻辑有错误，请检查", MESSAGE_TYPE_ERROR);
        return;
    }
    if (projec_info_creat() == false) {
        mainwindow->my_message_box("工程过大，请删减", MESSAGE_TYPE_ERROR);
        return;
    }
    if (mainwindow->mydevice_class->device_pass_verify() == false) {
        return;
    }
    if (project_verify() == false) {
        return;
    }
    if (mainwindow->project_transmit_class->transmit_from_project(&total_file_data, project_info)) {
        // update_device_md5(project_info.md5);
    }
}

void project_management::project_readback_from_device()
{
    if (mainwindow->serial_is_connect == false) {
        mainwindow->my_message_box("请检查连接线束并查看端口是否打开", MESSAGE_TYPE_WARNING);
        return;
    }
    if (mainwindow->user_permissions != USER_AUTHORIZED) {
        mainwindow->my_message_box("普通用户无权限,请授权后重试", MESSAGE_TYPE_WARNING);
        return;
    }
    if (project_management_info.is_valid) {
        if (mainwindow->my_message_box("从设备读取工程会覆盖当前工程，是否继续读取？", MESSAGE_TYPE_QUESTION)
            != QMessageBox::Yes) {
            return;
        }
    }
    if (mainwindow->mydevice_class->device_pass_verify() == false) {
        return;
    }
    projec_info_creat();
    if (project_verify() == false) {
        return;
    }
    if (mainwindow->project_transmit_class->readback_project_file(project_info)) {
        project_management_reset();
        memcpy(( uint8_t* )&project_info, ( uint8_t* )&mainwindow->project_transmit_class->read_project_info,
               sizeof(project_info_t));
        QByteArray project_file = mainwindow->project_transmit_class->readback_info.project_file.mid(
            0, mainwindow->project_transmit_class->read_project_info.project_size);
        project_file_prase(project_file);
        update_project_md5();
        update_project_date();
        // update_device_md5(project_info.md5);
        project_management_info.is_new   = true;
        project_management_info.is_valid = true;
        ui->tabWidget_logic->setCurrentIndex(TAB_LOGIC_PROJECT_OVERVIEW_ID);
        mainwindow->tabwidget_setenable(true);
        ui->action_save_project->setEnabled(true);
        ui->action_project_debug->setEnabled(true);
        if (mainwindow->serial_is_connect) {
            ui->actiona_transmit_todevice->setEnabled(true);
        }
    }
}

/* user slots */

void project_management::project_verify_enter_slot()
{
    if (project_verify_ack.ack_info[0].responsed == true && project_verify_ack.ack_info[1].responsed == true) {
        if (project_verify_ack.ack_info[0].ack_code != 0 || project_verify_ack.ack_info[1].ack_code != 0) {
            mainwindow->my_message_box("当前工程与设备工程一致!", MESSAGE_TYPE_WARNING);
            project_verify_ack.ack_status = ACK_STATUS_FAIL;
            return;
        }
        project_verify_ack.ack_status = ACK_STATUS_SUCCESS;
    } else {
        if (project_verify_ack.retry++ <= 3) {
            project_verify_send_cmd();
            project_verify_timer.start(500);
            return;
        }
        mainwindow->my_message_box("设备无相关指令响应", MESSAGE_TYPE_WARNING);
        project_verify_ack.ack_status = ACK_STATUS_FAIL;
    }
}

void project_management::project_new_slot()
{
    if (project_management_info.is_valid) {
        int res = mainwindow->my_message_box("新建工程会覆盖当前工程，是否保存当前工程?", MESSAGE_TYPE_QUESTION);
        if (res == QMessageBox::Yes) {
            if (project_save_slot() != 0) {
                return;
            }
        } else if (res == QMessageBox::No) {

        } else {
            return;
        }
    }
    /* reset */
    project_management_reset();
    mainwindow->tabwidget_setenable(true);
    project_management_info.is_new = true;
    ui->tabWidget_logic->setCurrentIndex(TAB_LOGIC_PROJECT_OVERVIEW_ID);
    ui->action_save_project->setEnabled(true);
    project_management_info.is_valid = true;
    if (mainwindow->serial_is_connect) {
        ui->actiona_transmit_todevice->setEnabled(true);
        ui->action_project_debug->setEnabled(true);
    }
}

int project_management::project_save_slot()
{
    if (project_management_info.is_valid == false) {
        return -1;
    }
    QString folderPath;
    if (ui->lineEdit_project_name->text().isEmpty()) {
        mainwindow->my_message_box("项目信息填写不完整，请完善项目信息", MESSAGE_TYPE_WARNING);
        return -2;
    }
    if (project_management_info.is_new) {
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::Directory);   // 设置对话框模式为选择文件夹
        dialog.setOption(QFileDialog::ShowDirsOnly);  // 只显示文件夹
        folderPath = dialog.getExistingDirectory(this, tr("选择保存路径"), QDir::homePath());
        if (folderPath.isEmpty()) {
            mainwindow->my_message_box("保存路径为空", MESSAGE_TYPE_WARNING);
            return -3;
        }
        ui->lineEdit_path_name->setText(folderPath);
    }
    if (projec_info_creat() == false) {
        mainwindow->my_message_box("工程过大，无法保存", MESSAGE_TYPE_ERROR);
        return -3;
    }
    QByteArray pro_info;
    pro_info.append(( char* )(&project_info), sizeof(project_info));
    QByteArray project_file = total_file_data + pro_info;
    for (int i = 0; i < project_file.size(); i++) {
        char byte = project_file[i];
        byte ^= PROJECT_LOAD_ENCRY_XOR_KEY;
        project_file[i] = byte;
    }
    QString projectname = ui->lineEdit_project_name->text();
    QString path        = ui->lineEdit_path_name->text();
    QFile   outputFile(path + "/" + projectname + "~.spc100");
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
        mainwindow->dispaly_status_message("工程保存成功", 3000);
    } else {
        mainwindow->my_message_box("项目文件生成或打开失败，请检查文件权限或文件是否被占用", MESSAGE_TYPE_ERROR);
    }
    return 0;
}

void project_management::project_import_slot()
{
    if (project_management_info.is_valid) {
        int res = mainwindow->my_message_box("是否保存当前工程？", MESSAGE_TYPE_QUESTION);
        if (res == QMessageBox::Yes) {
            if (project_save_slot() != 0) {
                return;
            }
        } else if (res == QMessageBox::No) {

        } else {
            return;
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

        QByteArray jsonData = inputFile.readAll();
        inputFile.close();
        for (int i = 0; i < jsonData.size(); i++) {
            char byte = jsonData[i];
            byte ^= PROJECT_LOAD_ENCRY_XOR_KEY;
            jsonData[i] = byte;
        }
        project_info_t tempinfo;
        memcpy(( uint8_t* )&tempinfo,
               jsonData.mid(jsonData.size() - sizeof(project_info_t), sizeof(project_info_t)).constData(),
               sizeof(project_info_t));
        uint8_t md5[16];
        mbedtls_md5(( unsigned char* )jsonData.constData(), jsonData.size() - sizeof(project_info_t), md5);
        if (memcmp(tempinfo.md5, md5, 16) != 0) {
            mainwindow->my_message_box("项目文件已损坏，导入失败", MESSAGE_TYPE_ERROR);
            return;
        }
        memcpy(( uint8_t* )&project_info, ( uint8_t* )&tempinfo, sizeof(project_info_t));
        project_management_reset();
        update_project_date();
        update_project_md5();
        project_management_info.is_new   = false;
        project_management_info.is_valid = true;
        project_management_info.filename = filename.mid(ret + 1);
        project_management_info.filepath = filename;
        project_management_info.filepath.remove(ret, project_management_info.filename.length() + 1);
        QByteArray project_file = jsonData.mid(0, project_info.project_size);
        project_file_prase(project_file);
        ui->tabWidget_logic->setCurrentIndex(TAB_LOGIC_PROJECT_OVERVIEW_ID);
        mainwindow->tabwidget_setenable(true);
        ui->action_save_project->setEnabled(true);
        ui->lineEdit_path_name->setText(project_management_info.filepath);
        if (mainwindow->serial_is_connect) {
            ui->actiona_transmit_todevice->setEnabled(true);
            ui->action_project_debug->setEnabled(true);
        }
    }
}

void project_management::project_transmit_to_device_slot()
{
    ui->actiona_transmit_todevice->setEnabled(false);
    project_transmit_to_device();
    ui->actiona_transmit_todevice->setEnabled(true);
}

void project_management::project_readback_from_device_slot()
{
    ui->action_read_from_device->setEnabled(false);
    project_readback_from_device();
    ui->action_read_from_device->setEnabled(true);
}

void project_management::lua_debug_creat_slot()
{
    if (mainwindow->logic_view_class->blocks_error_detect()) {
        return;
    }
    project_lua_code_creat();
    usercode_dialog.exec();
}
