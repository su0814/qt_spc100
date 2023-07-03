#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "PT/pt.h"
#include "def.h"
#include "my_serialport.h"
#include "transportcrc.h"
#include <QMainWindow>
#define TEXT_SIZE_SMALL  "15"
#define TEXT_SIZE_MEDIUM "17"
#define TEXT_SIZE_LARGE  "19"

#define TEXT_COLOR_RED(STRING, SIZE)                                                    \
    ("<font style = 'font-size:" + QString(SIZE) + "px; color:red;'>" + QString(STRING) \
     + "</font>"                                                                        \
       "<font color=black> </font>")
#define TEXT_COLOR_BLUE(STRING, SIZE)                                                    \
    ("<font style = 'font-size:" + QString(SIZE) + "px; color:blue;'>" + QString(STRING) \
     + "</font>"                                                                         \
       "<font color=black> </font>")
#define TEXT_COLOR_GREEN(STRING, SIZE)                                                    \
    ("<font style = 'font-size:" + QString(SIZE) + "px; color:green;'>" + QString(STRING) \
     + "</font>"                                                                          \
       "<font color=black> </font>")
#define TEXT_COLOR_GREY(STRING, SIZE)                                                    \
    ("<font style = 'font-size:" + QString(SIZE) + "px; color:grey;'>" + QString(STRING) \
     + "</font>"                                                                         \
       "<font color=black> </font>")

#define TEXT_COLOR_WHILE(STRING, SIZE)                                                      \
    ("<font style = 'font-size:" + QString(SIZE) + "px; color:#FFFFFF;'>" + QString(STRING) \
     + "</font>"                                                                            \
       "<font color=black> </font>")
#define TEXT_COLOR_YELLOW(STRING, SIZE)                                                     \
    ("<font style = 'font-size:" + QString(SIZE) + "px; color:#FFFF00;'>" + QString(STRING) \
     + "</font>"                                                                            \
       "<font color=black> </font>")

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    /* user variable */
private:
    /* about serial */
    my_serialport* my_serial = nullptr;
    transportcrc*  transport = nullptr;

    /* about upgrade */
    QString firmware_pathname;
    QString firmware_filename;

    iap_info_t      iap_info;
    firmware_info_t app_tail;
    QStringList     module_name_list;
    QStringList     upgrade_id_list;
    uint8_t         boot_status       = SUB_BL_STS_START;
    uint8_t         upgrade_quit_flag = DISABLE_FLAG;
    struct pt       pt_upgrade;
    /* user function */
private:
    /* about init */
    void ui_init(void);
    /* about serial */
    void serial_switch_ctrl(void);  //串口开关控制
    void serial_search(void);       //串口自动搜索
    /* about warning */
    void my_message_box(QString title, QString text);

    /* about upgrade */
    int  boot_upgrade_thread(void);
    void boot_cmd_response(uint8_t* frame, int32_t length);
    int  firmware_info_encrypt_decrypt(firmware_info_t* merge_firmware_info, key_crypt_e key);
    int  upgrade_ack_soh_result_phase(uint8_t* retry_cnt, uint8_t* bootid);
    int  upgrade_ack_stx_result_phase(uint8_t* retry_cnt, uint8_t* bootid);
    int  upgrade_ack_eot_result_phase(qint64 starttime);
    /* user slot */
public slots:
    /* about serial */
    int  serial_error_callback(QSerialPort::SerialPortError error);
    void serial_data_proc(void);
    void cmd_callback(uint8_t* frame, int32_t length);

protected:
    bool eventFilter(QObject* watched, QEvent* event);
private slots:
    void on_serial_switch_pushButton_clicked();
    void on_select_fw_pushButton_clicked();
    void on_start_upgrade_pushButton_clicked();
    void on_quit_upgrade_pushButton_clicked();
};

#endif  // MAINWINDOW_H
