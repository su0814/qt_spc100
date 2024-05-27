#include "apply_freq_monitor_setting.h"
#include "logic_view/apply_logic_block.h"
#include "ui_apply_freq_monitor_setting.h"

apply_freq_monitor_setting::apply_freq_monitor_setting(apply_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::apply_freq_monitor_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui_init();
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

apply_freq_monitor_setting::~apply_freq_monitor_setting()
{
    delete ui;
}

void apply_freq_monitor_setting::ui_init()
{
    paramname.append(ui->label_mincycle_name);
    paramname.append(ui->label_maxcycle_name);
    paramname.append(ui->label_pulsetime_name);
    paramname.append(ui->label_tolerance_name);
    paramfold.append(ui->label_mincycle_fold);
    paramfold.append(ui->label_maxcycle_fold);
    paramfold.append(ui->label_pulsetime_fold);
    paramfold.append(ui->label_tolerance_fold);
    paramvalue.append(ui->spinBox_min_cycle2);
    paramvalue.append(ui->spinBox_max_cycle2);
    paramvalue.append(ui->spinBox_pulse_time2);
    paramvalue.append(ui->spinBox_pulse_time_tolerance2);
    paramresult.append(ui->label_min_cycle_result2);
    paramresult.append(ui->label_max_cycle_result2);
    paramresult.append(ui->label_pulse_time_result2);
    paramresult.append(ui->label_pulse_time_tolerance_result2);
    einputlabel.append(ui->label_einput1);
    einputlabel.append(ui->label_einput2);
    einputname.append(ui->lineEdit_einput1);
    einputname.append(ui->lineEdit_einput2);
    eoutputlabel.append(ui->label_eoutput1);
    eoutputlabel.append(ui->label_eoutput2);
    eoutputlabel.append(ui->label_eoutput3);
    eoutputlabel.append(ui->label_eoutput4);
    eoutputlabel.append(ui->label_eoutput5);
    eoutputlabel.append(ui->label_eoutput6);
    eoutputlabel.append(ui->label_eoutput7);
    eoutputname.append(ui->lineEdit_eoutput1);
    eoutputname.append(ui->lineEdit_eoutput2);
    eoutputname.append(ui->lineEdit_eoutput3);
    eoutputname.append(ui->lineEdit_eoutput4);
    eoutputname.append(ui->lineEdit_eoutput5);
    eoutputname.append(ui->lineEdit_eoutput6);
    eoutputname.append(ui->lineEdit_eoutput7);
    sys_output_label.append("输出1");
    sys_output_label.append("频率1错误");
    sys_output_label.append("频率1持续高");
    sys_output_label.append("输出2");
    sys_output_label.append("频率2错误");
    sys_output_label.append("频率2持续高");
    sys_output_label.append("故障");
}

void apply_freq_monitor_setting::freq1_param_detected()
{
    int mincycle  = ui->spinBox_min_cycle1->value();
    int maxcycle  = ui->spinBox_max_cycle1->value();
    int pulsetime = ui->spinBox_pulse_time1->value();
    int tolerance = ui->spinBox_pulse_time_tolerance1->value();
    if (mincycle >= maxcycle) {
        ui->label_freq_error1->setText("最小周期时间应小于最大周期时间");
        ui->pushButton_apply->setEnabled(false);
        return;
    }
    if (pulsetime + tolerance >= mincycle) {
        ui->label_freq_error1->setText("脉冲时间与脉冲时间容差的和应小于最小周期时间");
        ui->pushButton_apply->setEnabled(false);
        return;
    }
    if (tolerance >= pulsetime && tolerance != 0) {
        ui->label_freq_error1->setText("脉冲时间容差应小于脉冲时间");
        ui->pushButton_apply->setEnabled(false);
        return;
    }
    ui->label_freq_error1->setText("");
    if (ui->label_freq_error2->text() == "") {
        ui->pushButton_apply->setEnabled(true);
    }
}

void apply_freq_monitor_setting::freq2_param_detected()
{
    int mincycle  = ui->spinBox_min_cycle2->value();
    int maxcycle  = ui->spinBox_max_cycle2->value();
    int pulsetime = ui->spinBox_pulse_time2->value();
    int tolerance = ui->spinBox_pulse_time_tolerance2->value();
    if (mincycle >= maxcycle) {
        ui->label_freq_error2->setText("最小周期时间应小于最大周期时间");
        ui->pushButton_apply->setEnabled(false);
        return;
    }
    if (pulsetime + tolerance >= mincycle) {
        ui->label_freq_error2->setText("脉冲时间与脉冲时间容差的和应小于最小周期时间");
        ui->pushButton_apply->setEnabled(false);
        return;
    }
    if (tolerance >= pulsetime && tolerance != 0) {
        ui->label_freq_error2->setText("脉冲时间容差应小于脉冲时间");
        ui->pushButton_apply->setEnabled(false);
        return;
    }
    ui->label_freq_error2->setText("");
    if (ui->label_freq_error1->text() == "") {
        ui->pushButton_apply->setEnabled(true);
    }
}

void apply_freq_monitor_setting::set_outputnum(int num, bool fault)
{
    outputnum = num;
    logic_output_label.clear();
    for (int i = 0; i < 7; i++) {
        if (i < num) {
            eoutputlabel[i]->setVisible(true);
            eoutputname[i]->setVisible(true);
            eoutputlabel[i]->setText(sys_output_label[i]);
            logic_output_label.append(sys_output_label[i]);
        } else {
            eoutputlabel[i]->setVisible(false);
            eoutputname[i]->setVisible(false);
        }
    }
    if (fault) {
        eoutputlabel[num]->setVisible(true);
        eoutputname[num]->setVisible(true);
        eoutputlabel[num]->setText("故障");
        logic_output_label.append("故障");
    }
}

void apply_freq_monitor_setting::setting_exec()
{
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    int         outnum     = baselogic->get_output_point_num();
    for (int i = 0; i < 2; i++) {
        einputname[i]->setText(inputname[i]);
    }
    for (int i = 0; i < 7; i++) {
        eoutputname[i]->setText(outputname[i]);
    }
    if (baselogic->fault_output) {
        outnum -= 1;
    }
    set_outputnum(outnum, baselogic->fault_output);
    ui->checkBox_fault->blockSignals(true);
    ui->checkBox_fault->setChecked(baselogic->fault_output);
    ui->checkBox_fault->blockSignals(false);
    ui->spinBox_min_cycle1->blockSignals(true);
    ui->spinBox_max_cycle1->blockSignals(true);
    ui->spinBox_pulse_time1->blockSignals(true);
    ui->spinBox_pulse_time_tolerance1->blockSignals(true);
    ui->spinBox_min_cycle2->blockSignals(true);
    ui->spinBox_max_cycle2->blockSignals(true);
    ui->spinBox_pulse_time2->blockSignals(true);
    ui->spinBox_pulse_time_tolerance2->blockSignals(true);
    ui->spinBox_min_cycle1->setValue(baselogic->freq_param[0] / 10);
    ui->spinBox_max_cycle1->setValue(baselogic->freq_param[1] / 10);
    ui->spinBox_pulse_time1->setValue(baselogic->freq_param[2] / 10);
    ui->spinBox_pulse_time_tolerance1->setValue(baselogic->freq_param[3] / 10);
    ui->spinBox_min_cycle2->setValue(baselogic->freq_param[4] / 10);
    ui->spinBox_max_cycle2->setValue(baselogic->freq_param[5] / 10);
    ui->spinBox_pulse_time2->setValue(baselogic->freq_param[6] / 10);
    ui->spinBox_pulse_time_tolerance2->setValue(baselogic->freq_param[7] / 10);
    ui->spinBox_min_cycle1->blockSignals(false);
    ui->spinBox_max_cycle1->blockSignals(false);
    ui->spinBox_pulse_time1->blockSignals(false);
    ui->spinBox_pulse_time_tolerance1->blockSignals(false);
    ui->spinBox_min_cycle2->blockSignals(false);
    ui->spinBox_max_cycle2->blockSignals(false);
    ui->spinBox_pulse_time2->blockSignals(false);
    ui->spinBox_pulse_time_tolerance2->blockSignals(false);
    ui->checkBox_freq2_enable->setChecked(baselogic->freq_enable);
    exec();
}

void apply_freq_monitor_setting::on_spinBox_min_cycle1_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    freq1_param_detected();
    ui->label_min_cycle_result1->setText(QString::number(arg1 * 10) + " ms");
}

