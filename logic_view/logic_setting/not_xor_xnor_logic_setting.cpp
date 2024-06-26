#include "not_xor_xnor_logic_setting.h"
#include "logic_view/base_logic_block.h"
#include "ui_not_xor_xnor_logic_setting.h"

not_xor_xnor_logic_setting::not_xor_xnor_logic_setting(base_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::not_xor_xnor_logic_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    if (baselogic->get_config_block_data()->config_param_data.model_id == MODEL_ID_LOGIC_BASE_NOT) {
        ui->lineEdit_einput2->setVisible(false);
        ui->label_einput2->setVisible(false);
    }
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

not_xor_xnor_logic_setting::~not_xor_xnor_logic_setting()
{
    delete ui;
}

void not_xor_xnor_logic_setting::setting_exec()
{
    block_base_param       = baselogic->get_block_base_param();
    old_param              = baselogic->block_param_info();
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    ui->lineEdit_einput1->setText(inputname[0]);
    ui->lineEdit_eoutput1->setText(outputname[0]);
    ui->lineEdit_einput2->setText(inputname[1]);
    exec();
}

void not_xor_xnor_logic_setting::on_pushButton_cancle_clicked()
{
    close();
}

void not_xor_xnor_logic_setting::on_pushButton_apply_clicked()
{
    QStringList inputname;
    QStringList outputname;
    inputname.append(ui->lineEdit_einput1->text());
    inputname.append(ui->lineEdit_einput2->text());
    outputname.append(ui->lineEdit_eoutput1->text());
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    if (!(block_base_param == baselogic->get_block_base_param())) {
        baselogic->set_block_old_param(old_param);
        baselogic->send_param_change_signal();
    }
    close();
}
