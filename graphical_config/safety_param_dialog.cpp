#include "safety_param_dialog.h"
#include "algorithm/MD5/md5.h"
#include "mainwindow.h"
#include "ui_safety_param_dialog.h"

Safety_Param_Dialog::Safety_Param_Dialog(module_param_t* param, config_block_data_t* data, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::Safety_Param_Dialog)
{
    ui->setupUi(this);
    safe_param  = param;
    config_data = data;
    init();
}

Safety_Param_Dialog::~Safety_Param_Dialog()
{
    delete ui;
}

void Safety_Param_Dialog::init()
{
    setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    ui->stackedWidget->setVisible(false);
    setWindowTitle("设置");
}

void Safety_Param_Dialog::module_param_init(module_param_t* module_param)
{
    module_param->can_master_nodeID                = ( uint8_t )0x01;
    module_param->can_slave_nodeID_A               = ( uint8_t )0x51;
    module_param->can_slave_nodeID_B               = ( uint8_t )0x52;
    module_param->can_hb_consumer_gap              = 0;
    module_param->can_hb_producer_gap              = 1000;
    module_param->ai_slv.ai_slv_byte               = 0x03;
    module_param->di_slv.di_slv_bytes              = 0xff;
    module_param->relay_slv.relay_slv_byte         = 0x03;
    module_param->work_state.work_state_byte       = 0x35;
    module_param->safe_state.safe_state_byte       = 0X7E;
    module_param->piqep12_cross_check              = 0;
    module_param->qep12_cross_check                = 0;
    module_param->qep1_2_ratio                     = 100;
    module_param->piqep1_2_ratio                   = 100;
    module_param->qep_speed_allow_dif[0]           = 1;
    module_param->qep_speed_allow_dif[1]           = 3;
    module_param->piqep_speed_allow_dif[0]         = 1;
    module_param->piqep_speed_allow_dif[1]         = 3;
    module_param->sai_allow_dif[0]                 = 1;
    module_param->sai_allow_dif[1]                 = 3;
    module_param->spi_allow_dif[0]                 = 1;
    module_param->spi_allow_dif[1]                 = 3;
    module_param->pi_qep_allow_dif[0]              = 1;
    module_param->pi_qep_allow_dif[1]              = 3;
    module_param->sqep_allow_dif[0]                = 1;
    module_param->sqep_allow_dif[1]                = 3;
    module_param->sai_sample_interval              = 10;
    module_param->spi_sample_interval              = 10;
    module_param->pi_sqep_sample_interval          = 10;
    module_param->sqep_sample_interval             = 10;
    module_param->fault_code2_safe_state           = 0;
    module_param->fault_code2_safe_state_delaytime = 0;
    /*lua software version*/
    module_param->check_factor     = 1;
    module_param->can_baudrate     = 1000000;
    module_param->can_pdo_time_gap = 1000;
    mbedtls_md5(( const unsigned char* )module_param, sizeof(module_param_t) - sizeof(module_param->md5),
                module_param->md5);
}

