#include "status.h"
#include "mainwindow.h"
#include "qdebug.h"
#include <QDir>
#include <QFile>
#include <QTextCodec>
status::status(QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )parent;
    label_init();
    label_read();
}

void status::label_init()
{
    // input
    label_lineedit[LABEL_DI1]  = ui->DI1_label_lineEdit;
    label_lineedit[LABEL_DI2]  = ui->DI2_label_lineEdit;
    label_lineedit[LABEL_DI3]  = ui->DI3_label_lineEdit;
    label_lineedit[LABEL_DI4]  = ui->DI4_label_lineEdit;
    label_lineedit[LABEL_DI5]  = ui->DI5_label_lineEdit;
    label_lineedit[LABEL_DI6]  = ui->DI6_label_lineEdit;
    label_lineedit[LABEL_DI7]  = ui->DI7_label_lineEdit;
    label_lineedit[LABEL_DI8]  = ui->DI8_label_lineEdit;
    label_lineedit[LABEL_DI9]  = ui->DI9_label_lineEdit;
    label_lineedit[LABEL_DI10] = ui->DI10_label_lineEdit;
    label_lineedit[LABEL_DI11] = ui->DI11_label_lineEdit;
    label_lineedit[LABEL_DI12] = ui->DI12_label_lineEdit;

    default_combox[LABEL_DI1]  = ui->di1_default_comboBox;
    default_combox[LABEL_DI2]  = ui->di2_default_comboBox;
    default_combox[LABEL_DI3]  = ui->di3_default_comboBox;
    default_combox[LABEL_DI4]  = ui->di4_default_comboBox;
    default_combox[LABEL_DI5]  = ui->di5_default_comboBox;
    default_combox[LABEL_DI6]  = ui->di6_default_comboBox;
    default_combox[LABEL_DI7]  = ui->di7_default_comboBox;
    default_combox[LABEL_DI8]  = ui->di8_default_comboBox;
    default_combox[LABEL_DI9]  = ui->di9_default_comboBox;
    default_combox[LABEL_DI10] = ui->di10_default_comboBox;
    default_combox[LABEL_DI11] = ui->di11_default_comboBox;
    default_combox[LABEL_DI12] = ui->di12_default_comboBox;

    a_di_data_lineedit[LABEL_DI1]  = ui->ADI1_lineEdit;
    a_di_data_lineedit[LABEL_DI2]  = ui->ADI2_lineEdit;
    a_di_data_lineedit[LABEL_DI3]  = ui->ADI3_lineEdit;
    a_di_data_lineedit[LABEL_DI4]  = ui->ADI4_lineEdit;
    a_di_data_lineedit[LABEL_DI5]  = ui->ADI5_lineEdit;
    a_di_data_lineedit[LABEL_DI6]  = ui->ADI6_lineEdit;
    a_di_data_lineedit[LABEL_DI7]  = ui->ADI7_lineEdit;
    a_di_data_lineedit[LABEL_DI8]  = ui->ADI8_lineEdit;
    a_di_data_lineedit[LABEL_DI9]  = ui->ADI9_lineEdit;
    a_di_data_lineedit[LABEL_DI10] = ui->ADI10_lineEdit;
    a_di_data_lineedit[LABEL_DI11] = ui->ADI11_lineEdit;
    a_di_data_lineedit[LABEL_DI12] = ui->ADI12_lineEdit;

    b_di_data_lineedit[LABEL_DI1]  = ui->BDI1_lineEdit;
    b_di_data_lineedit[LABEL_DI2]  = ui->BDI2_lineEdit;
    b_di_data_lineedit[LABEL_DI3]  = ui->BDI3_lineEdit;
    b_di_data_lineedit[LABEL_DI4]  = ui->BDI4_lineEdit;
    b_di_data_lineedit[LABEL_DI5]  = ui->BDI5_lineEdit;
    b_di_data_lineedit[LABEL_DI6]  = ui->BDI6_lineEdit;
    b_di_data_lineedit[LABEL_DI7]  = ui->BDI7_lineEdit;
    b_di_data_lineedit[LABEL_DI8]  = ui->BDI8_lineEdit;
    b_di_data_lineedit[LABEL_DI9]  = ui->BDI9_lineEdit;
    b_di_data_lineedit[LABEL_DI10] = ui->BDI10_lineEdit;
    b_di_data_lineedit[LABEL_DI11] = ui->BDI11_lineEdit;
    b_di_data_lineedit[LABEL_DI12] = ui->BDI12_lineEdit;

    di_default_combobox[LABEL_DI1]  = ui->di1_default_comboBox;
    di_default_combobox[LABEL_DI2]  = ui->di2_default_comboBox;
    di_default_combobox[LABEL_DI3]  = ui->di3_default_comboBox;
    di_default_combobox[LABEL_DI4]  = ui->di4_default_comboBox;
    di_default_combobox[LABEL_DI5]  = ui->di5_default_comboBox;
    di_default_combobox[LABEL_DI6]  = ui->di6_default_comboBox;
    di_default_combobox[LABEL_DI7]  = ui->di7_default_comboBox;
    di_default_combobox[LABEL_DI8]  = ui->di8_default_comboBox;
    di_default_combobox[LABEL_DI9]  = ui->di9_default_comboBox;
    di_default_combobox[LABEL_DI10] = ui->di10_default_comboBox;
    di_default_combobox[LABEL_DI11] = ui->di11_default_comboBox;
    di_default_combobox[LABEL_DI12] = ui->di12_default_comboBox;

    // pi
    label_lineedit[LABEL_PI_DIR1]    = ui->pi_dir1_label_lineEdit;
    label_lineedit[LABEL_PI_DIR2]    = ui->pi_dir2_label_lineEdit;
    label_lineedit[LABEL_PI_ENCCNT1] = ui->pi_enccnt1_label_lineEdit;
    label_lineedit[LABEL_PI_ENCCNT2] = ui->pi_enccnt2_label_lineEdit;
    label_lineedit[LABEL_PI_CH1CNT]  = ui->pi_ch1cnt_label_lineEdit;
    label_lineedit[LABEL_PI_CH2CNT]  = ui->pi_ch2cnt_label_lineEdit;
    // qep
    label_lineedit[LABEL_QEP_DIR1] = ui->qep_dir1_label_lineEdit;
    label_lineedit[LABEL_QEP_CNT1] = ui->qep_cnt1_label_lineEdit;
    label_lineedit[LABEL_QEP_DIR2] = ui->qep_dir2_label_lineEdit;
    label_lineedit[LABEL_QEP_CNT2] = ui->qep_cnt2_label_lineEdit;
    // output
    label_lineedit[LABEL_RELAY1] = ui->RELAY1_label_lineEdit;
    label_lineedit[LABEL_RELAY2] = ui->RELAY2_label_lineEdit;
    label_lineedit[LABEL_SMOS1]  = ui->SMOS1_label_lineEdit;
    label_lineedit[LABEL_SMOS2]  = ui->SMOS2_label_lineEdit;
    label_lineedit[LABEL_SMOS3]  = ui->SMOS3_label_lineEdit;
    label_lineedit[LABEL_SMOS4]  = ui->SMOS4_label_lineEdit;
    // ai
    label_lineedit[LABEL_AI1] = ui->AI1_LABEL_lineEdit;
    label_lineedit[LABEL_AI2] = ui->AI2_LABEL_lineEdit;
    for (int i = LABEL_DI1; i < LABEL_NUM; i++) {
        connect(label_lineedit[i % LABEL_NUM], SIGNAL(returnPressed()), label_lineedit[(i + 1) % LABEL_NUM],
                SLOT(setFocus()));
        // label_lineedit[i]->setValidator(new QRegExpValidator(QRegExp("[0-1]+$")));
    }
    //    QString str = "123 ";
    //    str         = str.mid(0, 1);
    //    str.remove(" ");
}

