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

condition_block::condition_block(int x, int y, tool_info_t* tool_info, uint32_t id, QWidget* uiparent,
                                 QGraphicsItem* parent)
    : QGraphicsRectItem(x, y, defaultWidth, defaultHeight, parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )uiparent;
    QPen pen(QColor(50, 50, 50));
    pen.setWidth(2);  // 设置边框宽度
    QBrush brush(QColor(173, 216, 230));
    this->setPen(pen);
    this->setBrush(brush);
    setPos(x - defaultWidth / 2, y - defaultHeight / 2);
    block_attribute.self_id = id;
    block_attribute.parent_id.append(id);
    block_attribute.block_info.tool_type = tool_info->tool_type;
    block_attribute.block_info.tool_id   = tool_info->tool_id;
    block_attribute.other_name           = mainwindow->condition_view_class->condition_get_name(
        block_attribute.block_info.tool_type, block_attribute.block_info.tool_id);
    connect_block* condition_point = new connect_block(x + defaultWidth, y + defaultHeight / 4,
                                                       CONNECT_POINT_TYPE_OUTPUT, 0, &block_attribute, this);
    output_point_list.append(condition_point);
    block_info_init();
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}
condition_block::condition_block(QJsonObject project, QWidget* uiparent, QGraphicsItem* parent)
    : QGraphicsRectItem(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )uiparent;
    QPen pen(QColor(50, 50, 50));
    pen.setWidth(2);  // 设置边框宽度
    QBrush brush(QColor(173, 216, 230));
    this->setPen(pen);
    this->setBrush(brush);
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
    QRect rect(x, y, defaultWidth, defaultHeight);
    setRect(rect);
    setPos(x - defaultWidth / 2, y - defaultHeight / 2);
    block_info_init();
    connect_block* condition_point = new connect_block(x + defaultWidth, y + defaultHeight / 4,
                                                       CONNECT_POINT_TYPE_OUTPUT, 0, &block_attribute, this);
    output_point_list.append(condition_point);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

void condition_block::block_info_init()
{

    settingsAction = new QAction("设置", this);
    deleteAction   = new QAction("删除", this);
    deleteAction->setIcon(QIcon(":/new/photo/photo/delete_block.png"));
    settingsAction->setIcon(QIcon(":/new/photo/photo/setting_block.png"));
    menu.addAction(settingsAction);
    menu.addAction(deleteAction);

    QList<QStringList> func_list;
    func_list.append(lua_di_func);
    func_list.append(lua_ai_func);
    func_list.append(lua_pi_func);
    func_list.append(lua_qep_func);
    if (block_attribute.block_info.tool_type == TOOL_TYPE_CONDI_DI) {
        block_attribute.logic_string =
            "(" + func_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id] + " == true"
            + ")";
    } else if (block_attribute.block_info.tool_type == TOOL_TYPE_CONDI_AI) {
        block_attribute.logic_string =
            "(" + func_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id] + " > 12" + ")";
        condition_ai_pi_qep_set.value = 12;
    } else {
        block_attribute.logic_string =
            "(" + func_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id] + " > 1000" + ")";
        condition_ai_pi_qep_set.value = 1000;
    }
    dispaly_label = new QGraphicsTextItem(block_attribute.other_name, this);
    dispaly_label->setFont(QFont("Arial", 4));  // 设置字体大小
    dispaly_label->setPos(this->boundingRect().center() - dispaly_label->boundingRect().center());
}

QJsonObject condition_block::condition_block_project_info()
{
    QJsonObject rootObject;
    rootObject["x"]          = ( int )this->pos().x() + defaultWidth / 2;
    rootObject["y"]          = ( int )this->pos().y() + defaultHeight / 2;
    rootObject["is_reverse"] = (uint8_t)(condition_di_set.is_reverse);
    rootObject["calc_type"]  = (condition_ai_pi_qep_set.calc_type_index);
    rootObject["calc_value"] = static_cast<int>(condition_ai_pi_qep_set.value);
    rootObject["self_id"]    = static_cast<int>(block_attribute.self_id);
    rootObject["othername"]  = (block_attribute.other_name);
    rootObject["tooltype"]   = (block_attribute.block_info.tool_type);
    rootObject["toolid"]     = (block_attribute.block_info.tool_id);
    return rootObject;
}

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

