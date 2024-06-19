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
    block_base_param               = baselogic->get_block_base_param();
    old_param                      = baselogic->block_param_info();
    speed_motionless_monitor_param = baselogic->speed_motionless_monitor_param;
    QStringList outputname         = baselogic->get_user_outputpoint_labels();
    QStringList inputname          = baselogic->get_user_inputpoint_labels();
    ui->lineEdit_eoutput1->setText(outputname[0]);
    ui->lineEdit_einput1->setText(inputname[0]);
    ui->spinBox_motionless_speed->setValue(baselogic->speed_motionless_monitor_param.motionless_speed);
    ui->spinBox_motion_min_time->setValue(baselogic->speed_motionless_monitor_param.motionless_min_time);
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
    baselogic->speed_motionless_monitor_param.motionless_speed    = ui->spinBox_motionless_speed->value();
    baselogic->speed_motionless_monitor_param.motionless_min_time = ui->spinBox_motion_min_time->value();
    if (!(block_base_param == baselogic->get_block_base_param())
        || !(speed_motionless_monitor_param == baselogic->speed_motionless_monitor_param)) {
        baselogic->set_block_old_param(old_param);
        baselogic->send_param_change_signal();
    }
    close();
}
