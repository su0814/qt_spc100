#include "speed_cross_check_setting.h"
#include "logic_view/speed_logic_block.h"
#include "ui_speed_cross_check_setting.h"
#include <QDebug>

speed_cross_check_setting::speed_cross_check_setting(speed_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::speed_cross_check_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui_init();
}

speed_cross_check_setting::~speed_cross_check_setting()
{
    delete ui;
}

void speed_cross_check_setting::ui_init()
{
    einputlabel.append(ui->label_einput1);
    einputlabel.append(ui->label_einput2);
    einputlabel.append(ui->label_einput3);
    einputname.append(ui->lineEdit_einput1);
    einputname.append(ui->lineEdit_einput2);
    einputname.append(ui->lineEdit_einput3);
    eoutputlabel.append(ui->label_eoutput1);
    eoutputlabel.append(ui->label_eoutput2);
    eoutputlabel.append(ui->label_eoutput3);
    eoutputname.append(ui->lineEdit_eoutput1);
    eoutputname.append(ui->lineEdit_eoutput2);
    eoutputname.append(ui->lineEdit_eoutput3);
    eoutputlabel[2]->setVisible(false);
    eoutputname[2]->setVisible(false);
}

void speed_cross_check_setting::setting_exec()
{
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    for (int i = 0; i < 3; i++) {
        einputname[i]->setText(inputname[i]);
        eoutputname[i]->setText(outputname[i]);
    }
    ui->comboBox_encoder_output->setCurrentIndex(baselogic->encoder_output_mode);
    ui->spinBox_full_percentage->setValue(baselogic->crosscheck_percentage[0]);
    ui->spinBox_actual_percentage->setValue(baselogic->crosscheck_percentage[1]);
    ui->spinBox_error_time->setValue(baselogic->error_keep_time);
    ui->spinBox_reliability_maxtime->setValue(baselogic->reliability_monitor_max_time);
    ui->spinBox_reliability_minspeed->setValue(baselogic->reliability_monitor_min_speed);
    ui->checkBox_reliability_monitor->setChecked(baselogic->encoder_reliability_monitor);
    exec();
}

void speed_cross_check_setting::on_checkBox_reliability_monitor_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    bool state = ui->checkBox_reliability_monitor->isChecked();
    ui->label_reliability_maxtime->setEnabled(state);
    ui->label_reliability_minspeed->setEnabled(state);
    ui->spinBox_reliability_maxtime->setEnabled(state);
    ui->spinBox_reliability_minspeed->setEnabled(state);
    ui->label_maxtime_unit->setEnabled(state);
    ui->label_minspeed_unit->setEnabled(state);
    if (state) {
        outputnum = 3;
        eoutputlabel[2]->setVisible(true);
        eoutputname[2]->setVisible(true);
    } else {
        outputnum = 3;
        eoutputlabel[2]->setVisible(false);
        eoutputname[2]->setVisible(false);
    }
}

void speed_cross_check_setting::on_pushButton_cancle_clicked()
{
    close();
}

void speed_cross_check_setting::on_pushButton_apply_clicked()
{
    QStringList inputname;
    QStringList outputname;
    for (int i = 0; i < 3; i++) {
        outputname.append(eoutputname[i]->text());
        inputname.append(einputname[i]->text());
    }
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    baselogic->set_output_num(outputnum);
    baselogic->encoder_output_mode           = ui->comboBox_encoder_output->currentIndex();
    baselogic->crosscheck_percentage[0]      = ui->spinBox_full_percentage->value();
    baselogic->crosscheck_percentage[1]      = ui->spinBox_actual_percentage->value();
    baselogic->error_keep_time               = ui->spinBox_error_time->value();
    baselogic->reliability_monitor_max_time  = ui->spinBox_reliability_maxtime->value();
    baselogic->reliability_monitor_min_speed = ui->spinBox_reliability_minspeed->value();
    baselogic->encoder_reliability_monitor   = ui->checkBox_reliability_monitor->isChecked();
    close();
}