void condition_block::block_delete()
{
    uint8_t resource_start_num[4] = { 0, INPUT_DI_RESOURCE_NUM, INPUT_DI_RESOURCE_NUM + INPUT_AI_RESOURCE_NUM,
                                      INPUT_DI_RESOURCE_NUM + INPUT_AI_RESOURCE_NUM + INPUT_QEP_RESOURCE_NUM };
    mainwindow->project_report_class->input_resource_info
        .is_used[resource_start_num[block_attribute.block_info.tool_type] + block_attribute.block_info.tool_id] = false;
    emit block_delete_signal(this);
    scene()->removeItem(this);
    delete this;
}

void condition_block::set_display_label(QString text)
{
    dispaly_label->setPlainText(text);
}

void condition_block::right_menu_di()
{
    QDialog     dialog;
    QFormLayout layout(&dialog);
    QCheckBox   is_reverse;
    is_reverse.setText("输出反向");
    is_reverse.setChecked(condition_di_set.is_reverse);
    layout.addRow(&is_reverse);
    QPushButton okButton("确定");
    layout.addRow(&okButton);
    QObject::connect(&okButton, &QPushButton::clicked, [&]() {
        if (is_reverse.isChecked()) {
            block_attribute.logic_string = "(" + lua_di_func[block_attribute.block_info.tool_id] + "==false )";
        } else {
            block_attribute.logic_string = "(" + lua_di_func[block_attribute.block_info.tool_id] + "==true )";
        }
        condition_di_set.is_reverse = is_reverse.isChecked();
        // 处理参数
        dialog.close();
    });
    dialog.exec();
}

void condition_block::right_menu_ai_pi_qep()
{
    QList<QStringList> func_list;
    func_list.append(lua_di_func);
    func_list.append(lua_ai_func);
    func_list.append(lua_pi_func);
    func_list.append(lua_qep_func);
    QDialog     dialog;
    QFormLayout layout(&dialog);
    QComboBox   calc_type;
    calc_type.addItem(">");
    calc_type.addItem("<");
    calc_type.addItem(">=");
    calc_type.addItem("<=");
    calc_type.addItem("==");
    QSpinBox     value;
    QLabel       unit;
    QHBoxLayout* hboxLayout = new QHBoxLayout;
    if (block_attribute.block_info.tool_type == TOOL_TYPE_CONDI_AI) {
        value.setMaximum(30);
        unit.setText("V");
    } else {
        value.setMaximum(200000);
        unit.setText("pulse/s");
    }
    calc_type.setCurrentIndex(condition_ai_pi_qep_set.calc_type_index);
    value.setValue(condition_ai_pi_qep_set.value);
    hboxLayout->addWidget(&value);
    hboxLayout->addWidget(&unit);
    layout.addRow(&calc_type);
    layout.addRow(hboxLayout);

    QPushButton okButton("确定");
    layout.addRow(&okButton);
    QObject::connect(&okButton, &QPushButton::clicked, [&]() {
        block_attribute.logic_string =
            "(" + func_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id]
            + calc_type.currentText() + " " + QString::number(value.value()) + " )";
        condition_ai_pi_qep_set.calc_type_index = calc_type.currentIndex();
        condition_ai_pi_qep_set.value           = value.value();
        // 处理参数
        dialog.close();
    });
    dialog.exec();
}

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

void condition_block::condition_tool_detect()
{
    error_info.clear();
    if (ui->treeWidget_condi->topLevelItem(( int )block_attribute.block_info.tool_type)
            ->child(( int )block_attribute.block_info.tool_id)
            ->checkState(0)
        == Qt::Unchecked) {
        block_error.other_error.error_bit.no_tool = 1;
        error_info.append("Resource not exist\r\n\r\n");
    } else {
        block_error.other_error.error_bit.no_tool = 0;
        if (block_attribute.other_name
            != mainwindow->condition_view_class->condition_get_name(block_attribute.block_info.tool_type,
                                                                    block_attribute.block_info.tool_id)) {
            block_attribute.other_name = mainwindow->condition_view_class->condition_get_name(
                block_attribute.block_info.tool_type, block_attribute.block_info.tool_id);
            dispaly_label->setPlainText(block_attribute.other_name);
            dispaly_label->setPos(this->boundingRect().center() - dispaly_label->boundingRect().center());
        }
    }

    if (output_point_list[0]->get_connect_num() == 0) {
        block_error.output_error.error_bit.output1 = 1;
        error_info.append("Output1 not connect");
    } else {
        block_error.output_error.error_bit.output1 = 0;
    }
    if (block_error.input_error.value != 0 || block_error.output_error.value != 0
        || block_error.other_error.value != 0) {
        QPen pen(QColor(255, 100, 110));
        pen.setWidth(4);  // 设置边框宽度为2像素
        this->setPen(pen);
    } else {
        error_info.append("No error");
        QPen pen(QColor(50, 50, 50));
        pen.setWidth(2);  // 设置边框宽度为2像素
        this->setPen(pen);
    }
}

