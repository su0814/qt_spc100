#ifndef SPEED_MONITOR_SETTING_H
#define SPEED_MONITOR_SETTING_H

#include "logic_view/base_logic_block.h"
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
namespace Ui {
class speed_monitor_setting;
}
class speed_logic_block;
class speed_monitor_setting : public QDialog {
    Q_OBJECT

public:
    explicit speed_monitor_setting(speed_logic_block* logic, QWidget* parent = 0);
    ~speed_monitor_setting();

private:
    speed_logic_block*               baselogic;
    QList<QLabel*>                   ramplabel;
    QList<QGroupBox*>                ramp_monitor_group;
    QList<QLineEdit*>                einputname;
    QList<QSpinBox*>                 ramp_time;
    QList<QSpinBox*>                 ramp_speed;
    QList<QSpinBox*>                 ramp_max_speed;
    QList<QSpinBox*>                 ramp_min_speed;
    int                              inputnum = 4;
    QStringList                      error_info;
    int                              error_flag = 0;
    void                             ui_init(void);
    void                             ramp_value_check(int id);
    block_base_param_t               block_base_param;
    QJsonObject                      old_param;
    speed_decelerate_monitor_param_t speed_decelerate_monitor_param;

public:
    void setting_exec();
private slots:
    void on_spinBox_ramp_speed_valueChanged(int arg1);

    void on_spinBox_ramp_max_speed_valueChanged(int arg1);

    void on_spinBox_ramp_min_speed_valueChanged(int arg1);

    void on_spinBox_ramp_speed_2_valueChanged(int arg1);

    void on_spinBox_ramp_max_speed_2_valueChanged(int arg1);

    void on_spinBox_ramp_min_speed_2_valueChanged(int arg1);

    void on_spinBox_ramp_speed_3_valueChanged(int arg1);

    void on_spinBox_ramp_max_speed_3_valueChanged(int arg1);

    void on_spinBox_ramp_min_speed_3_valueChanged(int arg1);

    void on_spinBox_ramp_speed_4_valueChanged(int arg1);

    void on_spinBox_ramp_max_speed_4_valueChanged(int arg1);

    void on_spinBox_ramp_min_speed_4_valueChanged(int arg1);

    void on_verticalSlider_rampnum_valueChanged(int value);

    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::speed_monitor_setting* ui;
};

#endif  // SPEED_MONITOR_SETTING_H
