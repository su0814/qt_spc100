#ifndef COUNTER_LOGGING_SETTING_H
#define COUNTER_LOGGING_SETTING_H

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
namespace Ui {
class counter_logging_setting;
}
class delay_counter_logic_block;
class counter_logging_setting : public QDialog {
    Q_OBJECT

public:
    explicit counter_logging_setting(delay_counter_logic_block* logic, QWidget* parent = 0);
    ~counter_logging_setting();

private:
    delay_counter_logic_block* baselogic;
    QList<QLabel*>             einputlabel;
    QList<QLineEdit*>          einputname;
    QList<QLabel*>             inputlabel;
    QList<QLineEdit*>          log_text;
    QList<QComboBox*>          log_edge;
    void                       ui_init(void);

public:
    void setting_exec();
private slots:

    void on_verticalSlider_inputnum_valueChanged(int value);

    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::counter_logging_setting* ui;
};

#endif  // COUNTER_LOGGING_SETTING_H