void apply_freq_monitor_setting::on_spinBox_max_cycle1_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    freq1_param_detected();
    ui->label_max_cycle_result1->setText(QString::number(arg1 * 10) + " ms");
}

void apply_freq_monitor_setting::on_spinBox_pulse_time1_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    freq1_param_detected();
    ui->label_pulse_time_result1->setText(QString::number(arg1 * 10) + " ms");
    if (arg1 == 0) {
        ui->spinBox_pulse_time_tolerance1->setValue(0);
    }
}

void apply_freq_monitor_setting::on_spinBox_pulse_time_tolerance1_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    freq1_param_detected();
    ui->label_pulse_time_tolerance_result1->setText(QString::number(arg1 * 10) + " ms");
}

void apply_freq_monitor_setting::on_spinBox_min_cycle2_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    freq2_param_detected();
    ui->label_min_cycle_result2->setText(QString::number(arg1 * 10) + " ms");
}

void apply_freq_monitor_setting::on_spinBox_max_cycle2_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    freq2_param_detected();
    ui->label_max_cycle_result2->setText(QString::number(arg1 * 10) + " ms");
}

void apply_freq_monitor_setting::on_spinBox_pulse_time2_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    freq2_param_detected();
    ui->label_pulse_time_result2->setText(QString::number(arg1 * 10) + " ms");
    if (arg1 == 0) {
        ui->spinBox_pulse_time_tolerance2->setValue(0);
    }
}

