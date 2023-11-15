#include "param.h"
#include "mainwindow.h"
#include "md5.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
param::param(QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )parent;
    param_ui_init();
}

void param::param_ui_init()
{
    param_write_wait_timer = new QTimer(this);
    param_write_wait_timer->setSingleShot(true);  // 设置为单次触发
    connect(param_write_wait_timer, &QTimer::timeout, this, &param_write_enter_slot);
    param_read_wait_timer = new QTimer(this);
    param_read_wait_timer->setSingleShot(true);  // 设置为单次触发
    connect(param_read_wait_timer, &QTimer::timeout, this, &param_read_enter_slot);
    slv_cb[SLV_DI1]    = ui->di1_cat3_checkBox;
    slv_cb[SLV_DI2]    = ui->di2_cat3_checkBox;
    slv_cb[SLV_DI3]    = ui->di3_cat3_checkBox;
    slv_cb[SLV_DI4]    = ui->di4_cat3_checkBox;
    slv_cb[SLV_DI5]    = ui->di5_cat3_checkBox;
    slv_cb[SLV_DI6]    = ui->di6_cat3_checkBox;
    slv_cb[SLV_DI7]    = ui->di7_cat3_checkBox;
    slv_cb[SLV_DI8]    = ui->di8_cat3_checkBox;
    slv_cb[SLV_RELAY1] = ui->relay1_cat3_checkBox;
    slv_cb[SLV_RELAY2] = ui->relay2_cat3_checkBox;
    slv_cb[SLV_AI1]    = ui->ai1_cat3_checkBox;
    slv_cb[SLV_AI2]    = ui->ai2_cat3_checkBox;

    ss_cb[SS_OUTPUT] = ui->param_ss_mode_checkBox;
    ss_cb[SS_RELAY1] = ui->param_ss_relay1_checkBox;
    ss_cb[SS_RELAY2] = ui->param_ss_relay2_checkBox;
    ss_cb[SS_SMOS1]  = ui->param_ss_smos1_checkBox;
    ss_cb[SS_SMOS2]  = ui->param_ss_smos2_checkBox;
    ss_cb[SS_SMOS3]  = ui->param_ss_smos3_checkBox;
    ss_cb[SS_SMOS4]  = ui->param_ss_smos4_checkBox;
    param_ui_clear();
}

void param::param_serial_disconnect_callback()
{
    ui->read_param_pushButton->setEnabled(false);
    ui->write_param_pushButton->setEnabled(false);
}

void param::param_serial_connect_callback()
{
    ui->read_param_pushButton->setEnabled(true);
    ui->write_param_pushButton->setEnabled(true);
}

void param::param_read_param()
{
    if (param_read_status[0] == PARAM_WR_STATUS_WAIT || param_read_status[1] == PARAM_WR_STATUS_WAIT) {
        return;
    }
    ui->param_log_lineEdit->setText("读取中......");
    ui->param_log_lineEdit->setStyleSheet("color: rgb(0, 200, 0);");
    param_read_status[0] = PARAM_WR_STATUS_WAIT;
    param_read_status[1] = PARAM_WR_STATUS_WAIT;
    param_read_wait_timer->start(500);
    uint8_t cmd[6] = { 0, CMD_TYPE_READ, CMD_READ_PARAM, SUB_READ_PARAM_SS, 0X00, 0X00 };
    mainwindow->my_serial->port_sendframe(cmd, 6);
    cmd[3] = SUB_READ_PARAM_MODULE_INFO;
    mainwindow->my_serial->port_sendframe(cmd, 6);
}

