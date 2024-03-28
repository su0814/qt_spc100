#include "safety_param_dialog.h"
#include "mainwindow.h"
#include "md5.h"
#include "ui_safety_param_dialog.h"

Safety_Param_Dialog::Safety_Param_Dialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::Safety_Param_Dialog)
{
    ui->setupUi(this);
    mainwindow = ( MainWindow* )parent;
    // setFixedSize(700, 600);
    // layout()->setContentsMargins(10, 10, 10, 10);
    setWindowFlags(Qt::Tool);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    init();
}

Safety_Param_Dialog::~Safety_Param_Dialog()
{
    delete ui;
}

void Safety_Param_Dialog::init()
{
    config_title.clear();
    config_title << "系统参数"
                 << "CANopen参数"
                 << "AI参数"
                 << "DI参数"
                 << "RELAY参数"
                 << "MOS参数"
                 << "ENCODER参数"
                 << "PI参数";
    di_cat_list.append(ui->di1_cat_combo);
    di_cat_list.append(ui->di2_cat_combo);
    di_cat_list.append(ui->di3_cat_combo);
    di_cat_list.append(ui->di4_cat_combo);
    di_cat_list.append(ui->di5_cat_combo);
    di_cat_list.append(ui->di6_cat_combo);
    di_cat_list.append(ui->di7_cat_combo);
    di_cat_list.append(ui->di8_cat_combo);
    ai_cat_list.append(ui->ai1_cat_combo);
    ai_cat_list.append(ui->ai2_cat_combo);
    relay_cat_list.append(ui->relay1_cat_combo);
    relay_cat_list.append(ui->relay2_cat_combo);
    ss_relevant_list.append(ui->ss_relay1_comboBox);
    ss_relevant_list.append(ui->ss_relay2_comboBox);
    ss_relevant_list.append(ui->ss_mos1_comboBox);
    ss_relevant_list.append(ui->ss_mos2_comboBox);
    ss_relevant_list.append(ui->ss_mos3_comboBox);
    ss_relevant_list.append(ui->ss_mos4_comboBox);
    qep_select_list.append(ui->qepspeed_select1_comboBox);
    qep_select_list.append(ui->qepspeed_select2_comboBox);
    qep_select_list.append(ui->qepspeed_select3_comboBox);
    qep_select_list.append(ui->qepspeed_select4_comboBox);
    qep_select_list.append(ui->qepspeed_select5_comboBox);
    piqep_select_list.append(ui->piqepspeed_select1_comboBox);
    piqep_select_list.append(ui->piqepspeed_select2_comboBox);
    piqep_select_list.append(ui->piqepspeed_select3_comboBox);
    piqep_select_list.append(ui->piqepspeed_select4_comboBox);
    piqep_select_list.append(ui->piqepspeed_select5_comboBox);
    decelerate_check_intreval_list.append(ui->decelerate_check_interval1_spinbox);
    decelerate_check_intreval_list.append(ui->decelerate_check_interval2_spinbox);
    decelerate_check_intreval_list.append(ui->decelerate_check_interval3_spinbox);
    decelerate_check_intreval_list.append(ui->decelerate_check_interval4_spinbox);
    decelerate_check_intreval_list.append(ui->decelerate_check_interval5_spinbox);
    decelerate_sublevel1_threshold_list.append(ui->decelerate_sublevel1_thresshold1_spinBox);
    decelerate_sublevel1_threshold_list.append(ui->decelerate_sublevel1_thresshold2_spinBox);
    decelerate_sublevel1_threshold_list.append(ui->decelerate_sublevel1_thresshold3_spinBox);
    decelerate_sublevel1_threshold_list.append(ui->decelerate_sublevel1_thresshold4_spinBox);
    decelerate_sublevel1_threshold_list.append(ui->decelerate_sublevel1_thresshold5_spinBox);
    decelerate_sublevel2_threshold_list.append(ui->decelerate_sublevel2_thresshold1_spinBox);
    decelerate_sublevel2_threshold_list.append(ui->decelerate_sublevel2_thresshold2_spinBox);
    decelerate_sublevel2_threshold_list.append(ui->decelerate_sublevel2_thresshold3_spinBox);
    decelerate_sublevel2_threshold_list.append(ui->decelerate_sublevel2_thresshold4_spinBox);
    decelerate_sublevel2_threshold_list.append(ui->decelerate_sublevel2_thresshold5_spinBox);
    decelerate_sublevel3_threshold_list.append(ui->decelerate_sublevel3_thresshold1_spinBox);
    decelerate_sublevel3_threshold_list.append(ui->decelerate_sublevel3_thresshold2_spinBox);
    decelerate_sublevel3_threshold_list.append(ui->decelerate_sublevel3_thresshold3_spinBox);
    decelerate_sublevel3_threshold_list.append(ui->decelerate_sublevel3_thresshold4_spinBox);
    decelerate_sublevel3_threshold_list.append(ui->decelerate_sublevel3_thresshold5_spinBox);
    param_reset();
    // setWindowFlags(Qt::Window);
    // setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    connect(ui->ai1_af_comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(ai1_af_comboBox_currentIndexChanged(int)));
    connect(ui->ai2_af_comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(ai2_af_comboBox_currentIndexChanged(int)));
    connect(ui->pi1_af_comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(pi1_af_comboBox_currentIndexChanged(int)));
    connect(ui->pi2_af_comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(pi2_af_comboBox_currentIndexChanged(int)));
    for (int i = 0; i < piqep_select_list.size(); i++) {
        connect(piqep_select_list[i], SIGNAL(currentIndexChanged(int)), this, SLOT(param_piqepspeed_select_slot(int)));
    }
    for (int i = 0; i < ss_relevant_list.size(); i++) {
        connect(ss_relevant_list[i], SIGNAL(currentIndexChanged(int)), this, SLOT(ss_combo_currentIndexChanged(int)));
    }
}

