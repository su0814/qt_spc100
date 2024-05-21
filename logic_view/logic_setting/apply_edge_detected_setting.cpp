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
}

apply_edge_detected_setting::~apply_edge_detected_setting()
{
    delete ui;
}

void apply_edge_detected_setting::setting_exec()
{
    QStringList outputname = baselogic->get_user_outputpoint_labels();
    QStringList inputname  = baselogic->get_user_inputpoint_labels();
    ui->lineEdit_einput1->setText(inputname[0]);
    ui->lineEdit_eoutput1->setText(outputname[0]);
    ui->comboBox_edge_detecte_mode->setCurrentIndex(baselogic->edge_detected_mode);
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
    baselogic->edge_detected_mode = ui->comboBox_edge_detecte_mode->currentIndex();
    close();
}
