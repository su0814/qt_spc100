#ifndef CONFIG_PARAM_H
#define CONFIG_PARAM_H
#include "graphical_config_param.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QGraphicsRectItem>

class MainWindow;
class config_param : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    config_param(int x, int y, int w, int h, config_tool_e type, QWidget* mparent, module_param_t* param,
        QGraphicsItem* parent = nullptr);
    Ui::MainWindow* ui = nullptr;
    MainWindow* mainwindow = nullptr;
    /********** 变量 **********/
public:
    config_tool_e get_type(void);

private:
    module_param_t* module_param;
    config_tool_e tool_type;
    QComboBox* pi1_af_combo;
    QComboBox* pi2_af_combo;

    /********** 函数 **********/
private:
    void sys_param_config_dialog(void);
    void can_param_config_dialog(void);
    void ai_param_config_dialog(void);
    void di_param_config_dialog(void);
    void relay_param_config_dialog(void);
    void mos_param_config_dialog(void);
    void encode_param_config_dialog(void);
    void pi_param_config_dialog(void);
    void deceleration_curve_config_dialog(void);
    /********** 事件 **********/
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private slots:
    void pi1_afstate_changed_slot(int index);
    void pi2_afstate_changed_slot(int index);
    void ai1_afstate_changed_slot(int index);
    void ai2_afstate_changed_slot(int index);
    void piqep_crosscheck_slot(int index);
    void param_piqepspeed_select_slot(int index);
};

#endif // CONFIG_PARAM_H
