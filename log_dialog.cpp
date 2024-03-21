#include "log_dialog.h"
#include "QDateTime"
#include "def.h"
#include "mainwindow.h"
#include "ui_log_dialog.h"

log_dialog::log_dialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::log_dialog)
{
    ui->setupUi(this);
    sync_id_list << "COMMON"
                 << "A"
                 << "B";
    mainwindow = ( MainWindow* )parent;
    connect(mainwindow->mydevice_class, &mydevice::device_line_status_change_signal, this,
            device_line_status_change_slots);
    connect(mainwindow->ui->action_log, &QAction::triggered, this, log_dialog_show_slot);
    // move(QApplication::desktop()->screen()->rect().center() - rect().center());
}

log_dialog::~log_dialog()
{
    delete ui;
}

/**
 * @brief 显示log
 * @param sub
 * @param frame
 * @param length
 */
void log_dialog::log_display(uint8_t sub, uint8_t* frame, int32_t length)
{

    QDateTime current_date_time = QDateTime::currentDateTime();
    QString   current_tim       = current_date_time.toString("hh:mm:ss.zzz");
    QString   current_date      = current_date_time.toString("yyyy-MM-dd");

    char str[512] = { 0 };
    memcpy(str, &frame[6], length - 6);
    QString play_str = str;

    QStringList play_list = play_str.split(QRegExp("[\n]"), QString::SkipEmptyParts);
    for (uint16_t i = 0; i < play_list.length(); i++) {
        if (i == 0) {
            if (frame[6] != '[' && sub == CMD_REPORT_LOG) {
                ui->log_textBrowser->moveCursor(QTextCursor::End);
                ui->log_textBrowser->moveCursor(QTextCursor::PreviousCharacter);
                ui->log_textBrowser->insertHtml(TEXT_COLOR_WHILE(play_list[i], TEXT_SIZE_MEDIUM));
                ui->log_textBrowser->append(TEXT_COLOR_BLUE(
                    "[" + current_date + "  " + current_tim + "]" + "# MCU" + sync_id_list[frame[0]], TEXT_SIZE_LARGE));
                continue;
            } else {
                ui->log_textBrowser->append(TEXT_COLOR_BLUE(
                    "[" + current_date + "  " + current_tim + "]" + "# MCU" + sync_id_list[frame[0]], TEXT_SIZE_LARGE));
            }
        }
        ui->log_textBrowser->append(TEXT_COLOR_WHILE(play_list[i], TEXT_SIZE_MEDIUM));
    }
}

/**
 * @brief log显示信息指令回调
 * @param frame
 * @param length
 */
void log_dialog::cmd_report_response(uint8_t* frame, int32_t length)
{
    uint8_t cmd    = frame[2];
    uint8_t bootid = frame[0];
    if (bootid >= SYNC_ID_MAX) {
        return;
    }
    switch (cmd) {
    case CMD_REPORT_INFO:
    case CMD_REPORT_LOG:
        log_display(cmd, frame, length);
        break;
    default:
        break;
    }
}

/* user slots */
void log_dialog::device_line_status_change_slots(device_line_status_e status)
{
    switch (status) {
    case DEVICE_LINE_STATUS_OFF:
        ui->read_a_log->setDisabled(true);
        ui->read_b_log->setDisabled(true);
        break;
    case DEVICE_LINE_STATUS_ON:
        ui->read_a_log->setDisabled(false);
        ui->read_b_log->setDisabled(false);
        break;
    }
}

void log_dialog::log_dialog_show_slot()
{
    show();
}

/* sys slots */
void log_dialog::on_clear_log_clicked()
{
    ui->log_textBrowser->clear();
}

void log_dialog::on_save_log_clicked()
{
    if (ui->log_textBrowser->toPlainText().isEmpty()) {
        mainwindow->my_message_box("日志保存", "当前窗口无日志需要保存", false);
        return;
    }
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString   current_tim       = current_date_time.toString("hh:mm:ss.zzz");
    QString   current_date      = current_date_time.toString("yyyy-MM-dd");
    QString   tim               = "\r\n[ " + current_tim + " ] Save Log\r\n";
    QString   curPath           = QDir::currentPath();  //获取系统当前目录
    curPath += "/" + current_date + ".log";
    QFile file(curPath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        char*      str;
        QString    line_str = tim + ui->log_textBrowser->toPlainText();
        QByteArray ch       = line_str.toUtf8();
        str                 = ch.data();
        file.write(str);
        file.close();
        mainwindow->my_message_box("日志保存", "日志保存成功", false);
    } else {
        mainwindow->my_message_box("日志保存", "日志保存失败", false);
    }
}

void log_dialog::on_read_a_log_clicked()
{
    uint16_t len    = ui->a_log_size->value();
    uint8_t  cmd[8] = { SYNC_ID_A, CMD_TYPE_READ, CMD_READ_LOG, 0, 0X02, 0X00, ( uint8_t )len, (uint8_t)(len >> 8) };
    mainwindow->my_serial->port_sendframe(cmd, 8);
}

void log_dialog::on_read_b_log_clicked()
{
    uint16_t len    = ui->b_log_size->value();
    uint8_t  cmd[8] = { SYNC_ID_B, CMD_TYPE_READ, CMD_READ_LOG, 0, 0X02, 0X00, ( uint8_t )len, (uint8_t)(len >> 8) };
    mainwindow->my_serial->port_sendframe(cmd, 8);
}
