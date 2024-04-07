#include "condition_block.h"
#include "list_condi/condition_view.h"
#include "mainwindow.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QFormLayout>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <qgraphicsscene.h>
const int condition_block::defaultWidth  = CONDITION_BLOCK_WIDTH;
const int condition_block::defaultHeight = CONDITION_BLOCK_HEIGHT;

/**
 * @brief 条件块的构造函数，通过拖动块生成的构造
 * @param x 坐标x
 * @param y 坐标y
 * @param tool_info 条件工具信息
 * @param id 块ID
 * @param uiparent mainwindow，用于调用主程序元素
 * @param parent 继承自QGraphicsRectItem，与其parent参数一致
 */
condition_block::condition_block(int x, int y, tool_info_t* tool_info, uint32_t id, QWidget* uiparent,
                                 QGraphicsItem* parent)
    : QGraphicsRectItem(0, 0, defaultWidth, defaultHeight, parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )uiparent;
    setPos(x - defaultWidth / 2, y - defaultHeight / 2);          //设置块坐标
    block_attribute.self_id = id;                                 //设置自身ID
    block_attribute.parent_id.clear();                            //清空父亲列表
    block_attribute.block_info.tool_type = tool_info->tool_type;  //条件工具快的类型
    block_attribute.block_info.tool_id   = tool_info->tool_id;    //条件工具快的类型ID
    block_attribute.other_name           = mainwindow->condition_view_class->condition_get_name(
        block_attribute.block_info.tool_type, block_attribute.block_info.tool_id);  //块的别名
    condition_ai_pi_qep_set.value  = 1000;  // AI/PI/QEP块设置初始值
    condition_ai_pi_qep_set.dir    = 0;
    connect_block* condition_point = new connect_block(defaultWidth, defaultHeight / 4, CONNECT_POINT_TYPE_OUTPUT, 0,
                                                       &block_attribute, this);  //设置连接点
    output_point_list.append(condition_point);                                   //输出连接点列表
    block_info_init();                                                           //信息初始化
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);  //几何属性发生修改时发送信号(位置信息)
    setFlag(QGraphicsItem::ItemIsMovable);             //设置块可移动
    setFlag(QGraphicsItem::ItemIsFocusable);           //设置可获取焦点以接收键盘操作
    setFocus();                                        //设置可获取焦点
}

/**
 * @brief 条件块构造函数，通过工程重建的构造
 * @param project 工程文件信息
 * @param uiparent mainwindow，用于调用主程序元素
 * @param parent 继承自QGraphicsRectItem，与其parent参数一致
 */
