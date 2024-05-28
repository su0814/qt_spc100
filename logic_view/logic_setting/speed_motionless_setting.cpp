#include "speed_motionless_setting.h"
#include "logic_view/speed_logic_block.h"
#include "ui_speed_motionless_setting.h"

speed_motionless_setting::speed_motionless_setting(speed_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::speed_motionless_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

speed_motionless_setting::~speed_motionless_setting()
{
    delete ui;
}

void speed_motionless_setting::setting_exec()
{
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    ui->lineEdit_eoutput1->setText(outputname[0]);
    ui->lineEdit_einput1->setText(inputname[0]);
    ui->spinBox_motionless_speed->setValue(baselogic->motionless_speed);
    ui->spinBox_motion_min_time->setValue(baselogic->motionless_min_time);
    exec();
}

void speed_motionless_setting::on_pushButton_cancle_clicked()
{
    close();
}

void speed_motionless_setting::on_pushButton_apply_clicked()
{
    QStringList inputname;
    QStringList outputname;
    outputname.append(ui->lineEdit_eoutput1->text());
    inputname.append(ui->lineEdit_einput1->text());
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    baselogic->motionless_speed    = ui->spinBox_motionless_speed->value();
    baselogic->motionless_min_time = ui->spinBox_motion_min_time->value();
    close();
}
