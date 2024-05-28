#include "speed_value_compairsons_setting.h"
#include "logic_view/speed_logic_block.h"
#include "ui_speed_value_compairsons_setting.h"
speed_value_compairsons_setting::speed_value_compairsons_setting(speed_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::speed_value_compairsons_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

speed_value_compairsons_setting::~speed_value_compairsons_setting()
{
    delete ui;
}

void speed_value_compairsons_setting::setting_exec()
{
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    ui->lineEdit_eoutput1->setText(outputname[0]);
    ui->lineEdit_einput1->setText(inputname[0]);
    ui->comboBox_calc_mode->setCurrentIndex(baselogic->calc_mode);
    ui->spinBox_speed_value->setValue(baselogic->speed_value);
    ui->spinBox_min_time->setValue(baselogic->min_time);
    exec();
}

void speed_value_compairsons_setting::on_pushButton_cancle_clicked()
{
    close();
}

void speed_value_compairsons_setting::on_pushButton_apply_clicked()
{
    QStringList inputname;
    QStringList outputname;
    outputname.append(ui->lineEdit_eoutput1->text());
    inputname.append(ui->lineEdit_einput1->text());
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    baselogic->calc_mode   = ui->comboBox_calc_mode->currentIndex();
    baselogic->speed_value = ui->spinBox_speed_value->value();
    baselogic->min_time    = ui->spinBox_min_time->value();
    close();
}
