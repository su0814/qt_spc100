#include "apply_extern_device_monitors_setting.h"
#include "logic_view/apply_logic_block.h"
#include "ui_apply_extern_device_monitors_setting.h"

apply_extern_device_monitors_setting::apply_extern_device_monitors_setting(apply_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::apply_extern_device_monitors_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

apply_extern_device_monitors_setting::~apply_extern_device_monitors_setting()
{
    delete ui;
}

void apply_extern_device_monitors_setting::setting_exec()
{
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    ui->lineEdit_einput1->setText(inputname[0]);
    ui->lineEdit_einput2->setText(inputname[1]);
    ui->lineEdit_eoutput1->setText(outputname[0]);
    ui->lineEdit_eoutput2->setText(outputname[1]);
    ui->spinBox_feedback_delay_time->setValue(baselogic->max_feedback_delay / 10);
    exec();
}

void apply_extern_device_monitors_setting::on_spinBox_feedback_delay_time_valueChanged(int arg1)
{
    ui->label_time->setText(QString::number(arg1 * 10) + " ms");
}

void apply_extern_device_monitors_setting::on_pushButton_cancle_clicked()
{
    close();
}

void apply_extern_device_monitors_setting::on_pushButton_apply_clicked()
{
    QStringList inputname;
    inputname.append(ui->lineEdit_einput1->text());
    inputname.append(ui->lineEdit_einput2->text());
    QStringList outputname;
    outputname.append(ui->lineEdit_eoutput1->text());
    outputname.append(ui->lineEdit_eoutput2->text());
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    baselogic->max_feedback_delay = ui->spinBox_feedback_delay_time->value() * 10;
    close();
}
