#ifndef CONFIG_VIEW_H
#define CONFIG_VIEW_H
#include "graphical_config/config_photo.h"
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
    qreal maxscale = VIEW_MAX_SCALE;
    qreal minscale = VIEW_MIN_SCALE;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    QGraphicsScene* my_scene;

private:
    void view_init(QWidget* uiparent);
};

#endif  // CONFIG_VIEW_H