void apply_freq_monitor_setting::on_spinBox_pulse_time_tolerance2_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    freq2_param_detected();
    ui->label_pulse_time_tolerance_result2->setText(QString::number(arg1 * 10) + " ms");
}

void apply_freq_monitor_setting::on_checkBox_fault_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    set_outputnum(outputnum, ui->checkBox_fault->isChecked());
}

void apply_freq_monitor_setting::on_checkBox_freq2_enable_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    bool state = ui->checkBox_freq2_enable->isChecked();
    for (int i = 0; i < 4; i++) {
        paramname[i]->setEnabled(state);
        paramfold[i]->setEnabled(state);
        paramvalue[i]->setEnabled(state);
        paramresult[i]->setEnabled(state);
    }
    if (state) {
        set_outputnum(6, ui->checkBox_fault->isChecked());
    } else {
        set_outputnum(3, ui->checkBox_fault->isChecked());
    }
}

void apply_freq_monitor_setting::on_pushButton_cancle_clicked()
{
    close();
}

void apply_freq_monitor_setting::on_pushButton_apply_clicked()
{
    if (ui->checkBox_freq2_enable->isChecked()) {
        baselogic->set_input_num(2);
    } else {
        baselogic->set_input_num(1);
    }
    QStringList inputname;
    QStringList outputname;
    for (int i = 0; i < 2; i++) {
        inputname.append(einputname[i]->text());
    }
    for (int i = 0; i < 7; i++) {
        outputname.append(eoutputname[i]->text());
    }
    baselogic->fault_output = ui->checkBox_fault->isChecked();
    if (baselogic->fault_output) {
        baselogic->set_output_num(outputnum + 1);
    } else {
        baselogic->set_output_num(outputnum);
    }
    baselogic->set_sys_outputpoint_labels(logic_output_label);
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    baselogic->freq_enable   = ui->checkBox_freq2_enable->isChecked();
    baselogic->freq_param[0] = ui->spinBox_min_cycle1->value() * 10;
    baselogic->freq_param[1] = ui->spinBox_max_cycle1->value() * 10;
    baselogic->freq_param[2] = ui->spinBox_pulse_time1->value() * 10;
    baselogic->freq_param[3] = ui->spinBox_pulse_time_tolerance1->value() * 10;
    baselogic->freq_param[4] = ui->spinBox_min_cycle2->value() * 10;
    baselogic->freq_param[5] = ui->spinBox_max_cycle2->value() * 10;
    baselogic->freq_param[6] = ui->spinBox_pulse_time2->value() * 10;
    baselogic->freq_param[7] = ui->spinBox_pulse_time_tolerance2->value() * 10;
    close();
}