condition_block::condition_block(QJsonObject project, QWidget* uiparent, QGraphicsItem* parent)
    : QGraphicsRectItem(parent)
{
    ui                      = MainWindow::my_ui->ui;
    mainwindow              = ( MainWindow* )uiparent;
    int x                   = project["x"].toInt();
    int y                   = project["y"].toInt();
    block_attribute.self_id = project["self_id"].toInt();
    block_attribute.parent_id.clear();
    block_attribute.block_info.tool_type    = ( tool_type_e )project["tooltype"].toInt();
    block_attribute.block_info.tool_id      = ( tool_id_e )project["toolid"].toInt();
    block_attribute.other_name              = project["othername"].toString();
    condition_di_set.is_reverse             = ( bool )project["is_reverse"].toInt();
    condition_ai_pi_qep_set.calc_type_index = project["calc_type"].toInt();
    condition_ai_pi_qep_set.value           = project["calc_value"].toInt();
    condition_ai_pi_qep_set.dir             = project["qep_dir"].toInt();
    QRect rect(0, 0, defaultWidth, defaultHeight);
    setRect(rect);
    setPos(x, y);
    block_info_init();
    connect_block* condition_point =
        new connect_block(defaultWidth, defaultHeight / 4, CONNECT_POINT_TYPE_OUTPUT, 0, &block_attribute, this);
    output_point_list.append(condition_point);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

/**
 * @brief 条件块信息初始化
 */
void condition_block::block_info_init()
{
    set_pen_state(BLOCK_STATE_IDE);
    set_brush_state(BLOCK_STATE_IDE);
    dispaly_label = new QGraphicsTextItem(block_attribute.other_name, this);  // 设置字体
    set_display_label(block_attribute.other_name);
    param_label = new QGraphicsTextItem(this);
    QList<QStringList> func_list;  // lua函数列表
    func_list.append(lua_di_func);
    func_list.append(lua_ai_func);
    func_list.append(lua_pi_func);
    func_list.append(lua_qep_func);
    switch (block_attribute.block_info.tool_type) {
    case TOOL_TYPE_CONDI_DI:
        if (condition_di_set.is_reverse) {  // DI根据“反向”参数决定代码内容
            block_attribute.logic_string =
                "(" + func_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id]
                + " == false)";
        } else {
            block_attribute.logic_string =
                "(" + func_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id] + " == true)";
        }
        if (condition_di_set.is_reverse) {  //块头部参数显示
            param_label->setPlainText("=0");
        } else {
            param_label->setPlainText("=1");
        }
        break;
    case TOOL_TYPE_CONDI_AI:
        block_attribute.logic_string =
            "(" + func_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id]
            + calc_str[condition_ai_pi_qep_set.calc_type_index] + QString::number(condition_ai_pi_qep_set.value) + ")";
        param_label->setPlainText(calc_str[condition_ai_pi_qep_set.calc_type_index]
                                  + QString::number(condition_ai_pi_qep_set.value));
        break;
    case TOOL_TYPE_CONDI_PI:
        block_attribute.logic_string =
            "(" + func_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id]
            + calc_str[condition_ai_pi_qep_set.calc_type_index] + QString::number(condition_ai_pi_qep_set.value) + ")";
        param_label->setPlainText(calc_str[condition_ai_pi_qep_set.calc_type_index]
                                  + QString::number(condition_ai_pi_qep_set.value));
        break;
    case TOOL_TYPE_CONDI_QEP:
        block_attribute.logic_string =
            "((" + lua_qep_dir_func[block_attribute.block_info.tool_id]
            + " == " + QString::number(condition_ai_pi_qep_set.dir) + ") and ("
            + func_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id]
            + calc_str[condition_ai_pi_qep_set.calc_type_index] + QString::number(condition_ai_pi_qep_set.value) + ")";
        param_label->setPlainText("dir:" + QString::number(condition_ai_pi_qep_set.dir) + ", "
                                  + calc_str[condition_ai_pi_qep_set.calc_type_index] + " "
                                  + QString::number(condition_ai_pi_qep_set.value));
        break;
    case TOOL_TYPE_CONDI_BOOL: {
        QStringList bool_str         = { "(true)", "(false)" };
        block_attribute.logic_string = bool_str[block_attribute.block_info.tool_id];
        right_menu_show              = false;
    } break;
    default:
        break;
    }
    //条件的函数名
    block_attribute.func_string = block_attribute.other_name + QString::number(block_attribute.self_id) + "_func()";
    this->setToolTip(block_attribute.other_name);  //设置工具提示
    // param_label->setFont(QFont("Arial", 4));
    param_label->setPos(this->boundingRect().center().x() - param_label->boundingRect().center().x(),
                        this->boundingRect().center().y() - param_label->boundingRect().center().y() - 20);
    if (right_menu_show) {  //条件块是否拥有右键菜单
        settingsAction = new QAction("设置", this);
        deleteAction   = new QAction("删除", this);
        deleteAction->setIcon(QIcon(":/new/photo/photo/delete_block.png"));
        settingsAction->setIcon(QIcon(":/new/photo/photo/setting_block.png"));
        menu.addAction(settingsAction);
        menu.addAction(deleteAction);
    }
    connect(&update_timer, &QTimer::timeout, this, update_state_slot);  //状态更新定时器及槽函数
    update_timer.start(BLOCK_DATA_REFRESH_TIME);
}

void condition_block::set_focus(bool state)
{
    focus_state = state;
    QPen pen;
    if (mainwindow->project_debug_class->get_debug_state() == DEBUG_STATE_IDLE) {
        if (focus_state) {
            set_pen_state(BLOCK_STATE_FOCUS);
        } else {
            set_pen_state(BLOCK_STATE_IDE);
        }
        foreach (connect_block* item, output_point_list) {
            item->send_focus_state_signal(focus_state);
        }
    } else {
        set_pen_state(BLOCK_STATE_IDE);
    }
}

