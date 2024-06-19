#include "nencode_logic_setting.h"
#include "logic_view/base_logic_block.h"
#include "ui_nencode_logic_setting.h"

nencode_logic_setting::nencode_logic_setting(base_logic_block* logic, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::nencode_logic_setting)
{
    ui->setupUi(this);
    ui->tabWidget->setStyle(new MyProxyStyle);
    setWindowTitle("设置");
    baselogic = logic;
    ui_init();
    ui->pushButton_cancle->setAutoDefault(false);
    ui->pushButton_apply->setAutoDefault(false);
}

nencode_logic_setting::~nencode_logic_setting()
{
    delete ui;
}

void nencode_logic_setting::ui_init()
{
    inputlabel.append(ui->label_input1);
    inputlabel.append(ui->label_input2);
    inputlabel.append(ui->label_input3);
    inputlabel.append(ui->label_input4);
    inputlabel.append(ui->label_input5);
    inputlabel.append(ui->label_input6);
    inputlabel.append(ui->label_input7);
    inputlabel.append(ui->label_input8);
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
    eoutputlabel.append(ui->label_eoutput1);
    eoutputlabel.append(ui->label_eoutput2);
    eoutputlabel.append(ui->label_eoutput3);
    eoutputlabel.append(ui->label_eoutput4);
    eoutputname.append(ui->lineEdit_eoutput1);
    eoutputname.append(ui->lineEdit_eoutput2);
    eoutputname.append(ui->lineEdit_eoutput3);
    eoutputname.append(ui->lineEdit_eoutput4);
    sys_output_label << "输出A"
                     << "输出B"
                     << "输出C"
                     << "故障";
}

void nencode_logic_setting::set_outputnum(int num, bool fault)
{
    outputnum = num;
    logic_output_label.clear();
    for (int i = 0; i < 4; i++) {
        if (i < num) {
            eoutputlabel[i]->setVisible(true);
            eoutputname[i]->setVisible(true);
            eoutputlabel[i]->setText(sys_output_label[i]);
            logic_output_label.append(sys_output_label[i]);
        } else {
            eoutputlabel[i]->setVisible(false);
            eoutputname[i]->setVisible(false);
        }
    }
    if (fault) {
        eoutputlabel[num]->setVisible(true);
        eoutputname[num]->setVisible(true);
        eoutputlabel[num]->setText("故障");
        logic_output_label.append("故障");
    }
}

void nencode_logic_setting::setting_exec()
{
    block_base_param        = baselogic->get_block_base_param();
    old_param               = baselogic->block_param_info();
    logic_base_encode_param = baselogic->logic_base_encode_param;
    QStringList outputname  = baselogic->get_user_outputpoint_labels();
    QStringList inputname   = baselogic->get_user_inputpoint_labels();
    int         outnum      = baselogic->get_output_point_num();
    for (int i = 0; i < 8; i++) {
        einputname[i]->setText(inputname[i]);
    }
    for (int i = 0; i < 4; i++) {
        eoutputname[i]->setText(outputname[i]);
    }
    if (baselogic->logic_base_encode_param.nencode_fault_output) {
        outnum -= 1;
    }
    set_outputnum(outnum, baselogic->logic_base_encode_param.nencode_fault_output);
    ui->checkBox_fault->blockSignals(true);
    ui->checkBox_fault->setChecked(baselogic->logic_base_encode_param.nencode_fault_output);
    ui->checkBox_fault->blockSignals(false);
    ui->verticalSlider_inputnum->setValue(baselogic->get_input_point_num());
    exec();
}

void nencode_logic_setting::on_verticalSlider_inputnum_valueChanged(int value)
{
    if (value < 2) {
        ui->verticalSlider_inputnum->setValue(2);
        return;
    }
    int inputnum = ui->verticalSlider_inputnum->value();
    for (int i = 0; i < 8; i++) {
        if (i < inputnum) {
            inputlabel[i]->setEnabled(true);
            einputlabel[i]->setVisible(true);
            einputname[i]->setVisible(true);
        } else {
            inputlabel[i]->setEnabled(false);
            einputlabel[i]->setVisible(false);
            einputname[i]->setVisible(false);
        }
    }
    if (inputnum <= 2) {
        set_outputnum(1, ui->checkBox_fault->isChecked());
    } else if (inputnum <= 4) {
        set_outputnum(2, ui->checkBox_fault->isChecked());
    } else {
        set_outputnum(3, ui->checkBox_fault->isChecked());
    }
}

void nencode_logic_setting::on_checkBox_fault_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    set_outputnum(outputnum, ui->checkBox_fault->isChecked());
}

void nencode_logic_setting::on_pushButton_cancle_clicked()
{
    close();
}

void nencode_logic_setting::on_pushButton_apply_clicked()
{
    baselogic->set_input_num(ui->verticalSlider_inputnum->value());
    QStringList inputname;
    QStringList outputname;
    for (int i = 0; i < 8; i++) {
        inputname.append(einputname[i]->text());
    }
    for (int i = 0; i < 4; i++) {
        outputname.append(eoutputname[i]->text());
    }
    baselogic->logic_base_encode_param.nencode_fault_output = ui->checkBox_fault->isChecked();
    if (baselogic->logic_base_encode_param.nencode_fault_output) {
        baselogic->set_output_num(outputnum + 1);
    } else {
        baselogic->set_output_num(outputnum);
    }
    baselogic->set_sys_outputpoint_labels(logic_output_label);
    baselogic->set_user_inputpoint_labels(inputname);
    baselogic->set_user_outputpoint_labels(outputname);
    if (!(block_base_param == baselogic->get_block_base_param())
        || !(logic_base_encode_param == baselogic->logic_base_encode_param)) {
        baselogic->set_block_old_param(old_param);
        baselogic->send_param_change_signal();
    }
    close();
}