void status::label_clear()
{
    for (int i = LABEL_DI1; i < LABEL_NUM; i++) {
        label_lineedit[i]->clear();
    }
    for (int i = LABEL_DI1; i <= LABEL_DI12; i++) {
        default_combox[i]->setCurrentIndex(0);
    }
}

void status::label_save()
{
    QString curPath = QDir::currentPath();  //获取系统当前目录
    curPath += "/spc100.label";
    QFile file(curPath);
    if (file.exists()) {
        file.remove();
    }
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        for (int i = LABEL_DI1; i < LABEL_NUM; i++) {
            char*      str;
            QString    line_str = label_lineedit[i]->text() + "\n";
            QByteArray ch       = line_str.toUtf8();
            str                 = ch.data();
            file.write(str);
        }
        for (int i = LABEL_DI1; i <= LABEL_DI12; i++) {
            QString    line_str = QString::number(default_combox[i]->currentIndex()) + "\n";
            char*      ch;
            QByteArray ba = line_str.toLatin1();
            ch            = ba.data();
            file.write(ch);
        }
        file.close();
        ui->label_log_lineEdit->setText("标签保存成功！");
        ui->label_log_lineEdit->setStyleSheet("color: rgb(0, 200, 0);");
    } else {
        ui->label_log_lineEdit->setText("文件创建或打开失败！");
        ui->label_log_lineEdit->setStyleSheet("color: rgb(200, 0, 0);");
    }
}

