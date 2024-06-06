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
    module_param->can_master_nodeID          = ( uint8_t )0x01;
    module_param->can_slave_nodeID_A         = ( uint8_t )0x51;
    module_param->can_slave_nodeID_B         = ( uint8_t )0x52;
    module_param->can_hb_consumer_gap        = 0;
    module_param->can_hb_producer_gap        = 1000;
    module_param->ai_slv.ai_slv_byte         = 0x03;
    module_param->di_slv.di_slv_bytes        = 0xff;
    module_param->relay_slv.relay_slv_byte   = 0x03;
    module_param->work_state.work_state_byte = 0x35;
    module_param->sai_allow_dif[0]           = 1;
    module_param->sai_allow_dif[1]           = 3;
    module_param->spi_allow_dif[0]           = 1;
    module_param->spi_allow_dif[1]           = 3;
    module_param->pi_qep_allow_dif[0]        = 1;
    module_param->pi_qep_allow_dif[1]        = 3;
    module_param->sqep_allow_dif[0]          = 1;
    module_param->sqep_allow_dif[1]          = 3;
    module_param->sai_sample_interval        = 10;
    module_param->spi_sample_interval        = 10;
    module_param->pi_sqep_sample_interval    = 10;
    module_param->sqep_sample_interval       = 10;
    /*lua software version*/
    module_param->check_factor     = 1;
    module_param->can_baudrate     = 1000000;
    module_param->can_pdo_time_gap = 1000;
    module_param->qep1_pulse_num   = 1;
    module_param->qep1_distance    = 1;
    module_param->qep2_pulse_num   = 1;
    module_param->qep2_distance    = 1;
    module_param->piqep1_pulse_num = 1;
    module_param->piqep1_distance  = 1;
    module_param->piqep2_pulse_num = 1;
    module_param->piqep2_distance  = 1;
    mbedtls_md5(( const unsigned char* )module_param, sizeof(module_param_t) - sizeof(module_param->md5),
                module_param->md5);
}

QJsonObject Safety_Param_Dialog::param_project_info(module_param_t* module_param)
{
    QJsonObject rootObject;
    rootObject["baudrate"]       = ( int )module_param->can_baudrate;
    rootObject["anodeid"]        = module_param->can_slave_nodeID_A;
    rootObject["bnodeid"]        = module_param->can_slave_nodeID_B;
    rootObject["mnodeid"]        = module_param->can_master_nodeID;
    rootObject["checkbt"]        = module_param->can_hb_consumer_gap;
    rootObject["sendbt"]         = module_param->can_hb_producer_gap;
    rootObject["pdopt"]          = module_param->can_pdo_time_gap;
    rootObject["saisi"]          = module_param->sai_sample_interval;
    rootObject["spisi"]          = module_param->spi_sample_interval;
    rootObject["piqepsi"]        = module_param->pi_sqep_sample_interval;
    rootObject["sqepsi"]         = module_param->sqep_sample_interval;
    rootObject["saifull"]        = module_param->sai_allow_dif[0];
    rootObject["saiac"]          = module_param->sai_allow_dif[1];
    rootObject["spifull"]        = module_param->spi_allow_dif[0];
    rootObject["spiac"]          = module_param->spi_allow_dif[1];
    rootObject["piqepfull"]      = module_param->pi_qep_allow_dif[0];
    rootObject["piqepac"]        = module_param->pi_qep_allow_dif[1];
    rootObject["sqepfull"]       = module_param->sqep_allow_dif[0];
    rootObject["sqepac"]         = module_param->sqep_allow_dif[1];
    rootObject["crossc"]         = module_param->check_factor;
    rootObject["dilevel"]        = module_param->di_slv.di_slv_bytes;
    rootObject["ailevel"]        = module_param->ai_slv.ai_slv_byte;
    rootObject["relaylevel"]     = module_param->relay_slv.relay_slv_byte;
    rootObject["af"]             = module_param->work_state.work_state_byte;
    rootObject["qep1pulsenum"]   = module_param->qep1_pulse_num;
    rootObject["qep1distance"]   = module_param->qep1_distance;
    rootObject["qep2pulsenum"]   = module_param->qep2_pulse_num;
    rootObject["qep2distance"]   = module_param->qep2_distance;
    rootObject["piqep1pulsenum"] = module_param->piqep1_pulse_num;
    rootObject["piqep1distance"] = module_param->piqep1_distance;
    rootObject["piqep2pulsenum"] = module_param->piqep2_pulse_num;
    rootObject["piqep2distance"] = module_param->piqep2_distance;
    return rootObject;
}

