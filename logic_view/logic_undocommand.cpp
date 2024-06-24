#include "logic_undocommand.h"
#include "logic_view/apply_logic_block.h"
#include "logic_view/base_logic_block.h"
#include "logic_view/connection_line.h"
#include "logic_view/delay_counter_logic_block.h"
#include "logic_view/input_block.h"
#include "logic_view/output_block.h"
#include "logic_view/speed_logic_block.h"
#include <QDebug>
/* 新建指令 */
addcommand::addcommand(logic_view* view, config_block_data_t* cdata, QPointF apos, QWidget* uiparent,
                       QUndoCommand* parent)
{
    Q_UNUSED(parent);
    logicview = view;
    data      = *cdata;
    pos       = apos;
    mparent   = uiparent;
}

void addcommand::redo()
{
    qDebug() << "add-redo";
    int x = qRound(pos.x() / 10) * 10;
    int y = qRound(pos.y() / 10) * 10;
    pos.setX(x);
    pos.setY(y);
    uid = logicview->get_idle_block_uid();
    switch (data.config_param_data.model_iotype) {
    case MODEL_TYPE_LOGIC:
        switch (data.config_param_data.model_type) {
        case MODEL_LOGIC_BASE: {
            base_logic_block* base_logic = new base_logic_block(pos, data, uid, mparent);
            logicview->scene()->addItem(base_logic);
        } break;
        case MODEL_LOGIC_APPLICATION: {
            apply_logic_block* logic = new apply_logic_block(pos, data, uid, mparent);
            logicview->scene()->addItem(logic);
        } break;
        case MODEL_LOGIC_DELAY_COUNTER: {
            delay_counter_logic_block* logic = new delay_counter_logic_block(pos, data, uid, mparent);
            logicview->scene()->addItem(logic);
        } break;
        case MODEL_LOGIC_SPEED: {
            speed_logic_block* logic = new speed_logic_block(pos, data, uid, mparent);
            logicview->scene()->addItem(logic);
        } break;
        default:
            break;
        }
        break;
    case MODEL_TYPE_INPUT: {
        input_block* input = new input_block(pos, data, uid, mparent);
        logicview->scene()->addItem(input);
    } break;
    case MODEL_TYPE_OUTPUT: {
        output_block* output = new output_block(pos, data, uid, mparent);
        logicview->scene()->addItem(output);
    } break;
    default:
        break;
    }
    qDebug() << "add-redo end";
}

void addcommand::undo()
{
    qDebug() << "add-undo";
    QList<QGraphicsItem*> allBlocks = logicview->scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        if (item->type() >= QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK) {
            base_rect_class* base = dynamic_cast<base_rect_class*>(item);
            if (base) {
                if (base->get_attribute()->uid == uid) {
                    base->action_delete_callback();
                }
            }
        }
    }
    qDebug() << "add-undo  end";
}

/* 新建连线指令 */
linecommand::linecommand(logic_view* view, connection_line* connec_line, QWidget* uiparent, QUndoCommand* parent)
{
    Q_UNUSED(parent);
    logicview                   = view;
    mparent                     = uiparent;
    line_info                   = connec_line->connect_line_project_info();
    line_data.input_block_id    = connec_line->get_start_point()->self_attribute.uid;
    line_data.input_point_id    = connec_line->get_start_point()->connect_point_id;
    line_data.input_point_type  = connec_line->get_start_point()->get_io_type();
    line_data.output_block_id   = connec_line->get_end_point()->self_attribute.uid;
    line_data.output_point_id   = connec_line->get_end_point()->connect_point_id;
    line_data.output_point_type = connec_line->get_end_point()->get_io_type();
}

void linecommand::redo()
{
    qDebug() << "line-redo";
    if (allow_redo) {
        connection_line* line    = new connection_line(mparent);
        bool             success = false;
        logicview->scene()->addItem(line);
        success = line->connect_line_project_parse(line_info);
        if (success) {
            logicview->connection_line_list.append(line);
        } else {
            logicview->scene()->removeItem(line);
        }
    }
    qDebug() << "line-redo end";
}

