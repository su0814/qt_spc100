#include "mydevice.h"
#include "mainwindow.h"
#include <QDebug>
#include <QDialog>
#include <QFormLayout>

mydevice::mydevice(QWidget* uiparent, QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )uiparent;
    change_userpass_timer.setSingleShot(true);
    pass_verify_timer.setSingleShot(true);
    connect(ui->action_change_device_passwd, &QAction::triggered, this, device_change_userpass_slot);
    connect(&change_userpass_timer, &QTimer::timeout, this, change_userpass_enter_slot);
    connect(&pass_verify_timer, &QTimer::timeout, this, device_pass_verify_enter_slot);
    connect(&device_heartbeat_timer, &QTimer::timeout, this, device_heartbeat_slot);
    device_heartbeat_timer.start(500);
}

void mydevice::update_device_md5(uint8_t* md5)
{
    QString md5_str;
    for (int i = 0; i < 16; i++) {
        md5_str += QString::number(md5[i], 16);
    }
    ui->lineEdit_device_project_checknum->setText(md5_str);
}

void mydevice::device_pass_verify_send_cmd()
{
    int     pass_len  = mainwindow->user_authorization_passwd.size();
    uint8_t frame[40] = { 0 };
    char*   pass      = mainwindow->user_authorization_passwd.toUtf8().data();
    memcpy(frame, pass, pass_len);
    mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_DEVICE, CMD_DEVICE_PASSWD, SUB_DEVICE_PASSWD_VERIFY, frame,
                                              pass_len);
}

void mydevice::device_change_passwd_send_cmd()
{
    uint8_t len     = 0;
    uint8_t cmd[40] = { 0 };
    cmd[len++]      = old_passwd_str.size();
    char* old       = old_passwd_str.toUtf8().data();
    memcpy(&cmd[len], old, old_passwd_str.size());
    len += old_passwd_str.size();
    cmd[len++] = new_passwd_str.size();
    char* newp = new_passwd_str.toUtf8().data();
    memcpy(&cmd[len], newp, new_passwd_str.size());
    len += old_passwd_str.size();
    mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_DEVICE, CMD_DEVICE_PASSWD, SUB_DEVICE_PASSWD_CHANGE, cmd,
                                              len);
}

bool mydevice::device_pass_verify()
{
    pass_verify_ack.ack_info[0].ack_code  = 0;
    pass_verify_ack.ack_info[0].responsed = false;
    pass_verify_ack.ack_info[1].ack_code  = 0;
    pass_verify_ack.ack_info[1].responsed = false;
    pass_verify_ack.retry                 = 0;
    pass_verify_ack.ack_status            = ACK_STATUS_WAITING;
    device_pass_verify_send_cmd();
    pass_verify_timer.start(500);
    while (pass_verify_ack.ack_status == ACK_STATUS_WAITING) {
        QApplication::processEvents();
    }
    if (pass_verify_ack.ack_status == ACK_STATUS_SUCCESS) {
        return true;
    }
    return false;
}

device_line_status_e mydevice::device_get_line_status()
{
    return device_heartbeat.device_line_status;
}

void mydevice::device_cmd_response(uint8_t* frame, int32_t length)
{
    length          = length;
    uint8_t cmd     = frame[6];
    uint8_t sub     = frame[7];
    uint8_t sync_id = frame[0];
    uint8_t len     = frame[8] | frame[9] << 8;
    switch (cmd) {
    case CMD_DEVICE_PASSWD:
        switch (sub) {
        case SUB_DEVICE_PASSWD_CHANGE_ACK:
            if (sync_id == SYNC_ID_A) {
                change_passwd_reply[0] = true;
                change_passwd_error[0] = frame[10];
            } else if (sync_id == SYNC_ID_B) {
                change_passwd_reply[1] = true;
                change_passwd_error[1] = frame[10];
            }
            break;
        case SUB_DEVICE_PASSWD_VERIFY_ACK:
            if (sync_id == SYNC_ID_A) {
                pass_verify_ack.ack_info[0].responsed = true;
                pass_verify_ack.ack_info[0].ack_code  = frame[10];
            } else if (sync_id == SYNC_ID_B) {
                pass_verify_ack.ack_info[1].responsed = true;
                pass_verify_ack.ack_info[1].ack_code  = frame[10];
            }
            break;
        default:
            break;
        }
        break;
    case CMD_DEVICE_HEARTBEAT:
        device_heartbeat.heartbeat_responsed = true;
        if (sub == SUB_DEVICE_HEARTBEAT_RESPONSE) {
            if (frame[10] == 0 && len == 17) {
                update_device_md5(&frame[11]);
            }
        }
        break;
    default:
        break;
    }
}

/* user slots */

