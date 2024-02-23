#include "param.h"
#include "mainwindow.h"
#include "md5.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QJsonDocument>
#include <QMessageBox>
param::param(QWidget* mparent, QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )mparent;
    param_ui_init();
}

void param::param_ui_init()
{
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
    ui->param_ss_mode_checkBox->setEnabled(false);
    ui->param_ss_mode_checkBox->setVisible(false);
    ui->label_79->setEnabled(false);
    ui->label_79->setVisible(false);
    for (int i = 0; i < 6; i++) {
        connect(ss_cb[i + 1], &QCheckBox::stateChanged, this, ss_state_changed_slot);
    }
    connect(ui->param_af_pi1_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pi1_afstate_changed_slot(int)));
    connect(ui->param_af_pi2_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pi2_afstate_changed_slot(int)));
    connect(ui->param_af_ai1_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ai1_afstate_changed_slot(int)));
    connect(ui->param_af_ai2_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ai2_afstate_changed_slot(int)));
}

void param::param_ss_set(int id, int state)
{
    if (id >= 6) {
        return;
    }
    if (state == SS_RELEVANT) {
        if (ss_cb[id + 1]->isChecked() == false) {
            ss_cb[id + 1]->setChecked(true);
        }

    } else {
        if (ss_cb[id + 1]->isChecked()) {
            ss_cb[id + 1]->setChecked(false);
        }
    }
}

uint8_t param::param_ss_get()
{
    uint8_t res = 0;
    for (int i = 1; i < 7; i++) {
        if (ss_cb[i]->isChecked()) {
            res |= ((0x01) << (i - 1));
        }
    }
    return res;
}

QJsonObject param::param_project_info()
{
    param_ui_to_data(&module_param);
    QJsonObject rootObject;
    rootObject["baudrate"]   = ( int )module_param.can_baudrate;
    rootObject["anodeid"]    = module_param.can_slave_nodeID_A;
    rootObject["bnodeid"]    = module_param.can_slave_nodeID_B;
    rootObject["mnodeid"]    = module_param.can_master_nodeID;
    rootObject["checkbt"]    = module_param.can_hb_consumer_gap;
    rootObject["sendbt"]     = module_param.can_hb_producer_gap;
    rootObject["pdopt"]      = module_param.can_pdo_time_gap;
    rootObject["saisi"]      = module_param.sai_sample_interval;
    rootObject["spisi"]      = module_param.spi_sample_interval;
    rootObject["piqepsi"]    = module_param.pi_sqep_sample_interval;
    rootObject["sqepsi"]     = module_param.sqep_sample_interval;
    rootObject["fcsta"]      = module_param.fault_code2_safe_state;
    rootObject["fcdt"]       = module_param.fault_code2_safe_state_delaytime;
    rootObject["saifull"]    = module_param.sai_allow_dif[0];
    rootObject["saiac"]      = module_param.sai_allow_dif[1];
    rootObject["spifull"]    = module_param.spi_allow_dif[0];
    rootObject["spiac"]      = module_param.spi_allow_dif[1];
    rootObject["piqepfull"]  = module_param.pi_qep_allow_dif[0];
    rootObject["piqepac"]    = module_param.pi_qep_allow_dif[1];
    rootObject["sqepfull"]   = module_param.sqep_allow_dif[0];
    rootObject["sqepac"]     = module_param.sqep_allow_dif[1];
    rootObject["crossc"]     = module_param.check_factor;
    rootObject["dilevel"]    = module_param.di_slv.di_slv_bytes;
    rootObject["ailevel"]    = module_param.ai_slv.ai_slv_byte;
    rootObject["relaylevel"] = module_param.relay_slv.relay_slv_byte;
    rootObject["af"]         = module_param.work_state.work_state_byte;
    rootObject["ss"]         = module_param.safe_state.safe_state_byte;
    return rootObject;
}

