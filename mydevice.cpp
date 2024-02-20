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

void mydevice::device_pass_verify_send_cmd()
{
    int     pass_len  = mainwindow->user_authorization_passwd.size();
    uint8_t frame[40] = { 0, CMD_TYPE_DEVICE, CMD_DEVICE_PASSWD, SUB_DEVICE_PASSWD_VERIFY };
    frame[4]          = pass_len;
    frame[5]          = pass_len >> 8;
    char* pass        = mainwindow->user_authorization_passwd.toUtf8().data();
    memcpy(&frame[6], pass, pass_len);
    mainwindow->my_serial->port_sendframe(frame, pass_len + 6);
}

void mydevice::device_change_passwd_send_cmd()
{
    uint8_t len     = 6;
    uint8_t cmd[40] = { 0,
                        CMD_TYPE_DEVICE,
                        CMD_DEVICE_PASSWD,
                        SUB_DEVICE_PASSWD_CHANGE,
                        (uint8_t)(old_passwd_str.size() + new_passwd_str.size() + 2),
                        0 };
    cmd[len++]      = old_passwd_str.size();
    char* old       = old_passwd_str.toUtf8().data();
    memcpy(&cmd[len], old, old_passwd_str.size());
    len += old_passwd_str.size();
    cmd[len++] = new_passwd_str.size();
    char* newp = new_passwd_str.toUtf8().data();
    memcpy(&cmd[len], newp, new_passwd_str.size());
    len += old_passwd_str.size();
    mainwindow->my_serial->port_sendframe(cmd, len);
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
    uint8_t cmd     = frame[2];
    uint8_t sub     = frame[3];
    uint8_t sync_id = frame[0];
    switch (cmd) {
    case CMD_DEVICE_PASSWD:
        switch (sub) {
        case SUB_DEVICE_PASSWD_CHANGE_ACK:
            if (sync_id == SYNC_ID_A) {
                change_passwd_reply[0] = true;
                change_passwd_error[0] = frame[6];
            } else if (sync_id == SYNC_ID_B) {
                change_passwd_reply[1] = true;
                change_passwd_error[1] = frame[6];
            }
            break;
        case SUB_DEVICE_PASSWD_VERIFY_ACK:
            if (sync_id == SYNC_ID_A) {
                pass_verify_ack.ack_info[0].responsed = true;
                pass_verify_ack.ack_info[0].ack_code  = frame[6];
            } else if (sync_id == SYNC_ID_B) {
                pass_verify_ack.ack_info[1].responsed = true;
                pass_verify_ack.ack_info[1].ack_code  = frame[6];
            }
            break;
        default:
            break;
        }
        break;
    case CMD_DEVICE_HEARTBEAT:
        device_heartbeat.heartbeat_responsed = true;
        break;
    default:
        break;
    }
}

/* user slots */

void mydevice::device_heartbeat_slot()
{
    if (mainwindow->serial_is_connect == false) {
        ui->action_serial_open->setIcon(QIcon(":/new/photo/photo/connect.png"));
        ui->action_serial_open->setToolTip("端口未配置");
        device_heartbeat.offline_cnt         = 0;
        device_heartbeat.online_cnt          = 0;
        device_heartbeat.heartbeat_responsed = false;
        return;
    }
    if (device_heartbeat.heartbeat_responsed) {
        device_heartbeat.offline_cnt = 0;
        if (device_heartbeat.online_cnt == 1) {
            device_heartbeat.device_line_status = DEVICE_LINE_STATUS_ON;
            ui->action_serial_open->setIcon(QIcon(":/new/photo/photo/connect_online.png"));
            ui->action_serial_open->setToolTip("已连接SPC100设备");
        }
        device_heartbeat.online_cnt =
            device_heartbeat.online_cnt > 3 ? device_heartbeat.online_cnt : device_heartbeat.online_cnt + 1;
    } else {
        device_heartbeat.online_cnt = 0;
        if (device_heartbeat.offline_cnt == 1) {
            device_heartbeat.device_line_status = DEVICE_LINE_STATUS_OFF;
            ui->action_serial_open->setIcon(QIcon(":/new/photo/photo/connect_offline.png"));
            ui->action_serial_open->setToolTip("未找到SPC100设备");
        }
        device_heartbeat.offline_cnt =
            device_heartbeat.offline_cnt > 3 ? device_heartbeat.offline_cnt : device_heartbeat.offline_cnt + 1;
    }
    device_heartbeat.heartbeat_responsed = false;
    uint8_t cmd[6]                       = { 0, CMD_TYPE_DEVICE, CMD_DEVICE_HEARTBEAT, 0, 0, 0 };
    mainwindow->my_serial->port_sendframe(cmd, 6);
}

void mydevice::device_pass_verify_enter_slot()
{
    if (pass_verify_ack.ack_info[0].responsed == true && pass_verify_ack.ack_info[1].responsed == true) {
        if (pass_verify_ack.ack_info[0].ack_code != 0 || pass_verify_ack.ack_info[1].ack_code != 0) {
            mainwindow->my_message_box("设备密码验证", "设备密码错误!", false);
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
        mainwindow->my_message_box("设备密码验证", "设备无相关指令响应", false);
        pass_verify_ack.ack_status = ACK_STATUS_FAIL;
    }
}

void mydevice::change_userpass_enter_slot()
{
    static uint8_t retry = 0;
    if (change_passwd_reply[0] == true && change_passwd_reply[1] == true) {
        if (change_passwd_error[0] != 0 || change_passwd_error[1] != 0) {
            mainwindow->my_message_box("设备密码修改", "修改失败，原密码错误!", false);
        } else {
            mainwindow->my_message_box("设备密码修改", "修改成功", false);
        }
        retry                       = 0;
        device_change_passwd_status = DEVICE_CHANGE_PASSWD_STATUS_SUCCESS;
    } else {
        if (++retry <= 3) {
            device_change_passwd_send_cmd();
            change_userpass_timer.start(DEVICE_RETRY_TIME);
            return;
        }
        mainwindow->my_message_box("设备密码修改", "密码修改失败，设备无响应", false);
        device_change_passwd_status = DEVICE_CHANGE_PASSWD_STATUS_FAIL;
        retry                       = 0;
    }
}

void mydevice::device_change_userpass_slot()
{
    if (mainwindow->user_permissions != USER_AUTHORIZED) {
        mainwindow->my_message_box("操作失败", "普通用户无权限,请授权后重试", false);
        return;
    }
    QDialog dialog;
    dialog.setWindowTitle("修改设备密码");
    dialog.setFixedSize(450 * mainwindow->size().width() / UI_WIDTH,
                        200 * mainwindow->size().height() / ui_HEIGHT);  //设置框体大小
    QFormLayout* layout = new QFormLayout(&dialog);                      //获取窗体布局
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