void mydevice::device_heartbeat_slot()
{
    static device_line_status_e last_state = DEVICE_LINE_STATUS_ON;
    if (last_state != device_heartbeat.device_line_status) {
        last_state = device_heartbeat.device_line_status;
        emit device_line_status_change_signal(last_state);
    }
    if (mainwindow->serial_is_connect == false) {
        ui->action_serial_open->setIcon(QIcon(":/new/photo/photo/connect.png"));
        ui->action_serial_open->setToolTip("端口未配置");
        device_heartbeat.offline_cnt         = 0;
        device_heartbeat.online_cnt          = 0;
        device_heartbeat.heartbeat_responsed = false;
        device_heartbeat.device_line_status  = DEVICE_LINE_STATUS_OFF;
        mainwindow->status_class->read_status_switch(false);
        return;
    }
    if (device_heartbeat.heartbeat_responsed) {
        device_heartbeat.offline_cnt = 0;
        if (device_heartbeat.online_cnt == 1) {
            device_heartbeat.device_line_status = DEVICE_LINE_STATUS_ON;
            ui->action_serial_open->setIcon(QIcon(":/new/photo/photo/connect_online.png"));
            ui->action_serial_open->setToolTip("已连接SPC100设备");
            mainwindow->status_class->read_status_switch(true);
        }
        device_heartbeat.online_cnt =
            device_heartbeat.online_cnt > 3 ? device_heartbeat.online_cnt : device_heartbeat.online_cnt + 1;
    } else {
        device_heartbeat.online_cnt = 0;
        if (device_heartbeat.offline_cnt == 1) {
            device_heartbeat.device_line_status = DEVICE_LINE_STATUS_OFF;
            ui->action_serial_open->setIcon(QIcon(":/new/photo/photo/connect_offline.png"));
            ui->action_serial_open->setToolTip("未找到SPC100设备");
            mainwindow->status_class->read_status_switch(false);
        }
        device_heartbeat.offline_cnt =
            device_heartbeat.offline_cnt > 3 ? device_heartbeat.offline_cnt : device_heartbeat.offline_cnt + 1;
    }
    device_heartbeat.heartbeat_responsed = false;
    mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_DEVICE, CMD_DEVICE_HEARTBEAT, SUB_DEVICE_HEARTBEAT_REQUEST,
                                              NULL, 0);
}

void mydevice::device_pass_verify_enter_slot()
{
    if (pass_verify_ack.ack_info[0].responsed == true && pass_verify_ack.ack_info[1].responsed == true) {
        if (pass_verify_ack.ack_info[0].ack_code != 0 || pass_verify_ack.ack_info[1].ack_code != 0) {
            mainwindow->my_message_box("设备密码错误,请检查设备密码!", MESSAGE_TYPE_ERROR);
            pass_verify_ack.ack_status = ACK_STATUS_FAIL;
            return;
        }
        pass_verify_ack.ack_status = ACK_STATUS_SUCCESS;
    } else {
        if (pass_verify_ack.retry++ <= 3) {
            device_pass_verify_send_cmd();
            pass_verify_timer.start(DEVICE_RETRY_TIME);
            return;
        }
        mainwindow->my_message_box("设备无相关指令响应！", MESSAGE_TYPE_ERROR);
        pass_verify_ack.ack_status = ACK_STATUS_FAIL;
    }
}

void mydevice::change_userpass_enter_slot()
{
    static uint8_t retry = 0;
    if (change_passwd_reply[0] == true && change_passwd_reply[1] == true) {
        if (change_passwd_error[0] != 0 || change_passwd_error[1] != 0) {
            mainwindow->my_message_box("设备密码修改失败，原密码错误!", MESSAGE_TYPE_ERROR);
        } else {
            mainwindow->my_message_box("设备密码修改成功", MESSAGE_TYPE_INFO);
        }
        retry                       = 0;
        device_change_passwd_status = DEVICE_CHANGE_PASSWD_STATUS_SUCCESS;
    } else {
        if (++retry <= 3) {
            device_change_passwd_send_cmd();
            change_userpass_timer.start(DEVICE_RETRY_TIME);
            return;
        }
        mainwindow->my_message_box("设备密码修改失败，设备无响应", MESSAGE_TYPE_ERROR);
        device_change_passwd_status = DEVICE_CHANGE_PASSWD_STATUS_FAIL;
        retry                       = 0;
    }
}

