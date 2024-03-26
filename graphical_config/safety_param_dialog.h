#ifndef SAFETY_PARAM_DIALOG_H
#define SAFETY_PARAM_DIALOG_H

#include <QDialog>

namespace Ui {
class Safety_Param_Dialog;
}

class Safety_Param_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Safety_Param_Dialog(QWidget *parent = 0);
    ~Safety_Param_Dialog();

private:
    Ui::Safety_Param_Dialog *ui;
};

#endif // SAFETY_PARAM_DIALOG_H
