#ifndef LOGIC_VIEW_H
#define LOGIC_VIEW_H
#include "block_class/condition_block.h"
#include "block_class/connect_line.h"
#include "block_class/logic_block.h"
#include "config/data_config.h"
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
    connect_line* probe_line = nullptr;

    /* block */
    uint32_t         block_id = 1;
    drop_tool_info_t drop_tool_info;

public:
    sf_used_info_t          sf_used_inf;
    uint32_t                attribute_display_id = 0;
    QGraphicsScene*         my_scene;
    draw_line_state_e       draw_line_state = DRAW_LINE_STATE_IDLE;
    QList<logic_block*>     logic_block_list;
    QList<condition_block*> condition_block_list;
    QList<connect_line*>    line_list;

public:
    bool        blocks_error_detect(void);
    void        logic_view_reset(void);
    QJsonObject logic_view_project_info(void);
    bool        logic_view_project_parse(QJsonObject project);

protected:
    void init_ui(void);
    /* draw line connect both block */
    void draw_line_both_block(connect_block* block);
    void draw_line_delete(connect_line* line);

    /* creat block */
    void creat_logic_block(tool_info_t* tool_info, QPointF pos);

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
