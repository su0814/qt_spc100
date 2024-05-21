#ifndef NOT_XOR_XNOR_LOGIC_SETTING_H
#define NOT_XOR_XNOR_LOGIC_SETTING_H

#include <QDialog>

namespace Ui {
class not_xor_xnor_logic_setting;
}
class base_logic_block;
class not_xor_xnor_logic_setting : public QDialog {
    Q_OBJECT

public:
    explicit not_xor_xnor_logic_setting(base_logic_block* logic, QWidget* parent = 0);
    ~not_xor_xnor_logic_setting();

private:
    base_logic_block* baselogic;

public:
    void setting_exec();

private slots:
    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::not_xor_xnor_logic_setting* ui;
};

#endif  // NOT_XOR_XNOR_LOGIC_SETTING_H
