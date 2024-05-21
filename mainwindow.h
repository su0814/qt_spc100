#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config/def.h"
#include "device/log_dialog.h"
#include "device/mydevice.h"
#include "device/status.h"
#include "device/upgrade.h"
#include "device/version_dialog.h"
#include "graphical_config/config_menu.h"
#include "graphical_config/config_view.h"
#include "graphical_config/navigate_menu.h"
#include "logic_view/logic_menu.h"
#include "logic_view/logic_view.h"
#include "my_serialport/my_serialport.h"
#include "my_serialport/transportcrc.h"
#include "project/about_prajna.h"
#include "project/project_debug.h"
#include "project/project_management.h"
#include "project/project_transmit.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QMainWindow>

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
    project_transmit* project_transmit_class = nullptr;
    /* about status */
    status*            status_class = nullptr;
    QTimer             ui_resize_timer;
    int                screen_width              = DESKTOP_BASE_WIDTH;
    int                screen_height             = DESKTOP_BASE_HEIGHT;
    user_permissions_e user_permissions          = USER_REGULAR;
    QString            user_authorization_passwd = "";
    /* about logic */
    logic_view*         logic_view_class         = nullptr;
    project_management* project_management_class = nullptr;
    mydevice*           mydevice_class           = nullptr;
    project_debug*      project_debug_class      = nullptr;
    about_prajna*       about_prajna_class       = nullptr;
    config_view*        config_view_class        = nullptr;
    logic_menu*         logic_menu_class         = nullptr;
    /* about dialog */
    log_dialog*     log_dialog_class     = nullptr;
    version_Dialog* version_dialog_class = nullptr;
    config_menu*    config_menu_class    = nullptr;
    navigate_menu*  navigate_menu_class  = nullptr;
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
};

#endif  // MAINWINDOW_H