QJsonObject Safety_Param_Dialog::param_project_info(module_param_t* module_param)
{
    QJsonObject rootObject;
    rootObject["baudrate"]   = ( int )module_param->can_baudrate;
    rootObject["anodeid"]    = module_param->can_slave_nodeID_A;
    rootObject["bnodeid"]    = module_param->can_slave_nodeID_B;
    rootObject["mnodeid"]    = module_param->can_master_nodeID;
    rootObject["checkbt"]    = module_param->can_hb_consumer_gap;
    rootObject["sendbt"]     = module_param->can_hb_producer_gap;
    rootObject["pdopt"]      = module_param->can_pdo_time_gap;
    rootObject["saisi"]      = module_param->sai_sample_interval;
    rootObject["spisi"]      = module_param->spi_sample_interval;
    rootObject["piqepsi"]    = module_param->pi_sqep_sample_interval;
    rootObject["sqepsi"]     = module_param->sqep_sample_interval;
    rootObject["fcsta"]      = module_param->fault_code2_safe_state;
    rootObject["fcdt"]       = module_param->fault_code2_safe_state_delaytime;
    rootObject["saifull"]    = module_param->sai_allow_dif[0];
    rootObject["saiac"]      = module_param->sai_allow_dif[1];
    rootObject["spifull"]    = module_param->spi_allow_dif[0];
    rootObject["spiac"]      = module_param->spi_allow_dif[1];
    rootObject["piqepfull"]  = module_param->pi_qep_allow_dif[0];
    rootObject["piqepac"]    = module_param->pi_qep_allow_dif[1];
    rootObject["sqepfull"]   = module_param->sqep_allow_dif[0];
    rootObject["sqepac"]     = module_param->sqep_allow_dif[1];
    rootObject["crossc"]     = module_param->check_factor;
    rootObject["dilevel"]    = module_param->di_slv.di_slv_bytes;
    rootObject["ailevel"]    = module_param->ai_slv.ai_slv_byte;
    rootObject["relaylevel"] = module_param->relay_slv.relay_slv_byte;
    rootObject["af"]         = module_param->work_state.work_state_byte;
    rootObject["ss"]         = module_param->safe_state.safe_state_byte;
    /* 减速保护相关 */
    rootObject["qepratio"]       = module_param->qep1_2_ratio;
    rootObject["piqepratio"]     = module_param->piqep1_2_ratio;
    rootObject["qepcross"]       = module_param->qep12_cross_check;
    rootObject["piqepcross"]     = module_param->piqep12_cross_check;
    rootObject["qepspeedfull"]   = module_param->qep_speed_allow_dif[0];
    rootObject["qepspeedac"]     = module_param->qep_speed_allow_dif[1];
    rootObject["piqepspeedfull"] = module_param->piqep_speed_allow_dif[0];
    rootObject["piqepspeedac"]   = module_param->piqep_speed_allow_dif[1];
    return rootObject;
}

bool Safety_Param_Dialog::param_project_parse(QJsonObject project, module_param_t* module_param)
{
    if (project.isEmpty()) {
        return false;
    }
    module_param->can_baudrate                     = project["baudrate"].toInt();
    module_param->can_slave_nodeID_A               = project["anodeid"].toInt();
    module_param->can_slave_nodeID_B               = project["bnodeid"].toInt();
    module_param->can_master_nodeID                = project["mnodeid"].toInt();
    module_param->can_hb_consumer_gap              = project["checkbt"].toInt();
    module_param->can_hb_producer_gap              = project["sendbt"].toInt();
    module_param->can_pdo_time_gap                 = project["pdopt"].toInt();
    module_param->sai_sample_interval              = project["saisi"].toInt();
    module_param->spi_sample_interval              = project["spisi"].toInt();
    module_param->pi_sqep_sample_interval          = project["piqepsi"].toInt();
    module_param->sqep_sample_interval             = project["sqepsi"].toInt();
    module_param->fault_code2_safe_state           = project["fcsta"].toInt();
    module_param->fault_code2_safe_state_delaytime = project["fcdt"].toInt();
    module_param->sai_allow_dif[0]                 = project["saifull"].toInt();
    module_param->sai_allow_dif[1]                 = project["saiac"].toInt();
    module_param->spi_allow_dif[0]                 = project["spifull"].toInt();
    module_param->spi_allow_dif[1]                 = project["spiac"].toInt();
    module_param->pi_qep_allow_dif[0]              = project["piqepfull"].toInt();
    module_param->pi_qep_allow_dif[1]              = project["piqepac"].toInt();
    module_param->sqep_allow_dif[0]                = project["sqepfull"].toInt();
    module_param->sqep_allow_dif[1]                = project["sqepac"].toInt();
    module_param->check_factor                     = project["crossc"].toInt();
    module_param->di_slv.di_slv_bytes              = project["dilevel"].toInt();
    module_param->ai_slv.ai_slv_byte               = project["ailevel"].toInt();
    module_param->relay_slv.relay_slv_byte         = project["relaylevel"].toInt();
    module_param->work_state.work_state_byte       = project["af"].toInt();
    module_param->safe_state.safe_state_byte       = project["ss"].toInt();
    /* 速度保护相关 */
    module_param->qep1_2_ratio             = project["qepratio"].toInt();
    module_param->piqep1_2_ratio           = project["piqepratio"].toInt();
    module_param->qep12_cross_check        = project["qepcross"].toInt();
    module_param->piqep12_cross_check      = project["piqepcross"].toInt();
    module_param->qep_speed_allow_dif[0]   = project["qepspeedfull"].toInt();
    module_param->qep_speed_allow_dif[1]   = project["qepspeedac"].toInt();
    module_param->piqep_speed_allow_dif[0] = project["piqepspeedfull"].toInt();
    module_param->piqep_speed_allow_dif[1] = project["piqepspeedac"].toInt();
    mbedtls_md5(( const unsigned char* )module_param, sizeof(module_param_t) - sizeof(module_param->md5),
                module_param->md5);
    return true;
}

