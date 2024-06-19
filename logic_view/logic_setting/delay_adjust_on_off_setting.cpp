#include "delay_adjust_on_off_setting.h"
#include "logic_view/delay_counter_logic_block.h"
#include "ui_delay_adjust_on_off_setting.h"

delay_adjust_on_off_setting::delay_adjust_on_off_setting(delay_counter_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::delay_adjust_on_off_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui_init();
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

delay_adjust_on_off_setting::~delay_adjust_on_off_setting()
{
    delete ui;
}

void delay_adjust_on_off_setting::ui_init()
{
    delaytime_name.append(ui->label_delayname1);
    delaytime_name.append(ui->label_delayname2);
    delaytime_name.append(ui->label_delayname3);
    delaytime_name.append(ui->label_delayname4);
    delaytime_value.append(ui->spinBox_delay_time1);
    delaytime_value.append(ui->spinBox_delay_time2);
    delaytime_value.append(ui->spinBox_delay_time3);
    delaytime_value.append(ui->spinBox_delay_time4);
    delaytime_fold.append(ui->label_delaytime_fold1);
    delaytime_fold.append(ui->label_delaytime_fold2);
    delaytime_fold.append(ui->label_delaytime_fold3);
    delaytime_fold.append(ui->label_delaytime_fold4);
    delaytime_result.append(ui->label_time1);
    delaytime_result.append(ui->label_time2);
    delaytime_result.append(ui->label_time3);
    delaytime_result.append(ui->label_time4);
    einputlabel.append(ui->label_einput1);
    einputlabel.append(ui->label_einput2);
    einputlabel.append(ui->label_einput3);
    einputlabel.append(ui->label_einput4);
    einputlabel.append(ui->label_einput5);
    einputname.append(ui->lineEdit_einput1);
    einputname.append(ui->lineEdit_einput2);
    einputname.append(ui->lineEdit_einput3);
    einputname.append(ui->lineEdit_einput4);
    einputname.append(ui->lineEdit_einput5);
}

void delay_adjust_on_off_setting::setting_exec()
{
    block_base_param          = baselogic->get_block_base_param();
    old_param                 = baselogic->block_param_info();
    delay_adjust_on_off_param = baselogic->delay_adjust_on_off_param;
    QStringList outputname    = baselogic->get_user_outputpoint_labels();
    QStringList inputname     = baselogic->get_user_inputpoint_labels();
    for (int i = 0; i < 5; i++) {
        einputname[i]->setText(inputname[i]);
    }
    ui->lineEdit_eoutput1->setText(outputname[0]);
    ui->verticalSlider_inputnum->setValue(baselogic->get_input_point_num() - 1);
    for (int i = 0; i < 4; i++) {
        delaytime_value[i]->setValue(baselogic->delay_adjust_on_off_param.adjust_on_off_delay_time[i] / 10);
    }
    exec();
}

void delay_adjust_on_off_setting::on_verticalSlider_inputnum_valueChanged(int value)
{

    for (int i = 0; i < 4; i++) {
        if (i < value) {
            delaytime_name[i]->setEnabled(true);
            delaytime_value[i]->setEnabled(true);
            delaytime_fold[i]->setEnabled(true);
            delaytime_result[i]->setEnabled(true);
            einputlabel[i + 1]->setVisible(true);
            einputname[i + 1]->setVisible(true);
        } else {
            delaytime_value[i]->setValue(0);
            delaytime_name[i]->setEnabled(false);
            delaytime_value[i]->setEnabled(false);
            delaytime_fold[i]->setEnabled(false);
            delaytime_result[i]->setEnabled(false);
            einputlabel[i + 1]->setVisible(false);
            einputname[i + 1]->setVisible(false);
        }
    }
}

void delay_adjust_on_off_setting::on_spinBox_delay_time1_valueChanged(int arg1)
{
    ui->label_time1->setText(QString::number(arg1 * 10) + " ms");
}

void delay_adjust_on_off_setting::on_spinBox_delay_time2_valueChanged(int arg1)
{
    ui->label_time2->setText(QString::number(arg1 * 10) + " ms");
}

void delay_adjust_on_off_setting::on_spinBox_delay_time3_valueChanged(int arg1)
{
    ui->label_time3->setText(QString::number(arg1 * 10) + " ms");
}

void delay_adjust_on_off_setting::on_spinBox_delay_time4_valueChanged(int arg1)
{
    ui->label_time4->setText(QString::number(arg1 * 10) + " ms");
}

void delay_adjust_on_off_setting::on_pushButton_cancle_clicked()
{
    close();
}

void delay_adjust_on_off_setting::on_pushButton_apply_clicked()
{
    baselogic->set_input_num(ui->verticalSlider_inputnum->value() + 1);
    QStringList inputname;
    QStringList outputname;
    outputname.append(ui->lineEdit_eoutput1->text());
    for (int i = 0; i < 5; i++) {
        inputname.append(einputname[i]->text());
    }
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    for (int i = 0; i < 4; i++) {
        baselogic->delay_adjust_on_off_param.adjust_on_off_delay_time[i] = delaytime_value[i]->value() * 10;
    }
    if (!(block_base_param == baselogic->get_block_base_param())
        || !(delay_adjust_on_off_param == baselogic->delay_adjust_on_off_param)) {
        baselogic->set_block_old_param(old_param);
        baselogic->send_param_change_signal();
    }
    close();
}
