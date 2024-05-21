#include "status.h"
#include "mainwindow.h"
#include "qdebug.h"
#include <QDir>
#include <QFile>
#include <QTextCodec>
status::status(QWidget* mparent, QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )mparent;
    connect(&read_state_timer, SIGNAL(timeout()), this, SLOT(read_status_from_time_slot()));
    read_state_timer.setTimerType(Qt::PreciseTimer);
}

void status::read_status_switch(bool en)
{
    if (en) {
        read_state_timer.start(100);
    } else {
        read_state_timer.stop();
    }
}

void status::a_baseinfo_display(uint8_t* frame, int32_t length)
{
    Q_UNUSED(length);
    module_state_t module_state;
    memcpy(( uint8_t* )&module_state, frame, sizeof(module_state));
    //    double v24v = module_state.voltage_24v * 2.5 / 4095 * (100 + 8.2) / 8.2 + 0.7;
    //    double v5v  = module_state.voltage_5v * 2.5 / 4095 * (49.9f + 39) / 39;
    //    double v33v = module_state.voltage_3v3 * 2.5 / 4095 * 2.0;
    //    double v24c =
    //        (module_state.current_24v > 2048 ? module_state.current_24v - 2048 : 2048 - module_state.current_24v) /
    //        164.0f;
    mainwindow->config_view_class->config_photo_svg->a_set_base_data(module_state);
}

void status::b_baseinfo_display(uint8_t* frame, int32_t length)
{
    Q_UNUSED(length);
    module_state_t module_state;
    memcpy(( uint8_t* )&module_state, frame, sizeof(module_state));
    mainwindow->config_view_class->config_photo_svg->b_set_base_data(module_state);
}

void status::faultinfo_display(uint8_t* frame, int32_t length)
{
    Q_UNUSED(length);
    int            id = frame[0];
    module_fault_t amodule_error;
    module_fault_t bmodule_error;
    if (id == SYNC_ID_A) {
        memcpy(( uint8_t* )&amodule_error, &frame[10], sizeof(module_fault_t));
        memcpy(( uint8_t* )&bmodule_error, &frame[sizeof(module_fault_t) + 10], sizeof(module_fault_t));
    } else {
        memcpy(( uint8_t* )&bmodule_error, &frame[10], sizeof(module_fault_t));
        memcpy(( uint8_t* )&amodule_error, &frame[sizeof(module_fault_t) + 10], sizeof(module_fault_t));
    }
    mainwindow->config_view_class->config_photo_svg->set_fault_info(amodule_error, bmodule_error);
}

void status::status_serial_disconnect_callback()
{
    read_status_switch(false);
}

void status::type_status_response(uint8_t* frame, int32_t length)
{
    uint8_t cmd     = frame[6];
    uint8_t sub     = frame[7];
    uint8_t sync_id = frame[0];

    switch (cmd) {
    case CMD_READ_STATUS:
        switch (sub) {
        case SUB_READ_REPLY_BASE:
            if (sync_id == SYNC_ID_A) {
                a_baseinfo_display(&frame[10], length);
                b_baseinfo_display(&frame[sizeof(module_state_t) + 10], length);
            } else {
                b_baseinfo_display(&frame[10], length);
                a_baseinfo_display(&frame[sizeof(module_state_t) + 10], length);
            }
            break;
        case SUB_READ_REPLY_ERROR:
            faultinfo_display(frame, length);
            break;
        case SUB_READ_REPLY_VERSION:
            mainwindow->version_dialog_class->version_display(frame, length);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/* USER SLOTS */
void status::read_status_from_time_slot()
{
    static uint8_t read_status = SUB_READ_STATUS_BASE;
    mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_READ, CMD_READ_STATUS, read_status, NULL, 0);
    switch (read_status) {
    case SUB_READ_STATUS_BASE:
        read_status = SUB_READ_STATUS_ERROR;
        break;
    case SUB_READ_STATUS_ERROR:
        read_status = SUB_READ_STATUS_BASE;
        break;
    }
}