bool param::param_project_parse(QJsonObject project)
{
    if (project.isEmpty()) {
        param_ui_clear();
        return false;
    }
    module_param.can_baudrate                     = project["baudrate"].toInt();
    module_param.can_slave_nodeID_A               = project["anodeid"].toInt();
    module_param.can_slave_nodeID_B               = project["bnodeid"].toInt();
    module_param.can_master_nodeID                = project["mnodeid"].toInt();
    module_param.can_hb_consumer_gap              = project["checkbt"].toInt();
    module_param.can_hb_producer_gap              = project["sendbt"].toInt();
    module_param.can_pdo_time_gap                 = project["pdopt"].toInt();
    module_param.sai_sample_interval              = project["saisi"].toInt();
    module_param.spi_sample_interval              = project["spisi"].toInt();
    module_param.pi_sqep_sample_interval          = project["piqepsi"].toInt();
    module_param.sqep_sample_interval             = project["sqepsi"].toInt();
    module_param.fault_code2_safe_state           = project["fcsta"].toInt();
    module_param.fault_code2_safe_state_delaytime = project["fcdt"].toInt();
    module_param.sai_allow_dif[0]                 = project["saifull"].toInt();
    module_param.sai_allow_dif[1]                 = project["saiac"].toInt();
    module_param.spi_allow_dif[0]                 = project["spifull"].toInt();
    module_param.spi_allow_dif[1]                 = project["spiac"].toInt();
    module_param.pi_qep_allow_dif[0]              = project["piqepfull"].toInt();
    module_param.pi_qep_allow_dif[1]              = project["piqepac"].toInt();
    module_param.sqep_allow_dif[0]                = project["sqepfull"].toInt();
    module_param.sqep_allow_dif[1]                = project["sqepac"].toInt();
    module_param.check_factor                     = project["crossc"].toInt();
    module_param.di_slv.di_slv_bytes              = project["dilevel"].toInt();
    module_param.ai_slv.ai_slv_byte               = project["ailevel"].toInt();
    module_param.relay_slv.relay_slv_byte         = project["relaylevel"].toInt();
    module_param.work_state.work_state_byte       = project["af"].toInt();
    module_param.safe_state.safe_state_byte       = project["ss"].toInt();
    param_display(&module_param);
    return true;
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
}

void param::param_ui_to_data(module_param_t* param)
{
    param->di_slv.di_slv_bytes              = 0;
    param->relay_slv.relay_slv_byte         = 0;
    param->ai_slv.ai_slv_byte               = 0;
    param->work_state.work_state_byte       = 0;
    param->safe_state.safe_state_byte       = 0;
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
    QString    name      = ui->lineEdit_projectname->text().left(24);
    QByteArray namearray = name.toUtf8();
    memcpy(param->lua_file_ver, namearray.data(), namearray.size());
    mbedtls_md5(( const unsigned char* )param, sizeof(module_param_t) - sizeof(param->md5), param->md5);
}

void param::param_ui_clear()
{
    module_param_t default_param;
    memset(( uint8_t* )&default_param, 0, sizeof(default_param));
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
    memcpy(( uint8_t* )&module_param, ( uint8_t* )&default_param, sizeof(default_param));
    param_display(&default_param);
    ui->bootloader_version_lineEdit->setText("Boot:na");
    ui->app_version_lineEdit->setText("App:na");
    ui->bottom_version_lineEdit->setText("Bottom:na");
    ui->core_version_lineEdit->setText("Core:na");
}

/**
 * @brief 自适应UI大小
 * @param 当前界面宽度
 * @param 当前界面高度
 */
