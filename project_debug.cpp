#include "project_debug.h"
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
    ui->emu_widget->setVisible(false);
    ui_init();
}

void project_debug::ui_init()
{
    output_led_label_list.append(ui->relaya1_led_label);
    output_led_label_list.append(ui->relaya2_led_label);
    output_led_label_list.append(ui->relayb1_led_label);
    output_led_label_list.append(ui->relayb2_led_label);
    output_led_label_list.append(ui->mos1_led_label);
    output_led_label_list.append(ui->mos2_led_label);
    output_led_label_list.append(ui->mos3_led_label);
    output_led_label_list.append(ui->mos4_led_label);
    defaultsf_label_list.append(ui->label_sf_sync);
    defaultsf_label_list.append(ui->label_sf_hardfault);
    defaultsf_label_list.append(ui->label_sf_cancmd);
    defaultsf_label_list.append(ui->label_sf_canheartbeat);
    defaultsf_label_list.append(ui->label_sf_init_selfcheck);
    defaultsf_label_list.append(ui->label_sf_run_selfcheck);
    for (int i = 0; i < output_led_label_list.size(); i++) {
        set_led(output_led_label_list[i], LED_GREY);
    }
}

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
    foreach (condition_block* item, mainwindow->logic_view_class->condition_block_list) {
        item->set_mode(BLOCK_MODE_DEBUG);
    }
    foreach (logic_block* item, mainwindow->logic_view_class->logic_block_list) {
        item->set_mode(BLOCK_MODE_DEBUG);
    }
    foreach (connect_line* item, mainwindow->logic_view_class->line_list) {
        item->set_mode(BLOCK_MODE_DEBUG);
    }
    debug_state = DEBUG_STATE_ING;
    ui->action_new_project->setEnabled(false);
    ui->action_import_project->setEnabled(false);
    ui->action_save_project->setEnabled(false);
    ui->actiona_transmit_todevice->setEnabled(false);
    ui->action_read_from_device->setEnabled(false);
    ui->action_change_device_passwd->setEnabled(false);
    ui->action_project_debug->setIcon(QIcon(":/new/photo/photo/emuing.png"));
    ui->tabWidget->setCurrentIndex(TAB_CENTER_LOGIC_ID);
    ui->tabWidget_logic->setCurrentIndex(TAB_LOGIC_GRAPHICAL_CODE_ID);
    ui->emu_widget->setVisible(true);
    ui->toolBox_tools->setEnabled(false);
    ui->label_error_a->clear();
    ui->label_error_b->clear();
    ui->label_error_a->setStyleSheet("background-color: transparent;");
    ui->label_error_b->setStyleSheet("background-color: transparent;");
    if (ui->start_read_status_pushButton->isEnabled()) {
        ui->start_read_status_pushButton->click();
    }
    project_debug_timer.start(200);
}

void project_debug::project_debug_stop()
{
    project_debug_timer.stop();
    foreach (condition_block* item, mainwindow->logic_view_class->condition_block_list) {
        item->set_mode(BLOCK_MODE_NORMAL);
    }
    foreach (logic_block* item, mainwindow->logic_view_class->logic_block_list) {
        item->set_mode(BLOCK_MODE_NORMAL);
    }
    foreach (connect_line* item, mainwindow->logic_view_class->line_list) {
        item->set_mode(BLOCK_MODE_NORMAL);
    }
    debug_state = DEBUG_STATE_IDLE;
    ui->action_new_project->setEnabled(true);
    ui->action_import_project->setEnabled(true);
    if (mainwindow->serial_is_connect) {
        ui->action_save_project->setEnabled(true);
        ui->actiona_transmit_todevice->setEnabled(true);
        ui->action_read_from_device->setEnabled(true);
        ui->action_change_device_passwd->setEnabled(true);
    }
    ui->action_project_debug->setIcon(QIcon(":/new/photo/photo/emu.png"));
    ui->emu_widget->setVisible(false);
    ui->toolBox_tools->setEnabled(true);
    if (ui->stop_read_status_pushButton->isEnabled()) {
        ui->stop_read_status_pushButton->click();
    }
}

