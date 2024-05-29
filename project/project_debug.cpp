#include "project_debug.h"
#include "logic_view/logic_view.h"
#include "mainwindow.h"
#include <QDebug>
project_debug::project_debug(QWidget* mparent, QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )mparent;
    project_verify_timer.setSingleShot(true);
    connect(&project_verify_timer, &QTimer::timeout, this, project_verify_enter_slot);
    connect(&project_debug_timer, &QTimer::timeout, this, project_debug_slot);
    connect(ui->action_project_debug, &QAction::triggered, this, project_debug_action_slot);
    ui_init();
}

void project_debug::ui_init() {}

void project_debug::set_led(QLabel* label, QString rgb_color)
{
    uint8_t size = 25;
    // 将label中的文字清空
    label->setText("");
    QString min_width  = QString("min-width: %1px;").arg(size);   // 最小宽度：size
    QString min_height = QString("min-height: %1px;").arg(size);  // 最小高度：size
    QString max_width  = QString("max-width: %1px;").arg(size);   // 最大宽度：size
    QString max_height = QString("max-height: %1px;").arg(size);  // 最大高度：size
    // 再设置边界形状及边框
    QString border_radius = QString("border-radius: %1px;").arg(size / 2);  // 边框是圆角，半径为size/2
    QString border        = QString("border:1px solid black;");             // 边框为1px黑色
    // 最后设置背景颜色
    QString       background = "background-color:" + rgb_color;
    const QString SheetStyle = min_width + min_height + max_width + max_height + border_radius + border + background;
    label->setStyleSheet(SheetStyle);
    label->setAlignment(Qt::AlignCenter);
}

debug_state_e project_debug::get_debug_state()
{
    return debug_state;
}

bool project_debug::project_verify()
{
    project_verify_ack.ack_info[0].ack_code  = 0;
    project_verify_ack.ack_info[0].responsed = false;
    project_verify_ack.ack_info[1].ack_code  = 0;
    project_verify_ack.ack_info[1].responsed = false;
    project_verify_ack.retry                 = 0;
    project_verify_ack.ack_status            = ACK_STATUS_WAITING;
    mainwindow->project_management_class->project_verify_send_cmd();
    project_verify_timer.start(500);
    while (project_verify_ack.ack_status == ACK_STATUS_WAITING) {
        QApplication::processEvents();
    }
    if (project_verify_ack.ack_status == ACK_STATUS_SUCCESS) {
        return true;
    }
    return false;
}

void project_debug::project_debug_start()
{
    debug_state = DEBUG_STATE_ING;
    foreach (input_block* item, mainwindow->logic_view_class->input_block_list) {
        item->setEnabled(false);
    }
    foreach (output_block* item, mainwindow->logic_view_class->output_block_list) {
        item->setEnabled(false);
    }
    foreach (base_logic_block* item, mainwindow->logic_view_class->base_logic_block_list) {
        item->setEnabled(false);
    }
    foreach (apply_logic_block* item, mainwindow->logic_view_class->apply_logic_block_list) {
        item->setEnabled(false);
    }
    foreach (delay_counter_logic_block* item, mainwindow->logic_view_class->delay_counter_block_list) {
        item->setEnabled(false);
    }
    foreach (connection_line* item, mainwindow->logic_view_class->connection_line_list) {
        item->setEnabled(false);
    }
    ui->config_menu_groupBox->setEnabled(false);
    ui->logic_menu_groupBox->setEnabled(false);
    mainwindow->config_view_class->config_photo_svg->setEnabled(false);
    ui->action_new_project->setEnabled(false);
    ui->action_import_project->setEnabled(false);
    ui->action_save_project->setEnabled(false);
    ui->actiona_transmit_todevice->setEnabled(false);
    ui->action_read_from_device->setEnabled(false);
    ui->action_change_device_passwd->setEnabled(false);
    ui->action_project_debug->setIcon(QIcon(":/new/photo/photo/emuing.png"));
    ui->tabWidget_logic->setCurrentIndex(TAB_LOGIC_GRAPHICAL_CODE_ID);
    project_debug_timer.start(200);
}

void project_debug::project_debug_stop()
{
    project_debug_timer.stop();
    foreach (input_block* item, mainwindow->logic_view_class->input_block_list) {
        item->setEnabled(true);
        item->debug_data_set(0);
    }
    foreach (output_block* item, mainwindow->logic_view_class->output_block_list) {
        item->setEnabled(true);
        item->debug_data_set(0);
    }
    foreach (base_logic_block* item, mainwindow->logic_view_class->base_logic_block_list) {
        item->setEnabled(true);
        item->debug_data_set(0);
    }
    foreach (apply_logic_block* item, mainwindow->logic_view_class->apply_logic_block_list) {
        item->setEnabled(true);
        item->debug_data_set(0);
    }
    foreach (delay_counter_logic_block* item, mainwindow->logic_view_class->delay_counter_block_list) {
        item->setEnabled(true);
        item->debug_data_set(0);
    }
    foreach (connection_line* item, mainwindow->logic_view_class->connection_line_list) {
        item->setEnabled(true);
    }
    debug_state = DEBUG_STATE_IDLE;
    ui->config_menu_groupBox->setEnabled(true);
    ui->logic_menu_groupBox->setEnabled(true);
    mainwindow->config_view_class->config_photo_svg->setEnabled(true);
    ui->action_new_project->setEnabled(true);
    ui->action_import_project->setEnabled(true);
    if (mainwindow->serial_is_connect) {
        ui->action_save_project->setEnabled(true);
        ui->actiona_transmit_todevice->setEnabled(true);
        ui->action_read_from_device->setEnabled(true);
        ui->action_change_device_passwd->setEnabled(true);
    }
    ui->action_project_debug->setIcon(QIcon(":/new/photo/photo/emu.png"));
}