void status::label_read()
{
    QTextCodec* codec   = QTextCodec::codecForName("UTF8");
    QString     curPath = QDir::currentPath();  //获取系统当前目录
    curPath += "/spc100.label";
    QFile file(curPath);
    if (!file.exists()) {
        ui->label_log_lineEdit->setText("未找到标签文件！");
        ui->label_log_lineEdit->setStyleSheet("color: rgb(200, 0, 0);");
        return;
    }
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream data(&file);
        data.setCodec(codec);
        for (int i = LABEL_DI1; i < LABEL_NUM; i++) {
            if (!data.atEnd()) {
                label_lineedit[i]->setText(data.readLine().remove("\n"));

            } else {
                break;
            }
        }
        for (int i = LABEL_DI1; i <= LABEL_DI12; i++) {
            if (!data.atEnd()) {
                QString str = data.readLine().remove("\n");
                str.remove(" ");
                str = str.mid(0, 1);
                if (str.contains(QRegExp("[0-1]+$"))) {
                    default_combox[i]->setCurrentIndex(str.toInt());
                }
            } else {
                break;
            }
        }
        ui->label_log_lineEdit->setText("标签读取成功");
        ui->label_log_lineEdit->setStyleSheet("color: rgb(0, 200, 0);");
    } else {
        ui->label_log_lineEdit->setText("文件打开失败！");
        ui->label_log_lineEdit->setStyleSheet("color: rgb(200, 0, 0);");
    }
}

void status::read_status_switch(bool en)
{
    if (en) {
        ui->start_read_status_pushButton->setEnabled(false);
        ui->stop_read_status_pushButton->setEnabled(true);
        connect(&read_state_timer, SIGNAL(timeout()), this, SLOT(read_status_thread()));
        read_state_timer.setTimerType(Qt::PreciseTimer);
        read_state_timer.start(100);
    } else {
        read_state_timer.stop();
        disconnect(&read_state_timer, SIGNAL(timeout()), this, SLOT(read_status_thread()));
        ui->start_read_status_pushButton->setEnabled(true);
        ui->stop_read_status_pushButton->setEnabled(false);
    }
}

void status::read_status_thread()
{
    static bool is_self  = true;
    uint8_t     frame[6] = { 0, CMD_TYPE_READ, CMD_READ_STATUS, SUB_READ_STATUS_BASE_SELF, 00, 00 };
    if (!is_self) {
        frame[3] = SUB_READ_STATUS_BASE_PAIR;
    }
    mainwindow->my_serial->port_sendframe(frame, 6);
    is_self = !is_self;
}