void project_debug::project_debug_data_cmd_prase(uint8_t* frame, int32_t length)
{
#define SF_OUTPUT_BYTE_LEN (5)
#define NORMAL_CMD_LEN     (6)
#define MAX_DEBUG_DATA_LEN (128)
#define DEBUG_DATA_SEQ_LEN (1)
    uint16_t len = frame[4] | frame[5] << 8;
    if (len < (DEBUG_DATA_SEQ_LEN + SF_OUTPUT_BYTE_LEN + NORMAL_CMD_LEN)) {
        return;
    }

    uint16_t data_len = len - (DEBUG_DATA_SEQ_LEN + SF_OUTPUT_BYTE_LEN + NORMAL_CMD_LEN);
    uint8_t  seq      = frame[6];
    for (int i = 0; i < data_len; i++) {
        bool res = ( bool )frame[7 + i];
        int  id  = seq * MAX_DEBUG_DATA_LEN + i;
        if (id < mainwindow->logic_view_class->condition_block_list.size()) {
            mainwindow->logic_view_class->condition_block_list[id]->debug_data_set(res);
        } else if (id < (mainwindow->logic_view_class->condition_block_list.size()
                         + mainwindow->logic_view_class->logic_block_list.size())) {
            mainwindow->logic_view_class
                ->logic_block_list[id - mainwindow->logic_view_class->condition_block_list.size()]
                ->debug_data_set(res);
        }
    }
    uint32_t sf_en_flag =
        frame[7 + data_len] | frame[8 + data_len] << 8 | frame[9 + data_len] << 16 | frame[10 + data_len] << 24;
    uint8_t output_flag = frame[11 + data_len];
    for (int i = 0; i < defaultsf_label_list.size(); i++) {
        if ((sf_en_flag >> i) & (0x01)) {
            defaultsf_label_list[i]->setStyleSheet("QLabel  { background-color: rgb(0, 255, 0); }");
        } else {
            defaultsf_label_list[i]->setStyleSheet("QLabel  { background-color: transparent; }");
        }
    }
    for (int i = 0; i < output_led_label_list.size(); i++) {
        if ((output_flag >> i) & (0x01)) {
            set_led(output_led_label_list[i], LED_GREEN);
        } else {
            set_led(output_led_label_list[i], LED_GREY);
        }
    }
}

void project_debug::project_debug_cmd_prase(uint8_t* frame, int32_t length)
{
    uint8_t cmd     = frame[2];
    uint8_t sub     = frame[3];
    uint8_t sync_id = frame[0];
    switch (cmd) {
    case CMD_PROJECT_INFO:
        switch (sub) {
        case SUB_PROJECT_INFO_VERIFY_ACK:
            if (sync_id == SYNC_ID_A) {
                project_verify_ack.ack_info[0].responsed = true;
                project_verify_ack.ack_info[0].ack_code  = frame[6];
            } else if (sync_id == SYNC_ID_B) {
                project_verify_ack.ack_info[1].responsed = true;
                project_verify_ack.ack_info[1].ack_code  = frame[6];
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

void project_debug::project_error_code_flash()
{
    static bool toggle = false;
    if (ui->label_error_a->text().isEmpty()) {
        ui->label_error_a->setStyleSheet("background-color: transparent;");
    } else {
        if (toggle) {
            ui->label_error_a->setStyleSheet("background-color: transparent;");
        } else {
            ui->label_error_a->setStyleSheet("background-color: red;");
        }
    }

    if (ui->label_error_b->text().isEmpty()) {
        ui->label_error_b->setStyleSheet("background-color: transparent;");
    } else {
        if (toggle) {
            ui->label_error_b->setStyleSheet("background-color: transparent;");
        } else {
            ui->label_error_b->setStyleSheet("background-color: red;");
        }
    }
    toggle = !toggle;
}

/* user slots */

void project_debug::project_debug_action_slot()
{
    if (debug_state == DEBUG_STATE_IDLE) {
        if (mainwindow->serial_is_connect == false) {
            mainwindow->my_message_box("设备未连接", "请检查连接线束并查看端口是否打开", false);
            return;
        }
        if (mainwindow->user_permissions != USER_AUTHORIZED) {
            mainwindow->my_message_box("操作失败", "普通用户无权限,请授权后重试", false);
            return;
        }
        if (mainwindow->mydevice_class->device_get_line_status() == DEVICE_LINE_STATUS_OFF) {
            mainwindow->my_message_box("设备未连接", "请检查连接线束,设备当前未在线", false);
            return;
        }
        if (mainwindow->logic_view_class->blocks_error_detect()) {
            return;
        }
        if (mainwindow->mydevice_class->device_pass_verify() == false) {
            return;
        }
        if (mainwindow->project_management_class->projec_info_creat() == false) {
            mainwindow->my_message_box("传输失败", "工程过大，请删减", false);
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
        mainwindow->my_message_box("断开连接", "与设备断开连接，仿真中断", false);
        return;
    }
    uint8_t frame[6] = { 0, CMD_TYPE_PROJECT, CMD_PROJECT_DEBUG, SUB_PROJECT_DEBUG_DATA, 0, 0 };
    mainwindow->my_serial->port_sendframe(frame, 6);
    project_error_code_flash();
    if (ui->start_read_status_pushButton->isEnabled()) {
        ui->start_read_status_pushButton->click();
    }
}

void project_debug::project_verify_enter_slot()
{
    if (project_verify_ack.ack_info[0].responsed == true && project_verify_ack.ack_info[1].responsed == true) {
        if (project_verify_ack.ack_info[0].ack_code == 0 || project_verify_ack.ack_info[1].ack_code == 0) {
            mainwindow->my_message_box("工程验证", "当前工程与设备工程不一致!", false);
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
        mainwindow->my_message_box("工程验证", "设备无相关指令响应", false);
        project_verify_ack.ack_status = ACK_STATUS_FAIL;
    }
}
