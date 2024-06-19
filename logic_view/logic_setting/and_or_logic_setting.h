#ifndef AND_OR_LOGIC_SETTING_H
#define AND_OR_LOGIC_SETTING_H

#include "logic_view/logic_view_config.h"
#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
namespace Ui {
class and_or_logic_setting;
}
class base_logic_block;
class and_or_logic_setting : public QDialog {
    Q_OBJECT

public:
    explicit and_or_logic_setting(base_logic_block* logic, QWidget* parent = 0);
    ~and_or_logic_setting();

private:
    base_logic_block*  baselogic;
    QList<QLabel*>     inputlabel;
    QList<QCheckBox*>  inputreverse;
    QList<QLabel*>     einputlabel;
    QList<QLineEdit*>  einputname;
    void               ui_init(void);
    block_base_param_t block_base_param;
    QJsonObject        old_param;

public:
    void setting_exec();

private slots:
    void on_verticalSlider_inputnum_valueChanged(int value);

    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::and_or_logic_setting* ui;
};

#endif  // AND_OR_LOGIC_SETTING_H
