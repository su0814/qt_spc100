#include "mainwindow.h"
#include "QDateTime"
#include "QTime"
#include "lua.h"
#include "md5.h"
#include "qdebug.h"
#include "qmessagebox.h"
#include "status.h"
#include "stdio.h"
#include "ui_mainwindow.h"
#include "upgrade.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFontMetrics>
#include <QFormLayout>
#include <QInputDialog>
#include <QPainter>
#include <QProxyStyle>
#include <QScreen>
#include <QShortcut>
#include <QStyle>
#include <QStyleOptionTab>
#include <QUndoStack>
#include <QWindow>
#include <windows.h>
MainWindow* MainWindow::my_ui = nullptr;
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(("SPC100 Config tool " + QString(APP_VERSION)));
    my_ui = this;
    connect(&ui_resize_timer, &QTimer::timeout, this, &MainWindow::ui_resize_slot);
    upgrade_class             = new upgrade(this);
    lua_class                 = new lua(this);
    status_class              = new status(this);
    config_view_class         = new config_view(this);
    safety_param_dialog_class = new Safety_Param_Dialog(this);
    condition_view_class      = new condition_view(this);
    logic_tools_class         = new logic_tools(this);
    logic_view_class          = new logic_view(this);
    ui->groupBox_logic->layout()->addWidget(logic_view_class);
    coroutine_lua_class      = new coroutine_lua(this);
    project_management_class = new project_management(this);
    project_report_class     = new project_report(this);
    mydevice_class           = new mydevice(this);
    project_debug_class      = new project_debug(this);
    about_prajna_class       = new about_prajna(this);
    log_dialog_class         = new log_dialog(this);
    version_dialog_class     = new version_Dialog(this);
    // config_menu_class        = new config_menu(this);
    ui->config_menu_treeWidget->setVisible(false);
    ui->groupBox_config_view->layout()->addWidget(config_view_class);
    ui_init();
    ui_resize_timer.start(100);
    condition_view_class->update_tim.start(1000);
}

MainWindow::~MainWindow()
{
    delete logic_view_class;
    my_serial->close_port();
    delete ui;
}

void MainWindow::ui_init()
{
    my_serial = new my_serialport;
    transport = new transportcrc;
    serial_port_combobox.installEventFilter(this);
    serial_search();

    connect(my_serial->my_serial, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this,
            SLOT(serial_error_callback(QSerialPort::SerialPortError)));
    connect(my_serial->my_serial, SIGNAL(readyRead()), this, SLOT(serial_data_proc()));
    connect(my_serial->transport, SIGNAL(signal_onFrameDetected(uint8_t*, int32_t)), this,
            SLOT(cmd_callback(uint8_t*, int32_t)));
    serial_disconnect_callback();
    /* 串口弹窗 */
    serial_connect_button.setText("打开端口");
    QStringList baudrateitems;
    baudrateitems << "9600"
                  << "115200"
                  << "256000"
                  << "921600";
    serial_baudrate_combobox.addItems(baudrateitems);
    serial_baudrate_combobox.setCurrentIndex(1);
    serial_dialog.setWindowTitle("端口配置");
    serial_dialog.setWindowFlags(Qt::Tool);
    serial_dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    serial_dialog.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    QFormLayout* layout = new QFormLayout(&serial_dialog);  //获取窗体布局
    layout->addRow("端口", &serial_port_combobox);
    layout->addRow("波特率", &serial_baudrate_combobox);
    layout->addRow(&serial_connect_button);
    layout->setContentsMargins(10, 10, 10, 10);
    connect(&serial_connect_button, &QPushButton::clicked, this, serial_connect_slot);
    tabwidget_setenable(false);
}

void MainWindow::tabwidget_setenable(bool state)
{
    for (int i = TAB_LOGIC_PROJECT_OVERVIEW_ID; i <= TAB_LOGIC_ERROR_ID; i++) {
        ui->tabWidget_logic->setTabEnabled(i, state);
    }
    ui->action_usercode->setEnabled(state);
}

