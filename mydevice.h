#ifndef MYDEVICE_H
#define MYDEVICE_H

#include "config/data_config.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QWidget>

#define DEVICE_PASSWD_LENGTH (16)
#define DEVICE_RETRY_TIME    (500)

typedef enum {
    DEVICE_CHANGE_PASSWD_STATUS_IDLE = 0,
    DEVICE_CHANGE_PASSWD_STATUS_ING,
    DEVICE_CHANGE_PASSWD_STATUS_SUCCESS,
    DEVICE_CHANGE_PASSWD_STATUS_FAIL,
} device_change_passwd_status_t;

class MainWindow;
class mydevice : public QWidget {
    Q_OBJECT
public:
    explicit mydevice(QWidget* uiparent, QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

private:
    ack_enter_t                   pass_verify_ack;
    QTimer                        pass_verify_timer;
    bool                          change_passwd_reply[2]      = { false, false };
    uint8_t                       change_passwd_error[2]      = { 0 };
    device_change_passwd_status_t device_change_passwd_status = DEVICE_CHANGE_PASSWD_STATUS_IDLE;
    QTimer                        change_userpass_timer;
    QString                       old_passwd_str;
    QString                       new_passwd_str;

private:
    void device_change_passwd_send_cmd(void);
    void device_pass_verify_send_cmd(void);

public:
    void device_cmd_response(uint8_t* frame, int32_t length);
    bool device_pass_verify(void);
signals:

private slots:
    void device_change_userpass_slot(void);
    void change_userpass_enter_slot(void);
    void device_pass_verify_enter_slot(void);
};

#endif  // MYDEVICE_H
