#ifndef APPLY_EDGE_DETECTED_SETTING_H
#define APPLY_EDGE_DETECTED_SETTING_H

#include "logic_view/base_logic_block.h"
#include <QDialog>
namespace Ui {
class apply_edge_detected_setting;
}
class apply_logic_block;
class apply_edge_detected_setting : public QDialog {
    Q_OBJECT

public:
    explicit apply_edge_detected_setting(apply_logic_block* logic, QWidget* parent = 0);
    ~apply_edge_detected_setting();

private:
    apply_logic_block*        baselogic;
    block_base_param_t        block_base_param;
    QJsonObject               old_param;
    apply_edge_detect_param_t apply_edge_detect_param;

public:
    void setting_exec();
private slots:
    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::apply_edge_detected_setting* ui;
};

#endif  // APPLY_EDGE_DETECTED_SETTING_H
