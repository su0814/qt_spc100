#include "version_dialog.h"
#include "mainwindow.h"
#include "ui_version_dialog.h"

version_Dialog::version_Dialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::version_Dialog)
{
    ui->setupUi(this);
    mainwindow = ( MainWindow* )parent;
    connect(mainwindow->mydevice_class, &mydevice::device_line_status_change_signal, this,
            device_line_status_change_slots);
    version_read_wait_timer.setSingleShot(true);
    connect(&version_read_wait_timer, &QTimer::timeout, this, read_version_result_check_slot);
    connect(mainwindow->ui->action_device_version, &QAction::triggered, this, dialog_show_slot);
    mainwindow->ui->action_device_version->setEnabled(false);
}

version_Dialog::~version_Dialog()
{
    delete ui;
}

void version_Dialog::version_display(uint8_t* frame, int32_t length)
{
    module_info_t module_info;
    if ((uint32_t)(length - 10) < sizeof(module_info_t)) {
        return;
    }
    version_read_success = true;
    memcpy(&module_info, &frame[10], sizeof(module_info));
    ui->bootloader_version_lineEdit->setText(QString(module_info.bootloader_version));
    ui->app_version_lineEdit->setText(QString(module_info.firmware_version));
    ui->bottom_version_lineEdit->setText(QString(module_info.bottomboard_hardware_version));
    ui->core_version_lineEdit->setText(QString(module_info.coreboard_hardware_version));
}

/* user slots */
void version_Dialog::device_line_status_change_slots(device_line_status_e status)
{
    switch (status) {
    case DEVICE_LINE_STATUS_OFF:
        mainwindow->ui->action_device_version->setEnabled(false);
        break;
    case DEVICE_LINE_STATUS_ON:
        mainwindow->ui->action_device_version->setEnabled(true);

        break;
    }
}

void version_Dialog::dialog_show_slot()
{
    version_read_success = false;
    mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_READ, CMD_READ_STATUS, SUB_READ_STATUS_VERSION, NULL, 0);
    version_read_wait_timer.start(500);
    ui->bootloader_version_lineEdit->setText("读取中· · ·");
    ui->app_version_lineEdit->setText("读取中· · ·");
    ui->bottom_version_lineEdit->setText("读取中· · ·");
    ui->core_version_lineEdit->setText("读取中· · ·");
    exec();
}

void version_Dialog::read_version_result_check_slot()
{
    static uint8_t retry = 0;
    if (version_read_success) {
        retry = 0;
        return;
    } else {
        if (++retry <= 3) {
            mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_READ, CMD_READ_STATUS, SUB_READ_STATUS_VERSION, NULL,
                                                      0);
            version_read_wait_timer.start(500);
            return;
        }
        retry = 0;
    }
}