void linecommand::undo()
{
    qDebug() << "line-undo";
    foreach (connection_line* line, logicview->connection_line_list) {
        if (line) {
            line_copy_data_t data;
            data.input_block_id    = line->get_start_point()->self_attribute.uid;
            data.input_point_id    = line->get_start_point()->connect_point_id;
            data.input_point_type  = line->get_start_point()->get_io_type();
            data.output_block_id   = line->get_end_point()->self_attribute.uid;
            data.output_point_id   = line->get_end_point()->connect_point_id;
            data.output_point_type = line->get_end_point()->get_io_type();
            if (memcmp(( uint8_t* )&data, ( uint8_t* )&line_data, sizeof(line_copy_data_t)) == 0) {
                line->line_delete();
            }
        }
    }
    allow_redo = true;
    qDebug() << "line-undo end";
}

/* 删除块 */
deletecommand::deletecommand(logic_view* view, QList<QGraphicsItem*> blocks, QList<QGraphicsItem*> lines,
                             QWidget* uiparent, QUndoCommand* parent)
{
    Q_UNUSED(parent);
    logicview = view;
    mparent   = uiparent;
    foreach (QGraphicsItem* item, blocks) {
        base_rect_class* base = dynamic_cast<base_rect_class*>(item);
        if (base) {
            id_list.append(base->get_attribute()->uid);
            block_info.append(base->block_project_info());
            type_list.append(base->type());
        }
    }
    QList<QGraphicsItem*> selected_lines;
    foreach (connection_line* line, logicview->connection_line_list) {
        if (line) {
            if (blocks.contains(line->get_start_point()->parentItem())
                || blocks.contains(line->get_end_point()->parentItem())) {
                if (!selected_lines.contains(line)) {
                    selected_lines.append(line);
                    line_info.append(line->connect_line_project_info());
                    if (lines.contains(line)) {
                        lines.removeOne(line);
                    }
                }
            }
        }
    }
    foreach (QGraphicsItem* item, lines) {
        connection_line* line = dynamic_cast<connection_line*>(item);
        if (line) {
            selected_lines.append(line);
            line_info.append(line->connect_line_project_info());
        }
    }
}

void deletecommand::redo()
{
    qDebug() << "delete-redo";
    QList<QGraphicsItem*> selected_blocks;
    /* 预先保存信息 */
    QList<QGraphicsItem*> allBlocks = logicview->scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        base_rect_class* base = dynamic_cast<base_rect_class*>(item);
        if (base) {
            if (id_list.contains(base->get_attribute()->uid)) {
                selected_blocks.append(item);
            }
        }
    }
    /* 删除块 */
    foreach (QGraphicsItem* item, selected_blocks) {
        base_rect_class* base = dynamic_cast<base_rect_class*>(item);
        if (base) {
            base->action_delete_callback();
        }
    }
    /* 删除线 */
    foreach (connection_line* line, logicview->connection_line_list) {
        if (line) {
            if (line_info.contains(line->connect_line_project_info())) {
                line->line_delete();
            }
        }
    }
    qDebug() << "delete-redo end";
}

void deletecommand::undo()
{
    qDebug() << "delete-undo";
    /* 生成块 */
    for (int i = 0; i < block_info.size(); i++) {
        switch (type_list[i]) {
        case QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK: {
            input_block* input = new input_block(block_info[i], mparent);
            logicview->scene()->addItem(input);
        } break;
        case QGraphicsItem::UserType + BLOCK_TYPE_OUTPUTBLOCK: {
            output_block* output = new output_block(block_info[i], mparent);
            logicview->scene()->addItem(output);
        } break;
        case QGraphicsItem::UserType + BLOCK_TYPE_BASELOGIC: {
            base_logic_block* base_logic = new base_logic_block(block_info[i], mparent);
            logicview->scene()->addItem(base_logic);
        } break;
        case QGraphicsItem::UserType + BLOCK_TYPE_APPLYLOGIC: {
            apply_logic_block* logic = new apply_logic_block(block_info[i], mparent);
            logicview->scene()->addItem(logic);
        } break;
        case QGraphicsItem::UserType + BLOCK_TYPE_DELAY_COUNTER: {
            delay_counter_logic_block* logic = new delay_counter_logic_block(block_info[i], mparent);
            logicview->scene()->addItem(logic);
        } break;
        case QGraphicsItem::UserType + BLOCK_TYPE_SPEED: {
            speed_logic_block* logic = new speed_logic_block(block_info[i], mparent);
            logicview->scene()->addItem(logic);
        } break;
        default:
            break;
        }
    }
    /* 生成线 */
    foreach (QJsonObject info, line_info) {
        connection_line* line    = new connection_line(mparent);
        bool             success = false;
        logicview->scene()->addItem(line);
        success = line->connect_line_project_parse(info);
        if (success) {
            logicview->connection_line_list.append(line);
        } else {
            logicview->scene()->removeItem(line);
        }
    }
    qDebug() << "delete-undo end";
}