void Safety_Param_Dialog::exec_from_id(config_tool_e id)
{
    setWindowTitle(config_title[id]);
    ui->stackedWidget->setCurrentIndex(id);
    setLayout(layout());
    exec();
}

void Safety_Param_Dialog::module_param_init()
{
    module_param.can_master_nodeID                = ( uint8_t )0x01;
    module_param.can_slave_nodeID_A               = ( uint8_t )0x51;
    module_param.can_slave_nodeID_B               = ( uint8_t )0x52;
    module_param.can_hb_consumer_gap              = 0;
    module_param.can_hb_producer_gap              = 1000;
    module_param.ai_slv.ai_slv_byte               = 0x03;
    module_param.di_slv.di_slv_bytes              = 0xff;
    module_param.relay_slv.relay_slv_byte         = 0x03;
    module_param.work_state.work_state_byte       = 0x35;
    module_param.safe_state.safe_state_byte       = 0X7E;
    module_param.piqep12_cross_check              = 0;
    module_param.qep12_cross_check                = 0;
    module_param.qep1_2_ratio                     = 100;
    module_param.piqep1_2_ratio                   = 100;
    module_param.qep_speed_allow_dif[0]           = 1;
    module_param.qep_speed_allow_dif[1]           = 3;
    module_param.piqep_speed_allow_dif[0]         = 1;
    module_param.piqep_speed_allow_dif[1]         = 3;
    module_param.sai_allow_dif[0]                 = 1;
    module_param.sai_allow_dif[1]                 = 3;
    module_param.spi_allow_dif[0]                 = 1;
    module_param.spi_allow_dif[1]                 = 3;
    module_param.pi_qep_allow_dif[0]              = 1;
    module_param.pi_qep_allow_dif[1]              = 3;
    module_param.sqep_allow_dif[0]                = 1;
    module_param.sqep_allow_dif[1]                = 3;
    module_param.sai_sample_interval              = 10;
    module_param.spi_sample_interval              = 10;
    module_param.pi_sqep_sample_interval          = 10;
    module_param.sqep_sample_interval             = 10;
    module_param.fault_code2_safe_state           = 0;
    module_param.fault_code2_safe_state_delaytime = 0;
    /*lua software version*/
    module_param.check_factor     = 1;
    module_param.can_baudrate     = 1000000;
    module_param.can_pdo_time_gap = 1000;
    /* 减速曲线类 */
    for (int i = 0; i < MAX_DECELERATE_NUM; i++) {
        module_param.qep_speed_select[i]                 = SPEED_SELECT_NONE;
        module_param.piqep_speed_select[i]               = SPEED_SELECT_NONE;
        module_param.decelerate_check_interval[i]        = 100;
        module_param.decelerate_sublevel_threshold[i][0] = 3000;
        module_param.decelerate_sublevel_threshold[i][1] = 2000;
        module_param.decelerate_sublevel_threshold[i][2] = 1000;
    }
    mbedtls_md5(( const unsigned char* )&module_param, sizeof(module_param) - sizeof(module_param.md5),
                module_param.md5);
}