void condition_block::set_pen_state(block_state_e state)
{
    QPen pen;
    switch (state) {
    case BLOCK_STATE_IDE:
        pen.setColor(QColor(128, 128, 128));
        pen.setWidth(1);
        this->setPen(pen);
        break;
    case BLOCK_STATE_DEBUG:
        pen.setColor(QColor(0, 200, 0));
        pen.setWidth(2);
        setPen(pen);
        break;
    case BLOCK_STATE_FOCUS:
        pen.setColor(QColor(0, 0, 255));
        pen.setWidth(2);
        this->setPen(pen);
        break;
    default:
        break;
    }
}

void condition_block::set_brush_state(block_state_e state)
{
    switch (state) {
    case BLOCK_STATE_IDE: {
        QBrush brush(QColor(173, 216, 230));
        this->setBrush(brush);
    } break;
    case BLOCK_STATE_ERROR: {
        QBrush brush(QColor(255, 100, 10));
        setBrush(brush);
    } break;
    default:
        break;
    }
}

/**
 * @brief 生成并返回条件块的工程信息
 * @return json格式的工程信息
 */
QJsonObject condition_block::condition_block_project_info()
{
    QJsonObject rootObject;
    rootObject["x"]          = ( int )this->pos().x();
    rootObject["y"]          = ( int )this->pos().y();
    rootObject["is_reverse"] = (uint8_t)(condition_di_set.is_reverse);
    rootObject["calc_type"]  = (condition_ai_pi_qep_set.calc_type_index);
    rootObject["calc_value"] = static_cast<int>(condition_ai_pi_qep_set.value);
    rootObject["qep_dir"]    = condition_ai_pi_qep_set.dir;
    rootObject["self_id"]    = static_cast<int>(block_attribute.self_id);
    rootObject["othername"]  = (block_attribute.other_name);
    rootObject["tooltype"]   = (block_attribute.block_info.tool_type);
    rootObject["toolid"]     = (block_attribute.block_info.tool_id);
    return rootObject;
}

/**
 * @brief 右键菜单事件
 * @param event
 */
void condition_block::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QAction* selectedItem = menu.exec(event->screenPos());
    if (selectedItem == settingsAction) {
        right_menu_setting();
    } else if (selectedItem == deleteAction) {
        if (scene()) {
            block_delete();
        }
    }
}

/**
 * @brief 条件块自删除函数
 */
void condition_block::block_delete()
{
    if (mainwindow->logic_view_class->draw_line_state != DRAW_LINE_STATE_IDLE) {
        return;  //连线状态下不可删除
    }
    update_timer.stop();
    uint8_t resource_start_num[4] = { INPUT_DI_RESOURCE_START, INPUT_AI_RESOURCE_START, INPUT_PI_RESOURCE_START,
                                      INPUT_QEP_RESOURCE_START };
    /* 解除资源使用状态，从块列表和视图中删除自己 */
    mainwindow->project_report_class->input_resource_info
        .is_used[resource_start_num[block_attribute.block_info.tool_type] + block_attribute.block_info.tool_id] = false;
    if (mainwindow->logic_view_class->condition_block_list.contains(this)) {
        mainwindow->logic_view_class->condition_block_list.removeOne(this);
    }
    scene()->removeItem(this);
    delete this;
}

/**
 * @brief 设置块内要显示的标签内容
 * @param text 要显示的字符
 */
void condition_block::set_display_label(QString text)
{
    dispaly_label->setPlainText(text);
    dispaly_label->setPos(this->boundingRect().center() - dispaly_label->boundingRect().center());
}

/**
 * @brief DI条件块的右键菜单
 */
void condition_block::right_menu_di()
{
    QDialog     dialog;
    QFormLayout layout(&dialog);
    dialog.setWindowTitle("设置");
    dialog.setStyleSheet("QDialog { background-color: rgb(210,230,255); }");
    dialog.setWindowFlags(Qt::Tool);
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    layout.setContentsMargins(10, 10, 10, 10);
    QCheckBox is_reverse;
    is_reverse.setText("输出反向");
    is_reverse.setChecked(condition_di_set.is_reverse);
    layout.addRow(&is_reverse);
    QPushButton okButton("确定");
    layout.addRow(&okButton);
    QObject::connect(&okButton, &QPushButton::clicked, [&]() {
        if (is_reverse.isChecked()) {
            block_attribute.logic_string = "(" + lua_di_func[block_attribute.block_info.tool_id] + "==false )";
            param_label->setPlainText("=0");
        } else {
            block_attribute.logic_string = "(" + lua_di_func[block_attribute.block_info.tool_id] + "==true )";
            param_label->setPlainText("=1");
        }
        condition_di_set.is_reverse = is_reverse.isChecked();
        dialog.close();
    });
    dialog.exec();
}