void Safety_Param_Dialog::my_exec(void)
{
    this->setFixedSize(200, 100);
    ui->label_title->setText(config_data->type_name + ", " + config_data->source_name);
    ui->lineEdit_name->setText(config_data->name);
    switch (config_data->config_param_data.model_iotype) {
    case MODEL_TYPE_INPUT:
        switch (config_data->config_param_data.model_type) {
        case MODEL_INPUT_AI:
            if (config_data->safe_level == SAFE_LEVEL_CAT3) {
                ui->stackedWidget->setVisible(true);
                this->setFixedSize(200, 150);
            }
            ui->stackedWidget->setCurrentWidget(ui->page_ai);
            ui->ai_simple_interval_spinBox->setValue(safe_param->sai_sample_interval);
            ui->ai_full_spinbox->setValue(safe_param->sai_allow_dif[0]);
            ui->ai_actual_spinbox->setValue(safe_param->sai_allow_dif[1]);
            break;

        case MODEL_INPUT_PI:
            if (config_data->safe_level == SAFE_LEVEL_CAT3) {
                ui->stackedWidget->setVisible(true);
                this->setFixedSize(200, 170);
            }
            ui->stackedWidget->setCurrentWidget(ui->page_pi);
            ui->pi_simple_interval_spinBox->setValue(safe_param->spi_sample_interval);
            ui->pi_full_spinBox->setValue(safe_param->spi_allow_dif[0]);
            ui->pi_actual_spinBox->setValue(safe_param->spi_allow_dif[1]);
            break;
        case MODEL_INPUT_QEP:
            ui->stackedWidget->setVisible(true);
            ui->stackedWidget->setCurrentWidget(ui->page_encode);
            if (config_data->safe_level == SAFE_LEVEL_CAT2) {
                this->setFixedSize(200, 180);
                ui->qep_double_encode_groupBox->setVisible(false);
                ui->line->setVisible(false);
            } else {
                this->setFixedSize(210, 300);
            }
            switch (config_data->config_param_data.model_id) {
            case MODEL_ID_QEP1:
            case MODEL_ID_QEP2:
                ui->qep_simple_interval_spinBox->setValue(safe_param->sqep_sample_interval);
                ui->qep_full_spinBox->setValue(safe_param->sqep_allow_dif[0]);
                ui->qep_actual_spinBox->setValue(safe_param->sqep_allow_dif[1]);
                ui->qep12_ratio_spinbox->setValue(safe_param->qep1_2_ratio);
                ui->qep12_cross_comboBox->setCurrentIndex(safe_param->qep12_cross_check);
                ui->qep12_full_spinbox->setValue(safe_param->qep_speed_allow_dif[0]);
                ui->qep12_actual_spinbox->setValue(safe_param->qep_speed_allow_dif[1]);
                break;
            case MODEL_ID_PIQEP1:
            case MODEL_ID_PIQEP2:
                ui->qep_simple_interval_spinBox->setValue(safe_param->pi_sqep_sample_interval);
                ui->qep_full_spinBox->setValue(safe_param->pi_qep_allow_dif[0]);
                ui->qep_actual_spinBox->setValue(safe_param->pi_qep_allow_dif[1]);
                ui->qep12_ratio_spinbox->setValue(safe_param->piqep1_2_ratio);
                ui->qep12_cross_comboBox->setCurrentIndex(safe_param->piqep12_cross_check);
                ui->qep12_full_spinbox->setValue(safe_param->piqep_speed_allow_dif[0]);
                ui->qep12_actual_spinbox->setValue(safe_param->piqep_speed_allow_dif[1]);
                break;
            default:
                break;
            }
            break;
        case MODEL_INPUT_DI:
            break;
        default:
            break;
        }
        break;
    }
    exec();
}

/* sys slots */
// void Safety_Param_Dialog::on_a_nodeid_spinBox_editingFinished()
//{
//    int arg1 = ui->a_nodeid_spinBox->value();
//    if (arg1 == ui->b_nodeid_spinBox->value() || arg1 == ui->master_nodeid_spinBox->value()) {
//        ui->a_nodeid_spinBox->setValue(safe_param->can_slave_nodeID_A);
//    }
//}

