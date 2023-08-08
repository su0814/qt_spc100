#include "param.h"
#include "mainwindow.h"
#include <QDebug>
param::param(QWidget* parent)
    : QWidget(parent)
{
    ui = MainWindow::my_ui->ui;
    mainwindow = (MainWindow*)parent;
    param_ui_init();
}

void param::param_ui_init()
{
    slv_cb[SLV_DI1] = ui->di1_cat3_checkBox;
    slv_cb[SLV_DI2] = ui->di2_cat3_checkBox;
    slv_cb[SLV_DI3] = ui->di3_cat3_checkBox;
    slv_cb[SLV_DI4] = ui->di4_cat3_checkBox;
    slv_cb[SLV_DI5] = ui->di5_cat3_checkBox;
    slv_cb[SLV_DI6] = ui->di6_cat3_checkBox;
    slv_cb[SLV_DI7] = ui->di7_cat3_checkBox;
    slv_cb[SLV_DI8] = ui->di8_cat3_checkBox;
    slv_cb[SLV_RELAY1] = ui->relay1_cat3_checkBox;
    slv_cb[SLV_RELAY2] = ui->relay2_cat3_checkBox;
    slv_cb[SLV_AI1] = ui->ai1_cat3_checkBox;
    slv_cb[SLV_AI2] = ui->ai2_cat3_checkBox;

    ss_cb[SS_OUTPUT] = ui->param_ss_mode_checkBox;
    ss_cb[SS_RELAY1] = ui->param_ss_relay1_checkBox;
    ss_cb[SS_RELAY2] = ui->param_ss_relay2_checkBox;
    ss_cb[SS_SMOS1] = ui->param_ss_smos1_checkBox;
    ss_cb[SS_SMOS2] = ui->param_ss_smos2_checkBox;
    ss_cb[SS_SMOS3] = ui->param_ss_smos3_checkBox;
    ss_cb[SS_SMOS4] = ui->param_ss_smos4_checkBox;
}

void param::param_serial_disconnect_callback()
{
    ui->read_param_pushButton->setEnabled(false);
}

void param::param_serial_connect_callback()
{
    ui->read_param_pushButton->setEnabled(true);
}

void param::param_read_param()
{
    uint8_t cmd[6] = { 0, CMD_TYPE_READ, CMD_READ_PARAM, SUB_READ_PARAM_SS, 0X00, 0X00 };
    mainwindow->my_serial->port_sendframe(cmd, 6);
    cmd[3] = SUB_READ_PARAM_MODULE_INFO;
    mainwindow->my_serial->port_sendframe(cmd, 6);
}

void param::param_display(uint8_t* frame, int32_t length)
{
    QComboBox* pi_work[2] = { ui->param_af_pi1_comboBox, ui->param_af_pi2_comboBox };
    QComboBox* ai_work[2] = { ui->param_af_ai1_comboBox, ui->param_af_ai2_comboBox };
    module_param_t module_param;
    if (length - 6 < sizeof(module_param_t)) {
        return;
    }
    memcpy(&module_param, &frame[6], sizeof(module_param_t));
    uint32_t bit_val = module_param.di_slv.di_slv_bytes | module_param.relay_slv.relay_slv_byte << 8
        | module_param.ai_slv.ai_slv_byte << 10;
    for (int i = SLV_DI1; i < SLV_NUM; i++) {
        if (bit_val & (0x01 << i)) {
            slv_cb[i]->setChecked(true);
        } else {
            slv_cb[i]->setChecked(false);
        }
    }
    for (uint8_t i = 0; i < 2; i++) {
        pi_work[i]->setCurrentIndex(((module_param.work_state.work_state_byte >> (i * 2)) & 0x03));
    }
    for (uint8_t i = 0; i < 2; i++) {
        ai_work[i]->setCurrentIndex(((module_param.work_state.work_state_byte >> (i + 4)) & 0x01));
    }
    for (uint8_t i = SS_OUTPUT; i < SS_NUM; i++) {
        if (module_param.safe_state.safe_state_byte & (0x01 << i)) {
            ss_cb[i]->setChecked(true);
        } else {
            ss_cb[i]->setChecked(false);
        }
    }
    if (module_param.fault_code2_safe_state) {
        ui->param_faultcode_status_checkBox->setChecked(true);
    } else {
        ui->param_faultcode_status_checkBox->setChecked(false);
    }

    ui->param_faultcode_delay_spinbox->setValue(module_param.fault_code2_safe_state_delaytime);
    ui->param_sai_sample_interval->setValue(module_param.sai_sample_interval);
    ui->param_sai_fullscale->setValue(module_param.sai_allow_dif[0]);
    ui->param_sai_actualdata->setValue(module_param.sai_allow_dif[1]);

    ui->param_spi_sample_interval->setValue(module_param.spi_sample_interval);
    ui->param_spi_fullscale->setValue(module_param.spi_allow_dif[0]);
    ui->param_spi_actualdata->setValue(module_param.spi_allow_dif[1]);

    ui->param_pi_qep_sample_interval->setValue(module_param.pi_sqep_sample_interval);
    ui->param_pi_qep_fullscale->setValue(module_param.pi_qep_allow_dif[0]);
    ui->param_pi_qep_actualdata->setValue(module_param.pi_qep_allow_dif[1]);

    ui->param_sqep_sample_interval->setValue(module_param.sqep_sample_interval);
    ui->param_sqep_fullscale->setValue(module_param.sqep_allow_dif[0]);
    ui->param_sqep_actualdata->setValue(module_param.sqep_allow_dif[1]);

    ui->param_canbaudrate_comboBox->setCurrentText(QString::number(module_param.can_baudrate));
    ui->aslave_nodeid_spinbox->setValue(module_param.can_slave_nodeID_A);
    ui->bslave_nodeid_spinbox->setValue(module_param.can_slave_nodeID_B);
    ui->master_nodeid_spinbox->setValue(module_param.can_master_nodeID);
    ui->check_bt_spinbox->setValue(module_param.can_hb_consumer_gap);
    ui->send_bt_spinbox->setValue(module_param.can_hb_producer_gap);
    ui->pdo_pt_spinbox->setValue(module_param.can_pdo_time_gap);
    ui->param_cross_checktime->setValue(module_param.check_factor);
    ui->lua_version_lineEdit->setText(module_param.lua_file_ver);
}

