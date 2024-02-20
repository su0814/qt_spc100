#include "mainwindow.h"
#include "QDateTime"
#include "QTime"
#include "lua.h"
#include "md5.h"
#include "param.h"
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
//:/new/photo/photo/logo.png
class CustomTabStyle : public QProxyStyle {
public:
    uint32_t tabbar_width  = 130;
    uint32_t tabbar_height = 100;

public:
    QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& size,
                           const QWidget* widget) const
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab) {
            s.transpose();
            s.rwidth()  = tabbar_width;  // 设置每个tabBar中item的大小
            s.rheight() = tabbar_height;
        }
        return s;
    }

    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
    {
        if (element == CE_TabBarTabLabel) {
            if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(option)) {
                QRect allRect = tab->rect;

                if (tab->state & QStyle::State_Selected) {
                    painter->save();
                    painter->setPen(0xc8c8ff);
                    painter->setBrush(QBrush(0xc8c8ff));
                    painter->drawRect(allRect.adjusted(5, 5, -5, -5));
                    painter->restore();
                }

                QRect tabRect = allRect.adjusted(0, 0, -0, -allRect.height() / 2);  // 调整绘制文本的区域

                if (!tab->icon.isNull()) {
                    int   iconSize = tabbar_height / 2;                                   // 设置图标大小
                    int   iconX    = tabRect.width() / 2 - iconSize / 2;                  // 图标的横坐标
                    int   iconY    = tabRect.top() + tabRect.height() - iconSize * 0.75;  // 图标的纵坐标
                    QRect iconRect(iconX, iconY, iconSize, iconSize);

                    tab->icon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);  // 绘制图标
                }

                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                if (tab->state & QStyle::State_Selected) {
                    painter->setPen(0xf8fcff);
                } else {
                    painter->setPen(0x5d5d5d);
                }
                QRect textRect = QRect(tabRect.left(), tabRect.bottom(), tabRect.width(), allRect.height() / 2);
                painter->drawText(textRect, tab->text, option);
                return;
            }
        }

        if (element == CE_TabBarTab) {
            QProxyStyle::drawControl(element, option, painter, widget);
        }
    }
};

MainWindow* MainWindow::my_ui = nullptr;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(("SPC100 Config tool " + QString(APP_VERSION)));
    my_ui = this;
    resizeTimer.setSingleShot(true);  // 设置为单次触发
    connect(&resizeTimer, &QTimer::timeout, this, &MainWindow::handleResize);
    connect(&ui_resize_timer, &QTimer::timeout, this, &MainWindow::ui_resize_slot);
    upgrade_class            = new upgrade(this);
    lua_class                = new lua(this);
    status_class             = new status(this);
    param_class              = new param(this);
    condition_view_class     = new condition_view(this);
    logic_tools_class        = new logic_tools(this);
    logic_view_class         = new logic_view(this, ui->frame_widget_logic);
    coroutine_lua_class      = new coroutine_lua(this);
    project_management_class = new project_management(this);
    project_report_class     = new project_report(this);
    mydevice_class           = new mydevice(this);
    project_debug_class      = new project_debug(this);
    about_prajna_class       = new about_prajna(this);
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

void MainWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    QSize newSize = event->size();
    tabbar_height = 110 * newSize.width() / UI_WIDTH;
    tabbar_height = 85 * newSize.height() / ui_HEIGHT;
    param_class->param_ui_resize(newSize.width(), newSize.height());
    status_class->status_ui_resize(newSize.width(), newSize.height());
    QSize iconSize(32 * newSize.width() / UI_WIDTH, 32 * newSize.width() / UI_WIDTH);
    ui->toolBar->setIconSize(iconSize);
    resizeTimer.start(50);  // 设置定时器的间隔时间，单位为毫秒
    QWidget::resizeEvent(event);
}

void MainWindow::handleResize()
{
    CustomTabStyle* style = new CustomTabStyle;
    style->tabbar_width   = tabbar_height;
    style->tabbar_height  = tabbar_height;
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
}

void MainWindow::user_authorization_passwd_window()
{
    QDialog dialog;
    dialog.setWindowTitle("用户授权");
    dialog.setFixedSize(450 * size().width() / UI_WIDTH, 200 * size().height() / ui_HEIGHT);
    dialog.setStyleSheet("QDialog { background-color: rgb(210,230,255); }");
    QFormLayout* layout = new QFormLayout(&dialog);
    QLineEdit    passwd_edit;
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
            lua_class->lua_cmd_report_response(frame, length);
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
    lua_class->lua_serial_connect_callback();
    status_class->status_serial_connect_callback();
    upgrade_class->upgrade_serial_connect_callback();
    ui->pushButton_read_version->setEnabled(true);
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
    lua_class->lua_serial_disconnect_callback();
    status_class->status_serial_disconnect_callback();
    upgrade_class->upgrade_serial_disconnect_callback();
    ui->pushButton_read_version->setEnabled(false);
    ui->actiona_transmit_todevice->setEnabled(false);
    ui->action_read_from_device->setEnabled(false);
    ui->action_change_device_passwd->setEnabled(false);
    ui->action_project_debug->setEnabled(false);
}

