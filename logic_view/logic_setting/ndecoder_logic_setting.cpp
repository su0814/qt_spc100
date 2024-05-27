#include "ndecoder_logic_setting.h"
#include "logic_view/base_logic_block.h"
#include "ui_ndecoder_logic_setting.h"

ndecoder_logic_setting::ndecoder_logic_setting(base_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ndecoder_logic_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui_init();
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

ndecoder_logic_setting::~ndecoder_logic_setting()
{
    delete ui;
}

void ndecoder_logic_setting::ui_init()
{
    inputlabel.append(ui->label_input1);
    inputlabel.append(ui->label_input2);
    inputlabel.append(ui->label_input3);
    einputlabel.append(ui->label_einput1);
    einputlabel.append(ui->label_einput2);
    einputlabel.append(ui->label_einput3);
    einputname.append(ui->lineEdit_einput1);
    einputname.append(ui->lineEdit_einput2);
    einputname.append(ui->lineEdit_einput3);
    eoutputlabel.append(ui->label_eoutput1);
    eoutputlabel.append(ui->label_eoutput2);
    eoutputlabel.append(ui->label_eoutput3);
    eoutputlabel.append(ui->label_eoutput4);
    eoutputlabel.append(ui->label_eoutput5);
    eoutputlabel.append(ui->label_eoutput6);
    eoutputlabel.append(ui->label_eoutput7);
    eoutputlabel.append(ui->label_eoutput8);
    eoutputname.append(ui->lineEdit_eoutput1);
    eoutputname.append(ui->lineEdit_eoutput2);
    eoutputname.append(ui->lineEdit_eoutput3);
    eoutputname.append(ui->lineEdit_eoutput4);
    eoutputname.append(ui->lineEdit_eoutput5);
    eoutputname.append(ui->lineEdit_eoutput6);
    eoutputname.append(ui->lineEdit_eoutput7);
    eoutputname.append(ui->lineEdit_eoutput8);
    ui->verticalSlider_inputnum->setValue(1);
}

void ndecoder_logic_setting::set_outputnum(int num)
{
    outputnum = num;
    for (int i = 0; i < 8; i++) {
        if (i < num) {
            eoutputlabel[i]->setVisible(true);
            eoutputname[i]->setVisible(true);
        } else {
            eoutputlabel[i]->setVisible(false);
            eoutputname[i]->setVisible(false);
        }
    }
}

void ndecoder_logic_setting::setting_exec()
{
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    for (int i = 0; i < 3; i++) {
        einputname[i]->setText(inputname[i]);
    }
    for (int i = 0; i < 8; i++) {
        eoutputname[i]->setText(outputname[i]);
    }
    ui->verticalSlider_inputnum->setValue(baselogic->get_input_point_num());
    set_outputnum(baselogic->get_output_point_num());
    exec();
}

void ndecoder_logic_setting::on_verticalSlider_inputnum_valueChanged(int value)
{
    for (int i = 0; i < 3; i++) {
        if (i < value) {
            inputlabel[i]->setEnabled(true);
            einputlabel[i]->setVisible(true);
            einputname[i]->setVisible(true);
        } else {
            inputlabel[i]->setEnabled(false);
            einputlabel[i]->setVisible(false);
            einputname[i]->setVisible(false);
        }
    }
    if (value == 1) {
        set_outputnum(2);
    } else if (value == 2) {
        set_outputnum(4);
    } else {
        set_outputnum(8);
    }
}

void ndecoder_logic_setting::on_pushButton_cancle_clicked()
{
    close();
}

void ndecoder_logic_setting::on_pushButton_apply_clicked()
{
    baselogic->set_input_num(ui->verticalSlider_inputnum->value());
    QStringList inputname;
    QStringList outputname;
    for (int i = 0; i < 3; i++) {
        inputname.append(einputname[i]->text());
    }
    for (int i = 0; i < 8; i++) {
        outputname.append(eoutputname[i]->text());
    }
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    baselogic->set_output_num(outputnum);
    close();
}
