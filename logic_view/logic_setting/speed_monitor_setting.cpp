#include "speed_monitor_setting.h"
#include "logic_view/speed_logic_block.h"
#include "ui_speed_monitor_setting.h"
#include <QDebug>

speed_monitor_setting::speed_monitor_setting(speed_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::speed_monitor_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui_init();
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

speed_monitor_setting::~speed_monitor_setting()
{
    delete ui;
}

void speed_monitor_setting::ui_init()
{
    einputname.append(ui->lineEdit_einput1);
    einputname.append(ui->lineEdit_einput2);
    einputname.append(ui->lineEdit_einput3);
    einputname.append(ui->lineEdit_einput4);
    einputname.append(ui->lineEdit_einput5);
    ramplabel.append(ui->label_ramp1);
    ramplabel.append(ui->label_ramp2);
    ramplabel.append(ui->label_ramp3);
    ramplabel.append(ui->label_ramp4);
    ramp_monitor_group.append(ui->groupBox_ramp_monitor1);
    ramp_monitor_group.append(ui->groupBox_ramp_monitor2);
    ramp_monitor_group.append(ui->groupBox_ramp_monitor3);
    ramp_monitor_group.append(ui->groupBox_ramp_monitor4);
    ramp_time.append(ui->spinBox_ramp_time);
    ramp_time.append(ui->spinBox_ramp_time_2);
    ramp_time.append(ui->spinBox_ramp_time_3);
    ramp_time.append(ui->spinBox_ramp_time_4);
    ramp_speed.append(ui->spinBox_ramp_speed);
    ramp_speed.append(ui->spinBox_ramp_speed_2);
    ramp_speed.append(ui->spinBox_ramp_speed_3);
    ramp_speed.append(ui->spinBox_ramp_speed_4);
    ramp_max_speed.append(ui->spinBox_ramp_max_speed);
    ramp_max_speed.append(ui->spinBox_ramp_max_speed_2);
    ramp_max_speed.append(ui->spinBox_ramp_max_speed_3);
    ramp_max_speed.append(ui->spinBox_ramp_max_speed_4);
    ramp_min_speed.append(ui->spinBox_ramp_min_speed);
    ramp_min_speed.append(ui->spinBox_ramp_min_speed_2);
    ramp_min_speed.append(ui->spinBox_ramp_min_speed_3);
    ramp_min_speed.append(ui->spinBox_ramp_min_speed_4);
    ui->label_einput5->setVisible(false);
    ui->lineEdit_einput5->setVisible(false);
    error_info.append("");
    error_info.append("");
    error_info.append("");
    error_info.append("");
}

void speed_monitor_setting::setting_exec()
{
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    ui->lineEdit_eoutput1->setText(outputname[0]);
    for (int i = 0; i < 5; i++) {
        einputname[i]->setText(inputname[i]);
    }
    ui->verticalSlider_rampnum->setValue(baselogic->ramp_num);
    ui->spinBox_ramp_delaytime->setValue(baselogic->ramp_delay_time);

    for (int i = 0; i < 4; i++) {
        ramp_time[i]->setValue(baselogic->ramp_time[i]);
        ramp_speed[i]->setValue(baselogic->ramp_speed[i]);
        ramp_max_speed[i]->setValue(baselogic->ramp_max_speed[i]);
        ramp_min_speed[i]->setValue(baselogic->ramp_min_speed[i]);
    }

    exec();
}

void speed_monitor_setting::ramp_value_check(int id)
{
    bool error = false;
    if (ramp_min_speed[id]->value() >= ramp_max_speed[id]->value()) {
        error_info[id] = "减速检测" + QString::number(id + 1) + ":" + "最大安全速度应大于最小安全速度";
        error          = true;
        ramp_min_speed[id]->setStyleSheet("color: red;");
        ramp_max_speed[id]->setStyleSheet("color: red;");
    } else if (ramp_speed[id]->value() > (ramp_max_speed[id]->value() - ramp_min_speed[id]->value())) {
        error_info[id] = "减速检测" + QString::number(id + 1) + ":" + "减速阈值不应大于(最大安全速度-最小安全速度)";
        error = true;
        ramp_min_speed[id]->setStyleSheet("");
        ramp_max_speed[id]->setStyleSheet("");
        ramp_speed[id]->setStyleSheet("color: red;");
    } else {
        ramp_min_speed[id]->setStyleSheet("");
        ramp_max_speed[id]->setStyleSheet("");
        ramp_speed[id]->setStyleSheet("");
        error_info[id] = "";
    }
    if (error) {
        error_flag |= (0x01 << id);
    } else {
        error_flag &= ~(0x01 << id);
    }
    if (error_flag != 0) {
        ui->pushButton_apply->setEnabled(false);
    } else {
        ui->pushButton_apply->setEnabled(true);
    }
    QString error_info_label = "";
    for (int i = 0; i < 4; i++) {
        if (error_info[i] != "") {
            error_info_label += "\r\n" + error_info[i];
        }
    }
    ui->label_error_info->setText(error_info_label);
}

void speed_monitor_setting::on_spinBox_ramp_speed_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ramp_value_check(0);
}