void status::a_baseinfo_display(uint8_t* frame, int32_t length)
{
    module_state_t module_state;
    memcpy(( uint8_t* )&module_state, &frame[6], sizeof(module_state));
    double v24v = module_state.voltage_24v * 2.5 / 4095 * (100 + 8.2) / 8.2 + 0.7;
    double v5v  = module_state.voltage_5v * 2.5 / 4095 * (49.9f + 39) / 39;
    double v33v = module_state.voltage_3v3 * 2.5 / 4095 * 2.0;
    double v24c =
        (module_state.current_24v > 2048 ? module_state.current_24v - 2048 : 2048 - module_state.current_24v) / 164.0f;
    ui->A24VV_lineEdit->setText(QString::number(v24v, 'f', 2) + "V");
    ui->A5VV_lineEdit->setText(QString::number(v5v, 'f', 2) + "V");
    ui->A33VV_lineEdit->setText(QString::number(v33v, 'f', 2) + "V");
    ui->A24VC_lineEdit->setText(QString::number(v24c, 'f', 2) + "A");

    for (uint8_t i = LABEL_DI1; i <= LABEL_DI12; i++) {
        a_di_data_lineedit[i]->setText(QString::number((module_state.di_state.di_s >> i) & 0x01));
        if (((module_state.di_state.di_s >> i) & 0x01) == di_default_combobox[i]->currentIndex()) {
            a_di_data_lineedit[i]->setStyleSheet("background-color:rgb(200,0,0);");
        } else {
            a_di_data_lineedit[i]->setStyleSheet("background-color:rgb(200,200,150);");
        }
    }
    ui->A_RELAY1_lineEdit->setText(QString::number(module_state.relay_state.relay_bit.set_bit1));
    ui->A_RELAY2_lineEdit->setText(QString::number(module_state.relay_state.relay_bit.set_bit2));
    ui->A_SMOS1_lineEdit->setText(QString::number(module_state.mos_state.mos_bit.set_bit1));
    ui->A_SMOS2_lineEdit->setText(QString::number(module_state.mos_state.mos_bit.set_bit2));
    ui->A_SMOS3_lineEdit->setText(QString::number(module_state.mos_state.mos_bit.mos_p_set_bit1));
    ui->A_SMOS4_lineEdit->setText(QString::number(module_state.mos_state.mos_bit.mos_p_set_bit2));
    ui->AQEP_DIR1_lineEdit->setText(QString::number(module_state.dir_state.dir_bit.qep_dir_ma_read_bit));
    ui->AQEP_DIR2_lineEdit->setText(QString::number(module_state.dir_state.dir_bit.qep_dir_mb_read_bit));
    ui->AQEP_CNT1_lineEdit->setText(QString::number(module_state.qep_ma_cnt) + "/s");
    ui->AQEP_CNT2_lineEdit->setText(QString::number(module_state.qep_mb_cnt) + "/s");
    ui->A_PI_DIR1_lineEdit->setText(QString::number(module_state.dir_state.dir_bit.pi_dir_ma_read_bit));
    ui->A_PI_DIR2_lineEdit->setText(QString::number(module_state.dir_state.dir_bit.pi_dir_mb_read_bit));
    ui->A_PI_ENCCNT1_lineEdit->setText(QString::number(module_state.encoder_pi_ma_cnt) + "/s");
    ui->A_PI_ENCCNT2_lineEdit->setText(QString::number(module_state.encoder_pi_mb_cnt) + "/s");
    ui->A_PI_CH1CNT_lineEdit->setText(QString::number(module_state.pi1_cnt) + "/s");
    ui->A_PI_CH2CNT_lineEdit->setText(QString::number(module_state.pi2_cnt) + "/s");
    double ai1_v = module_state.analog_value1 * 0.0075;
    double ai2_v = module_state.analog_value2 * 0.0075;
    ui->A_AI1_lineEdit->setText(QString::number(ai1_v, 'f', 2) + "V");
    ui->A_AI2_lineEdit->setText(QString::number(ai2_v, 'f', 2) + "V");
    if (module_state.module_state == STATE_SAFE) {
        ui->A_SOFT_STATUS_checkBox->setChecked(true);
    } else if (module_state.module_state == STATE_RUN) {
        ui->A_SOFT_STATUS_checkBox->setChecked(false);
    }
}