QJsonObject Safety_Param_Dialog::param_project_info()
{
    ui_to_param();
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
    /* 减速保护相关 */
    rootObject["qepratio"]       = module_param.qep1_2_ratio;
    rootObject["piqepratio"]     = module_param.piqep1_2_ratio;
    rootObject["qepcross"]       = module_param.qep12_cross_check;
    rootObject["piqepcross"]     = module_param.piqep12_cross_check;
    rootObject["qepspeedfull"]   = module_param.qep_speed_allow_dif[0];
    rootObject["qepspeedac"]     = module_param.qep_speed_allow_dif[1];
    rootObject["piqepspeedfull"] = module_param.piqep_speed_allow_dif[0];
    rootObject["piqepspeedac"]   = module_param.piqep_speed_allow_dif[1];

    for (int i = 0; i < MAX_DECELERATE_NUM; i++) {
        rootObject["qepspeedselect" + QString::number(i)]            = module_param.qep_speed_select[i];
        rootObject["piqepspeedselect" + QString::number(i)]          = module_param.piqep_speed_select[i];
        rootObject["decelerate_check_interval" + QString::number(i)] = module_param.decelerate_check_interval[i];
        rootObject["decelerate_sublevel1_" + QString::number(i)] =
            ( int )module_param.decelerate_sublevel_threshold[i][0];
        rootObject["decelerate_sublevel2_" + QString::number(i)] =
            ( int )module_param.decelerate_sublevel_threshold[i][1];
        rootObject["decelerate_sublevel3_" + QString::number(i)] =
            ( int )module_param.decelerate_sublevel_threshold[i][2];
    }
    return rootObject;
}

