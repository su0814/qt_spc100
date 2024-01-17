#ifndef PROJECT_MANAGEMENT_H
#define PROJECT_MANAGEMENT_H

#include "config/data_config.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QWidget>
class MainWindow;
class project_management : public QWidget {
    Q_OBJECT
public:
    explicit project_management(QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

public:
    project_info_t            project_info;
    QByteArray                total_file_data;
    project_management_info_t project_management_info;

private:
    ack_enter_t project_verify_ack;
    QTimer      project_verify_timer;

private:
    void       project_save(void);
    bool       projec_info_creat(void);
    void       project_management_reset(void);
    QByteArray project_lua_code_creat(void);
    QByteArray project_file_creat(void);
    void       project_file_prase(QByteArray file);
    void       project_verify_send_cmd(void);
    void       project_transmit_to_device(void);
    void       project_readback_from_device(void);

public:
    bool project_verify(void);
    void project_cmd_response(uint8_t* frame, int32_t length);
signals:

private slots:
    void project_new_slot(void);
    int  project_save_slot(void);
    void project_import_slot(void);
    void project_transmit_to_device_slot(void);
    void project_readback_from_device_slot(void);
    void project_advanced_program_slot(int state);
    void lua_debug_creat_slot(void);
    void project_verify_enter_slot(void);
};

#endif  // PROJECT_MANAGEMENT_H
