#ifndef USERCODE_DIALOG_H
#define USERCODE_DIALOG_H

#include <QDialog>

namespace Ui {
class Usercode_Dialog;
}

class Usercode_Dialog : public QDialog {
    Q_OBJECT

public:
    explicit Usercode_Dialog(QWidget* parent = 0);
    ~Usercode_Dialog();

public:
    void set_usercode(QString code);

private:
    Ui::Usercode_Dialog* ui;
};

#endif  // USERCODE_DIALOG_H