void speed_monitor_setting::on_spinBox_ramp_max_speed_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ramp_value_check(0);
}

void speed_monitor_setting::on_spinBox_ramp_min_speed_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ramp_value_check(0);
}

void speed_monitor_setting::on_spinBox_ramp_speed_2_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ramp_value_check(1);
}

void speed_monitor_setting::on_spinBox_ramp_max_speed_2_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ramp_value_check(1);
}

void speed_monitor_setting::on_spinBox_ramp_min_speed_2_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ramp_value_check(1);
}

void speed_monitor_setting::on_spinBox_ramp_speed_3_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ramp_value_check(2);
}

void speed_monitor_setting::on_spinBox_ramp_max_speed_3_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ramp_value_check(2);
}

void speed_monitor_setting::on_spinBox_ramp_min_speed_3_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ramp_value_check(2);
}

void speed_monitor_setting::on_spinBox_ramp_speed_4_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ramp_value_check(3);
}

void speed_monitor_setting::on_spinBox_ramp_max_speed_4_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ramp_value_check(3);
}

void speed_monitor_setting::on_spinBox_ramp_min_speed_4_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    ramp_value_check(3);
}

void speed_monitor_setting::on_verticalSlider_rampnum_valueChanged(int value)
{
    for (int i = 0; i < 4; i++) {
        if (i < value) {
            ramplabel[i]->setEnabled(true);
            ramp_monitor_group[i]->setEnabled(true);
        } else {
            ramplabel[i]->setEnabled(false);
            ramp_monitor_group[i]->setEnabled(false);
        }
    }
    if (value >= 3) {
        ui->label_einput5->setVisible(true);
        ui->lineEdit_einput5->setVisible(true);
        inputnum = 5;
    } else {
        ui->label_einput5->setVisible(false);
        ui->lineEdit_einput5->setVisible(false);
        inputnum = 4;
    }
}

void speed_monitor_setting::on_pushButton_cancle_clicked()
{
    close();
}

void speed_monitor_setting::on_pushButton_apply_clicked()
{
    QStringList inputname;
    QStringList outputname;
    outputname.append(ui->lineEdit_eoutput1->text());
    for (int i = 0; i < 5; i++) {
        inputname.append(einputname[i]->text());
    }
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    baselogic->ramp_num = ui->verticalSlider_rampnum->value();
    baselogic->set_input_num(inputnum);
    baselogic->ramp_delay_time = ui->spinBox_ramp_delaytime->value();
    for (int i = 0; i < 4; i++) {
        baselogic->ramp_time[i]      = ramp_time[i]->value();
        baselogic->ramp_speed[i]     = ramp_speed[i]->value();
        baselogic->ramp_max_speed[i] = ramp_max_speed[i]->value();
        baselogic->ramp_min_speed[i] = ramp_min_speed[i]->value();
    }
    close();
}