void status::b_baseinfo_display(uint8_t* frame, int32_t length)
{
    module_state_t module_state;
    memcpy(( uint8_t* )&module_state, &frame[6], sizeof(module_state));
    double v24v = module_state.voltage_24v * 2.5 / 4095 * (100 + 8.2) / 8.2 + 0.7;
    double v5v  = module_state.voltage_5v * 2.5 / 4095 * (49.9f + 39) / 39;
    double v33v = module_state.voltage_3v3 * 2.5 / 4095 * 2.0;
    double v24c =
        (module_state.current_24v > 2048 ? module_state.current_24v - 2048 : 2048 - module_state.current_24v) / 164.0f;
    ui->B24VV_lineEdit->setText(QString::number(v24v, 'f', 2) + "V");
    ui->B5VV_lineEdit->setText(QString::number(v5v, 'f', 2) + "V");
    ui->B33VV_lineEdit->setText(QString::number(v33v, 'f', 2) + "V");
    ui->B24VC_lineEdit->setText(QString::number(v24c, 'f', 2) + "A");

    for (uint8_t i = LABEL_DI1; i <= LABEL_DI12; i++) {
        b_di_data_lineedit[i]->setText(QString::number((module_state.di_state.di_s >> i) & 0x01));
        if (((module_state.di_state.di_s >> i) & 0x01) == di_default_combobox[i]->currentIndex()) {
            b_di_data_lineedit[i]->setStyleSheet("background-color:rgb(200,0,0);");
        } else {
            b_di_data_lineedit[i]->setStyleSheet("background-color:rgb(200,200,150);");
        }
    }
    ui->B_RELAY1_lineEdit->setText(QString::number(module_state.relay_state.relay_bit.set_bit1));
    ui->B_RELAY2_lineEdit->setText(QString::number(module_state.relay_state.relay_bit.set_bit2));
    ui->B_SMOS1_lineEdit->setText(QString::number(module_state.mos_state.mos_bit.set_bit1));
    ui->B_SMOS2_lineEdit->setText(QString::number(module_state.mos_state.mos_bit.set_bit2));
    ui->B_SMOS3_lineEdit->setText(QString::number(module_state.mos_state.mos_bit.mos_p_set_bit1));
    ui->B_SMOS4_lineEdit->setText(QString::number(module_state.mos_state.mos_bit.mos_p_set_bit2));
    ui->BQEP_DIR1_lineEdit->setText(QString::number(module_state.dir_state.dir_bit.qep_dir_ma_read_bit));
    ui->BQEP_DIR2_lineEdit->setText(QString::number(module_state.dir_state.dir_bit.qep_dir_mb_read_bit));
    ui->BQEP_CNT1_lineEdit->setText(QString::number(module_state.qep_ma_cnt) + "/s");
    ui->BQEP_CNT2_lineEdit->setText(QString::number(module_state.qep_mb_cnt) + "/s");
    ui->B_PI_DIR1_lineEdit->setText(QString::number(module_state.dir_state.dir_bit.pi_dir_ma_read_bit));
    ui->B_PI_DIR2_lineEdit->setText(QString::number(module_state.dir_state.dir_bit.pi_dir_mb_read_bit));
    ui->B_PI_ENCCNT1_lineEdit->setText(QString::number(module_state.encoder_pi_ma_cnt) + "/s");
    ui->B_PI_ENCCNT2_lineEdit->setText(QString::number(module_state.encoder_pi_mb_cnt) + "/s");
    ui->B_PI_CH1CNT_lineEdit->setText(QString::number(module_state.pi1_cnt) + "/s");
    ui->B_PI_CH2CNT_lineEdit->setText(QString::number(module_state.pi2_cnt) + "/s");
    double ai1_v = module_state.analog_value1 * 0.0075;
    double ai2_v = module_state.analog_value2 * 0.0075;
    ui->B_AI1_lineEdit->setText(QString::number(ai1_v, 'f', 2) + "V");
    ui->B_AI2_lineEdit->setText(QString::number(ai2_v, 'f', 2) + "V");
    if (module_state.module_state == STATE_SAFE) {
        ui->B_SOFT_STATUS_checkBox->setChecked(true);
    } else if (module_state.module_state == STATE_RUN) {
        ui->B_SOFT_STATUS_checkBox->setChecked(false);
    }
}

