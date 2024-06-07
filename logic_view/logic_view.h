#ifndef LOGIC_VIEW_H
#define LOGIC_VIEW_H
#include "config/data_config.h"
#include "config/param_config.h"
#include "graphical_config/graphical_config_param.h"
#include "logic_view/apply_logic_block.h"
#include "logic_view/base_logic_block.h"
#include "logic_view/connection_line.h"
#include "logic_view/delay_counter_logic_block.h"
#include "logic_view/input_block.h"
#include "logic_view/output_block.h"
#include "logic_view/speed_logic_block.h"
#include "ui_mainwindow.h"
#include <QGraphicsView>
#include <QJsonObject>
#include <QMouseEvent>
#include <QTimer>
#include <QWidget>
typedef enum {
    DRAW_LINE_STATE_IDLE = 0,
    DRAW_LINE_STATE_ING,
} draw_line_state_e;
class MainWindow;
class logic_view : public QGraphicsView {
    Q_OBJECT
public:
    explicit logic_view(QWidget* uiparent, QWidget* parent = nullptr);
    ~logic_view();
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

protected:
    QWidget* mparent = nullptr;

    /* draw line connect both block */
    connection_line* probe_line = nullptr;

    /* block */
    drop_tool_info_t drop_tool_info;

    /* 视角缩放 */
    qreal maxscale = VIEW_MAX_SCALE;
    qreal minscale = VIEW_MIN_SCALE;

public:
    QGraphicsScene*                   my_scene;
    draw_line_state_e                 draw_line_state = DRAW_LINE_STATE_IDLE;
    QList<connection_line*>           connection_line_list;
    QList<input_block*>               input_block_list;
    QList<output_block*>              output_block_list;
    QList<base_logic_block*>          base_logic_block_list;
    QList<base_logic_block*>          rs_trigger_logic_block_list;
    QList<apply_logic_block*>         apply_logic_block_list;
    QList<apply_logic_block*>         apply_reset_block_list;
    QList<apply_logic_block*>         apply_extern_device_monitor_list;
    QList<apply_logic_block*>         apply_edge_detected_list;
    QList<apply_logic_block*>         apply_freq_monitor_list;
    QList<delay_counter_logic_block*> delay_counter_block_list;
    QList<delay_counter_logic_block*> delay_on_list;
    QList<delay_counter_logic_block*> delay_off_list;
    QList<delay_counter_logic_block*> delay_adjust_on_list;
    QList<delay_counter_logic_block*> delay_adjust_off_list;
    QList<delay_counter_logic_block*> counter_event_list;
    QList<delay_counter_logic_block*> counter_logging_list;
    QList<speed_logic_block*>         speed_logic_block_list;
    QList<speed_logic_block*>         speed_cross_check_list;
    QList<speed_logic_block*>         speed_monitor_list;
    QList<speed_logic_block*>         speed_motionless_list;
    QList<speed_logic_block*>         speed_value_compairsons_list;

public:
    bool        blocks_error_detect(void);
    void        logic_view_reset(void);
    QJsonObject logic_view_project_info(void);
    bool        logic_view_project_parse(QJsonObject project);

protected:
    void init_ui(void);
    /* draw line connect both block */
    void draw_line_both_block(connect_point* block);
    void draw_line_delete(connection_line* line);

    /* creat block */
    void creat_logic_block(config_block_data_t* data, QPointF pos);
    int  get_idle_block_uid(void);
    void set_block_focus(QPointF pos);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
public slots:
private slots:
};

#endif  // LOGIC_VIEW_H