void MainWindow::user_authorization_passwd_window()
{
    QDialog dialog;
    dialog.setWindowFlag(Qt::Tool);
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    dialog.setWindowTitle("用户授权");
    dialog.setStyleSheet("QDialog { background-color: rgb(210,230,255); }");
    QFormLayout* layout = new QFormLayout(&dialog);
    layout->setContentsMargins(10, 10, 10, 10);
    QLineEdit passwd_edit;
    passwd_edit.setMaxLength(16);
    passwd_edit.setEchoMode(QLineEdit::Password);
    QRegExp           regExp("[A-Za-z0-9_.*%@]*");
    QRegExpValidator* validator = new QRegExpValidator(regExp, &passwd_edit);
    passwd_edit.setValidator(validator);
    QLineEdit autopass_edit;
    autopass_edit.setMaxLength(16);
    autopass_edit.setEchoMode(QLineEdit::Password);
    QRegExpValidator* validator1 = new QRegExpValidator(regExp, &autopass_edit);
    autopass_edit.setValidator(validator1);
    QCheckBox* dispaly_pass = new QCheckBox;
    dispaly_pass->setStyleSheet("QCheckBox::indicator:unecked {width:20 px;height: 20px;}"
                                "QCheckBox::indicator:enabled:unchecked {image: url(:/new/photo/photo/close_eye.png);}"
                                "QCheckBox::indicator:checked {width:20px;height: 20px;}"
                                "QCheckBox::indicator:enabled:checked {image: url(:/new/photo/photo/open_eye.png);}");
    connect(dispaly_pass, &QCheckBox::stateChanged, [&](int state) {
        if (state == Qt::Checked) {
            passwd_edit.setEchoMode(QLineEdit::Normal);
            autopass_edit.setEchoMode(QLineEdit::Normal);
        } else {
            passwd_edit.setEchoMode(QLineEdit::Password);
            autopass_edit.setEchoMode(QLineEdit::Password);
        }
    });
    QHBoxLayout* passlayout = new QHBoxLayout;
    passlayout->addWidget(&autopass_edit);
    passlayout->addWidget(dispaly_pass);
    layout->addRow("输入授权密码:", passlayout);
    layout->addRow("输入设备密码:", &passwd_edit);
    QLabel tip;
    tip.setStyleSheet("color: gray;");
    tip.setText("密码最大长度为16位，仅支持大小写字母、数字和部分特殊字符(_.*%@)");
    layout->addRow(&tip);
    QStatusBar* statusBar = new QStatusBar;
    statusBar->setStyleSheet("color: red;");
    statusBar->setSizeGripEnabled(false);
    layout->addRow(statusBar);
    QPushButton okButton("确定");
    layout->addRow(&okButton);
    QObject::connect(&okButton, &QPushButton::clicked, [&]() {
        if (passwd_edit.text().isEmpty() || autopass_edit.text().isEmpty()) {
            statusBar->showMessage("密码不能为空", 3000);
            return;
        }
        if (autopass_edit.text() != "rhdzkj8888") {
            statusBar->showMessage("授权密码错误", 3000);
            return;
        }
        user_permissions          = USER_AUTHORIZED;
        user_authorization_passwd = passwd_edit.text();
        ui->action_permissions->setIcon(QIcon(":/new/photo/photo/permissions_success.png"));
        dialog.close();
    });
    dialog.exec();
}

void MainWindow::user_authorization()
{
    switch (user_permissions) {
    case USER_REGULAR:
        user_authorization_passwd_window();
        break;
    case USER_AUTHORIZED:
        user_permissions = USER_REGULAR;
        ui->action_permissions->setIcon(QIcon(":/new/photo/photo/permissions.png"));
        break;
    default:
        break;
    }
}

void MainWindow::serial_search()
{
    serial_port_combobox.clear();
    my_serial->serch_port();
    int len = my_serial->portname_list.length();
    if (len > 0) {
        for (uint8_t i = 0; i < len; i++) {
            serial_port_combobox.addItem(my_serial->portname_list[i].portName() + " #"
                                         + my_serial->portname_list[i].description());
        }
    }
}

void MainWindow::serial_data_proc()
{

    QByteArray tmp_buf;
    while (my_serial->my_serial->bytesAvailable()) {
        tmp_buf.append(my_serial->my_serial->readAll());
    }
    foreach (uint8_t byte, tmp_buf) {
        my_serial->data_decrypt(&byte, 1);
        my_serial->transport->receiveByte(byte);
    }
}