/**
 * @brief AI/PI/QEP类型的条件块的右键菜单
 */
void condition_block::right_menu_ai_pi_qep()
{
    QList<QStringList> func_list;
    func_list.append(lua_di_func);
    func_list.append(lua_ai_func);
    func_list.append(lua_pi_func);
    func_list.append(lua_qep_func);
    QDialog     dialog;
    QFormLayout layout(&dialog);
    dialog.setWindowTitle("设置");
    dialog.setStyleSheet("QDialog { background-color: rgb(210,230,255); }");
    dialog.setWindowFlags(Qt::Tool);
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    layout.setContentsMargins(10, 10, 10, 10);
    QComboBox calc_type;
    calc_type.addItem(">");
    calc_type.addItem("<");
    calc_type.addItem(">=");
    calc_type.addItem("<=");
    calc_type.addItem("==");
    QSpinBox     value;
    QLabel       unit;
    QHBoxLayout* hboxLayout = new QHBoxLayout;
    QComboBox    dir;
    dir.addItem("0");
    dir.addItem("1");
    dir.setCurrentIndex(condition_ai_pi_qep_set.dir);
    calc_type.setCurrentIndex(condition_ai_pi_qep_set.calc_type_index);

    hboxLayout->addWidget(&value);
    hboxLayout->addWidget(&unit);
    layout.addRow(&calc_type);
    layout.addRow(hboxLayout);
    switch (block_attribute.block_info.tool_type) {
    case TOOL_TYPE_CONDI_AI:
        value.setMaximum(30000);
        unit.setText("mV");
        break;
    case TOOL_TYPE_CONDI_PI:
        value.setMaximum(200000);
        unit.setText("pulse/s");
        break;
    case TOOL_TYPE_CONDI_QEP:
        value.setMaximum(200000);
        unit.setText("pulse/s");
        layout.addRow("计数方向", &dir);
        break;
    default:
        break;
    }
    value.setValue(condition_ai_pi_qep_set.value);
    QPushButton okButton("确定");
    layout.addRow(&okButton);
    QObject::connect(&okButton, &QPushButton::clicked, [&]() {
        condition_ai_pi_qep_set.calc_type_index = calc_type.currentIndex();
        condition_ai_pi_qep_set.value           = value.value();
        switch (block_attribute.block_info.tool_type) {
        case TOOL_TYPE_CONDI_AI:
        case TOOL_TYPE_CONDI_PI:
            block_attribute.logic_string =
                "(" + func_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id]
                + calc_type.currentText() + " " + QString::number(value.value()) + " )";
            param_label->setPlainText(calc_type.currentText() + " " + QString::number(value.value()));
            break;
        case TOOL_TYPE_CONDI_QEP:
            condition_ai_pi_qep_set.dir = dir.currentIndex();
            block_attribute.logic_string =
                "((" + lua_qep_dir_func[block_attribute.block_info.tool_id]
                + " == " + QString::number(condition_ai_pi_qep_set.dir) + ") and ("
                + func_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id]
                + calc_type.currentText() + " " + QString::number(value.value()) + " ))";
            param_label->setPlainText("dir:" + QString::number(condition_ai_pi_qep_set.dir) + ", "
                                      + calc_str[condition_ai_pi_qep_set.calc_type_index] + " "
                                      + QString::number(condition_ai_pi_qep_set.value));
            break;
        default:
            break;
        }

        dialog.close();
    });
    dialog.exec();
}

/**
 * @brief 右键菜单设置，根据块的类型显示不同菜单
 */
void condition_block::right_menu_setting()
{

    switch (block_attribute.block_info.tool_type) {
    case TOOL_TYPE_CONDI_DI:
        right_menu_di();
        break;
    default:
        right_menu_ai_pi_qep();
        break;
    }
}

/**
 * @brief 检查输入资源的条件工具是否还存在以及块的错误状态检测
 */
