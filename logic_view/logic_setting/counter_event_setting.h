#ifndef COUNTER_EVENT_SETTING_H
#define COUNTER_EVENT_SETTING_H

#include "logic_view/base_logic_block.h"
#include <QDialog>
#include <QLineEdit>
namespace Ui {
class counter_event_setting;
}
class delay_counter_logic_block;
class counter_event_setting : public QDialog {
    Q_OBJECT

public:
    explicit counter_event_setting(delay_counter_logic_block* logic, QWidget* parent = 0);
    ~counter_event_setting();

private:
    delay_counter_logic_block* baselogic;
    QList<QLineEdit*>          einputname;
    QList<QLineEdit*>          eoutputname;
    void                       ui_init(void);
    block_base_param_t         block_base_param;
    QJsonObject                old_param;
    counter_event_param_t      counter_event_param;

public:
    void setting_exec();
private slots:
    void on_comboBox_upover_reset_mode_currentIndexChanged(int index);

    void on_comboBox_downover_reset_mode_currentIndexChanged(int index);

    void on_spinBox_over_keep_time_valueChanged(int arg1);

    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

    void on_spinBox_upover_value_valueChanged(int arg1);

    void on_spinBox_start_value_valueChanged(int arg1);

private:
    Ui::counter_event_setting* ui;
};

#endif  // COUNTER_EVENT_SETTING_H
