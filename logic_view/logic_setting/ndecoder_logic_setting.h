#ifndef NDECODER_LOGIC_SETTING_H
#define NDECODER_LOGIC_SETTING_H
#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
namespace Ui {
class ndecoder_logic_setting;
}
class base_logic_block;
class ndecoder_logic_setting : public QDialog {
    Q_OBJECT

public:
    explicit ndecoder_logic_setting(base_logic_block* logic, QWidget* parent = 0);
    ~ndecoder_logic_setting();

private:
    base_logic_block* baselogic;
    QList<QLabel*>    inputlabel;
    QList<QLabel*>    einputlabel;
    QList<QLineEdit*> einputname;
    QList<QLabel*>    eoutputlabel;
    QList<QLineEdit*> eoutputname;
    int               outputnum = 0;
    void              ui_init(void);
    void              set_outputnum(int num);

public:
    void setting_exec();

private slots:
    void on_verticalSlider_inputnum_valueChanged(int value);

    void on_pushButton_cancle_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::ndecoder_logic_setting* ui;
};

#endif  // NDECODER_LOGIC_SETTING_H