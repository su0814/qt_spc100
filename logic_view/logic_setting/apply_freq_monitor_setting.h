#ifndef APPLY_FREQ_MONITOR_SETTING_H
#define APPLY_FREQ_MONITOR_SETTING_H

#include <QDialog>
#include <QLabel>
#include <QSpinBox>
namespace Ui {
class apply_freq_monitor_setting;
}
class apply_logic_block;
class apply_freq_monitor_setting : public QDialog {
    Q_OBJECT

public:
    explicit apply_freq_monitor_setting(apply_logic_block* logic, QWidget* parent = 0);
    ~apply_freq_monitor_setting();

private:
    apply_logic_block* baselogic;
    QList<QLabel*>     paramname;
    QList<QLabel*>     paramfold;
    QList<QSpinBox*>   paramvalue;
    QList<QLabel*>     paramresult;
    QList<QLabel*>     einputlabel;
    QList<QLineEdit*>  einputname;
    QList<QLabel*>     eoutputlabel;
    QList<QLineEdit*>  eoutputname;
    QStringList        sys_output_label;
    QStringList        logic_output_label;
    int                outputnum = 0;
    void               ui_init(void);
    void               set_outputnum(int num, bool fault);
    void               freq1_param_detected(void);
    void               freq2_param_detected(void);

public:
    void setting_exec();
private slots:
    void on_spinBox_min_cycle1_valueChanged(int arg1);

    void on_spinBox_max_cycle1_valueChanged(int arg1);

    void on_spinBox_pulse_time1_valueChanged(int arg1);

    void on_spinBox_pulse_time_tolerance1_valueChanged(int arg1);

    void on_spinBox_min_cycle2_valueChanged(int arg1);

    void on_spinBox_max_cycle2_valueChanged(int arg1);

    void on_spinBox_pulse_time2_valueChanged(int arg1);

    void on_spinBox_pulse_time_tolerance2_valueChanged(int arg1);

    void on_checkBox_fault_stateChanged(int arg1);

    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

    void on_checkBox_freq2_enable_stateChanged(int arg1);

private:
    Ui::apply_freq_monitor_setting* ui;
};

#endif  // APPLY_FREQ_MONITOR_SETTING_H