// void Safety_Param_Dialog::on_b_nodeid_spinBox_editingFinished()
//{
//    int arg1 = ui->b_nodeid_spinBox->value();
//    if (arg1 == ui->a_nodeid_spinBox->value() || arg1 == ui->master_nodeid_spinBox->value()) {
//        ui->b_nodeid_spinBox->setValue(safe_param->can_slave_nodeID_B);
//    }
//}

// void Safety_Param_Dialog::on_master_nodeid_spinBox_editingFinished()
//{
//    int arg1 = ui->master_nodeid_spinBox->value();
//    if (arg1 == ui->b_nodeid_spinBox->value() || arg1 == ui->a_nodeid_spinBox->value()) {
//        ui->master_nodeid_spinBox->setValue(safe_param->can_master_nodeID);
//    }
//}

// void Safety_Param_Dialog::on_master_heartbeat_spinBox_editingFinished()
//{
//    int val = ui->master_heartbeat_spinBox->value();
//    if (val > 0 || val < 100) {
//        ui->master_heartbeat_spinBox->setValue(100);
//    }
//}

// void Safety_Param_Dialog::on_slave_heartbeat_spinBox_editingFinished()
//{
//    int val = ui->slave_heartbeat_spinBox->value();
//    if (val > 0 || val < 100) {
//        ui->slave_heartbeat_spinBox->setValue(100);
//    }
//}

// void Safety_Param_Dialog::on_pdo_interval_spinBox_editingFinished()
//{
//    int val = ui->pdo_interval_spinBox->value();
//    if (val > 0 || val < 100) {
//        ui->pdo_interval_spinBox->setValue(100);
//    }
//}

void Safety_Param_Dialog::on_apply_pushButton_clicked()
{
    config_data->name = ui->lineEdit_name->text();
    switch (config_data->config_param_data.model_iotype) {
    case MODEL_TYPE_INPUT:
        switch (config_data->config_param_data.model_type) {
        case MODEL_INPUT_AI:
            safe_param->sai_sample_interval = ui->ai_simple_interval_spinBox->value();
            safe_param->sai_allow_dif[0]    = ui->ai_full_spinbox->value();
            safe_param->sai_allow_dif[1]    = ui->ai_actual_spinbox->value();
            break;
        case MODEL_INPUT_PI:
            safe_param->spi_sample_interval = ui->pi_simple_interval_spinBox->value();
            safe_param->spi_allow_dif[0]    = ui->pi_full_spinBox->value();
            safe_param->spi_allow_dif[1]    = ui->pi_actual_spinBox->value();
            break;
        case MODEL_INPUT_QEP:
            switch (config_data->config_param_data.model_id) {
            case MODEL_ID_QEP1:
            case MODEL_ID_QEP2:
                safe_param->sqep_sample_interval   = ui->qep_simple_interval_spinBox->value();
                safe_param->sqep_allow_dif[0]      = ui->qep_full_spinBox->value();
                safe_param->sqep_allow_dif[1]      = ui->qep_actual_spinBox->value();
                safe_param->qep1_2_ratio           = ui->qep12_ratio_spinbox->value();
                safe_param->qep12_cross_check      = ui->qep12_cross_comboBox->currentIndex();
                safe_param->qep_speed_allow_dif[0] = ui->qep12_full_spinbox->value();
                safe_param->qep_speed_allow_dif[1] = ui->qep12_actual_spinbox->value();
                break;
            case MODEL_ID_PIQEP1:
            case MODEL_ID_PIQEP2:
                safe_param->pi_sqep_sample_interval  = ui->qep_simple_interval_spinBox->value();
                safe_param->pi_qep_allow_dif[0]      = ui->qep_full_spinBox->value();
                safe_param->pi_qep_allow_dif[1]      = ui->qep_actual_spinBox->value();
                safe_param->piqep1_2_ratio           = ui->qep12_ratio_spinbox->value();
                safe_param->piqep12_cross_check      = ui->qep12_cross_comboBox->currentIndex();
                safe_param->piqep_speed_allow_dif[0] = ui->qep12_full_spinbox->value();
                safe_param->piqep_speed_allow_dif[1] = ui->qep12_actual_spinbox->value();
                break;
            default:
                break;
            }
            break;
        case MODEL_INPUT_DI:
            break;
        default:
            break;
        }
        break;
    }
    this->close();
}

void Safety_Param_Dialog::on_cancle_pushButton_clicked()
{
    this->close();
}
