#include "config_view.h"
#include "algorithm/MD5/md5.h"
#include "mainwindow.h"
#include <QScrollBar>
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
    //    setStyleSheet("background: transparent;");
    // 缩放设置
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    // 开启鼠标追踪。视图将能够跟踪鼠标的移动，无论鼠标是否按下任何按钮
    setMouseTracking(false);

    /* 取消边框 */
    setFrameStyle(QFrame::NoFrame);

    /* 设置场景 */
    my_scene = new QGraphicsScene(this);
    setScene(my_scene);
    my_scene->setSceneRect(0, 0, 3000, 3000);
    QScrollBar* vScrollBar = this->verticalScrollBar();
    vScrollBar->setMinimum(0);
    vScrollBar->setMaximum(3000);
    // vScrollBar->setValue(1250);
    QScrollBar* hScrollBar = this->horizontalScrollBar();
    hScrollBar->setMinimum(0);
    hScrollBar->setMaximum(3000);
    // hScrollBar->setValue(1325);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    /* 添加SPC100的矢量图 */
    config_photo_svg = new config_photo(QString(":/new/photo/photo/spc100.svg"), uiparent);
    my_scene->addItem(config_photo_svg);
    config_photo_svg->setPos(my_scene->sceneRect().center() - config_photo_svg->sceneBoundingRect().center());
    /* safe param init */
    config_view_reset();
    connect(mainwindow->ui->a_nodeid_spinBox, SIGNAL(valueChanged(int)), this,
            SLOT(on_a_nodeid_spinbox_valuechanged_slot(int)));
    connect(mainwindow->ui->b_nodeid_spinBox, SIGNAL(valueChanged(int)), this,
            SLOT(on_b_nodeid_spinbox_valuechanged_slot(int)));
    connect(mainwindow->ui->master_nodeid_spinBox, SIGNAL(valueChanged(int)), this,
            SLOT(on_master_nodeid_spinbox_valuechanged_slot(int)));
    connect(mainwindow->ui->master_heartbeat_spinBox, SIGNAL(valueChanged(int)), this,
            SLOT(on_master_heartbeat_spinbox_valuechanged_slot(int)));
    connect(mainwindow->ui->slave_heartbeat_spinBox, SIGNAL(valueChanged(int)), this,
            SLOT(on_slave_heartbeat_spinbox_valuechanged_slot(int)));
}

/* param */
void config_view::config_view_reset()
{
    Safety_Param_Dialog::module_param_init(&module_param);
    mainwindow->ui->a_nodeid_spinBox->blockSignals(true);
    mainwindow->ui->b_nodeid_spinBox->blockSignals(true);
    mainwindow->ui->master_nodeid_spinBox->blockSignals(true);
    mainwindow->ui->cross_check_interval_spinbox->setValue(module_param.check_factor);
    mainwindow->ui->can_baudrate_comboBox->setCurrentText(QString::number(module_param.can_baudrate));
    mainwindow->ui->a_nodeid_spinBox->setValue(module_param.can_slave_nodeID_A);
    mainwindow->ui->b_nodeid_spinBox->setValue(module_param.can_slave_nodeID_B);
    mainwindow->ui->master_nodeid_spinBox->setValue(module_param.can_master_nodeID);
    mainwindow->ui->master_heartbeat_spinBox->setValue(module_param.can_hb_consumer_gap);
    mainwindow->ui->slave_heartbeat_spinBox->setValue(module_param.can_hb_producer_gap);
    mainwindow->ui->pdo_interval_spinBox->setValue(module_param.can_pdo_time_gap);
    mainwindow->ui->a_nodeid_spinBox->blockSignals(false);
    mainwindow->ui->b_nodeid_spinBox->blockSignals(false);
    mainwindow->ui->master_nodeid_spinBox->blockSignals(false);
    config_photo_svg->config_photo_reset();
    centerOn(my_scene->sceneRect().center());
}

QJsonObject config_view::param_project_info()
{
    module_param.check_factor        = mainwindow->ui->cross_check_interval_spinbox->value();
    module_param.can_baudrate        = mainwindow->ui->can_baudrate_comboBox->currentText().toInt();
    module_param.can_slave_nodeID_A  = mainwindow->ui->a_nodeid_spinBox->value();
    module_param.can_slave_nodeID_B  = mainwindow->ui->b_nodeid_spinBox->value();
    module_param.can_master_nodeID   = mainwindow->ui->master_nodeid_spinBox->value();
    module_param.can_hb_consumer_gap = mainwindow->ui->master_heartbeat_spinBox->value();
    module_param.can_hb_producer_gap = mainwindow->ui->slave_heartbeat_spinBox->value();
    module_param.can_pdo_time_gap    = mainwindow->ui->pdo_interval_spinBox->value();
    return Safety_Param_Dialog::param_project_info(&module_param);
}