bool Safety_Param_Dialog::param_project_parse(QJsonObject project)
{
    if (project.isEmpty()) {
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
    /* 速度保护相关 */
    module_param.qep1_2_ratio             = project["qepratio"].toInt();
    module_param.piqep1_2_ratio           = project["piqepratio"].toInt();
    module_param.qep12_cross_check        = project["qepcross"].toInt();
    module_param.piqep12_cross_check      = project["piqepcross"].toInt();
    module_param.qep_speed_allow_dif[0]   = project["qepspeedfull"].toInt();
    module_param.qep_speed_allow_dif[1]   = project["qepspeedac"].toInt();
    module_param.piqep_speed_allow_dif[0] = project["piqepspeedfull"].toInt();
    module_param.piqep_speed_allow_dif[1] = project["piqepspeedac"].toInt();

    for (int i = 0; i < MAX_DECELERATE_NUM; i++) {
        module_param.qep_speed_select[i]          = project["qepspeedselect" + QString::number(i)].toInt();
        module_param.piqep_speed_select[i]        = project["piqepspeedselect" + QString::number(i)].toInt();
        module_param.decelerate_check_interval[i] = project["decelerate_check_interval" + QString::number(i)].toInt();
        module_param.decelerate_sublevel_threshold[i][0] =
            project["decelerate_sublevel1_" + QString::number(i)].toInt();
        module_param.decelerate_sublevel_threshold[i][1] =
            project["decelerate_sublevel2_" + QString::number(i)].toInt();
        module_param.decelerate_sublevel_threshold[i][2] =
            project["decelerate_sublevel3_" + QString::number(i)].toInt();
    }
    param_to_ui();
    return true;
}

void Safety_Param_Dialog::param_to_ui()
{

    for (int i = 0; i < di_cat_list.size(); i++) {
        di_cat_list[i]->setCurrentIndex((module_param.di_slv.di_slv_bytes >> i) & 0x01);
    }
    for (int i = 0; i < ai_cat_list.size(); i++) {
        ai_cat_list[i]->setCurrentIndex((module_param.ai_slv.ai_slv_byte >> i) & 0x01);
    }
    for (int i = 0; i < relay_cat_list.size(); i++) {
        relay_cat_list[i]->setCurrentIndex((module_param.relay_slv.relay_slv_byte >> i) & 0x01);
    }
    for (int i = 0; i < ss_relevant_list.size(); i++) {
        ss_relevant_list[i]->setCurrentIndex((module_param.safe_state.safe_state_byte >> (i + 1)) & 0x01);
    }
    for (int i = 0; i < qep_select_list.size(); i++) {
        qep_select_list[i]->setCurrentIndex(module_param.qep_speed_select[i]);
    }
    for (int i = 0; i < piqep_select_list.size(); i++) {
        piqep_select_list[i]->setCurrentIndex(module_param.piqep_speed_select[i]);
    }
    for (int i = 0; i < decelerate_check_intreval_list.size(); i++) {
        decelerate_check_intreval_list[i]->setValue(module_param.decelerate_check_interval[i]);
    }
    for (int i = 0; i < decelerate_sublevel1_threshold_list.size(); i++) {
        decelerate_sublevel1_threshold_list[i]->setValue(module_param.decelerate_sublevel_threshold[i][0]);
    }
    for (int i = 0; i < decelerate_sublevel2_threshold_list.size(); i++) {
        decelerate_sublevel2_threshold_list[i]->setValue(module_param.decelerate_sublevel_threshold[i][1]);
    }
    for (int i = 0; i < decelerate_sublevel3_threshold_list.size(); i++) {
        decelerate_sublevel3_threshold_list[i]->setValue(module_param.decelerate_sublevel_threshold[i][2]);
    }
    ui->pi1_af_comboBox->setCurrentIndex(1);
    ui->pi1_af_comboBox->setCurrentIndex(int(module_param.work_state.work_state_bit.pi1_work_state_bit));
    ui->pi2_af_comboBox->setCurrentIndex(module_param.work_state.work_state_bit.pi2_work_state_bit);
    ui->ai1_af_comboBox->setCurrentIndex(module_param.work_state.work_state_bit.ai1_work_state_bit);
    ui->ai2_af_comboBox->setCurrentIndex(module_param.work_state.work_state_bit.ai2_work_state_bit);
    ui->fault_to_ss_combobox->setCurrentIndex(module_param.fault_code2_safe_state);
    ui->fault_toss_delay_spinbox->setValue(module_param.fault_code2_safe_state_delaytime);
    ui->ai_simple_interval_spinBox->setValue(module_param.sai_sample_interval);
    ui->ai_full_spinbox->setValue(module_param.sai_allow_dif[0]);
    ui->ai_actual_spinbox->setValue(module_param.sai_allow_dif[1]);
    ui->pi_simple_interval_spinBox->setValue(module_param.spi_sample_interval);
    ui->pi_full_spinBox->setValue(module_param.spi_allow_dif[0]);
    ui->pi_actual_spinBox->setValue(module_param.spi_allow_dif[1]);
    ui->piqep_simple_interval_spinBox->setValue(module_param.pi_sqep_sample_interval);
    ui->piqep_full_spinBox->setValue(module_param.pi_qep_allow_dif[0]);
    ui->piqep_actual_spinBox->setValue(module_param.pi_qep_allow_dif[1]);
    ui->qep_simple_interval_spinBox->setValue(module_param.sqep_sample_interval);
    ui->qep_full_spinBox->setValue(module_param.sqep_allow_dif[0]);
    ui->qep_actual_spinBox->setValue(module_param.sqep_allow_dif[1]);
    ui->master_nodeid_spinBox->setValue(module_param.can_master_nodeID);
    ui->a_nodeid_spinBox->setValue(module_param.can_slave_nodeID_A);
    ui->b_nodeid_spinBox->setValue(module_param.can_slave_nodeID_B);
    ui->master_heartbeat_spinBox->setValue(module_param.can_hb_consumer_gap);
    ui->slave_heartbeat_spinBox->setValue(module_param.can_hb_producer_gap);
    ui->pdo_interval_spinBox->setValue(module_param.can_pdo_time_gap);
    ui->can_baudrate_comboBox->setCurrentText(QString::number(module_param.can_baudrate));
    ui->cross_check_interval_spinbox->setValue(module_param.check_factor);
    ui->qep12_cross_comboBox->setCurrentIndex(module_param.qep12_cross_check);
    ui->piqep12_cross_combo->setCurrentIndex(module_param.piqep12_cross_check);
    ui->qep12_ratio_spinbox->setValue(module_param.qep1_2_ratio);
    ui->piqep12_ratio_spinbox->setValue(module_param.piqep1_2_ratio);
    ui->qep12_full_spinbox->setValue(module_param.qep_speed_allow_dif[0]);
    ui->qep12_actual_spinbox->setValue(module_param.qep_speed_allow_dif[1]);
    ui->piqep12_full_spinbox->setValue(module_param.piqep_speed_allow_dif[0]);
    ui->piqep12_actual_spinbox->setValue(module_param.piqep_speed_allow_dif[1]);
}

void Safety_Param_Dialog::ui_to_param()
{
    for (int i = 0; i < di_cat_list.size(); i++) {
        if (di_cat_list[i]->currentIndex()) {
            module_param.di_slv.di_slv_bytes |= ((0x01) << i);
        } else {
            module_param.di_slv.di_slv_bytes &= (~((0x01) << i));
        }
    }
    for (int i = 0; i < ai_cat_list.size(); i++) {
        if (ai_cat_list[i]->currentIndex()) {
            module_param.ai_slv.ai_slv_byte |= ((0x01) << i);
        } else {
            module_param.ai_slv.ai_slv_byte &= (~((0x01) << i));
        }
    }
    for (int i = 0; i < relay_cat_list.size(); i++) {
        if (relay_cat_list[i]->currentIndex()) {
            module_param.relay_slv.relay_slv_byte |= ((0x01) << i);
        } else {
            module_param.relay_slv.relay_slv_byte &= (~((0x01) << i));
        }
    }
    for (int i = 0; i < ss_relevant_list.size(); i++) {
        if (ss_relevant_list[i]->currentIndex()) {
            module_param.safe_state.safe_state_byte |= ((0x01) << (i + 1));
        } else {
            module_param.safe_state.safe_state_byte &= (~((0x01) << (i + 1)));
        }
    }
    for (int i = 0; i < qep_select_list.size(); i++) {
        module_param.qep_speed_select[i] = qep_select_list[i]->currentIndex();
    }
    for (int i = 0; i < piqep_select_list.size(); i++) {
        module_param.piqep_speed_select[i] = piqep_select_list[i]->currentIndex();
    }
    for (int i = 0; i < decelerate_check_intreval_list.size(); i++) {
        module_param.decelerate_check_interval[i] = decelerate_check_intreval_list[i]->value();
    }
    for (int i = 0; i < decelerate_sublevel1_threshold_list.size(); i++) {
        module_param.decelerate_sublevel_threshold[i][0] = decelerate_sublevel1_threshold_list[i]->value();
    }
    for (int i = 0; i < decelerate_sublevel2_threshold_list.size(); i++) {
        module_param.decelerate_sublevel_threshold[i][1] = decelerate_sublevel2_threshold_list[i]->value();
    }
    for (int i = 0; i < decelerate_sublevel3_threshold_list.size(); i++) {
        module_param.decelerate_sublevel_threshold[i][2] = decelerate_sublevel3_threshold_list[i]->value();
    }
    module_param.work_state.work_state_bit.pi1_work_state_bit = ui->pi1_af_comboBox->currentIndex();
    module_param.work_state.work_state_bit.pi2_work_state_bit = ui->pi2_af_comboBox->currentIndex();
    module_param.work_state.work_state_bit.ai1_work_state_bit = ui->ai1_af_comboBox->currentIndex();
    module_param.work_state.work_state_bit.ai2_work_state_bit = ui->ai2_af_comboBox->currentIndex();
    module_param.fault_code2_safe_state                       = ui->fault_to_ss_combobox->currentIndex();
    module_param.fault_code2_safe_state_delaytime             = ui->fault_toss_delay_spinbox->value();
    module_param.sai_sample_interval                          = ui->ai_simple_interval_spinBox->value();
    module_param.sai_allow_dif[0]                             = ui->ai_full_spinbox->value();
    module_param.sai_allow_dif[1]                             = ui->ai_actual_spinbox->value();
    module_param.spi_sample_interval                          = ui->pi_simple_interval_spinBox->value();
    module_param.spi_allow_dif[0]                             = ui->pi_full_spinBox->value();
    module_param.spi_allow_dif[1]                             = ui->pi_actual_spinBox->value();
    module_param.pi_sqep_sample_interval                      = ui->piqep_simple_interval_spinBox->value();
    module_param.pi_qep_allow_dif[0]                          = ui->piqep_full_spinBox->value();
    module_param.pi_qep_allow_dif[1]                          = ui->piqep_actual_spinBox->value();
    module_param.sqep_sample_interval                         = ui->qep_simple_interval_spinBox->value();
    module_param.sqep_allow_dif[0]                            = ui->qep_full_spinBox->value();
    module_param.sqep_allow_dif[1]                            = ui->qep_actual_spinBox->value();
    module_param.can_master_nodeID                            = ui->master_nodeid_spinBox->value();
    module_param.can_slave_nodeID_A                           = ui->a_nodeid_spinBox->value();
    module_param.can_slave_nodeID_B                           = ui->b_nodeid_spinBox->value();
    module_param.can_hb_consumer_gap                          = ui->master_heartbeat_spinBox->value();
    module_param.can_hb_producer_gap                          = ui->slave_heartbeat_spinBox->value();
    module_param.can_pdo_time_gap                             = ui->pdo_interval_spinBox->value();
    module_param.can_baudrate                                 = ui->can_baudrate_comboBox->currentText().toInt();
    module_param.check_factor                                 = ui->cross_check_interval_spinbox->value();
    module_param.qep12_cross_check                            = ui->qep12_cross_comboBox->currentIndex();
    module_param.piqep12_cross_check                          = ui->piqep12_cross_combo->currentIndex();
    module_param.qep1_2_ratio                                 = ui->qep12_ratio_spinbox->value();
    module_param.piqep1_2_ratio                               = ui->piqep12_ratio_spinbox->value();
    module_param.qep_speed_allow_dif[0]                       = ui->qep12_full_spinbox->value();
    module_param.qep_speed_allow_dif[1]                       = ui->qep12_actual_spinbox->value();
    module_param.piqep_speed_allow_dif[0]                     = ui->piqep12_full_spinbox->value();
    module_param.piqep_speed_allow_dif[1]                     = ui->piqep12_actual_spinbox->value();
}

void Safety_Param_Dialog::param_ss_set(uint8_t state)
{
    for (int i = 0; i < ss_relevant_list.size(); i++) {
        ss_relevant_list[i]->setCurrentIndex((state >> i) & 0x01);
    }
}

uint8_t Safety_Param_Dialog::param_ss_get()
{
    uint8_t state = 0;
    for (int i = 0; i < ss_relevant_list.size(); i++) {
        if (ss_relevant_list[i]->currentIndex()) {
            state |= (0x01 << i);
        }
    }
    return state;
}

void Safety_Param_Dialog::param_reset()
{
    module_param_init();
    param_to_ui();
}

module_param_t Safety_Param_Dialog::get_module_param()
{
    mbedtls_md5(( const unsigned char* )&module_param, sizeof(module_param) - sizeof(module_param.md5),
                module_param.md5);
    return module_param;
}

/* sys slots */
void Safety_Param_Dialog::on_a_nodeid_spinBox_editingFinished()
{
    int arg1 = ui->a_nodeid_spinBox->value();
    if (arg1 == ui->b_nodeid_spinBox->value() || arg1 == ui->master_nodeid_spinBox->value()) {
        ui->a_nodeid_spinBox->setValue(module_param.can_slave_nodeID_A);
    }
}

void Safety_Param_Dialog::on_b_nodeid_spinBox_editingFinished()
{
    int arg1 = ui->b_nodeid_spinBox->value();
    if (arg1 == ui->a_nodeid_spinBox->value() || arg1 == ui->master_nodeid_spinBox->value()) {
        ui->b_nodeid_spinBox->setValue(module_param.can_slave_nodeID_B);
    }
}

void Safety_Param_Dialog::on_master_nodeid_spinBox_editingFinished()
{
    int arg1 = ui->master_nodeid_spinBox->value();
    if (arg1 == ui->b_nodeid_spinBox->value() || arg1 == ui->a_nodeid_spinBox->value()) {
        ui->master_nodeid_spinBox->setValue(module_param.can_master_nodeID);
    }
}

void Safety_Param_Dialog::on_master_heartbeat_spinBox_editingFinished()
{
    int val = ui->master_heartbeat_spinBox->value();
    if (val > 0 || val < 100) {
        ui->master_heartbeat_spinBox->setValue(100);
    }
}

void Safety_Param_Dialog::on_slave_heartbeat_spinBox_editingFinished()
{
    int val = ui->slave_heartbeat_spinBox->value();
    if (val > 0 || val < 100) {
        ui->slave_heartbeat_spinBox->setValue(100);
    }
}

void Safety_Param_Dialog::on_pdo_interval_spinBox_editingFinished()
{
    int val = ui->pdo_interval_spinBox->value();
    if (val > 0 || val < 100) {
        ui->pdo_interval_spinBox->setValue(100);
    }
}

void Safety_Param_Dialog::ai1_af_comboBox_currentIndexChanged(int index)
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

void Safety_Param_Dialog::ai2_af_comboBox_currentIndexChanged(int index)
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

void Safety_Param_Dialog::pi1_af_comboBox_currentIndexChanged(int index)
{
    if (index != PI_AF_QEP) {
        ui->piqep12_cross_combo->setCurrentIndex(0);
        for (int i = 0; i < piqep_select_list.count(); i++) {
            piqep_select_list[i]->setCurrentIndex(SPEED_SELECT_NONE);
        }
    }
    switch (index) {
    case PI_AF_PI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP1]->setDisabled(true);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setDisabled(false);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP1]->setCheckState(0, Qt::Unchecked);
        if (ui->pi2_af_comboBox->currentIndex() == PI_AF_QEP) {
            ui->pi2_af_comboBox->setCurrentIndex(index);
        }
        break;
    case PI_AF_QEP:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP1]->setDisabled(false);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setDisabled(true);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setCheckState(0, Qt::Unchecked);
        if (ui->pi2_af_comboBox->currentIndex() != index) {
            ui->pi2_af_comboBox->setCurrentIndex(index);
        }
        break;
    case PI_AF_DI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setDisabled(false);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP1]->setDisabled(true);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setDisabled(true);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI11]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI1]->setCheckState(0, Qt::Unchecked);
        if (ui->pi2_af_comboBox->currentIndex() == PI_AF_QEP) {
            ui->pi2_af_comboBox->setCurrentIndex(index);
        }
        break;
    }
}