void status::status_serial_connect_callback()
{
    ui->start_read_status_pushButton->setEnabled(true);
    ui->stop_read_status_pushButton->setEnabled(false);
}

void status::status_serial_disconnect_callback()
{
    ui->start_read_status_pushButton->setEnabled(false);
    read_state_timer.stop();
}

void status::type_status_response(uint8_t* frame, int32_t length)
{
    uint8_t cmd     = frame[2];
    uint8_t sub     = frame[3];
    uint8_t sync_id = frame[0];

    switch (cmd) {
    case CMD_READ_STATUS:
        switch (sub) {
        case SUB_READ_REPLY_BASE_SELF:
            if (sync_id == SYNC_ID_A) {
                a_baseinfo_display(frame, length);
            } else {
                b_baseinfo_display(frame, length);
            }
            break;
        case SUB_READ_REPLY_BASE_PAIR:
            if (sync_id == SYNC_ID_A) {
                b_baseinfo_display(frame, length);
            } else {
                a_baseinfo_display(frame, length);
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void status::status_ui_resize(uint32_t width, uint32_t height)
{
    uint32_t font_size        = 14 * height / ui_HEIGHT;
    uint32_t ctrl_height      = 20 * height / ui_HEIGHT;
    uint32_t ss_checkbox_size = 40 * height / ui_HEIGHT;
    ui->tab_status->setStyleSheet(  // QGroupBox StyleSheet
        "QGroupBox{border: 2px solid gray;border-radius:10px;margin-top:2ex;font-family:微软雅黑;font:bold "
        + QString::number(font_size)
        + "px;}"
          "QGroupBox::title{subcontrol-origin:margin;subcontrol-position:top center;padding:1px;}"
          // QLineEdit StyleSheet
          " QLineEdit{color:rgb(0,0,0);background-color:rgb(200,200,150);selection-color:white;selection-background-"
          "color:rgb(191,31,127);border-radius:"
        + QString::number(ctrl_height / 2 - 1) + "px; padding:2px 4px;font-size:" + QString::number(font_size)
        + "px;height: " + QString::number(ctrl_height - 1)
        + "px;}"
          "QCheckBox{spacing: 5px;color: green;}"
          "QCheckBox::indicator{width: "
        + QString::number(ss_checkbox_size * 2) + "px;height: " + QString::number(ss_checkbox_size)
        + "px;}"
          "QCheckBox::indicator:enabled:unchecked {image: url(:/new/photo/photo/run.png);}"
          "QCheckBox::indicator:enabled:checked {image: url(:/new/photo/photo/safe.png);}"
          // QComboBox StyleSheet
          "QComboBox {font-family:  Microsoft YaHei"
          ";font-size:"
        + QString::number(font_size) + "px;height: " + QString::number(ctrl_height)
        + "px;color: #000000;font-style: italic;font-weight: bold;padding-left: 10px;border-width: "
          "2px;border-style: solid;border-color: green;border-top-left-radius: "
        + QString::number(ctrl_height / 2) + "px;border-bottom-left-radius: " + QString::number(ctrl_height / 2)
        + "px;}");
}