void param::param_read_enter_slot()
{
    static uint8_t retry = 0;

    if (param_read_status[0] == PARAM_WR_STATUS_SUCCESS && param_read_status[1] == PARAM_WR_STATUS_SUCCESS) {
        ui->param_log_lineEdit->setText("读取成功");
        ui->param_log_lineEdit->setStyleSheet("color: rgb(0, 200, 0);");
        retry = 0;
        return;
    } else {
        if (++retry <= 3) {
            param_read_status[0] = PARAM_WR_STATUS_FAIL;
            param_read_status[1] = PARAM_WR_STATUS_FAIL;
            param_read_param();
            return;
        }
        retry = 0;
        if (param_read_status[0] != PARAM_WR_STATUS_SUCCESS) {
            param_read_status[0] = PARAM_WR_STATUS_FAIL;
            param_read_status[1] = PARAM_WR_STATUS_FAIL;
            ui->param_log_lineEdit->setText("参数读取失败");
            ui->param_log_lineEdit->setStyleSheet("color: rgb(200, 0, 0);");
            return;
        } else {
            param_read_status[1] = PARAM_WR_STATUS_FAIL;
            param_read_status[0] = PARAM_WR_STATUS_FAIL;
            ui->param_log_lineEdit->setText("版本读取失败");
            ui->param_log_lineEdit->setStyleSheet("color: rgb(200, 0, 0);");
            return;
        }
    }
}

void param::param_display(module_param_t* param)
{
    QComboBox*     pi_work[2]   = { ui->param_af_pi1_comboBox, ui->param_af_pi2_comboBox };
    QComboBox*     ai_work[2]   = { ui->param_af_ai1_comboBox, ui->param_af_ai2_comboBox };
    module_param_t module_param = *param;
    uint32_t       bit_val      = module_param.di_slv.di_slv_bytes | module_param.relay_slv.relay_slv_byte << 8
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
            if (i == SS_OUTPUT) {
                ss_cb[i]->setChecked(false);
            } else {
                ss_cb[i]->setChecked(true);
            }

        } else {
            if (i == SS_OUTPUT) {
                ss_cb[i]->setChecked(true);
            } else {
                ss_cb[i]->setChecked(false);
            }
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
    ui->lua_version_lineEdit->setText(("Lua:" + QString(module_param.lua_file_ver)));
}

void param::info_display(uint8_t* frame, int32_t length)
{
    module_info_t module_info;
    if (length - 6 < sizeof(module_info_t)) {
        return;
    }
    memcpy(&module_info, &frame[6], sizeof(module_info));
    ui->bootloader_version_lineEdit->setText("Boot:" + QString(module_info.bootloader_version));
    ui->app_version_lineEdit->setText("App:" + QString(module_info.firmware_version));
    ui->bottom_version_lineEdit->setText("Bottom:" + QString(module_info.bottomboard_hardware_version));
    ui->core_version_lineEdit->setText("Core:" + QString(module_info.coreboard_hardware_version));
}

