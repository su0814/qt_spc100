#ifndef SPEED_CROSS_CHECK_SETTING_H
#define SPEED_CROSS_CHECK_SETTING_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
namespace Ui {
class speed_cross_check_setting;
}
class speed_logic_block;
class speed_cross_check_setting : public QDialog {
    Q_OBJECT

public:
    explicit speed_cross_check_setting(speed_logic_block* logic, QWidget* parent = 0);
    ~speed_cross_check_setting();

private:
    speed_logic_block* baselogic;
    QList<QLabel*>     einputlabel;
    QList<QLineEdit*>  einputname;
    QList<QLabel*>     eoutputlabel;
    QList<QLineEdit*>  eoutputname;
    int                outputnum = 2;
    void               ui_init(void);

public:
    void setting_exec();

private slots:

    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

    void on_checkBox_reliability_monitor_stateChanged(int arg1);

private:
    Ui::speed_cross_check_setting* ui;
};

#endif  // SPEED_CROSS_CHECK_SETTING_H
