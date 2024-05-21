#include "and_or_logic_setting.h"
#include "logic_view/base_logic_block.h"
#include "ui_and_or_logic_setting.h"
#include <QDebug>
and_or_logic_setting::and_or_logic_setting(base_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::and_or_logic_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui_init();
}

and_or_logic_setting::~and_or_logic_setting()
{
    delete ui;
}

void and_or_logic_setting::ui_init()
{
    inputlabel.append(ui->label_input1);
    inputlabel.append(ui->label_input2);
    inputlabel.append(ui->label_input3);
    inputlabel.append(ui->label_input4);
    inputlabel.append(ui->label_input5);
    inputlabel.append(ui->label_input6);
    inputlabel.append(ui->label_input7);
    inputlabel.append(ui->label_input8);
    inputreverse.append(ui->checkBox_input1reverse);
    inputreverse.append(ui->checkBox_input2reverse);
    inputreverse.append(ui->checkBox_input3reverse);
    inputreverse.append(ui->checkBox_input4reverse);
    inputreverse.append(ui->checkBox_input5reverse);
    inputreverse.append(ui->checkBox_input6reverse);
    inputreverse.append(ui->checkBox_input7reverse);
    inputreverse.append(ui->checkBox_input8reverse);
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

void and_or_logic_setting::setting_exec()
{
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname = baselogic->get_user_inputpoint_labels();
    int reversedata = baselogic->get_input_reverse_data();
    for (int i = 0; i < 8; i++) {
        einputname[i]->setText(inputname[i]);
        if ((reversedata >> i) & 0x01) {
            inputreverse[i]->setChecked(true);
        } else {
            inputreverse[i]->setChecked(false);
        }
    }
    ui->lineEdit_eoutput1->setText(outputname[0]);
    ui->verticalSlider_inputnum->setValue(baselogic->get_input_point_num());
    exec();
}

void and_or_logic_setting::on_verticalSlider_inputnum_valueChanged(int value)
{
    if (value < 2) {
        ui->verticalSlider_inputnum->setValue(2);
        return;
    }
    int inputnum = ui->verticalSlider_inputnum->value();
    for (int i = 0; i < 8; i++) {
        if (i < inputnum) {
            inputlabel[i]->setEnabled(true);
            inputreverse[i]->setEnabled(true);
            einputlabel[i]->setVisible(true);
            einputname[i]->setVisible(true);
        } else {
            inputlabel[i]->setEnabled(false);
            inputreverse[i]->setChecked(false);
            inputreverse[i]->setEnabled(false);
            einputlabel[i]->setVisible(false);
            einputname[i]->setVisible(false);
        }
    }
}

void and_or_logic_setting::on_pushButton_cancle_clicked()
{
    close();
}

void and_or_logic_setting::on_pushButton_apply_clicked()
{
    baselogic->set_input_num(ui->verticalSlider_inputnum->value());
    int reverse = 0;
    QStringList inputname;
    QStringList outputname;
    outputname.append(ui->lineEdit_eoutput1->text());
    for (int i = 0; i < 8; i++) {
        if (inputreverse[i]->isChecked()) {
            reverse |= (0x01 << i);
        }
        inputname.append(einputname[i]->text());
    }
    baselogic->set_input_reverse_data(reverse);
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    close();
}