void condition_block::condition_tool_detect()
{
    error_info.clear();
    /* bool类型的条件不需要检测是否存在 */
    if (block_attribute.block_info.tool_type < TOOL_TYPE_CONDI_BOOL) {
        if (ui->treeWidget_condi->topLevelItem(( int )block_attribute.block_info.tool_type)
                ->child(( int )block_attribute.block_info.tool_id)
                ->checkState(0)
            == Qt::Unchecked) {
            block_error.other_error.error_bit.no_tool = 1;
            error_info.append("Resource not exist\r\n\r\n");
        } else {
            block_error.other_error.error_bit.no_tool = 0;
            /* 判断条件别名是否被修改 */
            if (block_attribute.other_name
                != mainwindow->condition_view_class->condition_get_name(block_attribute.block_info.tool_type,
                                                                        block_attribute.block_info.tool_id)) {
                block_attribute.other_name = mainwindow->condition_view_class->condition_get_name(
                    block_attribute.block_info.tool_type, block_attribute.block_info.tool_id);
                //只取前面部分字符显示
                set_display_label(block_attribute.other_name);
                this->setToolTip(block_attribute.other_name);
                block_attribute.func_string =
                    block_attribute.other_name + QString::number(block_attribute.self_id) + "_func()";
            }
        }
    }
    if (output_point_list[0]->get_connect_num() == 0) {
        block_error.output_error.error_bit.output1 = 1;
        error_info.append("Output1 not connect");
    } else {
        block_error.output_error.error_bit.output1 = 0;
    }
    /* 错误状态下边框变为红色 */
    if (block_error.input_error.value != 0 || block_error.output_error.value != 0
        || block_error.other_error.value != 0) {
        set_brush_state(BLOCK_STATE_ERROR);
    } else {
        error_info.append("No error");
        set_brush_state(BLOCK_STATE_IDE);
    }
}

/**
 * @brief 鼠标左键点击块时要显示的块的属性
 */
void condition_block::attribute_display()
{ /*
     if (mainwindow->logic_view_class->attribute_display_id != block_attribute.self_id) {
         return;  //判断要显示属性的块的ID是否与自身相同
     }*/
    if (focus_state == false) {
        return;
    }
    QList<QStringList> source_list = { di_resource, ai_resource, pi_resource, qep_resource };
    attribute_description.clear();
    attribute_name.clear();
    attribute_name.append("ID");
    attribute_description.append(QString::number(block_attribute.self_id));
    attribute_name.append("Name");
    attribute_description.append(block_attribute.other_name);
    switch (block_attribute.block_info.tool_type) {
    case TOOL_TYPE_CONDI_BOOL:

        break;
    case TOOL_TYPE_CONDI_DI:
        attribute_name.append("Source");
        attribute_description.append(
            source_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id]);
        attribute_name.append("Reverse");
        if (condition_di_set.is_reverse) {
            attribute_description.append("Yes");
        } else {
            attribute_description.append("Not");
        }
        break;
    default:
        attribute_name.append("Source");
        attribute_description.append(
            source_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id]);
        attribute_name.append("Calc");
        attribute_name.append("Value");
        attribute_description.append(calc_str[condition_ai_pi_qep_set.calc_type_index]);
        attribute_description.append(QString::number(condition_ai_pi_qep_set.value));
        break;
    }
    attribute_name.append("Error");
    attribute_description.append(error_info);

    ui->tableWidget_attribute->clearContents();
    ui->tableWidget_attribute->setRowCount(0);
    for (uint8_t i = 0; i < attribute_description.count(); i++) {
        ui->tableWidget_attribute->insertRow(i);
        QTableWidgetItem* item1 = new QTableWidgetItem(attribute_name[i]);
        item1->setTextAlignment(Qt::AlignCenter);
        QFont font = item1->font();
        font.setBold(true);  // 设置字体加粗
        item1->setFont(font);
        ui->tableWidget_attribute->setItem(i, 0, item1);
        QTableWidgetItem* item2 = new QTableWidgetItem(attribute_description[i]);
        ui->tableWidget_attribute->setItem(i, 1, item2);
    }
}

/**
 * @brief 向连接的块发送自身属性
 */
