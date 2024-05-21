#ifndef RS_TRIGGER_LOGIC_SETTING_H
#define RS_TRIGGER_LOGIC_SETTING_H

#include <QDialog>

namespace Ui {
class rs_trigger_logic_setting;
}
class base_logic_block;
class rs_trigger_logic_setting : public QDialog {
    Q_OBJECT

public:
    explicit rs_trigger_logic_setting(base_logic_block* logic, QWidget* parent = 0);
    ~rs_trigger_logic_setting();

private:
    base_logic_block* baselogic;

public:
    void setting_exec();
private slots:
    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::rs_trigger_logic_setting* ui;
};

#endif  // RS_TRIGGER_LOGIC_SETTING_H
