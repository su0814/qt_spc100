#include "delay_on_off_setting.h"
#include "logic_view/delay_counter_logic_block.h"
#include "ui_delay_on_off_setting.h"

delay_on_off_setting::delay_on_off_setting(delay_counter_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::delay_on_off_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
}

delay_on_off_setting::~delay_on_off_setting()
{
    delete ui;
}
void delay_on_off_setting::setting_exec()
{
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    ui->lineEdit_einput1->setText(inputname[0]);
    ui->lineEdit_eoutput1->setText(outputname[0]);
    ui->spinBox_delay_time->setValue(baselogic->on_off_delay_time / 10);
    exec();
}

void delay_on_off_setting::on_spinBox_delay_time_valueChanged(int arg1)
{
    ui->label_time->setText(QString::number(arg1 * 10) + " ms");
}

void delay_on_off_setting::on_pushButton_cancle_clicked()
{
    close();
}

void delay_on_off_setting::on_pushButton_apply_clicked()
{
    QStringList inputname;
    inputname.append(ui->lineEdit_einput1->text());
    QStringList outputname;
    outputname.append(ui->lineEdit_eoutput1->text());
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    baselogic->on_off_delay_time = ui->spinBox_delay_time->value() * 10;
    close();
}