bool Safety_Param_Dialog::param_project_parse(QJsonObject project, module_param_t* module_param)
{
    if (project.isEmpty()) {
        return false;
    }
    module_param->can_baudrate               = project["baudrate"].toInt();
    module_param->can_slave_nodeID_A         = project["anodeid"].toInt();
    module_param->can_slave_nodeID_B         = project["bnodeid"].toInt();
    module_param->can_master_nodeID          = project["mnodeid"].toInt();
    module_param->can_hb_consumer_gap        = project["checkbt"].toInt();
    module_param->can_hb_producer_gap        = project["sendbt"].toInt();
    module_param->can_pdo_time_gap           = project["pdopt"].toInt();
    module_param->sai_sample_interval        = project["saisi"].toInt();
    module_param->spi_sample_interval        = project["spisi"].toInt();
    module_param->pi_sqep_sample_interval    = project["piqepsi"].toInt();
    module_param->sqep_sample_interval       = project["sqepsi"].toInt();
    module_param->sai_allow_dif[0]           = project["saifull"].toInt();
    module_param->sai_allow_dif[1]           = project["saiac"].toInt();
    module_param->spi_allow_dif[0]           = project["spifull"].toInt();
    module_param->spi_allow_dif[1]           = project["spiac"].toInt();
    module_param->pi_qep_allow_dif[0]        = project["piqepfull"].toInt();
    module_param->pi_qep_allow_dif[1]        = project["piqepac"].toInt();
    module_param->sqep_allow_dif[0]          = project["sqepfull"].toInt();
    module_param->sqep_allow_dif[1]          = project["sqepac"].toInt();
    module_param->check_factor               = project["crossc"].toInt();
    module_param->di_slv.di_slv_bytes        = project["dilevel"].toInt();
    module_param->ai_slv.ai_slv_byte         = project["ailevel"].toInt();
    module_param->relay_slv.relay_slv_byte   = project["relaylevel"].toInt();
    module_param->work_state.work_state_byte = project["af"].toInt();
    module_param->qep1_pulse_num             = project["qep1pulsenum"].toInt();
    module_param->qep1_distance              = project["qep1distance"].toInt();
    module_param->qep2_pulse_num             = project["qep2pulsenum"].toInt();
    module_param->qep2_distance              = project["qep2distance"].toInt();
    module_param->piqep1_pulse_num           = project["piqep1pulsenum"].toInt();
    module_param->piqep1_distance            = project["piqep1distance"].toInt();
    module_param->piqep2_pulse_num           = project["piqep2pulsenum"].toInt();
    module_param->piqep2_distance            = project["piqep2distance"].toInt();
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
            ui->stackedWidget->setVisible(true);
            if (config_data->safe_level == SAFE_LEVEL_CAT3) {
                ui->groupBox_ai_sync->setVisible(true);
                ui->line_ai->setVisible(true);
                this->setFixedSize(200, 180);
            } else {
                ui->groupBox_ai_sync->setVisible(false);
                ui->line_ai->setVisible(false);
                this->setFixedSize(200, 130);
            }
            ui->stackedWidget->setCurrentWidget(ui->page_ai);
            ui->ai_simple_interval_spinBox->setValue(safe_param->sai_sample_interval);
            ui->ai_full_spinbox->setValue(safe_param->sai_allow_dif[0]);
            ui->ai_actual_spinbox->setValue(safe_param->sai_allow_dif[1]);
            break;

        case MODEL_INPUT_PI:
            ui->stackedWidget->setVisible(true);
            if (config_data->safe_level == SAFE_LEVEL_CAT3) {
                ui->groupBox_pi_sync->setVisible(true);
                ui->line_pi->setVisible(true);
                this->setFixedSize(200, 180);
            } else {
                ui->groupBox_pi_sync->setVisible(false);
                ui->line_pi->setVisible(false);
                this->setFixedSize(200, 130);
            }
            ui->stackedWidget->setCurrentWidget(ui->page_pi);
            ui->pi_simple_interval_spinBox->setValue(safe_param->spi_sample_interval);
            ui->pi_full_spinBox->setValue(safe_param->spi_allow_dif[0]);
            ui->pi_actual_spinBox->setValue(safe_param->spi_allow_dif[1]);
            break;
        case MODEL_INPUT_QEP:
            ui->stackedWidget->setVisible(true);
            ui->stackedWidget->setCurrentWidget(ui->page_encode);
            this->setFixedSize(245, 240);
            switch (config_data->config_param_data.model_id) {
            case MODEL_ID_QEP1:
                ui->qep_simple_interval_spinBox->setValue(safe_param->sqep_sample_interval);
                ui->qep_full_spinBox->setValue(safe_param->sqep_allow_dif[0]);
                ui->qep_actual_spinBox->setValue(safe_param->sqep_allow_dif[1]);
                ui->spinBox_pulse_num->setValue(safe_param->qep1_pulse_num);
                ui->spinBox_distance->setValue(safe_param->qep1_distance);
                break;
            case MODEL_ID_QEP2:
                ui->qep_simple_interval_spinBox->setValue(safe_param->sqep_sample_interval);
                ui->qep_full_spinBox->setValue(safe_param->sqep_allow_dif[0]);
                ui->qep_actual_spinBox->setValue(safe_param->sqep_allow_dif[1]);
                ui->spinBox_pulse_num->setValue(safe_param->qep2_pulse_num);
                ui->spinBox_distance->setValue(safe_param->qep2_distance);
                break;
            case MODEL_ID_PIQEP1:
                ui->qep_simple_interval_spinBox->setValue(safe_param->pi_sqep_sample_interval);
                ui->qep_full_spinBox->setValue(safe_param->pi_qep_allow_dif[0]);
                ui->qep_actual_spinBox->setValue(safe_param->pi_qep_allow_dif[1]);
                ui->spinBox_pulse_num->setValue(safe_param->piqep1_pulse_num);
                ui->spinBox_distance->setValue(safe_param->piqep1_distance);
                break;
            case MODEL_ID_PIQEP2:
                ui->qep_simple_interval_spinBox->setValue(safe_param->pi_sqep_sample_interval);
                ui->qep_full_spinBox->setValue(safe_param->pi_qep_allow_dif[0]);
                ui->qep_actual_spinBox->setValue(safe_param->pi_qep_allow_dif[1]);
                ui->spinBox_pulse_num->setValue(safe_param->piqep2_pulse_num);
                ui->spinBox_distance->setValue(safe_param->piqep2_distance);
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
                safe_param->sqep_sample_interval = ui->qep_simple_interval_spinBox->value();
                safe_param->sqep_allow_dif[0]    = ui->qep_full_spinBox->value();
                safe_param->sqep_allow_dif[1]    = ui->qep_actual_spinBox->value();
                safe_param->qep1_pulse_num       = ui->spinBox_pulse_num->value();
                safe_param->qep1_distance        = ui->spinBox_distance->value();
                break;
            case MODEL_ID_QEP2:
                safe_param->sqep_sample_interval = ui->qep_simple_interval_spinBox->value();
                safe_param->sqep_allow_dif[0]    = ui->qep_full_spinBox->value();
                safe_param->sqep_allow_dif[1]    = ui->qep_actual_spinBox->value();
                safe_param->qep2_pulse_num       = ui->spinBox_pulse_num->value();
                safe_param->qep2_distance        = ui->spinBox_distance->value();
                break;
            case MODEL_ID_PIQEP1:
                safe_param->pi_sqep_sample_interval = ui->qep_simple_interval_spinBox->value();
                safe_param->pi_qep_allow_dif[0]     = ui->qep_full_spinBox->value();
                safe_param->pi_qep_allow_dif[1]     = ui->qep_actual_spinBox->value();
                safe_param->piqep1_pulse_num        = ui->spinBox_pulse_num->value();
                safe_param->piqep1_distance         = ui->spinBox_distance->value();
                break;
            case MODEL_ID_PIQEP2:
                safe_param->pi_sqep_sample_interval = ui->qep_simple_interval_spinBox->value();
                safe_param->pi_qep_allow_dif[0]     = ui->qep_full_spinBox->value();
                safe_param->pi_qep_allow_dif[1]     = ui->qep_actual_spinBox->value();
                safe_param->piqep2_pulse_num        = ui->spinBox_pulse_num->value();
                safe_param->piqep2_distance         = ui->spinBox_distance->value();
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
