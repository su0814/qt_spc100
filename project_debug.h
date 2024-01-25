#ifndef PROJECT_DEBUG_H
#define PROJECT_DEBUG_H

#include "config/data_config.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QWidget>
typedef enum {
    DEBUG_STATE_IDLE = 0,
    DEBUG_STATE_ING,
} debug_state_e;

class MainWindow;
class project_debug : public QWidget {
    Q_OBJECT
public:
    explicit project_debug(QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

private:
    ack_enter_t    project_verify_ack;
    QTimer         project_verify_timer;
    QTimer         project_debug_timer;
    debug_state_e  debug_state = DEBUG_STATE_IDLE;
    QList<QLabel*> output_led_label_list;
    QList<QLabel*> defaultsf_label_list;

private:
    void ui_init(void);
    bool project_verify(void);
    void project_debug_start(void);
    void project_debug_stop(void);
    void project_debug_data_cmd_prase(uint8_t* frame, int32_t length);
    void set_led(QLabel* label, QString rgb_color);

public:
    void project_debug_cmd_prase(uint8_t* frame, int32_t length);

signals:

private slots:
    void project_verify_enter_slot(void);
    void project_debug_slot(void);
    void project_debug_action_slot(void);
};

#endif  // PROJECT_DEBUG_H