void param::param_ui_resize(uint32_t width, uint32_t height)
{
    uint32_t ctrl_height          = 20 * height / UI_HEIGHT;
    uint32_t sounbox_photo_height = 30 * height / UI_HEIGHT;
    uint32_t sounbox_photo_width  = 30 * width / UI_WIDTH;
    uint32_t font_size            = 14 * height / UI_HEIGHT;
    ui->tab_ssparam->setStyleSheet(
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
    uint32_t ss_level_checkbox_size = 50 * height / UI_HEIGHT;
    ui->ss_level_groupBox->setStyleSheet(
        "QCheckBox{spacing: 5px;color: green;}"
        "QCheckBox::indicator{width: "
        + QString::number(ss_level_checkbox_size) + "px;height: " + QString::number(ss_level_checkbox_size)
        + "px;}"
          "QCheckBox::indicator:enabled:unchecked {image: url(:/new/photo/photo/cat2.png);}"
          "QCheckBox::indicator:enabled:checked {image: url(:/new/photo/photo/cat3.png);}");
    uint32_t ss_checkbox_unchecked_size = 40 * height / UI_HEIGHT;
    uint32_t ss_checkbox_checked_width  = 80 * height / UI_HEIGHT;
    uint32_t ss_checkbox_checked_height = 50 * height / UI_HEIGHT;
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

/* user slots */
void param::ss_state_changed_slot(int index)
{
    index       = index;
    uint8_t res = 0;
    for (int i = 1; i < 7; i++) {
        if (ss_cb[i]->isChecked()) {
            res |= ((0x01) << (i - 1));
        }
    }
    mainwindow->condition_view_class->ss_default_set_state(res);
}

void param::pi1_afstate_changed_slot(int index)
{
    if (ui->param_af_pi2_comboBox->currentIndex() != index) {
        ui->param_af_pi2_comboBox->setCurrentIndex(index);
    }
    switch (index) {
    case PI_AF_PI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP1]->setDisabled(true);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setDisabled(false);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP1]->setCheckState(0, Qt::Unchecked);
        break;
    case PI_AF_QEP:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP1]->setDisabled(false);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setDisabled(true);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setCheckState(0, Qt::Unchecked);
        break;
    case PI_AF_DI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setDisabled(false);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP1]->setDisabled(true);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setDisabled(true);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setCheckState(0, Qt::Unchecked);
        break;
    }
}

void param::pi2_afstate_changed_slot(int index)
{
    if (ui->param_af_pi1_comboBox->currentIndex() != index) {
        ui->param_af_pi1_comboBox->setCurrentIndex(index);
    }
    switch (index) {
    case PI_AF_PI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setDisabled(true);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setDisabled(false);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setCheckState(0, Qt::Unchecked);
        break;
    case PI_AF_QEP:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setDisabled(false);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setDisabled(true);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setCheckState(0, Qt::Unchecked);
        break;
    case PI_AF_DI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setDisabled(false);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setDisabled(true);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setCheckState(0, Qt::Unchecked);
        break;
    }
}

void param::ai1_afstate_changed_slot(int index)
{
    switch (index) {
    case AI_AF_AI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI9]->setDisabled(true);
        mainwindow->condition_view_class->ai_item[TOOL_ID_AI1]->setDisabled(false);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI9]->setCheckState(0, Qt::Unchecked);
        break;
    case AI_AF_DI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI9]->setDisabled(false);
        mainwindow->condition_view_class->ai_item[TOOL_ID_AI1]->setDisabled(true);
        mainwindow->condition_view_class->ai_item[TOOL_ID_AI1]->setCheckState(0, Qt::Unchecked);
        break;
    }
}

void param::ai2_afstate_changed_slot(int index)
{
    switch (index) {
    case AI_AF_AI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI10]->setDisabled(true);
        mainwindow->condition_view_class->ai_item[TOOL_ID_AI2]->setDisabled(false);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI10]->setCheckState(0, Qt::Unchecked);
        break;
    case AI_AF_DI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI10]->setDisabled(false);
        mainwindow->condition_view_class->ai_item[TOOL_ID_AI2]->setDisabled(true);
        mainwindow->condition_view_class->ai_item[TOOL_ID_AI2]->setCheckState(0, Qt::Unchecked);
        break;
    }
}