void param::param_ui_to_data(module_param_t* param)
{
    param->can_baudrate                     = ui->param_canbaudrate_comboBox->currentText().toInt();
    param->can_slave_nodeID_A               = ui->aslave_nodeid_spinbox->value();
    param->can_slave_nodeID_B               = ui->bslave_nodeid_spinbox->value();
    param->can_master_nodeID                = ui->master_nodeid_spinbox->value();
    param->can_hb_consumer_gap              = ui->check_bt_spinbox->value();
    param->can_hb_producer_gap              = ui->send_bt_spinbox->value();
    param->can_pdo_time_gap                 = ui->pdo_pt_spinbox->value();
    param->sai_sample_interval              = ui->param_sai_sample_interval->value();
    param->spi_sample_interval              = ui->param_spi_sample_interval->value();
    param->pi_sqep_sample_interval          = ui->param_pi_qep_sample_interval->value();
    param->sqep_sample_interval             = ui->param_sqep_sample_interval->value();
    param->fault_code2_safe_state           = ui->param_faultcode_status_checkBox->isChecked() ? 1 : 0;
    param->fault_code2_safe_state_delaytime = ui->param_faultcode_delay_spinbox->value();
    param->sai_allow_dif[0]                 = ui->param_sai_fullscale->value();
    param->sai_allow_dif[1]                 = ui->param_sai_actualdata->value();
    param->spi_allow_dif[0]                 = ui->param_spi_fullscale->value();
    param->spi_allow_dif[1]                 = ui->param_spi_actualdata->value();
    param->pi_qep_allow_dif[0]              = ui->param_pi_qep_fullscale->value();
    param->pi_qep_allow_dif[1]              = ui->param_pi_qep_actualdata->value();
    param->sqep_allow_dif[0]                = ui->param_sqep_fullscale->value();
    param->sqep_allow_dif[1]                = ui->param_sqep_actualdata->value();
    param->check_factor                     = ui->param_cross_checktime->value();
    for (int i = SLV_DI1; i <= SLV_DI8; i++) {
        if (slv_cb[i]->isChecked()) {
            param->di_slv.di_slv_bytes |= (0x01 << i);
        }
    }
    for (int i = SLV_RELAY1; i <= SLV_RELAY2; i++) {
        if (slv_cb[i]->isChecked()) {
            param->relay_slv.relay_slv_byte |= (0x01 << (i - SLV_RELAY1));
        }
    }
    for (int i = SLV_AI1; i <= SLV_AI2; i++) {
        if (slv_cb[i]->isChecked()) {
            param->ai_slv.ai_slv_byte |= (0x01 << (i - SLV_AI1));
        }
    }
    param->work_state.work_state_bit.pi1_work_state_bit = ui->param_af_pi1_comboBox->currentIndex();
    param->work_state.work_state_bit.pi2_work_state_bit = ui->param_af_pi2_comboBox->currentIndex();
    param->work_state.work_state_bit.ai1_work_state_bit = ui->param_af_ai1_comboBox->currentIndex();
    param->work_state.work_state_bit.ai2_work_state_bit = ui->param_af_ai2_comboBox->currentIndex();
    for (uint8_t i = SS_OUTPUT; i < SS_NUM; i++) {
        if (ss_cb[i]->isChecked()) {
            if (i != SS_OUTPUT) {
                param->safe_state.safe_state_byte |= (0x01 << i);
            }
        } else {
            if (i == SS_OUTPUT) {
                param->safe_state.safe_state_byte |= (0x01 << i);
            }
        }
    }
    mbedtls_md5(( const unsigned char* )param, sizeof(module_param_t) - sizeof(param->md5), param->md5);
}