void Safety_Param_Dialog::pi2_af_comboBox_currentIndexChanged(int index)
{
    if (index != PI_AF_QEP) {
        ui->piqep12_cross_combo->setCurrentIndex(0);
        for (int i = 0; i < piqep_select_list.count(); i++) {
            piqep_select_list[i]->setCurrentIndex(SPEED_SELECT_NONE);
        }
    }
    switch (index) {
    case PI_AF_PI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setDisabled(true);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setDisabled(false);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setCheckState(0, Qt::Unchecked);
        if (ui->pi1_af_comboBox->currentIndex() == PI_AF_QEP) {
            ui->pi1_af_comboBox->setCurrentIndex(index);
        }

        break;
    case PI_AF_QEP:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setDisabled(false);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setDisabled(true);
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setCheckState(0, Qt::Unchecked);
        if (ui->pi1_af_comboBox->currentIndex() != index) {
            ui->pi1_af_comboBox->setCurrentIndex(index);
        }
        break;
    case PI_AF_DI:
        mainwindow->condition_view_class->di_item[TOOL_ID_DI12]->setDisabled(false);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setDisabled(true);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setDisabled(true);
        mainwindow->condition_view_class->qep_item[TOOL_ID_PI_QEP2]->setCheckState(0, Qt::Unchecked);
        mainwindow->condition_view_class->pi_item[TOOL_ID_PI2]->setCheckState(0, Qt::Unchecked);
        if (ui->pi1_af_comboBox->currentIndex() == PI_AF_QEP) {
            ui->pi1_af_comboBox->setCurrentIndex(index);
        }
        break;
    }
}

