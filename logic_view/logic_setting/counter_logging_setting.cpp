#include "counter_logging_setting.h"
#include "logic_view/delay_counter_logic_block.h"
#include "ui_counter_logging_setting.h"

counter_logging_setting::counter_logging_setting(delay_counter_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::counter_logging_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui_init();
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

counter_logging_setting::~counter_logging_setting()
{
    delete ui;
}

void counter_logging_setting::ui_init()
{
    inputlabel.append(ui->label_input1);
    inputlabel.append(ui->label_input2);
    inputlabel.append(ui->label_input3);
    inputlabel.append(ui->label_input4);
    inputlabel.append(ui->label_input5);
    inputlabel.append(ui->label_input6);
    inputlabel.append(ui->label_input7);
    inputlabel.append(ui->label_input8);
    log_edge.append(ui->comboBox_edge1);
    log_edge.append(ui->comboBox_edge2);
    log_edge.append(ui->comboBox_edge3);
    log_edge.append(ui->comboBox_edge4);
    log_edge.append(ui->comboBox_edge5);
    log_edge.append(ui->comboBox_edge6);
    log_edge.append(ui->comboBox_edge7);
    log_edge.append(ui->comboBox_edge8);
    log_text.append(ui->lineEdit_log);
    log_text.append(ui->lineEdit_log_2);
    log_text.append(ui->lineEdit_log_3);
    log_text.append(ui->lineEdit_log_4);
    log_text.append(ui->lineEdit_log_5);
    log_text.append(ui->lineEdit_log_6);
    log_text.append(ui->lineEdit_log_7);
    log_text.append(ui->lineEdit_log_8);
    einputlabel.append(ui->label_einput1);
    einputlabel.append(ui->label_einput2);
    einputlabel.append(ui->label_einput3);
    einputlabel.append(ui->label_einput4);
    einputlabel.append(ui->label_einput5);
    einputlabel.append(ui->label_einput6);
    einputlabel.append(ui->label_einput7);
    einputlabel.append(ui->label_einput8);
    einputname.append(ui->lineEdit_einput1);
    einputname.append(ui->lineEdit_einput2);
    einputname.append(ui->lineEdit_einput3);
    einputname.append(ui->lineEdit_einput4);
    einputname.append(ui->lineEdit_einput5);
    einputname.append(ui->lineEdit_einput6);
    einputname.append(ui->lineEdit_einput7);
    einputname.append(ui->lineEdit_einput8);
}

void counter_logging_setting::setting_exec()
{
    QStringList inputname = baselogic->get_user_inputpoint_labels();
    for (int i = 0; i < 8; i++) {
        einputname[i]->setText(inputname[i]);
        log_edge[i]->setCurrentIndex(baselogic->log_edge[i] - 1);
        log_text[i]->setText(baselogic->log_text[i]);
    }
    ui->verticalSlider_inputnum->setValue(baselogic->get_input_point_num());
    exec();
}

void counter_logging_setting::on_verticalSlider_inputnum_valueChanged(int value)
{
    for (int i = 0; i < 8; i++) {
        if (i < value) {
            inputlabel[i]->setEnabled(true);
            log_edge[i]->setEnabled(true);
            log_text[i]->setEnabled(true);
            einputlabel[i]->setVisible(true);
            einputname[i]->setVisible(true);
        } else {
            inputlabel[i]->setEnabled(false);
            log_edge[i]->setEnabled(false);
            log_text[i]->setEnabled(false);
            einputlabel[i]->setVisible(false);
            einputname[i]->setVisible(false);
        }
    }
}

void counter_logging_setting::on_pushButton_cancle_clicked()
{
    close();
}

void counter_logging_setting::on_pushButton_apply_clicked()
{
    QStringList inputname;
    baselogic->set_input_num(ui->verticalSlider_inputnum->value());
    for (int i = 0; i < 8; i++) {
        inputname.append(einputname[i]->text());
        baselogic->log_edge[i] = log_edge[i]->currentIndex() + 1;
        baselogic->log_text[i] = log_text[i]->text();
    }
    baselogic->set_user_inputpoint_labels(inputname);
    close();
}
