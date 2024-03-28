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
    config_param(int x, int y, int w, int h, config_tool_e type, QWidget* mparent, QGraphicsItem* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;
    /********** 变量 **********/
public:
    config_tool_e get_type(void);

private:
    config_tool_e tool_type;
    /********** 事件 **********/
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};

#endif  // CONFIG_PARAM_H
