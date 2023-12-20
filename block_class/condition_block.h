#ifndef CONDITION_BLOCK_H
#define CONDITION_BLOCK_H

#include "QLabel"
#include "config/data_config.h"
#include "config/param_config.h"
#include "connect_block.h"
#include "ui_mainwindow.h"
#include <QGraphicsRectItem>
#include <QJsonObject>
#include <QMenu>
#include <QObject>
#include <QPainterPath>
#include <qgraphicsitem.h>
// 在类内部定义默认的宽度和高度
class MainWindow;
class condition_block : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    condition_block(int x, int y, tool_info_t* tool_info, uint32_t id, QWidget* uiparent,
                    QGraphicsItem* parent = nullptr);
    condition_block(QJsonObject project, QWidget* uiparent, QGraphicsItem* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;
    enum { Type = QGraphicsItem::UserType + BLOCK_TYPE_CONDITION };  // 自定义类型
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
    void block_delete(void);
    void block_info_init(void);
    void right_menu_di(void);
    void right_menu_ai_pi_qep(void);
    void set_display_label(QString text);
    void condition_tool_detect(void);
    void attribute_display(void);
    void resource_config(void);
    bool block_collison_detect(void);

private:
    static const int defaultWidth;
    static const int defaultHeight;
    QPointF          originalPos;

    condition_di_set_t        condition_di_set;
    condition_ai_pi_qep_set_t condition_ai_pi_qep_set;
    QStringList               attribute_name;
    QStringList               attribute_description;
    QString                   error_info;

public:
    QMenu                 menu;
    QAction*              settingsAction = nullptr;
    QAction*              deleteAction;
    void                  right_menu_setting(void);
    block_attribute_t     block_attribute;
    QGraphicsTextItem*    dispaly_label;
    block_error_t         block_error;
    QList<connect_block*> output_point_list;

public:
    void        update_state(void);
    QJsonObject condition_block_project_info(void);
signals:
    void block_delete_signal(condition_block* block);
public slots:
};

#endif  // CONDITION_BLOCK_H