void condition_block::resource_config()
{
    output_point_list[0]->send_block_attribute();
    if (block_attribute.block_info.tool_type < TOOL_TYPE_CONDI_BOOL) {
        uint8_t resource_start_num[4] = { INPUT_DI_RESOURCE_START, INPUT_AI_RESOURCE_START, INPUT_PI_RESOURCE_START,
                                          INPUT_QEP_RESOURCE_START };
        mainwindow->project_report_class->input_resource_info
            .is_used[resource_start_num[block_attribute.block_info.tool_type] + block_attribute.block_info.tool_id] =
            true;
    }
}

/**
 * @brief 快碰撞检测
 * @return
 */
bool condition_block::block_collison_detect(QRectF rect)
{
    // 获取当前块的边界矩形
    QRectF currentRect = rect;
    if ((currentRect.x() < SCENE_MARGIN_MIN || currentRect.x() > SCENE_MARGIN_MAX)
        || (currentRect.y() < SCENE_MARGIN_MIN || currentRect.y() > SCENE_MARGIN_MAX)) {
        return true;
    }
    // 遍历所有块
    QList<QGraphicsItem*> allBlocks = scene()->items();
    foreach (QGraphicsItem* item, allBlocks) {
        if (item == this || childItems().contains(item))
            continue;

        if (item->type() >= QGraphicsItem::UserType + BLOCK_TYPE_LOGIC
            && item->type() <= QGraphicsItem::UserType + BLOCK_TYPE_CONNECT) {
            // 获取其他块的边界矩形
            QRectF otherRect = item->sceneBoundingRect();
            // 判断是否与其他块及其周围一定距离范围内重叠
            if (currentRect.intersects(otherRect.adjusted(-BLOCK_SPCING, -BLOCK_SPCING, BLOCK_SPCING, BLOCK_SPCING))) {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief 仿真模式下设置仿真数据
 * @param res 模块条件成立仿真数据
 */
void condition_block::debug_data_set(bool res)
{
    if (mainwindow->project_debug_class->get_debug_state() == DEBUG_STATE_ING) {
        foreach (connect_block* item, output_point_list) {
            item->send_debug_data(res);
        }
        if (res) {
            set_pen_state(BLOCK_STATE_DEBUG);
        } else {
            set_pen_state(BLOCK_STATE_IDE);
        }
    }
}

/* user slots */
/**
 * @brief 状态更新槽函数
 */
void condition_block::update_state_slot()
{
    condition_tool_detect();
    attribute_display();
    resource_config();
}
/* system event */

void condition_block::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (settingsAction != nullptr) {
            right_menu_setting();
        }
    }
}

void condition_block::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (temp_rect) {
        QPointF pos = mapToScene(event->pos());
        int     x   = qRound(pos.x() / 10) * 10;
        int     y   = qRound(pos.y() / 10) * 10;
        temp_rect->setPos(x - temp_rect->rect().width() / 2, y - temp_rect->rect().height() / 2);
        temp_rect->setVisible(true);
        if (block_collison_detect(temp_rect->sceneBoundingRect())) {
            temp_rect->setBrush(Qt::red);
        } else {
            temp_rect->setBrush(Qt::green);
        }
    }
}

void condition_block::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    setCursor(Qt::ArrowCursor);  // 设置鼠标样式为箭头
    QGraphicsRectItem::mouseReleaseEvent(event);
    if (temp_rect) {
        if (block_collison_detect(temp_rect->sceneBoundingRect())) {
            mainwindow->dispaly_status_message("此处已有其他块，禁止在此处放置", 3000);
        } else {
            setPos(temp_rect->scenePos());
            foreach (QGraphicsItem* child, childItems()) {
                connect_block* connectBlock = dynamic_cast<connect_block*>(child);
                if (connectBlock) {
                    connectBlock->position_change();
                }
            }
        }
        scene()->removeItem(temp_rect);
        temp_rect = nullptr;
    }
}

void condition_block::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    setCursor(Qt::ClosedHandCursor);  // 设置鼠标样式为手掌抓起
    // 记录块的原始位置
    temp_rect = new QGraphicsRectItem(rect());
    temp_rect->setVisible(false);
    temp_rect->setPos(pos());
    scene()->addItem(temp_rect);
    temp_rect->setOpacity(0.3);
    // 调用父类的事件处理函数
    QGraphicsRectItem::mousePressEvent(event);
}

void condition_block::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        block_delete();
    } else {
        QGraphicsItem::keyPressEvent(event);
    }
}