void param::param_write()
{
    if (mainwindow->user_permissions != USER_AUTHORIZED) {
        mainwindow->my_message_box("操作失败", "普通用户无参数写入权限,请授权后重试", false);
        return;
    }

    // 根据用户点击的按钮进行相应的处理
    if (mainwindow->my_message_box("参数写入", "参数成功写入后将重启SPC100，是否继续写入？", true)
        == QMessageBox::Cancel) {
        ui->param_log_lineEdit->setText("取消写入参数");
        return;
    }
    if (param_write_status == PARAM_WR_STATUS_WAIT) {
        return;
    }
    module_param_t module_param;
    memset(( uint8_t* )&module_param, 0, sizeof(module_param));
    param_ui_to_data(&module_param);
    ui->param_log_lineEdit->setText("参数写入中......");
    ui->param_log_lineEdit->setStyleSheet("color: rgb(0, 0, 0);");
    param_write_status  = PARAM_WR_STATUS_WAIT;
    param_write_flag[0] = PARAM_WR_STATUS_WAIT;
    param_write_flag[1] = PARAM_WR_STATUS_WAIT;
    uint8_t cmd[128]    = { 0, CMD_TYPE_WRITE, CMD_WRITE_PARAM, SUB_WRITE_PARAM_SAFE, 0X00, 0X00 };
    cmd[4]              = sizeof(module_param);
    cmd[5]              = sizeof(module_param) >> 8;
    memcpy(&cmd[6], ( uint8_t* )&module_param, sizeof(module_param));
    mainwindow->my_serial->port_sendframe(cmd, sizeof(module_param) + 6);
    param_write_wait_timer->start(1000);
}
void param::param_write_enter_slot()
{
    static uint8_t retry = 0;
    if (param_write_status == PARAM_WR_STATUS_WAIT) {
        if ((param_write_flag[0] == PARAM_WR_STATUS_SUCCESS) && (param_write_flag[1] == PARAM_WR_STATUS_SUCCESS)) {
            ui->param_log_lineEdit->setText("参数写入成功");
            ui->param_log_lineEdit->setStyleSheet("color: rgb(0, 200, 0);");
            param_write_status   = PARAM_WR_STATUS_SUCCESS;
            unsigned char cmd[6] = { 0x00, 0x20, 0x21, 0x00, 0x00, 0x00 };
            mainwindow->my_serial->port_sendframe(cmd, 6);
        } else {
            if (++retry <= 3) {
                param_write_status = PARAM_WR_STATUS_FAIL;
                param_write();
            }
            if (param_write_flag[0] != PARAM_WR_STATUS_SUCCESS && param_write_flag[1] != PARAM_WR_STATUS_SUCCESS) {
                ui->param_log_lineEdit->setText("参数写入失败");
                ui->param_log_lineEdit->setStyleSheet("color: rgb(200, 0, 0);");
            } else if (param_write_flag[0] != PARAM_WR_STATUS_SUCCESS) {
                ui->param_log_lineEdit->setText("MCUA 参数写入失败");
                ui->param_log_lineEdit->setStyleSheet("color: rgb(200, 0, 0);");
            } else {
                ui->param_log_lineEdit->setText("MCUB 参数写入失败");
                ui->param_log_lineEdit->setStyleSheet("color: rgb(200, 0, 0);");
            }
            param_write_status = PARAM_WR_STATUS_FAIL;
        }
    }
}

void param::param_ui_clear()
{
    module_param_t default_param;
    /*config can master node id, read from fram or use default*/
    default_param.can_master_nodeID   = ( uint8_t )0x01;
    default_param.can_slave_nodeID_A  = ( uint8_t )0x51;
    default_param.can_slave_nodeID_B  = ( uint8_t )0x52;
    default_param.can_hb_consumer_gap = 0;
    default_param.can_hb_producer_gap = 1000;
    /*config safe level, default level 3(1), level 2(0)*/
    /*config ai work state, default work as di(1), work as ai(0)*/
    /*config pi work state, default work as encoder(1), work as counter(0)*/
    default_param.ai_slv.ai_slv_byte               = 0x03;
    default_param.di_slv.di_slv_bytes              = 0xff;
    default_param.relay_slv.relay_slv_byte         = 0x03;
    default_param.work_state.work_state_byte       = 0x35;
    default_param.safe_state.safe_state_byte       = 0X7E;
    default_param.sai_allow_dif[0]                 = 1;
    default_param.sai_allow_dif[1]                 = 3;
    default_param.spi_allow_dif[0]                 = 1;
    default_param.spi_allow_dif[1]                 = 3;
    default_param.pi_qep_allow_dif[0]              = 1;
    default_param.pi_qep_allow_dif[1]              = 3;
    default_param.sqep_allow_dif[0]                = 1;
    default_param.sqep_allow_dif[1]                = 3;
    default_param.sai_sample_interval              = 10;
    default_param.spi_sample_interval              = 10;
    default_param.pi_sqep_sample_interval          = 10;
    default_param.sqep_sample_interval             = 10;
    default_param.fault_code2_safe_state           = 0;
    default_param.fault_code2_safe_state_delaytime = 0;
    /*lua software version*/
    strcpy(default_param.lua_file_ver, "na");
    default_param.check_factor     = 1;
    default_param.can_baudrate     = 1000000;
    default_param.can_pdo_time_gap = 1000;
    param_display(&default_param);
    ui->bootloader_version_lineEdit->setText("Boot:na");
    ui->app_version_lineEdit->setText("App:na");
    ui->bottom_version_lineEdit->setText("Bottom:na");
    ui->core_version_lineEdit->setText("Core:na");
}