void Safety_Param_Dialog::ss_combo_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    uint8_t state = 0;
    for (int i = 0; i < ss_relevant_list.size(); i++) {
        if (ss_relevant_list[i]->currentIndex()) {
            state |= (0x01 << i);
        }
    }
    mainwindow->condition_view_class->ss_default_set_state(state);
}

void Safety_Param_Dialog::param_piqepspeed_select_slot(int index)
{
    QObject*   senderObj      = QObject::sender();
    QComboBox* comboBoxSender = qobject_cast<QComboBox*>(senderObj);
    if (comboBoxSender) {
        if (index != SPEED_SELECT_NONE) {
            if (ui->pi1_af_comboBox->currentIndex() != PI_AF_QEP || ui->pi2_af_comboBox->currentIndex() != PI_AF_QEP) {
                comboBoxSender->setCurrentIndex(SPEED_SELECT_NONE);
                mainwindow->my_message_box("启用功能需要PI复用为QEP", MESSAGE_TYPE_WARNING);
                return;
            }
        }
    }
}

void Safety_Param_Dialog::on_piqep12_cross_combo_currentIndexChanged(int index)
{
    if (index == 1) {
        if (ui->pi1_af_comboBox->currentIndex() != PI_AF_QEP || ui->pi2_af_comboBox->currentIndex() != PI_AF_QEP) {
            ui->piqep12_cross_combo->setCurrentIndex(0);
            mainwindow->my_message_box("启用功能需要PI复用为QEP", MESSAGE_TYPE_WARNING);
            return;
        }
    }
}