void mydevice::device_change_userpass_slot()
{
    if (mainwindow->user_permissions != USER_AUTHORIZED) {
        mainwindow->my_message_box("普通用户无权限,请授权后重试", MESSAGE_TYPE_WARNING);
        return;
    }
    QDialog dialog;
    dialog.setWindowTitle("修改设备密码");
    dialog.setStyleSheet("QDialog { background-color: rgb(210,230,255); }");
    dialog.setWindowFlags(Qt::Tool);
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    QFormLayout* layout = new QFormLayout(&dialog);  //获取窗体布局
    layout->setContentsMargins(10, 10, 10, 10);
    /* 密码框设置 */
    QRegExp   regExp("[A-Za-z0-9_.*%@]*");  //密码规范
    QLineEdit old_passwd;
    QLineEdit new_passwd;
    QLineEdit new_repeat_passwd;
    old_passwd.setMaxLength(16);  //最大输入字符数
    new_passwd.setMaxLength(16);
    new_repeat_passwd.setMaxLength(16);
    old_passwd.setEchoMode(QLineEdit::Password);  //密码输入模式
    new_passwd.setEchoMode(QLineEdit::Password);
    new_repeat_passwd.setEchoMode(QLineEdit::Password);
    QRegExpValidator* validator1 = new QRegExpValidator(regExp, &old_passwd);  //设置密码规范
    QRegExpValidator* validator2 = new QRegExpValidator(regExp, &new_passwd);
    QRegExpValidator* validator3 = new QRegExpValidator(regExp, &new_repeat_passwd);
    old_passwd.setValidator(validator1);
    new_passwd.setValidator(validator2);
    new_repeat_passwd.setValidator(validator3);
    /* 密码显示按钮 */
    QCheckBox* dispaly_pass = new QCheckBox;
    dispaly_pass->setStyleSheet("QCheckBox::indicator:unecked {width:20 px;height: 20px;}"
                                "QCheckBox::indicator:enabled:unchecked {image: url(:/new/photo/photo/close_eye.png);}"
                                "QCheckBox::indicator:checked {width:20px;height: 20px;}"
                                "QCheckBox::indicator:enabled:checked {image: url(:/new/photo/photo/open_eye.png);}");
    connect(dispaly_pass, &QCheckBox::stateChanged, [&](int state) {
        if (state == Qt::Checked) {
            old_passwd.setEchoMode(QLineEdit::Normal);
            new_passwd.setEchoMode(QLineEdit::Normal);
            new_repeat_passwd.setEchoMode(QLineEdit::Normal);
        } else {
            old_passwd.setEchoMode(QLineEdit::Password);
            new_passwd.setEchoMode(QLineEdit::Password);
            new_repeat_passwd.setEchoMode(QLineEdit::Password);
        }
    });
    /* 密码提示 */
    QLabel tip;
    tip.setStyleSheet("color: gray;");
    tip.setText("密码最大长度为16位，仅支持大小写字母、数字和部分特殊字符(_.*%@)");
    /* 状态显示 */
    QStatusBar* statusBar = new QStatusBar;
    statusBar->setStyleSheet("color: red;");
    statusBar->setSizeGripEnabled(false);  //禁止缩放
    /* 确定修改按钮 */
    QPushButton okButton("确定");
    QObject::connect(&okButton, &QPushButton::clicked, [&]() {
        if (old_passwd.text().isEmpty() || new_passwd.text().isEmpty() || new_repeat_passwd.text().isEmpty()) {
            statusBar->showMessage("密码不能为空", 3000);
            return;
        }
        if (new_repeat_passwd.text() != new_passwd.text()) {
            statusBar->showMessage("新密码前后输入不一致", 3000);
            return;
        }
        if (new_passwd.text() == old_passwd.text()) {
            statusBar->showMessage("新密码与旧密码相同", 3000);
            return;
        }
        okButton.setEnabled(false);
        old_passwd_str              = old_passwd.text();
        new_passwd_str              = new_passwd.text();
        device_change_passwd_status = DEVICE_CHANGE_PASSWD_STATUS_ING;
        memset(change_passwd_reply, false, sizeof(change_passwd_reply));
        memset(change_passwd_error, 0, sizeof(change_passwd_error));
        device_change_passwd_send_cmd();
        change_userpass_timer.start(DEVICE_RETRY_TIME);
        statusBar->setStyleSheet("color: blue;");
        statusBar->showMessage("修改密码中······");
        while (device_change_passwd_status == DEVICE_CHANGE_PASSWD_STATUS_ING) {
            QApplication::processEvents();
        }
        dialog.close();
    });

    /* 布局 */
    QHBoxLayout* passlayout = new QHBoxLayout;  //旧密码输入
    passlayout->addWidget(&old_passwd);
    passlayout->addWidget(dispaly_pass);
    layout->addRow("输入原密码:", passlayout);
    layout->addRow(&tip);
    layout->addRow("输入新密码:", &new_passwd);
    layout->addRow("再次输入:", &new_repeat_passwd);
    layout->addRow(statusBar);
    layout->addRow(&okButton);
    /* 显示弹窗 */
    dialog.exec();
}
