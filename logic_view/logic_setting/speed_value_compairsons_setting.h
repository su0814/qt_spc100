#ifndef SPEED_VALUE_COMPAIRSONS_SETTING_H
#define SPEED_VALUE_COMPAIRSONS_SETTING_H

#include "logic_view/base_logic_block.h"
#include <QDialog>

namespace Ui {
class speed_value_compairsons_setting;
}
class speed_logic_block;
class speed_value_compairsons_setting : public QDialog {
    Q_OBJECT

public:
    explicit speed_value_compairsons_setting(speed_logic_block* logic, QWidget* parent = 0);
    ~speed_value_compairsons_setting();

private:
    speed_logic_block*              baselogic;
    block_base_param_t              block_base_param;
    QJsonObject                     old_param;
    speed_value_compairsons_param_t speed_value_compairsons_param;

public:
    void setting_exec();

private slots:
    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::speed_value_compairsons_setting* ui;
};

#endif  // SPEED_VALUE_COMPAIRSONS_SETTING_H
