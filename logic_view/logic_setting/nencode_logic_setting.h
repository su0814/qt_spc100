#ifndef NENCODE_LOGIC_SETTING_H
#define NENCODE_LOGIC_SETTING_H
#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
namespace Ui {
class nencode_logic_setting;
}
class base_logic_block;
class nencode_logic_setting : public QDialog {
    Q_OBJECT

public:
    explicit nencode_logic_setting(base_logic_block* logic, QWidget* parent = 0);
    ~nencode_logic_setting();

private:
    base_logic_block* baselogic;
    QList<QLabel*>    inputlabel;
    QList<QLabel*>    einputlabel;
    QList<QLineEdit*> einputname;
    QList<QLabel*>    eoutputlabel;
    QList<QLineEdit*> eoutputname;
    QStringList       sys_output_label;
    QStringList       logic_output_label;
    int               outputnum = 0;
    void              ui_init(void);
    void              set_outputnum(int num, bool fault);

public:
    void setting_exec();

private slots:
    void on_verticalSlider_inputnum_valueChanged(int value);

    void on_checkBox_fault_stateChanged(int arg1);

    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::nencode_logic_setting* ui;
};

#endif  // NENCODE_LOGIC_SETTING_H
