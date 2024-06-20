#include "apply_reset_setting.h"
#include "logic_view/apply_logic_block.h"
#include "ui_apply_reset_setting.h"
#include <QDebug>

apply_reset_setting::apply_reset_setting(apply_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::apply_reset_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui_init();
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

apply_reset_setting::~apply_reset_setting()
{
    delete ui;
}

void apply_reset_setting::ui_init()
{
    inputlabel.append(ui->label_input1);
    inputlabel.append(ui->label_input2);
    inputlabel.append(ui->label_input3);
    inputlabel.append(ui->label_input4);
    inputlabel.append(ui->label_input5);
    inputlabel.append(ui->label_input6);
    inputlabel.append(ui->label_input7);
    inputlabel.append(ui->label_input8);
    einputlabel.append(ui->label_einput1);
    einputlabel.append(ui->label_einput2);
    einputlabel.append(ui->label_einput3);
    einputlabel.append(ui->label_einput4);
    einputlabel.append(ui->label_einput5);
    einputlabel.append(ui->label_einput6);
    einputlabel.append(ui->label_einput7);
    einputlabel.append(ui->label_einput8);
    einputname.append(ui->lineEdit_einput1);
    einputname.append(ui->lineEdit_einput2);
    einputname.append(ui->lineEdit_einput3);
    einputname.append(ui->lineEdit_einput4);
    einputname.append(ui->lineEdit_einput5);
    einputname.append(ui->lineEdit_einput6);
    einputname.append(ui->lineEdit_einput7);
    einputname.append(ui->lineEdit_einput8);
}

void apply_reset_setting::setting_exec()
{
    block_base_param       = baselogic->get_block_base_param();
    old_param              = baselogic->block_param_info();
    apply_reset_param      = baselogic->apply_reset_param;
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    for (int i = 0; i < 8; i++) {
        einputname[i]->setText(inputname[i]);
    }
    ui->lineEdit_eoutput1->setText(outputname[0]);
    ui->lineEdit_eoutput2->setText(outputname[1]);
    ui->verticalSlider_inputnum->setValue(baselogic->get_input_point_number());
    ui->comboBox_min_reset_time->setCurrentText(QString::number(baselogic->apply_reset_param.min_reset_pulse_time));
    exec();
}

void apply_reset_setting::on_verticalSlider_inputnum_valueChanged(int value)
{
    if (value < 2) {
        ui->verticalSlider_inputnum->setValue(2);
        return;
    }
    int inputnum = ui->verticalSlider_inputnum->value();
    for (int i = 0; i < 8; i++) {
        if (i < inputnum) {
            inputlabel[i]->setEnabled(true);
            einputlabel[i]->setVisible(true);
            einputname[i]->setVisible(true);
        } else {
            inputlabel[i]->setEnabled(false);
            einputlabel[i]->setVisible(false);
            einputname[i]->setVisible(false);
        }
    }
}

void apply_reset_setting::on_pushButton_cancle_clicked()
{
    close();
}

void apply_reset_setting::on_pushButton_apply_clicked()
{
    baselogic->set_input_mask((0x01 << ui->verticalSlider_inputnum->value()) - 1);
    QStringList inputname;
    QStringList outputname;
    outputname.append(ui->lineEdit_eoutput1->text());
    outputname.append(ui->lineEdit_eoutput2->text());
    for (int i = 0; i < 8; i++) {
        inputname.append(einputname[i]->text());
    }
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    baselogic->apply_reset_param.min_reset_pulse_time = ui->comboBox_min_reset_time->currentText().toInt();
    if (!(block_base_param == baselogic->get_block_base_param())
        || !(apply_reset_param == baselogic->apply_reset_param)) {
        baselogic->set_block_old_param(old_param);
        baselogic->send_param_change_signal();
    }
    close();
}
