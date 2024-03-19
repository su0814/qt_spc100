#include "config_view.h"
#include "mainwindow.h"
#include <QSvgRenderer>
#include <climits>
config_view::config_view(QWidget* uiparent, QWidget* parent)
    : QGraphicsView(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )uiparent;
    view_init(uiparent);
}

/**
 * @brief 视图初始化
 */
void config_view::view_init(QWidget* uiparent)
{
    setInteractive(true);                            //启用交互模式
    setRenderHint(QPainter::Antialiasing);           //启用抗锯齿
    setRenderHint(QPainter::SmoothPixmapTransform);  //启用平滑的像素变换功能
    /* 图形项发生变化时，整个视口将被完全更新，重新绘制所有的图形项 */
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    /* 不使用抗锯齿 */
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    /* 禁用鼠标追踪 */
    setMouseTracking(false);
    /* 背景透明 */
    setStyleSheet("background: transparent;");
    /* 取消边框 */
    setFrameStyle(QFrame::NoFrame);
    verticalScrollBar()->setValue(0);
    horizontalScrollBar()->setValue(0);
    /* 设置场景 */
    my_scene = new QGraphicsScene(this);
    setScene(my_scene);
    /* 添加SPC100的矢量图 */
    config_photo_svg = new config_photo(QString(":/new/photo/photo/spc100.svg"), uiparent);
    my_scene->addItem(config_photo_svg);
    config_photo_svg->setPos(400, 100);
}

/* sys event */
void config_view::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    uint32_t screen_width = mainwindow->screen_width;
    qreal    ratio        = screen_width / DESKTOP_BASE_WIDTH;
    qreal    currentscale = transform().m11();
    scale(ratio / currentscale, ratio / currentscale);
    my_scene->setSceneRect(viewport()->rect());
}

void config_view::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
    qDebug() << event->pos();
}
