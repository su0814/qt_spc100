#include "logic_menu.h"
#include <QDebug>
#include <QDrag>
#include <QMimeData>

logic_menu::logic_menu(QWidget* parent)
    : QTreeWidget(parent)
{
    init();
}

void logic_menu::init()
{
    this->setHeaderHidden(true);
    this->setStyleSheet("QTreeWidget::branch:closed:has-children {image: "
                        "url(:/new/photo/photo/menu_close.png);}QTreeWidget::branch:open:has-children {image: "
                        "url(:/new/photo/photo/menu_open.png);}"
                        "QTreeWidget { font-size: 10px; }");
    this->setDragDropMode(QAbstractItemView::DragDrop);
    setDragDropMode(QAbstractItemView::DragDrop);
    setAcceptDrops(true);
    setItemDelegate(new CustomItemDelegate);
    setIconSize(QSize(50, 50));
    QFont font;
    font.setPointSize(10);
    font.setBold(true);
    input_menu = new QTreeWidgetItem(this);
    input_menu->setFont(0, font);
    input_menu->setText(0, "输入");
    function_menu = new QTreeWidgetItem(this);
    function_menu->setFont(0, font);
    function_menu->setText(0, "功能块");
    output_menu = new QTreeWidgetItem(this);
    output_menu->setFont(0, font);
    output_menu->setText(0, "输出");
    function_item_init();
}

void logic_menu::function_item_init()
{
    config_block_data_t data;
    QStringList         base_pixmap;
    QStringList         apply_pixmap;
    QStringList         delay_counter_pixmap;
    QStringList         speed_pixmap;
    QFont               font;
    font.setBold(true);
    base_pixmap << "://logicphoto/and.ico"
                << "://logicphoto/or.ico"
                << "://logicphoto/not.ico"
                << "://logicphoto/xnor.ico"
                << "://logicphoto/xor.ico"
                << "://logicphoto/rs_trigger.ico"
                << "://logicphoto/encoder.ico"
                << "://logicphoto/decoder.ico";
    /* base function */
    data.config_param_data.model_iotype = MODEL_TYPE_LOGIC;
    data.config_param_data.model_type   = MODEL_LOGIC_BASE;
    QTreeWidgetItem* base_menu          = new QTreeWidgetItem(function_menu);
    base_menu->setFont(0, font);
    base_menu->setText(0, "基础逻辑");
    for (int i = MODEL_ID_LOGIC_BASE_AND; i <= MODEL_ID_LOGIC_BASE_DECODER; i++) {
        data.pixmap                     = base_pixmap[i];
        data.config_param_data.model_id = i;
        data.source_name                = model_name[MODEL_TYPE_LOGIC][MODEL_LOGIC_BASE][i];
        logic_element* item             = new logic_element(data, base_menu);
        function_item_list.append(item);
    }

    apply_pixmap << "://logicphoto/reset.ico"
                 << "://logicphoto/external_device_monitor.ico"
                 << "://logicphoto/freq_monitor.ico"
                 << "://logicphoto/edge_detected.ico"
                 << "://logicphoto/ramp_down.ico";
    data.config_param_data.model_type = MODEL_LOGIC_APPLICATION;
    QTreeWidgetItem* app_menu         = new QTreeWidgetItem(function_menu);
    app_menu->setFont(0, font);
    app_menu->setText(0, "应用逻辑");
    for (int i = MODEL_ID_LOGIC_APPLICATION_RESET; i <= MODEL_ID_LOGIC_APPLICATION_EDGE_DETECT; i++) {
        data.pixmap                     = apply_pixmap[i];
        data.config_param_data.model_id = i;
        data.source_name                = model_name[MODEL_TYPE_LOGIC][MODEL_LOGIC_APPLICATION][i];
        logic_element* item             = new logic_element(data, app_menu);
        function_item_list.append(item);
    }

    delay_counter_pixmap << "://logicphoto/on_delay.ico"
                         << "://logicphoto/off_delay.ico"
                         << "://logicphoto/adjust_on_delay.ico"
                         << "://logicphoto/adjust_off_delay.ico"
                         << "://logicphoto/event_counter.ico"
                         << "://logicphoto/message_generator.ico";
    data.config_param_data.model_type = MODEL_LOGIC_DELAY_COUNTER;
    QTreeWidgetItem* dc_menu          = new QTreeWidgetItem(function_menu);
    dc_menu->setFont(0, font);
    dc_menu->setText(0, "延时和计数器");
    for (int i = MODEL_ID_LOGIC_DELAY_ON; i <= MODEL_ID_LOGIC_DELAY_ADJUST_OFF; i++) {
        data.pixmap                     = delay_counter_pixmap[i];
        data.config_param_data.model_id = i;
        data.source_name                = model_name[MODEL_TYPE_LOGIC][MODEL_LOGIC_DELAY_COUNTER][i];
        logic_element* item             = new logic_element(data, dc_menu);
        function_item_list.append(item);
    }

    speed_pixmap << "://logicphoto/on_delay.ico"
                 << "://logicphoto/off_delay.ico"
                 << "://logicphoto/adjust_on_delay.ico"
                 << "://logicphoto/adjust_off_delay.ico"
                 << "://logicphoto/event_counter.ico"
                 << "://logicphoto/message_generator.ico";
    data.config_param_data.model_type = MODEL_LOGIC_SPEED;
    QTreeWidgetItem* speed_menu       = new QTreeWidgetItem(function_menu);
    speed_menu->setFont(0, font);
    speed_menu->setText(0, "速度监控");
    for (int i = MODEL_ID_LOGIC_SPEED_CROSS_CHECK; i <= MODEL_ID_LOGIC_SPEED_MONITOR; i++) {
        data.pixmap                     = delay_counter_pixmap[i];
        data.config_param_data.model_id = i;
        data.source_name                = model_name[MODEL_TYPE_LOGIC][MODEL_LOGIC_SPEED][i];
        logic_element* item             = new logic_element(data, speed_menu);
        function_item_list.append(item);
    }
}

