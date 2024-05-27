#include "rs_trigger_logic_setting.h"
#include "logic_view/base_logic_block.h"
#include "ui_rs_trigger_logic_setting.h"

rs_trigger_logic_setting::rs_trigger_logic_setting(base_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::rs_trigger_logic_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

rs_trigger_logic_setting::~rs_trigger_logic_setting()
{
    delete ui;
}

void rs_trigger_logic_setting::setting_exec()
{
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    ui->lineEdit_einput1->setText(inputname[0]);
    ui->lineEdit_einput2->setText(inputname[1]);
    ui->lineEdit_eoutput1->setText(outputname[0]);
    ui->lineEdit_eoutput2->setText(outputname[1]);
    int reversedata = baselogic->get_input_reverse_data();
    if (reversedata & 0x01) {
        ui->checkBox_input1reverse->setChecked(true);
    } else {
        ui->checkBox_input1reverse->setChecked(false);
    }
    if ((reversedata >> 1) & 0x01) {
        ui->checkBox_input2reverse->setChecked(true);
    } else {
        ui->checkBox_input2reverse->setChecked(false);
    }
    exec();
}

void rs_trigger_logic_setting::on_pushButton_cancle_clicked()
{
    close();
}

void rs_trigger_logic_setting::on_pushButton_apply_clicked()
{
    QStringList inputname;
    QStringList outputname;
    inputname.append(ui->lineEdit_einput1->text());
    inputname.append(ui->lineEdit_einput2->text());
    outputname.append(ui->lineEdit_eoutput1->text());
    outputname.append(ui->lineEdit_eoutput2->text());
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    int reverse = 0;
    if (ui->checkBox_input1reverse->isChecked()) {
        reverse |= 0x01;
    }
    if (ui->checkBox_input2reverse->isChecked()) {
        reverse |= 0x02;
    }
    baselogic->set_input_reverse_data(reverse);
    close();
}