void param::param_save()
{
    QString curPath = QDir::currentPath();  //获取系统当前目录
    // qDebug() << curPath;
    curPath += "/spc100.param";
    QFile file(curPath);
    if (file.exists()) {
        file.remove();
    }
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        module_param_t module_param;
        memset(( uint8_t* )&module_param, 0, sizeof(module_param));
        param_ui_to_data(&module_param);

        file.write(( char* )&module_param, sizeof(module_param));
        file.close();
        ui->param_log_lineEdit->setText("参数保存成功！");
        ui->param_log_lineEdit->setStyleSheet("color: rgb(0, 200, 0);");
    } else {
        ui->param_log_lineEdit->setText("文件创建或打开失败！");
        ui->param_log_lineEdit->setStyleSheet("color: rgb(200, 0, 0);");
    }
}

void param::param_read_load()
{
    QString curPath = QDir::currentPath();  //获取系统当前目录
    // qDebug() << curPath;
    curPath += "/spc100.param";
    QFile file(curPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        module_param_t module_param;
        memset(( uint8_t* )&module_param, 0, sizeof(module_param));
        uint64_t len = file.read(( char* )&module_param, sizeof(module_param));
        if (len < sizeof(module_param)) {
            ui->param_log_lineEdit->setText("本地参数文件错误！");
            ui->param_log_lineEdit->setStyleSheet("color: rgb(200, 0, 0);");
            return;
        } else {
            unsigned char temp_md5[16];
            mbedtls_md5(( const unsigned char* )&module_param, sizeof(module_param_t) - sizeof(module_param.md5),
                        temp_md5);
            if (memcmp(temp_md5, module_param.md5, 16)) {
                ui->param_log_lineEdit->setText("本地参数校验错误！");
                ui->param_log_lineEdit->setStyleSheet("color: rgb(200, 0, 0);");
                return;
            }
        }
        param_display(&module_param);
        file.close();
        ui->param_log_lineEdit->setText("本地参数读取成功！");
        ui->param_log_lineEdit->setStyleSheet("color: rgb(0, 200, 0);");
    } else {
        ui->param_log_lineEdit->setText("本地参数文件打开失败！");
        ui->param_log_lineEdit->setStyleSheet("color: rgb(200, 0, 0);");
    }
}

void param::param_cmd_callback(uint8_t* frame, int32_t length)
{
    uint8_t sub = frame[3];
    uint8_t id  = frame[0];
    switch (sub) {
    case SUB_REPORT_PARAM_SS: {
        module_param_t module_param;
        if (length - 6 < sizeof(module_param_t)) {
            return;
        }
        memcpy(&module_param, &frame[6], sizeof(module_param_t));
        param_read_status[0] = PARAM_WR_STATUS_SUCCESS;
        param_display(&module_param);
    } break;
    case SUB_READ_PARAM_MODULE_INFO:
        info_display(frame, length);
        param_read_status[1] = PARAM_WR_STATUS_SUCCESS;
        break;
    case SUB_REPORT_PARAM_WRITE_ACK:
        if (id == SYNC_ID_A) {
            param_write_flag[0] = PARAM_WR_STATUS_SUCCESS;
        } else if (id == SYNC_ID_B) {
            param_write_flag[1] = PARAM_WR_STATUS_SUCCESS;
        }
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
    uint32_t ctrl_height          = 20 * height / ui_HEIGHT;
    uint32_t sounbox_photo_height = 30 * height / ui_HEIGHT;
    uint32_t sounbox_photo_width  = 30 * width / UI_WIDTH;
    uint32_t font_size            = 14 * height / ui_HEIGHT;
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
    uint32_t ss_checkbox_checked_width  = 80 * height / ui_HEIGHT;
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