void condition_block::attribute_display()
{
    if (mainwindow->logic_view_class->attribute_display_id != block_attribute.self_id) {
        return;
    }
    QList<QStringList> source_list = { di_resource, ai_resource, pi_resource, qep_resource };
    attribute_description.clear();
    attribute_name.clear();
    attribute_name << "ID"
                   << "Name"
                   << "Source"
                   << "Error";
    attribute_description.append(QString::number(block_attribute.self_id));
    attribute_description.append(block_attribute.other_name);
    attribute_description.append(source_list[block_attribute.block_info.tool_type][block_attribute.block_info.tool_id]);
    attribute_description.append(error_info);

    ui->tableWidget_attribute->clearContents();
    ui->tableWidget_attribute->setRowCount(0);
    for (uint8_t i = 0; i < attribute_description.count(); i++) {
        ui->tableWidget_attribute->insertRow(i);
        QTableWidgetItem* item1 = new QTableWidgetItem(attribute_name[i]);
        item1->setTextAlignment(Qt::AlignCenter);
        QFont font = item1->font();
        // font.setPointSize(9);  // 设置字体大小为12
        font.setBold(true);  // 设置字体加粗
        item1->setFont(font);
        ui->tableWidget_attribute->setItem(i, 0, item1);
        QTableWidgetItem* item2 = new QTableWidgetItem(attribute_description[i]);
        ui->tableWidget_attribute->setItem(i, 1, item2);
    }
}

void condition_block::resource_config()
{
    uint8_t resource_start_num[4] = { 0, INPUT_DI_RESOURCE_NUM, INPUT_DI_RESOURCE_NUM + INPUT_AI_RESOURCE_NUM,
                                      INPUT_DI_RESOURCE_NUM + INPUT_AI_RESOURCE_NUM + INPUT_QEP_RESOURCE_NUM };
    mainwindow->project_report_class->input_resource_info
        .is_used[resource_start_num[block_attribute.block_info.tool_type] + block_attribute.block_info.tool_id] = true;
}

void condition_block::update_state()
{
    output_point_list[0]->send_block_attribute();
    condition_tool_detect();
    attribute_display();
    resource_config();
}

bool condition_block::block_collison_detect()
{
    // 获取当前块的边界矩形
    QRectF currentRect = sceneBoundingRect();
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

/* user slots */

/* system event */

void condition_block::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (settingsAction != nullptr) {
        right_menu_setting();
    }
}

void condition_block::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    setCursor(Qt::ArrowCursor);  // 设置鼠标样式为箭头
    QGraphicsRectItem::mouseReleaseEvent(event);
    if (block_collison_detect()) {
        setPos(originalPos);
    }
    QBrush brush(QColor(173, 216, 230));
    setBrush(brush);
    setOpacity(1);
    foreach (QGraphicsItem* child, childItems()) {
        connect_block* connectBlock = dynamic_cast<connect_block*>(child);
        if (connectBlock) {
            connectBlock->position_change();
        }
    }
}

void condition_block::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // 记录块的原始位置
    originalPos = pos();
    if (event->button() == Qt::LeftButton) {
        mainwindow->logic_view_class->attribute_display_id = block_attribute.self_id;
    }
    // 调用父类的事件处理函数
    QGraphicsRectItem::mousePressEvent(event);
}

QVariant condition_block::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        setOpacity(0.3);
        if (block_collison_detect()) {
            setBrush(Qt::red);
        } else {
            setBrush(Qt::green);
        }
    }
    return QGraphicsRectItem::itemChange(change, value);
}

void condition_block::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        block_delete();
    } else {
        QGraphicsItem::keyPressEvent(event);
    }
}
