#ifndef APPLY_EXTERN_DEVICE_MONITORS_SETTING_H
#define APPLY_EXTERN_DEVICE_MONITORS_SETTING_H

#include "logic_view/base_logic_block.h"
#include <QDialog>
namespace Ui {
class apply_extern_device_monitors_setting;
}
class apply_logic_block;
class apply_extern_device_monitors_setting : public QDialog {
    Q_OBJECT

public:
    explicit apply_extern_device_monitors_setting(apply_logic_block* logic, QWidget* parent = 0);
    ~apply_extern_device_monitors_setting();

private:
    apply_logic_block* baselogic;
    block_base_param_t block_base_param;
    QJsonObject        old_param;
    apply_edm_param_t  apply_edm_param;

public:
    void setting_exec();
private slots:

    void on_spinBox_feedback_delay_time_valueChanged(int arg1);

    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::apply_extern_device_monitors_setting* ui;
};

#endif  // APPLY_EXTERN_DEVICE_MONITORS_SETTING_H