void MainWindow::cmd_callback(uint8_t* frame, int32_t length)
{
    uint8_t cmd_type = frame[1];
    uint8_t cmd      = frame[2];
    switch (cmd_type) {
    case CMD_TYPE_BL:
        upgrade_class->boot_cmd_response(frame, length);
        break;
    case CMD_TYPE_PROJECT:
        project_management_class->project_cmd_response(frame, length);
        break;
    case CMD_TYPE_READ:
        switch (cmd) {
        case CMD_READ_STATUS:
            status_class->type_status_response(frame, length);
            break;
        default:
            break;
        }

        break;
    case CMD_TYPE_REPORT:
        switch (cmd) {
        case CMD_REPORT_LOG:
        case CMD_REPORT_INFO:
            log_dialog_class->cmd_report_response(frame, length);
            break;
        default:
            break;
        }

        break;
    case CMD_TYPE_DEVICE:
        mydevice_class->device_cmd_response(frame, length);
        break;
    default:
        break;
    }
}

void MainWindow::serial_connect_callback()
{
    serial_is_connect = true;
    serial_baudrate_combobox.setEnabled(false);
    serial_port_combobox.setEnabled(false);
    ui->start_read_status_pushButton->setEnabled(true);
    ui->stop_read_status_pushButton->setEnabled(true);
    status_class->status_serial_connect_callback();
    upgrade_class->upgrade_serial_connect_callback();
    if (project_management_class->project_management_info.is_valid) {
        ui->actiona_transmit_todevice->setEnabled(true);
        ui->action_project_debug->setEnabled(true);
    }
    ui->action_read_from_device->setEnabled(true);
    ui->action_change_device_passwd->setEnabled(true);
}

void MainWindow::serial_disconnect_callback()
{
    serial_is_connect = false;
    my_serial->close_port();
    serial_baudrate_combobox.setEnabled(true);
    serial_port_combobox.setEnabled(true);
    ui->start_read_status_pushButton->setEnabled(false);
    ui->stop_read_status_pushButton->setEnabled(false);
    status_class->status_serial_disconnect_callback();
    upgrade_class->upgrade_serial_disconnect_callback();
    ui->actiona_transmit_todevice->setEnabled(false);
    ui->action_read_from_device->setEnabled(false);
    ui->action_change_device_passwd->setEnabled(false);
    ui->action_project_debug->setEnabled(false);
}

int MainWindow::my_message_box(QString text, messabe_type_e type)
{
    static bool isshow_flag = false;
    if (!isshow_flag) {
        isshow_flag = true;
        QMessageBox box;
        box.setText(text);

        switch (type) {
        case MESSAGE_TYPE_INFO:
            box.setWindowTitle("信息");
            box.setIcon(QMessageBox::Information);
            box.setStandardButtons(QMessageBox::Ok);
            break;
        case MESSAGE_TYPE_WARNING:
            box.setWindowTitle("警告");
            box.setIcon(QMessageBox::Warning);
            box.setStandardButtons(QMessageBox::Ok);
            break;
        case MESSAGE_TYPE_ERROR:
            box.setWindowTitle("错误");
            box.setIcon(QMessageBox::Critical);
            box.setStandardButtons(QMessageBox::Ok);
            break;
        case MESSAGE_TYPE_QUESTION:
            box.setWindowTitle("请确认");
            box.setIcon(QMessageBox::Question);
            box.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Close);
            box.button(QMessageBox::Close)->hide();
            box.setEscapeButton(QMessageBox::Close);
            break;
        default:
            break;
        }
        int result  = box.exec();
        isshow_flag = false;
        return result;
    }
    return -1;
}

void MainWindow::dispaly_status_message(QString info, int time)
{
    ui->statusBar->showMessage(info, time);
}

