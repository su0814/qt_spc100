#ifndef DELAY_ON_OFF_SETTING_H
#define DELAY_ON_OFF_SETTING_H

#include <QDialog>

namespace Ui {
class delay_on_off_setting;
}
class delay_counter_logic_block;
class delay_on_off_setting : public QDialog {
    Q_OBJECT

public:
    explicit delay_on_off_setting(delay_counter_logic_block* logic, QWidget* parent = 0);
    ~delay_on_off_setting();

private:
    delay_counter_logic_block* baselogic;

public:
    void setting_exec();
private slots:
    void on_spinBox_delay_time_valueChanged(int arg1);

    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::delay_on_off_setting* ui;
};

#endif  // DELAY_ON_OFF_SETTING_H