/* 移动块 */
movecommand::movecommand(logic_view* view, QList<QGraphicsItem*> blocks, QUndoCommand* parent)
{
    Q_UNUSED(parent);
    logicview = view;
    qDebug() << blocks.size() << "move";
    foreach (QGraphicsItem* item, blocks) {
        base_rect_class* base = dynamic_cast<base_rect_class*>(item);
        if (base) {
            id_list.append(base->get_attribute()->uid);
            old_pos_list.append(base->get_old_pos());
            new_pos_list.append(base->get_new_pos());
        }
    }
}

void movecommand::redo()
{
    qDebug() << "move-redo";
    QList<QGraphicsItem*> selected_blocks;
    /* 预先保存信息 */
    QList<QGraphicsItem*> allBlocks = logicview->scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        base_rect_class* base = dynamic_cast<base_rect_class*>(item);
        if (base) {
            if (id_list.contains(base->get_attribute()->uid)) {
                selected_blocks.append(item);
            }
        }
    }
    qDebug() << selected_blocks.size() << "moving";
    for (int i = 0; i < selected_blocks.size(); i++) {
        base_rect_class* base = dynamic_cast<base_rect_class*>(selected_blocks[i]);
        if (base) {
            base->set_current_pos(new_pos_list[i]);
        }
    }
    logicview->lines_path_update();
    qDebug() << "move-redo end";
}

void movecommand::undo()
{
    qDebug() << "move-undo";
    QList<QGraphicsItem*> selected_blocks;
    /* 预先保存信息 */
    QList<QGraphicsItem*> allBlocks = logicview->scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        base_rect_class* base = dynamic_cast<base_rect_class*>(item);
        if (base) {
            if (id_list.contains(base->get_attribute()->uid)) {
                selected_blocks.append(item);
            }
        }
    }
    for (int i = 0; i < selected_blocks.size(); i++) {
        base_rect_class* base = dynamic_cast<base_rect_class*>(selected_blocks[i]);
        if (base) {
            base->set_current_pos(old_pos_list[i]);
        }
    }
    logicview->lines_path_update();
    qDebug() << "move-undo end";
}

/* 粘贴块 */
pastecommand::pastecommand(logic_view* view, QPointF pos, QList<block_copy_data_t> blocks_data,
                           QList<line_copy_data_t> lines_data, QWidget* uiparent, QUndoCommand* parent)
{
    Q_UNUSED(parent);
    logicview       = view;
    mparent         = uiparent;
    block_copy_data = blocks_data;
    line_copy_data  = lines_data;
    origin_pos      = pos;
}

