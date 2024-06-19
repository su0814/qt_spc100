#ifndef LOGIC_UNDOCOMMAND_H
#define LOGIC_UNDOCOMMAND_H

#include "logic_view.h"
#include <QUndoCommand>
/* 新建单个块 */
class addcommand : public QUndoCommand {
public:
    addcommand(logic_view* view, config_block_data_t* cdata, QPointF apos, QWidget* uiparent, QUndoCommand* parent = 0);
    void redo() override;  //重写这两个函数
    void undo() override;

private:
    logic_view*         logicview;
    config_block_data_t data;
    QPointF             pos;
    QWidget*            mparent = nullptr;
    int                 uid;
};

/* 新建连线 */
class linecommand : public QUndoCommand {
public:
    linecommand(logic_view* view, connection_line* connec_line, QWidget* uiparent, QUndoCommand* parent = 0);
    void redo() override;  //重写这两个函数
    void undo() override;

private:
    bool             allow_redo = false;
    logic_view*      logicview;
    line_copy_data_t line_data;
    QJsonObject      line_info;
    QWidget*         mparent = nullptr;
};

/* 删除块 */
class deletecommand : public QUndoCommand {
public:
    deletecommand(logic_view* view, QList<QGraphicsItem*> blocks, QList<QGraphicsItem*> lines, QWidget* uiparent,
                  QUndoCommand* parent = 0);
    void redo() override;  //重写这两个函数
    void undo() override;

private:
    logic_view*        logicview;
    QWidget*           mparent = nullptr;
    QList<QJsonObject> block_info;
    QList<QJsonObject> line_info;
    QList<int>         type_list;
    QList<int>         id_list;
};

/* 移动块 */
class movecommand : public QUndoCommand {
public:
    movecommand(logic_view* view, QList<QGraphicsItem*> blocks, QUndoCommand* parent = 0);
    void redo() override;  //重写这两个函数
    void undo() override;

private:
    logic_view*    logicview;
    QList<QPointF> old_pos_list;
    QList<QPointF> new_pos_list;
    QList<int>     id_list;
};

/* 粘贴块 */
class pastecommand : public QUndoCommand {
public:
    pastecommand(logic_view* view, QPointF pos, QList<block_copy_data_t> blocks_data,
                 QList<line_copy_data_t> lines_data, QWidget* uiparent, QUndoCommand* parent = 0);
    void redo() override;  //重写这两个函数
    void undo() override;

private:
    logic_view*              logicview;
    QList<block_copy_data_t> block_copy_data;
    QList<line_copy_data_t>  line_copy_data;
    QPointF                  origin_pos;
    QWidget*                 mparent = nullptr;
    QList<int>               id_list;
};
/* 参数变更 */
class paramchangecommand : public QUndoCommand {
public:
    paramchangecommand(logic_view* view, QGraphicsItem* item, QUndoCommand* parent = 0);
    void redo() override;  //重写这两个函数
    void undo() override;

private:
    logic_view* logicview;
    int         block_uid = 0;
    QJsonObject old_param;
    QJsonObject new_param;
    bool        allow_redo = false;
};
#endif  // LOGIC_UNDOCOMMAND_H
