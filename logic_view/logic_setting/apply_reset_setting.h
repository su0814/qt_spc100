#ifndef APPLY_RESET_SETTING_H
#define APPLY_RESET_SETTING_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
namespace Ui {
class apply_reset_setting;
}
class apply_logic_block;
class apply_reset_setting : public QDialog {
    Q_OBJECT

public:
    explicit apply_reset_setting(apply_logic_block* logic, QWidget* parent = 0);
    ~apply_reset_setting();

private:
    apply_logic_block* baselogic;
    QList<QLabel*>     inputlabel;
    QList<QLabel*>     einputlabel;
    QList<QLineEdit*>  einputname;
    void               ui_init(void);

public:
    void setting_exec();
private slots:
    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

    void on_verticalSlider_inputnum_valueChanged(int value);

private:
    Ui::apply_reset_setting* ui;
};

#endif  // APPLY_RESET_SETTING_H