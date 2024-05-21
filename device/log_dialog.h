#ifndef LOG_DIALOG_H
#define LOG_DIALOG_H
#include "config/data_config.h"
#include <QDialog>

namespace Ui {
class log_dialog;
}
class MainWindow;
class log_dialog : public QDialog {
    Q_OBJECT

public:
    explicit log_dialog(QWidget* parent = 0);
    ~log_dialog();
    MainWindow* mainwindow = nullptr;

public:
    void cmd_report_response(uint8_t* frame, int32_t length);

private slots:
    void on_clear_log_clicked();

    void on_save_log_clicked();

    void on_read_a_log_clicked();

    void on_read_b_log_clicked();

private:
    QStringList sync_id_list;
    void        log_display(uint8_t sub, uint8_t* frame, int32_t length);

private:
    Ui::log_dialog* ui;

private slots:
    void device_line_status_change_slots(device_line_status_e status);
    void log_dialog_show_slot(void);
};

#endif  // LOG_DIALOG_H
