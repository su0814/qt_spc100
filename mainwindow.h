#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "def.h"
#include "my_serialport.h"
#include "transportcrc.h"
#include "ui_mainwindow.h"
#include <QMainWindow>

class upgrade;
class lua;
class status;
class param;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();
    static MainWindow* my_ui;
    Ui::MainWindow*    ui;

private:
    /* user variable */
    void serial_connect_callback(void);
    void serial_disconnect_callback(void);
    void resizeEvent(QResizeEvent* event);

public:
    /* about serial */
    my_serialport* my_serial = nullptr;
    transportcrc*  transport = nullptr;
    /* about upgrade */
    upgrade* upgrade_class = nullptr;
    /* about lua */
    lua* lua_class = nullptr;
    /* about status */
    status*          status_class = nullptr;
    param*           param_class  = nullptr;
    QTimer*          resizeTimer  = nullptr;
    QGuiApplication* app;
    uint32_t         tabbar_width  = 130;
    uint32_t         tabbar_height = 100;
    /* user function */
public:
    /* about init */
    void ui_init(void);
    /* about serial */
    void serial_switch_ctrl(void);  //串口开关控制
    void serial_search(void);       //串口自动搜索
    /* about warning */
    void my_message_box(QString title, QString text);
    /* about upgrade */

    /* about lua */
    /* user slot */
public slots:
    /* about serial */
    int  serial_error_callback(QSerialPort::SerialPortError error);
    void serial_data_proc(void);
    void cmd_callback(uint8_t* frame, int32_t length);
    void printf_log_upgrade(QString str, uint8_t is_clear);

protected:
    bool eventFilter(QObject* watched, QEvent* event);
private slots:
    void handleResize(void);
private slots:
    void on_serial_switch_pushButton_clicked();
    void on_select_fw_pushButton_clicked();
    void on_start_upgrade_pushButton_clicked();
    void on_quit_upgrade_pushButton_clicked();
    void on_lua_select_file_pushButton_clicked();
    void on_lua_download_pushButton_clicked();
    void on_lua_logclear_pushButton_clicked();
    void on_lua_logsave_pushButton_clicked();
    void on_get_a_log_pushButton_clicked();
    void on_get_b_log_pushButton_clicked();
    void on_A_SOFT_STATUS_checkBox_clicked(bool checked);
    void on_B_SOFT_STATUS_checkBox_clicked(bool checked);
    void on_start_read_status_pushButton_clicked();
    void on_stop_read_status_pushButton_clicked();
    void on_read_label_pushButton_clicked();
    void on_clear_label_pushButton_clicked();
    void on_save_label_pushButton_clicked();
    void on_read_param_pushButton_clicked();
    void on_write_param_pushButton_clicked();
    void on_param_save_pushButton_clicked();
    void on_param_read_load_pushButton_clicked();
    void on_param_clear_pushButton_clicked();
};

#endif  // MAINWINDOW_H
