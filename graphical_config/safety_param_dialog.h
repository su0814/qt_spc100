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
    explicit Safety_Param_Dialog(QWidget* parent = 0);
    ~Safety_Param_Dialog();

private:
    QStringList       config_title;
    module_param_t    module_param;
    QList<QComboBox*> di_cat_list;
    QList<QComboBox*> relay_cat_list;
    QList<QComboBox*> ai_cat_list;
    QList<QComboBox*> ss_relevant_list;
    QList<QComboBox*> qep_select_list;
    QList<QComboBox*> piqep_select_list;
    QList<QSpinBox*>  decelerate_check_intreval_list;
    QList<QSpinBox*>  decelerate_sublevel1_threshold_list;
    QList<QSpinBox*>  decelerate_sublevel2_threshold_list;
    QList<QSpinBox*>  decelerate_sublevel3_threshold_list;

public:
    void           exec_from_id(config_tool_e id);
    void           module_param_init(void);
    QJsonObject    param_project_info(void);
    bool           param_project_parse(QJsonObject project);
    void           param_ss_set(uint8_t state);
    uint8_t        param_ss_get(void);
    void           param_reset();
    module_param_t get_module_param(void);
private slots:
    void param_piqepspeed_select_slot(int index);
    void ai1_af_comboBox_currentIndexChanged(int index);
    void ai2_af_comboBox_currentIndexChanged(int index);
    void pi1_af_comboBox_currentIndexChanged(int index);
    void pi2_af_comboBox_currentIndexChanged(int index);
    void ss_combo_currentIndexChanged(int index);
private slots:
    void on_a_nodeid_spinBox_editingFinished();

    void on_b_nodeid_spinBox_editingFinished();

    void on_master_nodeid_spinBox_editingFinished();

    void on_master_heartbeat_spinBox_editingFinished();

    void on_slave_heartbeat_spinBox_editingFinished();

    void on_pdo_interval_spinBox_editingFinished();

    void on_piqep12_cross_combo_currentIndexChanged(int index);

private:
    void init(void);
    void param_to_ui(void);
    void ui_to_param(void);

private:
    Ui::Safety_Param_Dialog* ui;
    MainWindow*              mainwindow = nullptr;
};

#endif  // SAFETY_PARAM_DIALOG_H
