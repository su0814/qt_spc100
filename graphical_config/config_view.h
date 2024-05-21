#ifndef CONFIG_VIEW_H
#define CONFIG_VIEW_H
#include "graphical_config/config_photo.h"
#include "graphical_config_param.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QEvent>
#include <QGraphicsView>
#include <QScrollBar>
class MainWindow;
class config_view : public QGraphicsView {
    Q_OBJECT
public:
    config_view(QWidget* uiparent, QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

public:
    config_photo* config_photo_svg;
    /* 视角缩放 */
    qreal           maxscale = VIEW_MAX_SCALE;
    qreal           minscale = VIEW_MIN_SCALE;
    void            config_view_reset();
    QJsonObject     param_project_info(void);
    bool            param_project_parse(QJsonObject project);
    module_param_t* get_module_param(void);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;

private:
    QGraphicsScene* my_scene;
    module_param_t  module_param;

private:
    void view_init(QWidget* uiparent);
public slots:
    void on_a_nodeid_spinbox_valuechanged_slot(int value);
    void on_b_nodeid_spinbox_valuechanged_slot(int value);
    void on_master_nodeid_spinbox_valuechanged_slot(int value);
    void on_master_heartbeat_spinbox_valuechanged_slot(int value);
    void on_slave_heartbeat_spinbox_valuechanged_slot(int value);
};

#endif  // CONFIG_VIEW_H