bool config_view::param_project_parse(QJsonObject project)
{
    Safety_Param_Dialog::param_project_parse(project, &module_param);
    mainwindow->ui->cross_check_interval_spinbox->setValue(module_param.check_factor);
    mainwindow->ui->can_baudrate_comboBox->setCurrentText(QString::number(module_param.can_baudrate));
    mainwindow->ui->a_nodeid_spinBox->setValue(module_param.can_slave_nodeID_A);
    mainwindow->ui->b_nodeid_spinBox->setValue(module_param.can_slave_nodeID_B);
    mainwindow->ui->master_nodeid_spinBox->setValue(module_param.can_master_nodeID);
    mainwindow->ui->master_heartbeat_spinBox->setValue(module_param.can_hb_consumer_gap);
    mainwindow->ui->slave_heartbeat_spinBox->setValue(module_param.can_hb_producer_gap);
    mainwindow->ui->pdo_interval_spinBox->setValue(module_param.can_pdo_time_gap);
    return false;
}

module_param_t* config_view::get_module_param()
{
    mbedtls_md5(( const unsigned char* )&module_param, sizeof(module_param_t) - sizeof(module_param.md5),
                module_param.md5);
    return (&module_param);
}

/* user slots */
void config_view::on_a_nodeid_spinbox_valuechanged_slot(int value)
{

    if (value == mainwindow->ui->b_nodeid_spinBox->value() || value == mainwindow->ui->master_nodeid_spinBox->value()) {
        mainwindow->my_message_box("不允许设置相同的节点ID", MESSAGE_TYPE_WARNING);
        mainwindow->ui->a_nodeid_spinBox->setValue(81);
        mainwindow->ui->b_nodeid_spinBox->setValue(82);
        mainwindow->ui->master_nodeid_spinBox->setValue(1);
    }
}

void config_view::on_b_nodeid_spinbox_valuechanged_slot(int value)
{
    if (value == mainwindow->ui->a_nodeid_spinBox->value() || value == mainwindow->ui->master_nodeid_spinBox->value()) {
        mainwindow->my_message_box("不允许设置相同的节点ID", MESSAGE_TYPE_WARNING);
        mainwindow->ui->a_nodeid_spinBox->setValue(81);
        mainwindow->ui->b_nodeid_spinBox->setValue(82);
        mainwindow->ui->master_nodeid_spinBox->setValue(1);
    }
}

void config_view::on_master_nodeid_spinbox_valuechanged_slot(int value)
{
    if (value == mainwindow->ui->a_nodeid_spinBox->value() || value == mainwindow->ui->b_nodeid_spinBox->value()) {
        mainwindow->my_message_box("不允许设置相同的节点ID", MESSAGE_TYPE_WARNING);
        mainwindow->ui->a_nodeid_spinBox->setValue(81);
        mainwindow->ui->b_nodeid_spinBox->setValue(82);
        mainwindow->ui->master_nodeid_spinBox->setValue(1);
    }
}

void config_view::on_master_heartbeat_spinbox_valuechanged_slot(int value)
{
    if (value > 0 && value < 100) {
        ui->master_heartbeat_spinBox->setValue(100);
    }
}

void config_view::on_slave_heartbeat_spinbox_valuechanged_slot(int value)
{
    if (value > 0 && value < 100) {
        ui->slave_heartbeat_spinBox->setValue(100);
    }
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

void config_view::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("ruohui/config")) {
        event->acceptProposedAction();
        QByteArray      byteArray   = event->mimeData()->data("ruohui/config");
        element_data_t* config_data = *reinterpret_cast<element_data_t* const*>(byteArray.constData());
        element_data_t  data        = *config_data;
        config_photo_svg->display_available_source(data);
    }
}

void config_view::dragMoveEvent(QDragMoveEvent* event)
{
    event->accept();
}

void config_view::dragLeaveEvent(QDragLeaveEvent* event)
{
    Q_UNUSED(event);
    foreach (config_param* param, config_photo_svg->all_config_list) {
        param->display_hide();
    }
}

void config_view::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat("ruohui/config")) {
        QByteArray      byteArray   = event->mimeData()->data("ruohui/config");
        element_data_t* config_data = *reinterpret_cast<element_data_t* const*>(byteArray.constData());
        element_data_t  data        = *config_data;
        config_photo_svg->drap_available_source(data, mapToScene(event->pos()));
        foreach (config_param* param, config_photo_svg->all_config_list) {
            param->display_hide();
        }
    }
}
