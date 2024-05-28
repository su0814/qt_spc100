#ifndef SPEED_MOTIONLESS_SETTING_H
#define SPEED_MOTIONLESS_SETTING_H

#include <QDialog>

namespace Ui {
class speed_motionless_setting;
}
class speed_logic_block;
class speed_motionless_setting : public QDialog {
    Q_OBJECT

public:
    explicit speed_motionless_setting(speed_logic_block* logic, QWidget* parent = 0);
    ~speed_motionless_setting();

private:
    speed_logic_block* baselogic;

public:
    void setting_exec();

private slots:
    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::speed_motionless_setting* ui;
};

#endif  // SPEED_MOTIONLESS_SETTING_H