void param::info_display(uint8_t* frame, int32_t length)
{
    module_info_t module_info;
    if (length - 6 < sizeof(module_info_t)) {
        return;
    }
    memcpy(&module_info, &frame[6], sizeof(module_info));
    ui->bootloader_version_lineEdit->setText(module_info.bootloader_version);
    ui->app_version_lineEdit->setText(module_info.firmware_version);
    ui->bottom_version_lineEdit->setText(module_info.bottomboard_hardware_version);
    ui->core_version_lineEdit->setText(module_info.coreboard_hardware_version);
}

void param::param_cmd_callback(uint8_t* frame, int32_t length)
{
    uint8_t sub = frame[3];
    switch (sub) {
    case SUB_REPORT_PARAM_SS:
        param_display(frame, length);
        break;
    case SUB_READ_PARAM_MODULE_INFO:
        info_display(frame, length);
        break;
    default:
        break;
    }
}

/**
 * @brief 自适应UI大小
 * @param 当前界面宽度
 * @param 当前界面高度
 */
void param::param_ui_resize(uint32_t width, uint32_t height)
{
    uint32_t ctrl_height = 20 * height / ui_HEIGHT;
    uint32_t sounbox_photo_height = 30 * height / ui_HEIGHT;
    uint32_t sounbox_photo_width = 30 * width / UI_WIDTH;
    uint32_t font_size = 14 * height / ui_HEIGHT;
    ui->tab_param->setStyleSheet(
        // QGroupBox StyleSheet
        "QGroupBox{border: 2px solid gray;border-radius:10px;margin-top:2ex;font-family:微软雅黑;font:bold "
        + QString::number(font_size)
        + "px;}"
          "QGroupBox::title{subcontrol-origin:margin;subcontrol-position:top center;padding:1px;}"
          // QSpinBox StyleSheet
          "QSpinBox { height: "
        + QString::number(ctrl_height)
        + "px; }"
          "QSpinBox { background-color: #f2f2f2; border-radius: 5px; border: 1px solid #e0e0e0; padding: 5px; color: "
          "#333333; font-size: "
        + QString::number(font_size)
        + "px;text-align: center;border-width: 2px;border-style: solid;border-color: "
          "rgb(200,200,255);border-top-left-radius: 12px;border-bottom-left-radius: 12px;border-top-right-radius: "
          "12px;border-bottom-right-radius: 12px;  }"
          "QSpinBox::up-arrow{ border: none; width:"
        + QString::number(sounbox_photo_width) + "px; height: " + QString::number(sounbox_photo_height)
        + "px; image: url(:/new/photo/photo/add.png); }"
          "QSpinBox::down-arrow { border: none; width: "
        + QString::number(sounbox_photo_width) + "px; height: " + QString::number(sounbox_photo_height)
        + "px; image: "
          "url(:/new/photo/photo/minus.png); }"
          "QSpinBox::up-button{ subcontrol-origin: padding; subcontrol-position: "
          "center right; border: none; width: "
          "15px; background-color: transparent; }"
          "QSpinBox::down-button { subcontrol-origin: padding; "
          "subcontrol-position: center left; border: none; width: "
          "15px; background-color: transparent; }"
          "QSpinBox::up-button:hover, QSpinBox::down-button:hover { "
          "background-color: #e0e0e0; }"
          // QComboBox StyleSheet
          "QComboBox {font-family:  Microsoft YaHei"
          ";font-size:"
        + QString::number(font_size) + "px;height: " + QString::number(ctrl_height)
        + "px;color: #000000;font-style: italic;font-weight: bold;padding-left: 10px;border-width: "
          "2px;border-style: solid;border-color: green;border-top-left-radius: "
        + QString::number(ctrl_height / 2) + "px;border-bottom-left-radius: " + QString::number(ctrl_height / 2)
        + "px;}"
          // QLineEdit StyleSheet
          " QLineEdit{color:rgb(0,0,0);background-color:rgb(200,200,150);selection-color:white;selection-background-"
          "color:rgb(191,31,127);border-radius:"
        + QString::number(ctrl_height / 2 - 1) + "px; padding:2px 4px;font-size:" + QString::number(font_size)
        + "px;height: " + QString::number(ctrl_height - 1) + "px;}");

    // QCheckBox StyleSheet
    uint32_t ss_level_checkbox_size = 50 * height / ui_HEIGHT;
    ui->ss_level_groupBox->setStyleSheet(
        "QCheckBox{spacing: 5px;color: green;}"
        "QCheckBox::indicator{width: "
        + QString::number(ss_level_checkbox_size) + "px;height: " + QString::number(ss_level_checkbox_size)
        + "px;}"
          "QCheckBox::indicator:enabled:unchecked {image: url(:/new/photo/photo/cat2.png);}"
          "QCheckBox::indicator:enabled:checked {image: url(:/new/photo/photo/cat3.png);}");
    uint32_t ss_checkbox_unchecked_size = 40 * height / ui_HEIGHT;
    uint32_t ss_checkbox_checked_width = 80 * height / ui_HEIGHT;
    uint32_t ss_checkbox_checked_height = 50 * height / ui_HEIGHT;
    ui->ss_groupBox->setStyleSheet(
        "QCheckBox::indicator:unecked {width: " + QString::number(ss_checkbox_unchecked_size)
        + "px;height: " + QString::number(ss_checkbox_unchecked_size)
        + "px;}"
          "QCheckBox::indicator:enabled:unchecked {image: url(:/new/photo/photo/not.png);}"
          "QCheckBox::indicator:checked {width: "
        + QString::number(ss_checkbox_checked_width) + "px;height: " + QString::number(ss_checkbox_checked_height)
        + "px;}"
          "QCheckBox::indicator:enabled:checked {image: url(:/new/photo/photo/relevant.png);}");
    ui->param_ss_mode_checkBox->setStyleSheet(
        "QCheckBox::indicator:unecked {width: " + QString::number(ss_checkbox_checked_height)
        + "px;height: " + QString::number(ss_checkbox_checked_height)
        + "px;}"
          "QCheckBox::indicator:enabled:unchecked {image: url(:/new/photo/photo/close.png);}"
          "QCheckBox::indicator:checked {width: "
        + QString::number(ss_checkbox_checked_height) + "px;height: " + QString::number(ss_checkbox_checked_height)
        + "px;}"
          "QCheckBox::indicator:enabled:checked {image: url(:/new/photo/photo/open.png);}");
    ui->param_faultcode_status_checkBox->setStyleSheet(
        "QCheckBox::indicator:unecked {width: " + QString::number(ss_checkbox_checked_width)
        + "px;height: " + QString::number(ss_checkbox_unchecked_size)
        + "px;}"
          "QCheckBox::indicator:enabled:unchecked {image: url(:/new/photo/photo/disable.png);}"
          "QCheckBox::indicator:checked {width: "
        + QString::number(ss_checkbox_checked_width) + "px;height: " + QString::number(ss_checkbox_unchecked_size)
        + "px;}"
          "QCheckBox::indicator:enabled:checked {image: url(:/new/photo/photo/enable.png);}");
}
