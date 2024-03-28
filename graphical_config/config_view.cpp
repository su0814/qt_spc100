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
    // 启用缩放
    setInteractive(true);                            //启用交互模式
    setRenderHint(QPainter::Antialiasing);           //启用抗锯齿
    setRenderHint(QPainter::SmoothPixmapTransform);  //启用平滑的像素变换功能
    /* 图形视图的变换锚点被设置为鼠标下方， 平移、缩放、旋转等变换操作时，变换的中心点将根据鼠标的位置而动态改变 */
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    /* 图形项发生变化时，整个视口将被完全更新，重新绘制所有的图形项 */
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    /* 图形视图的拖拽模式被设置为滚动手势拖拽，使用鼠标或触摸手势来拖拽视图，实现滚动和平移的效果 */
    setDragMode(QGraphicsView::ScrollHandDrag);
    setStyleSheet("background: transparent;");
    // 缩放设置
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    // 开启鼠标追踪。视图将能够跟踪鼠标的移动，无论鼠标是否按下任何按钮
    setMouseTracking(false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    /* 取消边框 */
    setFrameStyle(QFrame::NoFrame);
    verticalScrollBar()->setValue(0);
    horizontalScrollBar()->setValue(0);
    /* 设置场景 */
    my_scene = new QGraphicsScene(this);
    setScene(my_scene);
    my_scene->setSceneRect(0, 0, 3000, 3000);
    /* 添加SPC100的矢量图 */
    config_photo_svg = new config_photo(QString(":/new/photo/photo/spc100.svg"), uiparent);
    my_scene->addItem(config_photo_svg);
    config_photo_svg->setPos(0, 50);
    scale(1, 1);
}

/* sys event */

void config_view::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
}

/**
 * @brief 鼠标滚轮事件
 * @param event
 */
void config_view::wheelEvent(QWheelEvent* event)
{
    int delta = event->delta();                      //获取鼠标滑轮滚动的距离
    if (event->modifiers() & Qt::ControlModifier) {  // ctrl+鼠标滑轮进行缩放
        qreal scaleFactor  = 0.05;                   // 缩放因子--缩放的速度调节这里
        qreal currentscale = transform().m11();
        qreal targetscale  = (currentscale + scaleFactor) / currentscale;
        if (delta > 0 && currentscale < maxscale) {
            scale(targetscale, targetscale);  // 向上滚动，放大视图
        } else if (delta < 0 && currentscale > minscale) {
            scale(1 / targetscale, 1 / targetscale);
        }
        event->accept();
    } else {
        int         scrollStep        = delta / 8;  // 鼠标滑轮运动时界面上下移动的速度在这里计算
        QScrollBar* verticalScrollBar = this->verticalScrollBar();
        int         currentValue      = verticalScrollBar->value();
        int         newValue          = currentValue - scrollStep;
        verticalScrollBar->setValue(newValue);
        event->accept();
    }
}
