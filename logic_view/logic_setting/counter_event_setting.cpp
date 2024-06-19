#include "counter_event_setting.h"
#include "logic_view/delay_counter_logic_block.h"
#include "ui_counter_event_setting.h"

counter_event_setting::counter_event_setting(delay_counter_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::counter_event_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui_init();
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

counter_event_setting::~counter_event_setting()
{
    delete ui;
}

void counter_event_setting::ui_init()
{
    einputname.append(ui->lineEdit_einput1);
    einputname.append(ui->lineEdit_einput2);
    einputname.append(ui->lineEdit_einput3);
    einputname.append(ui->lineEdit_einput4);
    eoutputname.append(ui->lineEdit_eoutput1);
    eoutputname.append(ui->lineEdit_eoutput2);
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

void counter_event_setting::setting_exec()
{
    block_base_param       = baselogic->get_block_base_param();
    old_param              = baselogic->block_param_info();
    counter_event_param    = baselogic->counter_event_param;
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    for (int i = 0; i < 4; i++) {
        einputname[i]->setText(inputname[i]);
    }
    for (int i = 0; i < 2; i++) {
        eoutputname[i]->setText(outputname[i]);
    }
    ui->comboBox_upover_reset_mode->setCurrentIndex(
        baselogic->counter_event_param.event_counter_param[UPOVER_RESET_MODE]);
    ui->comboBox_downover_reset_mode->setCurrentIndex(
        baselogic->counter_event_param.event_counter_param[DOWNOVER_RESET_MODE]);
    ui->spinBox_upover_value->setValue(baselogic->counter_event_param.event_counter_param[UPOVER_VALUE]);
    ui->spinBox_start_value->setValue(baselogic->counter_event_param.event_counter_param[START_VALUE]);
    ui->comboBox_reset_to_zero_pulse_time->setCurrentText(
        QString::number(baselogic->counter_event_param.event_counter_param[RESET_ZERO_PULSE_TIME]));
    ui->comboBox_reset_to_start_pulse_time->setCurrentText(
        QString::number(baselogic->counter_event_param.event_counter_param[RESET_START_PULSE_TIME]));
    ui->spinBox_over_keep_time->setValue(baselogic->counter_event_param.event_counter_param[RESET_KEEP_TIME] / 10);
    exec();
}

void counter_event_setting::on_comboBox_upover_reset_mode_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if (ui->comboBox_upover_reset_mode->currentIndex() == 0 || ui->comboBox_downover_reset_mode->currentIndex() == 0) {
        ui->label_reset_keeptime_name->setEnabled(true);
        ui->label_reset_keeptime_result->setEnabled(true);
        ui->label_reset_keeptime_unit->setEnabled(true);
        ui->spinBox_over_keep_time->setEnabled(true);
    } else {
        ui->label_reset_keeptime_name->setEnabled(false);
        ui->label_reset_keeptime_result->setEnabled(false);
        ui->label_reset_keeptime_unit->setEnabled(false);
        ui->spinBox_over_keep_time->setEnabled(false);
    }
}

void counter_event_setting::on_comboBox_downover_reset_mode_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if (ui->comboBox_upover_reset_mode->currentIndex() == 0 || ui->comboBox_downover_reset_mode->currentIndex() == 0) {
        ui->label_reset_keeptime_name->setEnabled(true);
        ui->label_reset_keeptime_result->setEnabled(true);
        ui->label_reset_keeptime_unit->setEnabled(true);
        ui->spinBox_over_keep_time->setEnabled(true);
    } else {
        ui->label_reset_keeptime_name->setEnabled(false);
        ui->label_reset_keeptime_result->setEnabled(false);
        ui->label_reset_keeptime_unit->setEnabled(false);
        ui->spinBox_over_keep_time->setEnabled(false);
    }
}

void counter_event_setting::on_spinBox_over_keep_time_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ui->label_reset_keeptime_result->setText(QString::number(ui->spinBox_over_keep_time->value() * 10) + "ms");
}

void counter_event_setting::on_pushButton_cancle_clicked()
{
    close();
}

void counter_event_setting::on_pushButton_apply_clicked()
{
    QStringList inputname;
    QStringList outputname;
    for (int i = 0; i < 4; i++) {
        inputname.append(einputname[i]->text());
    }
    for (int i = 0; i < 2; i++) {
        outputname.append(eoutputname[i]->text());
    }
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    baselogic->counter_event_param.event_counter_param[UPOVER_RESET_MODE] =
        ui->comboBox_upover_reset_mode->currentIndex();
    baselogic->counter_event_param.event_counter_param[DOWNOVER_RESET_MODE] =
        ui->comboBox_downover_reset_mode->currentIndex();
    baselogic->counter_event_param.event_counter_param[UPOVER_VALUE] = ui->spinBox_upover_value->value();
    baselogic->counter_event_param.event_counter_param[START_VALUE]  = ui->spinBox_start_value->value();
    baselogic->counter_event_param.event_counter_param[RESET_ZERO_PULSE_TIME] =
        ui->comboBox_reset_to_zero_pulse_time->currentText().toInt();
    baselogic->counter_event_param.event_counter_param[RESET_START_PULSE_TIME] =
        ui->comboBox_reset_to_start_pulse_time->currentText().toInt();
    baselogic->counter_event_param.event_counter_param[RESET_KEEP_TIME] = ui->spinBox_over_keep_time->value() * 10;
    if (!(block_base_param == baselogic->get_block_base_param())
        || !(counter_event_param == baselogic->counter_event_param)) {
        baselogic->set_block_old_param(old_param);
        baselogic->send_param_change_signal();
    }
    close();
}

void counter_event_setting::on_spinBox_upover_value_valueChanged(int arg1)
{
    if (arg1 < ui->spinBox_start_value->value()) {
        ui->label_error_info->setText("重载值不得大于上溢溢出值");
        ui->pushButton_apply->setEnabled(false);
    } else {
        ui->label_error_info->setText("");
        ui->pushButton_apply->setEnabled(true);
    }
}

void counter_event_setting::on_spinBox_start_value_valueChanged(int arg1)
{
    if (arg1 > ui->spinBox_upover_value->value()) {
        ui->label_error_info->setText("重载值不得大于上溢溢出值");
        ui->pushButton_apply->setEnabled(false);
    } else {
        ui->label_error_info->setText("");
        ui->pushButton_apply->setEnabled(true);
    }
}