logic_element* logic_menu::item_exist_detect(config_block_data_t data)
{
    if (data.config_param_data.model_iotype == MODEL_TYPE_INPUT) {
        foreach (logic_element* item, input_item_list) {
            if (item->is_exist(data)) {
                return item;
            }
        }
    } else {
        foreach (logic_element* item, output_item_list) {
            if (item->is_exist(data)) {
                return item;
            }
        }
    }
    return nullptr;
}

logic_element* logic_menu::get_function_item(config_block_data_t data)
{
    foreach (logic_element* item, function_item_list) {
        if (item->is_exist(data)) {
            return item;
        }
    }
}

/* user slots */
void logic_menu::add_item_from_config_slot(config_block_data_t data)
{
    if (item_exist_detect(data) != nullptr) {
        return;
    }
    logic_element* item;
    if (data.config_param_data.model_iotype == MODEL_TYPE_INPUT) {
        item = new logic_element(data, input_menu);
        input_item_list.append(item);
    } else {
        item = new logic_element(data, output_menu);
        output_item_list.append(item);
    }
}

void logic_menu::remove_item_from_config_slot(config_block_data_t data)
{
    logic_element* item = item_exist_detect(data);
    if (item) {
        if (data.config_param_data.model_iotype == MODEL_TYPE_INPUT) {
            input_item_list.removeOne(item);
            input_menu->removeChild(item);
            takeTopLevelItem(this->indexOfTopLevelItem(item));
        } else {
            input_item_list.removeOne(item);
            output_menu->removeChild(item);
            takeTopLevelItem(this->indexOfTopLevelItem(item));
        }
    }
}

void logic_menu::name_change_slot(config_block_data_t data)
{
    logic_element* item = item_exist_detect(data);
    if (item) {
        item->set_config_data(data);
    }
}

/* sys event */
void logic_menu::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions);
    QTreeWidgetItem* item = currentItem();
    if (item) {
        logic_element* element = dynamic_cast<logic_element*>(item);
        if (!element) {
            return;
        }
        QDrag*                drag     = new QDrag(this);
        QMimeData*            mimeData = new QMimeData;
        config_block_data_t*  s_data   = element->get_config_data();
        config_block_data_t** data     = &s_data;
        QByteArray            byteArray(reinterpret_cast<const char*>(data), sizeof(config_block_data_t*));
        mimeData->setData("ruohui/logic", byteArray);
        drag->setMimeData(mimeData);
        drag->exec(Qt::MoveAction);
    }
}
