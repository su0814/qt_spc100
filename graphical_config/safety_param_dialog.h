#ifndef SAFETY_PARAM_DIALOG_H
#define SAFETY_PARAM_DIALOG_H

#include "graphical_config_param.h"
#include <QComboBox>
#include <QDialog>
#include <QSpinBox>

class MainWindow;
namespace Ui {
class Safety_Param_Dialog;
}

class Safety_Param_Dialog : public QDialog {
    Q_OBJECT

public:
    explicit Safety_Param_Dialog(module_param_t* param, config_block_data_t* data, config_user_data_t* userdata,
                                 QWidget* parent = 0);
    ~Safety_Param_Dialog();

private:
    module_param_t*      safe_param;
    config_block_data_t* config_data;
    config_user_data_t*  user_data;

public:
    static void        module_param_init(module_param_t* module_param);
    static QJsonObject param_project_info(module_param_t* module_param);
    static bool        param_project_parse(QJsonObject project, module_param_t* module_param);
    void               my_exec();

private slots:

    void on_apply_pushButton_clicked();

    void on_cancle_pushButton_clicked();

private:
    void init(void);

private:
    Ui::Safety_Param_Dialog* ui;
};

#endif  // SAFETY_PARAM_DIALOG_H
