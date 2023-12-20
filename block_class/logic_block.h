#ifndef LOGIC_BLOCK_H
#define LOGIC_BLOCK_H

#include "QPainter"
#include "config/data_config.h"
#include "config/param_config.h"
#include "connect_block.h"
#include "logic_block.h"
#include "ui_mainwindow.h"
#include <QGraphicsRectItem>
#include <QJsonObject>
#include <QMenu>
#include <QObject>
#include <QPainterPath>
#include <qgraphicsitem.h>

class MainWindow;
class logic_block : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    logic_block(int x, int y, tool_info_t* tool_info, uint32_t id, QWidget* uiparent, QGraphicsItem* parent = nullptr);
    logic_block(QJsonObject project, QWidget* uiparent, QGraphicsItem* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_LOGIC };  // 自定义类型
    int type() const override
    {
        return Type;
    }

protected:
    void     contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void     mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void     mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void     mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void     mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void     keyPressEvent(QKeyEvent* event) override;

private:
    static const int defaultWidth;
    static const int defaultHeight;
    QPointF          originalPos;

    QString error_info;

private:
    void block_delete(void);
    void connect_point_init(int x, int y);
    void right_menu_setting(void);
    void error_detect(void);
    void logic_string_generate(void);
    void attribute_display(void);
    void logic_block_init(void);
    bool block_collison_detect(void);

public:
    QMenu             menu;
    QAction*          settingsAction = nullptr;
    QAction*          deleteAction   = nullptr;
    block_attribute_t block_attribute;
    block_error_t     block_error;
    sf_param_t        sf_param;

    QStringList           attribute_name;
    QStringList           attribute_description;
    QList<connect_block*> input_point_list;
    QList<connect_block*> output_point_list;

public:
    void        update_state(void);
    QJsonObject logic_block_project_info(void);
signals:
    void block_delete_signal(logic_block* block);

public slots:
};

#endif  // LOGIC_BLOCK_H
