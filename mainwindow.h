#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "about_prajna.h"
#include "def.h"
#include "list_condi/condition_view.h"
#include "logic_view.h"
#include "logic_view/coroutine_lua.h"
#include "logic_view/logic_tools.h"
#include "my_serialport.h"
#include "mydevice.h"
#include "project_debug.h"
#include "project_management.h"
#include "project_report.h"
#include "transportcrc.h"
#include "ui_mainwindow.h"
#include <QDialog>
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
    void user_authorization_passwd_window(void);

public:
    /* about serial */
    my_serialport* my_serial         = nullptr;
    transportcrc*  transport         = nullptr;
    bool           serial_is_connect = false;
    /* about upgrade */
    upgrade* upgrade_class = nullptr;
    /* about lua */
    lua* lua_class = nullptr;
    /* about status */
    status*            status_class = nullptr;
    param*             param_class  = nullptr;
    QTimer             ui_resize_timer;
    int                screen_width;
    int                screen_height;
    user_permissions_e user_permissions          = USER_REGULAR;
    QString            user_authorization_passwd = "";
    /* about logic */
    condition_view*     condition_view_class     = nullptr;
    logic_tools*        logic_tools_class        = nullptr;
    logic_view*         logic_view_class         = nullptr;
    coroutine_lua*      coroutine_lua_class      = nullptr;
    project_management* project_management_class = nullptr;
    project_report*     project_report_class     = nullptr;
    mydevice*           mydevice_class           = nullptr;
    project_debug*      project_debug_class      = nullptr;
    about_prajna*       about_prajna_class       = nullptr;
    /* user function */

private:
    /* 串口配置弹窗 */
    QDialog     serial_dialog;
    QComboBox   serial_port_combobox;
    QComboBox   serial_baudrate_combobox;
    QPushButton serial_connect_button;

public:
    /* about init */
    void ui_init(void);
    /* about serial */
    void serial_search(void);  //串口自动搜索
    /* about warning */
    int my_message_box(QString title, QString text, bool add_cancel);
    /* about upgrade */
    void user_authorization(void);
    /* about lua */
    void dispaly_status_message(QString info, int time);
    /* user slot */
public slots:
    /* about serial */
    int  serial_error_callback(QSerialPort::SerialPortError error);
    void serial_data_proc(void);
    void cmd_callback(uint8_t* frame, int32_t length);

protected:
    bool eventFilter(QObject* watched, QEvent* event);
private slots:
    void ui_resize_slot(void);
    void serial_connect_slot(void);
private slots:
    void on_action_serial_close_triggered();
    void on_action_permissions_triggered();
    void on_action_serial_open_triggered();
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
    void on_param_clear_pushButton_clicked();
    void on_aslave_nodeid_spinbox_editingFinished();
    void on_bslave_nodeid_spinbox_editingFinished();
    void on_master_nodeid_spinbox_editingFinished();
    void on_check_bt_spinbox_editingFinished();
    void on_send_bt_spinbox_editingFinished();
    void on_pdo_pt_spinbox_editingFinished();
    void on_param_sai_sample_interval_editingFinished();
    void on_param_spi_sample_interval_editingFinished();
    void on_param_pi_qep_sample_interval_editingFinished();
    void on_param_sqep_sample_interval_editingFinished();
};

#endif  // MAINWINDOW_H