void pastecommand::redo()
{
    qDebug() << "paste-redo";
    QList<base_rect_class*> copy_block;
    foreach (block_copy_data_t data, block_copy_data) {
        QPointF point = origin_pos + data.offset_pos;
        int     uid   = logicview->get_idle_block_uid();
        id_list.append(uid);
        switch (data.type) {
        case QGraphicsItem::UserType + BLOCK_TYPE_INPUTBLOCK: {
            input_block* input = new input_block(point, uid, data.block_info, mparent);
            logicview->scene()->addItem(input);
            copy_block.append(input);
        } break;
        case QGraphicsItem::UserType + BLOCK_TYPE_OUTPUTBLOCK: {
            output_block* output = new output_block(point, uid, data.block_info, mparent);
            logicview->scene()->addItem(output);
            copy_block.append(output);
        } break;
        case QGraphicsItem::UserType + BLOCK_TYPE_BASELOGIC: {
            base_logic_block* base_logic = new base_logic_block(point, uid, data.block_info, mparent);
            logicview->scene()->addItem(base_logic);
            copy_block.append(base_logic);
        } break;
        case QGraphicsItem::UserType + BLOCK_TYPE_APPLYLOGIC: {
            apply_logic_block* logic = new apply_logic_block(point, uid, data.block_info, mparent);
            logicview->scene()->addItem(logic);
            copy_block.append(logic);
        } break;
        case QGraphicsItem::UserType + BLOCK_TYPE_DELAY_COUNTER: {
            delay_counter_logic_block* logic = new delay_counter_logic_block(point, uid, data.block_info, mparent);
            logicview->scene()->addItem(logic);
            copy_block.append(logic);
        } break;
        case QGraphicsItem::UserType + BLOCK_TYPE_SPEED: {
            speed_logic_block* logic = new speed_logic_block(point, uid, data.block_info, mparent);
            logicview->scene()->addItem(logic);
            copy_block.append(logic);
        } break;
        default:
            break;
        }
    }
    foreach (line_copy_data_t data, line_copy_data) {
        connect_point* s_block = nullptr;
        connect_point* e_block = nullptr;
        if (data.input_block_id >= copy_block.size() || data.output_block_id >= copy_block.size()) {
            continue;
        }
        if ((data.input_point_id >= MAX_CONNECT_POINT_NUM || data.input_point_id < MIN_CONNECT_POINT_NUM)
            || (data.output_point_id >= MAX_CONNECT_POINT_NUM || data.output_point_id < MIN_CONNECT_POINT_NUM)) {
            continue;
        }
        if (data.input_point_type == CONNECT_POINT_IOTYPE_INPUT) {
            s_block = copy_block[data.input_block_id]->input_point_list[data.input_point_id];
        } else {
            s_block = copy_block[data.input_block_id]->output_point_list[data.input_point_id];
        }
        if (data.output_point_type == CONNECT_POINT_IOTYPE_INPUT) {
            e_block = copy_block[data.output_block_id]->input_point_list[data.output_point_id];
        } else {
            e_block = copy_block[data.output_block_id]->output_point_list[data.output_point_id];
        }
        if (s_block->get_io_type() != e_block->get_io_type()) {
            connection_line* line = new connection_line(mparent);
            logicview->scene()->addItem(line);
            line->set_start_point_block(s_block);
            line->set_end_point_block(e_block);
            logicview->connection_line_list.append(line);
        }
    }
    logicview->lines_path_update();
    qDebug() << "paste-redo end";
}

void pastecommand::undo()
{
    qDebug() << "paste-undo";
    QList<QGraphicsItem*> allBlocks = logicview->scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        base_rect_class* base = dynamic_cast<base_rect_class*>(item);
        if (base) {
            if (id_list.contains(base->get_attribute()->uid)) {
                base->action_delete_callback();
            }
        }
    }
    logicview->lines_path_update();
    qDebug() << "paste-undo end";
}

/* 参数变更 */
paramchangecommand::paramchangecommand(logic_view* view, QGraphicsItem* item, QUndoCommand* parent)
{
    logicview             = view;
    base_rect_class* base = dynamic_cast<base_rect_class*>(item);
    if (base) {
        block_uid = base->get_attribute()->uid;
        old_param = base->get_block_old_param();
        new_param = base->block_param_info();
    }
}

void paramchangecommand::redo()
{
    qDebug() << "paramchange-redo";
    if (allow_redo) {
        QList<QGraphicsItem*> allBlocks = logicview->scene()->items();
        foreach (QGraphicsItem* item, allBlocks) {
            base_rect_class* base = dynamic_cast<base_rect_class*>(item);
            if (base) {
                if (block_uid == base->get_attribute()->uid) {
                    base->block_param_prase(new_param);
                }
            }
        }
    }
    qDebug() << "paramchange-redo end";
}

void paramchangecommand::undo()
{
    qDebug() << "paramchange-undo";
    QList<QGraphicsItem*> allBlocks = logicview->scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        base_rect_class* base = dynamic_cast<base_rect_class*>(item);
        if (base) {
            if (block_uid == base->get_attribute()->uid) {
                base->block_param_prase(old_param);
            }
        }
    }
    allow_redo = true;
    qDebug() << "paramchange-undo end";
}
