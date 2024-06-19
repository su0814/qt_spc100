#include "apply_edge_detected_setting.h"
#include "logic_view/apply_logic_block.h"
#include "ui_apply_edge_detected_setting.h"

apply_edge_detected_setting::apply_edge_detected_setting(apply_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::apply_edge_detected_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

apply_edge_detected_setting::~apply_edge_detected_setting()
{
    delete ui;
}

void apply_edge_detected_setting::setting_exec()
{
    block_base_param        = baselogic->get_block_base_param();
    old_param               = baselogic->block_param_info();
    apply_edge_detect_param = baselogic->apply_edge_detect_param;
    QStringList outputname  = baselogic->get_user_outputpoint_labels();
    QStringList inputname   = baselogic->get_user_inputpoint_labels();
    ui->lineEdit_einput1->setText(inputname[0]);
    ui->lineEdit_eoutput1->setText(outputname[0]);
    ui->comboBox_edge_detecte_mode->setCurrentIndex(baselogic->apply_edge_detect_param.edge_detected_mode);
    exec();
}

void apply_edge_detected_setting::on_pushButton_cancle_clicked()
{
    close();
}

void apply_edge_detected_setting::on_pushButton_apply_clicked()
{
    QStringList inputname;
    inputname.append(ui->lineEdit_einput1->text());
    QStringList outputname;
    outputname.append(ui->lineEdit_eoutput1->text());
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    baselogic->apply_edge_detect_param.edge_detected_mode = ui->comboBox_edge_detecte_mode->currentIndex();
    if (!(block_base_param == baselogic->get_block_base_param())
        || !(apply_edge_detect_param == baselogic->apply_edge_detect_param)) {
        baselogic->set_block_old_param(old_param);
        baselogic->send_param_change_signal();
    }
    close();
}