int MainWindow::my_message_box(QString title, QString text, bool add_cancel)
{
    static bool isshow_flag = false;
    if (!isshow_flag) {
        isshow_flag = true;
        QMessageBox box;
        box.setStyleSheet("QLabel{"
                          "min-width: 300px;"
                          "min-height: 300px; "
                          "font-size:20px;"
                          "}"
                          "QPushButton {"
                          "background-color:#89AFDE;"
                          " border-style: outset;"
                          " border-width: 10px;"
                          " border-radius: 20px;"
                          " border-color: beige;"
                          " font: bold 15px;"
                          " min-width: 15em;"
                          " min-height: 5em;"
                          "}"
                          "");
        box.setText(text);
        box.setWindowTitle(title);
        box.setWindowIcon(QIcon(":/new/photo/photo/logo.png"));
        box.setIconPixmap(QPixmap(":/new/photo/photo/logo.ico"));
        // box.setWindowFlags(box.windowFlags() & ~Qt::WindowCloseButtonHint);
        if (add_cancel) {
            box.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Close);
            box.button(QMessageBox::Close)->hide();
            box.setEscapeButton(QMessageBox::Close);
        } else {
            box.setStandardButtons(QMessageBox::Yes);
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
        upgrade_class->iap_info.status = IAP_DOWNLOAD_END;
        serial_search();
        my_message_box("串口警告", "串口异常断开，请检查串口状态！", false);
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
    bool isLeftButtonPressed = (QGuiApplication::mouseButtons() & Qt::LeftButton);
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
            if (screen_height != screenHeight || screen_width != screenWidth) {
                screen_width  = screenWidth;
                screen_height = screenHeight;
                resize(screen_width / DESKTOP_BASE_WIDTH * UI_WIDTH, screen_height / DESKTOP_BASE_HEIGHT * ui_HEIGHT);
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
    int width_ratio = this->size().width() / UI_WIDTH;
    serial_dialog.setFixedSize(300 * width_ratio, 200);  //设置框体大小
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
        my_message_box("操作失败", "未检测到可用端口", false);
        return;
    }
    int i   = serial_port_combobox.currentIndex();
    int ret = my_serial->open_port(my_serial->portname_list[i].portName(), baudrate, databits, parity, stopbits);
    if (ret != 0) {
        my_message_box("操作失败", "端口打开失败", false);
        return;
    }
    ui->action_serial_open->setIcon(QIcon(":/new/photo/photo/connect_offline.png"));
    serial_connect_button.setEnabled(false);
    serial_connect_callback();
    serial_dialog.close();
}

void MainWindow::on_lua_logclear_pushButton_clicked()
{
    ui->lua_log_textBrowser->clear();
}
void MainWindow::on_lua_logsave_pushButton_clicked()
{
    lua_class->lua_log_save();
}

void MainWindow::on_get_a_log_pushButton_clicked()
{
    lua_class->lua_cmd_log(SYNC_ID_A);
}

void MainWindow::on_get_b_log_pushButton_clicked()
{
    lua_class->lua_cmd_log(SYNC_ID_B);
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

void MainWindow::on_param_clear_pushButton_clicked()
{
    param_class->param_ui_clear();
}

void MainWindow::on_aslave_nodeid_spinbox_editingFinished()
{
    int arg1 = ui->aslave_nodeid_spinbox->value();
    if (arg1 == ui->bslave_nodeid_spinbox->value() || arg1 == ui->master_nodeid_spinbox->value()) {
        ui->aslave_nodeid_spinbox->setValue(param_class->a_slave_id);
    } else {
        param_class->a_slave_id = arg1;
    }
}

void MainWindow::on_bslave_nodeid_spinbox_editingFinished()
{
    int arg1 = ui->bslave_nodeid_spinbox->value();
    if (arg1 == ui->aslave_nodeid_spinbox->value() || arg1 == ui->master_nodeid_spinbox->value()) {
        ui->bslave_nodeid_spinbox->setValue(param_class->b_slave_id);
    } else {
        param_class->b_slave_id = arg1;
    }
}

void MainWindow::on_master_nodeid_spinbox_editingFinished()
{
    int arg1 = ui->master_nodeid_spinbox->value();
    if (arg1 == ui->bslave_nodeid_spinbox->value() || arg1 == ui->aslave_nodeid_spinbox->value()) {
        ui->master_nodeid_spinbox->setValue(param_class->master_id);
    } else {
        param_class->master_id = arg1;
    }
}
void MainWindow::on_check_bt_spinbox_editingFinished()
{
    if (ui->check_bt_spinbox->value() > 0 && ui->check_bt_spinbox->value() < 100) {
        ui->check_bt_spinbox->setValue(100);
    }
}

void MainWindow::on_send_bt_spinbox_editingFinished()
{
    if (ui->send_bt_spinbox->value() > 0 && ui->send_bt_spinbox->value() < 100) {
        ui->send_bt_spinbox->setValue(100);
    }
}

void MainWindow::on_pdo_pt_spinbox_editingFinished()
{
    if (ui->pdo_pt_spinbox->value() > 0 && ui->pdo_pt_spinbox->value() < 100) {
        ui->pdo_pt_spinbox->setValue(100);
    }
}

void MainWindow::on_param_sai_sample_interval_editingFinished()
{
    if (ui->param_sai_sample_interval->value() < 10) {
        ui->param_sai_sample_interval->setValue(10);
    }
}

void MainWindow::on_param_spi_sample_interval_editingFinished()
{
    if (ui->param_spi_sample_interval->value() < 10) {
        ui->param_spi_sample_interval->setValue(10);
    }
}

void MainWindow::on_param_pi_qep_sample_interval_editingFinished()
{
    if (ui->param_pi_qep_sample_interval->value() < 10) {
        ui->param_pi_qep_sample_interval->setValue(10);
    }
}

void MainWindow::on_param_sqep_sample_interval_editingFinished()
{
    if (ui->param_sqep_sample_interval->value() < 10) {
        ui->param_sqep_sample_interval->setValue(10);
    }
}
