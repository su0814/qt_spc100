#ifndef DELAY_ADJUST_ON_OFF_SETTING_H
#define DELAY_ADJUST_ON_OFF_SETTING_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
namespace Ui {
class delay_adjust_on_off_setting;
}
class delay_counter_logic_block;
class delay_adjust_on_off_setting : public QDialog {
    Q_OBJECT

public:
    explicit delay_adjust_on_off_setting(delay_counter_logic_block* logic, QWidget* parent = 0);
    ~delay_adjust_on_off_setting();

private:
    delay_counter_logic_block* baselogic;
    QList<QLabel*>             delaytime_name;
    QList<QSpinBox*>           delaytime_value;
    QList<QLabel*>             delaytime_fold;
    QList<QLabel*>             delaytime_result;
    QList<QLabel*>             einputlabel;
    QList<QLineEdit*>          einputname;
    void                       ui_init(void);

public:
    void setting_exec();
private slots:
    void on_verticalSlider_inputnum_valueChanged(int value);

    void on_spinBox_delay_time1_valueChanged(int arg1);

    void on_spinBox_delay_time2_valueChanged(int arg1);

    void on_spinBox_delay_time3_valueChanged(int arg1);

    void on_spinBox_delay_time4_valueChanged(int arg1);

    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::delay_adjust_on_off_setting* ui;
};

#endif  // DELAY_ADJUST_ON_OFF_SETTING_H
