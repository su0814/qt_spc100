#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "about_prajna.h"
#include "def.h"
#include "device/version_dialog.h"
#include "graphical_config/config_view.h"
#include "graphical_config/safety_param_dialog.h"
#include "list_condi/condition_view.h"
#include "log_dialog.h"
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
    QTimer             ui_resize_timer;
    int                screen_width              = DESKTOP_BASE_WIDTH;
    int                screen_height             = DESKTOP_BASE_HEIGHT;
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
    config_view*        config_view_class        = nullptr;
    /* about dialog */
    log_dialog*          log_dialog_class          = nullptr;
    version_Dialog*      version_dialog_class      = nullptr;
    Safety_Param_Dialog* safety_param_dialog_class = nullptr;
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
    int my_message_box(QString text, messabe_type_e type);
    /* about upgrade */
    void user_authorization(void);
    /* about lua */
    void dispaly_status_message(QString info, int time);
    /* about tabwidget */
    void tabwidget_setenable(bool state);
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
    void on_A_SOFT_STATUS_checkBox_clicked(bool checked);
    void on_B_SOFT_STATUS_checkBox_clicked(bool checked);
    void on_start_read_status_pushButton_clicked();
    void on_stop_read_status_pushButton_clicked();
    void on_read_label_pushButton_clicked();
    void on_clear_label_pushButton_clicked();
    void on_save_label_pushButton_clicked();

    void on_lineEdit_projectname_textChanged(const QString& arg1);
    void on_lineEdit_author_name_textChanged(const QString& arg1);
    void on_lineEdit_company_name_textChanged(const QString& arg1);
    void on_lineEdit_project_version_textChanged(const QString& arg1);
    void on_lineEdit_project_path_textChanged(const QString& arg1);
};

#endif  // MAINWINDOW_H