int MainWindow::serial_error_callback(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError || error == QSerialPort::PermissionError) {
        upgrade_class->iap_info.result_status = IAP_DOWNLOAD_FAIL;
        serial_search();
        serial_connect_button.setEnabled(true);
        serial_disconnect_callback();
        my_message_box("端口异常断开，请检查端口状态！", MESSAGE_TYPE_ERROR);
    }
    return 0;
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == &serial_port_combobox) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->buttons() & Qt::LeftButton) {
                serial_search();
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

/* user slot */

void MainWindow::ui_resize_slot()
{
    static bool first               = true;
    bool        isLeftButtonPressed = (QGuiApplication::mouseButtons() & Qt::LeftButton);
    if (isLeftButtonPressed) {
        return;
    }
    QWindow* window = this->windowHandle();
    if (window) {
        QScreen* screen = window->screen();
        if (screen) {
            QSize resolution   = screen->size();  // 获取屏幕分辨率
            int   screenWidth  = resolution.width();
            int   screenHeight = resolution.height();
            if (screen_height != screenHeight || screen_width != screenWidth || first) {
                first         = false;
                screen_width  = screenWidth;
                screen_height = screenHeight;
                resize(950, 700);
                QDesktopWidget desktop;
                QRect          screenGeometry = desktop.screenGeometry(QCursor::pos());
                move(screenGeometry.center() - this->rect().center());
            }
        }
    }
}

void MainWindow::on_action_serial_close_triggered()
{
    if (serial_is_connect) {
        serial_connect_button.setEnabled(true);
        ui->action_serial_open->setIcon(QIcon(":/new/photo/photo/connect.png"));
        ui->action_serial_open->setToolTip("端口未配置");
        serial_disconnect_callback();
    }
}

void MainWindow::on_action_permissions_triggered()
{
    user_authorization();
}

void MainWindow::on_action_serial_open_triggered()
{
    serial_dialog.setStyleSheet("QDialog { background-color: rgb(210,230,255); }");
    serial_dialog.setLayout(serial_dialog.layout());
    serial_dialog.exec();
}

void MainWindow::serial_connect_slot()
{
    QSerialPort::DataBits databits = QSerialPort::Data8;
    QSerialPort::StopBits stopbits = QSerialPort::OneStop;
    QSerialPort::Parity   parity   = QSerialPort::NoParity;
    qint32                baudrate = (serial_baudrate_combobox.currentText().toUInt());
    if (my_serial->portname_list.length() == 0) {
        my_message_box("未检测到可用端口！", MESSAGE_TYPE_WARNING);
        return;
    }
    int i   = serial_port_combobox.currentIndex();
    int ret = my_serial->open_port(my_serial->portname_list[i].portName(), baudrate, databits, parity, stopbits);
    if (ret != 0) {
        my_message_box("端口打开失败", MESSAGE_TYPE_ERROR);
        return;
    }
    ui->action_serial_open->setIcon(QIcon(":/new/photo/photo/connect_offline.png"));
    serial_connect_button.setEnabled(false);
    serial_connect_callback();
    serial_dialog.close();
}

void MainWindow::on_A_SOFT_STATUS_checkBox_clicked(bool checked)
{
    ui->A_SOFT_STATUS_checkBox->setChecked(!checked);
}

void MainWindow::on_B_SOFT_STATUS_checkBox_clicked(bool checked)
{
    ui->B_SOFT_STATUS_checkBox->setChecked(!checked);
}

void MainWindow::on_start_read_status_pushButton_clicked()
{
    status_class->read_status_switch(true);
}

void MainWindow::on_stop_read_status_pushButton_clicked()
{
    status_class->read_status_switch(false);
}

void MainWindow::on_read_label_pushButton_clicked()
{
    status_class->label_read();
}

void MainWindow::on_clear_label_pushButton_clicked()
{
    status_class->label_clear();
}

void MainWindow::on_save_label_pushButton_clicked()
{
    status_class->label_save();
}

void MainWindow::on_lineEdit_projectname_textChanged(const QString& arg1)
{
    if (arg1.isEmpty()) {
        ui->label_project_name->setText("-");
    } else {
        ui->label_project_name->setText(arg1);
    }
}

void MainWindow::on_lineEdit_author_name_textChanged(const QString& arg1)
{
    if (arg1.isEmpty()) {
        ui->label_author_name->setText("-");
    } else {
        ui->label_author_name->setText(arg1);
    }
}

void MainWindow::on_lineEdit_company_name_textChanged(const QString& arg1)
{
    if (arg1.isEmpty()) {
        ui->label_company_name->setText("-");
    } else {
        ui->label_company_name->setText(arg1);
    }
}

void MainWindow::on_lineEdit_project_version_textChanged(const QString& arg1)
{
    if (arg1.isEmpty()) {
        ui->label_project_version->setText("-");
    } else {
        ui->label_project_version->setText(arg1);
    }
}

void MainWindow::on_lineEdit_project_path_textChanged(const QString& arg1)
{
    if (arg1.isEmpty()) {
        ui->label_project_path->setText("-");
    } else {
        ui->label_project_path->setText(arg1);
    }
}