void project_debug::project_debug_data_cmd_prase(uint8_t* frame, int32_t length)
{
#define MAX_DEBUG_DATA_LEN (512)

    length              = length;
    uint16_t len        = frame[8] | frame[9] << 8;
    uint8_t  seq        = frame[10];
    int      start      = seq * (MAX_DEBUG_DATA_LEN - 1);
    int      data_count = 0;
    int      datanum    = len - 1;
    int      input_end  = mainwindow->logic_view_class->input_block_list.size();
    int      output_end = input_end + mainwindow->logic_view_class->output_block_list.size();
    int      base_end   = output_end + mainwindow->logic_view_class->base_logic_block_list.size();
    int      apply_end  = base_end + mainwindow->logic_view_class->apply_logic_block_list.size();
    int      dc_end     = apply_end + mainwindow->logic_view_class->delay_counter_block_list.size();
    int      speed_end  = dc_end + mainwindow->logic_view_class->speed_logic_block_list.size();

    while (start < input_end && data_count < datanum) {
        mainwindow->logic_view_class->input_block_list[start]->debug_data_set(frame[11 + data_count]);
        data_count++;
        start++;
    }
    while (start < output_end && data_count < datanum) {
        mainwindow->logic_view_class->output_block_list[start - dc_end]->debug_data_set(frame[11 + data_count]);
        data_count++;
        start++;
    }
    while (start < base_end && data_count < datanum) {
        mainwindow->logic_view_class->base_logic_block_list[start - input_end]->debug_data_set(frame[11 + data_count]);
        data_count++;
        start++;
    }
    while (start < apply_end && data_count < datanum) {
        mainwindow->logic_view_class->apply_logic_block_list[start - base_end]->debug_data_set(frame[11 + data_count]);
        data_count++;
        start++;
    }
    while (start < dc_end && data_count < datanum) {
        mainwindow->logic_view_class->delay_counter_block_list[start - apply_end]->debug_data_set(
            frame[11 + data_count]);
        data_count++;
        start++;
    }
    while (start < speed_end && data_count < datanum) {
        mainwindow->logic_view_class->speed_logic_block_list[start - apply_end]->debug_data_set(frame[11 + data_count]);
        data_count++;
        start++;
    }
}

void project_debug::project_debug_cmd_prase(uint8_t* frame, int32_t length)
{
    uint8_t cmd     = frame[6];
    uint8_t sub     = frame[7];
    uint8_t sync_id = frame[0];
    switch (cmd) {
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
        }
        break;
    case CMD_PROJECT_DEBUG:
        switch (sub) {
        case SUB_PROJECT_DEBUG_DATA_ACK:
            project_debug_data_cmd_prase(frame, length);
            break;
        }
        break;
    }
}

/* user slots */

void project_debug::project_debug_action_slot()
{
    if (debug_state == DEBUG_STATE_IDLE) {
        if (mainwindow->serial_is_connect == false) {
            mainwindow->my_message_box("请检查连接线束并查看端口是否打开！", MESSAGE_TYPE_WARNING);
            return;
        }
        if (mainwindow->user_permissions != USER_AUTHORIZED) {
            mainwindow->my_message_box("普通用户无权限,请授权后重试", MESSAGE_TYPE_WARNING);
            return;
        }
        if (mainwindow->mydevice_class->device_get_line_status() == DEVICE_LINE_STATUS_OFF) {
            mainwindow->my_message_box("请检查连接线束,设备当前未在线", MESSAGE_TYPE_WARNING);
            return;
        }
        if (mainwindow->logic_view_class->blocks_error_detect()) {
            return;
        }
        if (mainwindow->mydevice_class->device_pass_verify() == false) {
            return;
        }
        if (mainwindow->project_management_class->projec_info_creat() == false) {
            mainwindow->my_message_box("工程过大，请删减", MESSAGE_TYPE_ERROR);
            return;
        }
        if (project_verify() == false) {
            return;
        }
        ui->action_project_debug->setText("结束仿真");
        project_debug_start();
    } else {
        ui->action_project_debug->setText("开始仿真");
        project_debug_stop();
    }
}

void project_debug::project_debug_slot()
{
    if (mainwindow->serial_is_connect == false
        || mainwindow->mydevice_class->device_get_line_status() == DEVICE_LINE_STATUS_OFF) {
        project_debug_stop();
        mainwindow->my_message_box("与设备断开连接，仿真中断", MESSAGE_TYPE_ERROR);
        return;
    }
    mainwindow->my_serial->port_cmd_sendframe(0, CMD_TYPE_PROJECT, CMD_PROJECT_DEBUG, SUB_PROJECT_DEBUG_DATA, NULL, 0);
}

void project_debug::project_verify_enter_slot()
{
    if (project_verify_ack.ack_info[0].responsed == true && project_verify_ack.ack_info[1].responsed == true) {
        if (project_verify_ack.ack_info[0].ack_code == 0 || project_verify_ack.ack_info[1].ack_code == 0) {
            mainwindow->my_message_box("当前工程与设备工程不一致!", MESSAGE_TYPE_ERROR);
            project_verify_ack.ack_status = ACK_STATUS_FAIL;
            return;
        }
        project_verify_ack.ack_status = ACK_STATUS_SUCCESS;
    } else {
        if (project_verify_ack.retry++ <= 3) {
            mainwindow->project_management_class->project_verify_send_cmd();
            project_verify_timer.start(500);
            return;
        }
        mainwindow->my_message_box("设备无相关指令响应", MESSAGE_TYPE_ERROR);
        project_verify_ack.ack_status = ACK_STATUS_FAIL;
    }
}
