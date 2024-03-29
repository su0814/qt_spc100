#include "logic_block.h"
#include "logic_view.h"
#include "mainwindow.h"
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QFormLayout>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <qgraphicsscene.h>
const int logic_block::defaultWidth  = LOGIC_BLOCK_WIDTH;
const int logic_block::defaultHeight = LOGIC_BLOCK_HEIGHT;

/**
 * @brief 逻辑块构造函数，通过拖拽工具生成
 * @param x 坐标x
 * @param y 坐标y
 * @param tool_info 块类型信息
 * @param id 块ID
 * @param uiparent mainwindow，用于调用主程序元素
 * @param parent
 */
logic_block::logic_block(int x, int y, tool_info_t* tool_info, uint32_t id, QWidget* uiparent, QGraphicsItem* parent)
    : QGraphicsRectItem(0, 0, defaultWidth, defaultHeight, parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )uiparent;
    setPos(x - defaultWidth / 2, y - defaultHeight / 2);
    block_attribute.self_id = id;
    block_attribute.parent_id.append(id);
    block_attribute.block_info.tool_type = tool_info->tool_type;
    block_attribute.block_info.tool_id   = tool_info->tool_id;
    QStringList name;
    name << "AND"
         << "OR"
         << "NOT"
         << "SF"
         << "EXIT";
    block_attribute.other_name =
        name[block_attribute.block_info.tool_type - TOOL_TYPE_LOGIC_AND] + QString::number(block_attribute.self_id);
    if (block_attribute.block_info.tool_type == TOOL_TYPE_LOGIC_SF) {
        sf_param.name        = "sf" + QString::number(block_attribute.self_id);
        sf_param.sf_type     = SAFE_TYPE_ESTOP;
        sf_param.ss_code     = DEFAULT_SS_CODE;
        sf_param.delay_time  = 0;
        sf_param.option_time = 10;
        for (uint8_t i = 0; i < MAX_SF_NUM; i++) {
            if (!mainwindow->logic_view_class->sf_used_inf.sf_code[i].is_used) {
                sf_param.sf_code = mainwindow->logic_view_class->sf_used_inf.sf_code[i].code;
                mainwindow->logic_view_class->sf_used_inf.sf_code[i].is_used = true;
                mainwindow->logic_view_class->sf_used_inf.used_number++;
                mainwindow->logic_view_class->sf_used_inf.sf_param[i]   = sf_param;
                mainwindow->logic_view_class->sf_used_inf.block_name[i] = block_attribute.other_name;
                break;
            }
        }
    }
    logic_block_init();
    connect_point_init();
}

/**
 * @brief 逻辑块构造函数，通过工程重建的构造
 * @param project 工程文件信息
 * @param uiparent mainwindow，用于调用主程序元素
 * @param parent
 */
logic_block::logic_block(QJsonObject project, QWidget* uiparent, QGraphicsItem* parent)
    : QGraphicsRectItem(parent)
{
    ui                      = MainWindow::my_ui->ui;
    mainwindow              = ( MainWindow* )uiparent;
    int x                   = project["x"].toInt();
    int y                   = project["y"].toInt();
    block_attribute.self_id = project["self_id"].toInt();
    block_attribute.parent_id.append(block_attribute.self_id);
    block_attribute.block_info.tool_type = ( tool_type_e )project["tooltype"].toInt();
    block_attribute.block_info.tool_id   = ( tool_id_e )project["toolid"].toInt();
    block_attribute.other_name           = project["othername"].toString();
    exit_delay_time                      = project["exitdelaytime"].toInt();
    sf_param.name                        = project["sfname"].toString();
    sf_param.sf_code                     = project["sfcode"].toInt();
    sf_param.sf_type                     = project["sftype"].toInt();
    sf_param.ss_code                     = project["sscode"].toInt();
    sf_param.delay_time                  = project["sfdelaytime"].toInt();
    sf_param.option_time                 = project["sfoptiontime"].toInt();
    if (block_attribute.block_info.tool_type == TOOL_TYPE_LOGIC_SF) {
        mainwindow->logic_view_class->sf_used_inf.sf_code[sf_param.sf_code - SF_USER_CODE].is_used = true;
        mainwindow->logic_view_class->sf_used_inf.used_number++;
        mainwindow->logic_view_class->sf_used_inf.sf_param[sf_param.sf_code - SF_USER_CODE] = sf_param;
        mainwindow->logic_view_class->sf_used_inf.block_name[sf_param.sf_code - SF_USER_CODE] =
            block_attribute.other_name;
        if (sf_param.sf_type >= SAFE_TYPE_DECELERATION_DETECTION1
            && sf_param.sf_type < SAFE_TYPE_DECELERATION_DETECTION1 + MAX_DECELERATE_NUM) {
            mainwindow->logic_view_class
                ->sf_type_decelerate_isused[sf_param.sf_type - SAFE_TYPE_DECELERATION_DETECTION1] = true;
        }
    }
    QRect rect(0, 0, defaultWidth, defaultHeight);
    setRect(rect);
    setPos(x, y);
    logic_block_init();
    connect_point_init();
}

/**
 * @brief 逻辑块初始化
 */
void logic_block::logic_block_init()
{
    QPen pen(QColor(50, 50, 50));
    pen.setWidth(2);  // 设置边框宽度为2像素
    QBrush brush(QColor(200, 200, 10));
    this->setPen(pen);
    this->setBrush(brush);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
    deleteAction = new QAction("删除", this);
    deleteAction->setIcon(QIcon(":/new/photo/photo/delete_block.png"));
    //只有SF和EXIT块有设置选项
    if (block_attribute.block_info.tool_type >= TOOL_TYPE_LOGIC_SF) {
        settingsAction = new QAction("设置", this);
        settingsAction->setIcon(QIcon(":/new/photo/photo/setting_block.png"));
        menu.addAction(settingsAction);
    }
    menu.addAction(deleteAction);
    setCursor(Qt::ArrowCursor);  // 设置鼠标样式为箭头
    connect(&update_timer, &QTimer::timeout, this, update_state_slot);
    update_timer.start(BLOCK_DATA_REFRESH_TIME);
    //同时只允许一个EXIT存在
    if (block_attribute.block_info.tool_type == TOOL_TYPE_LOGIC_EXIT) {
        mainwindow->logic_tools_class->logic_tools_list[TOOL_TYPE_LOGIC_EXIT - TOOL_TYPE_LOGIC_AND]->setEnabled(false);
    }
}

/**
 * @brief 设置运行模式
 * @param mode 运行模式
 */
void logic_block::set_mode(block_mode_e mode)
{
    block_mode = mode;
    if (mode == BLOCK_MODE_NORMAL) {
        QPen pen(QColor(50, 50, 50));
        pen.setWidth(2);  // 设置边框宽度为2像素
        this->setPen(pen);
        QBrush brush(QColor(0, 0, 0));
        foreach (connect_block* item, output_point_list) {
            item->setBrush(brush);
        }
        foreach (connect_block* item, input_point_list) {
            item->setBrush(brush);
        }
    }
}

/**
 * @brief 生成工程信息
 * @return json格式的工程信息
 */
QJsonObject logic_block::logic_block_project_info()
{
    QJsonObject rootObject;
    rootObject["x"]             = ( int )this->pos().x();
    rootObject["y"]             = ( int )this->pos().y();
    rootObject["self_id"]       = static_cast<int>(block_attribute.self_id);
    rootObject["othername"]     = (block_attribute.other_name);
    rootObject["tooltype"]      = (block_attribute.block_info.tool_type);
    rootObject["toolid"]        = (block_attribute.block_info.tool_id);
    rootObject["sfname"]        = sf_param.name;
    rootObject["sfcode"]        = sf_param.sf_code;
    rootObject["sftype"]        = sf_param.sf_type;
    rootObject["sscode"]        = sf_param.ss_code;
    rootObject["sfdelaytime"]   = sf_param.delay_time;
    rootObject["sfoptiontime"]  = sf_param.option_time;
    rootObject["exitdelaytime"] = exit_delay_time;
    return rootObject;
}

/**
 * @brief 右键菜单事件
 * @param event
 */
void logic_block::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (block_mode != BLOCK_MODE_NORMAL) {
        return;
    }
    QAction* selectedItem = menu.exec(event->screenPos());
    if (selectedItem == nullptr) {
        return;
    }
    if (selectedItem == settingsAction) {
        right_menu_setting();
    } else if (selectedItem == deleteAction) {
        if (scene()) {
            block_delete();
        }
    }
}

/**
 * @brief 块自删除
 */
void logic_block::block_delete()
{
    if (mainwindow->logic_view_class->draw_line_state != DRAW_LINE_STATE_IDLE) {
        return;
    }
    update_timer.stop();
    /* 释放占用的SF名额 */
    if (block_attribute.block_info.tool_type == TOOL_TYPE_LOGIC_SF) {
        mainwindow->logic_view_class->sf_used_inf.sf_code[sf_param.sf_code - SF_USER_CODE].is_used = false;
        mainwindow->logic_view_class->sf_used_inf.used_number--;
    }
    /* 释放占用的EXIT名额 */
    if (block_attribute.block_info.tool_type == TOOL_TYPE_LOGIC_EXIT) {
        mainwindow->logic_tools_class->logic_tools_list[TOOL_TYPE_LOGIC_EXIT - TOOL_TYPE_LOGIC_AND]->setEnabled(true);
    }
    if (mainwindow->logic_view_class->logic_block_list.contains(this)) {
        mainwindow->logic_view_class->logic_block_list.removeOne(this);
    }
    scene()->removeItem(this);
    delete this;
}

/**
 * @brief 连接点初始化
 */
void logic_block::connect_point_init()
{
    switch (block_attribute.block_info.tool_type) {
    case TOOL_TYPE_LOGIC_AND:
    case TOOL_TYPE_LOGIC_OR:
        for (uint8_t i = 0; i < 2; i++) {
            connect_block* point = new connect_block(-CONNECT_POINT_WIDTH, (defaultHeight * (i + 1) / 3),
                                                     CONNECT_POINT_TYPE_INPUT, i, &block_attribute, this);
            input_point_list.append(point);
        }
        break;

    default:
        connect_block* point = new connect_block(-CONNECT_POINT_WIDTH, (defaultHeight / 2), CONNECT_POINT_TYPE_INPUT, 0,
                                                 &block_attribute, this);
        input_point_list.append(point);
        break;
    }
    if (block_attribute.block_info.tool_type < TOOL_TYPE_LOGIC_SF) {
        connect_block* output_point =
            new connect_block(defaultWidth, defaultHeight / 2, CONNECT_POINT_TYPE_OUTPUT, 0, &block_attribute, this);
        output_point_list.append(output_point);
    }
    QStringList icon_list;
    icon_list << ":/new/photo/photo/and.png"
              << ":/new/photo/photo/OR.png"
              << ":/new/photo/photo/LOGIC_NOT.png"
              << ":/new/photo/photo/SF.png"
              << ":/new/photo/photo/exit.png";
    QPixmap              pixmap(icon_list[block_attribute.block_info.tool_type - TOOL_TYPE_LOGIC_AND]);
    QGraphicsPixmapItem* pixmapItem =
        new QGraphicsPixmapItem(pixmap.scaled(LOGIC_BLOCK_WIDTH, LOGIC_BLOCK_WIDTH), this);
    pixmapItem->setPos(this->boundingRect().center() - pixmapItem->boundingRect().center());

    QGraphicsTextItem* label = new QGraphicsTextItem(block_attribute.other_name, this);
    label->setFont(QFont("Arial", 4));  // 设置字体
    label->setPos(this->boundingRect().center().x() - label->boundingRect().center().x(),
                  this->boundingRect().center().y() + LOGIC_BLOCK_WIDTH / 2 - label->boundingRect().center().y());
    if (block_attribute.block_info.tool_type == TOOL_TYPE_LOGIC_SF) {
        sfname_label = new QGraphicsTextItem(sf_param.name, this);
        sfname_label->setFont(QFont("Arial", 4));  // 设置字体
        sfname_label->setPos(this->boundingRect().center().x() - sfname_label->boundingRect().center().x(),
                             this->boundingRect().y() - sfname_label->boundingRect().height());
    }
    block_attribute.func_string = block_attribute.other_name + "_" + sf_param.name + "_func()";
}

/**
 * @brief 右键菜单设置，不同块进入不同菜单
 */
void logic_block::right_menu_setting()
{
    switch (block_attribute.block_info.tool_type) {
    case TOOL_TYPE_LOGIC_SF:
        sf_right_menu_setting();
        break;
    case TOOL_TYPE_LOGIC_EXIT:
        exit_right_menu_setting();
        break;
    default:
        break;
    }
}

/**
 * @brief SF块的右键菜单
 */
void logic_block::sf_right_menu_setting()
{
    QDialog dialog;
    dialog.setModal(false);
    QFormLayout* layout = new QFormLayout(&dialog);
    QLineEdit    name_edit;
    QComboBox    sf_code_combo;
    QSpinBox*    delay_time_spin  = new QSpinBox;
    QSpinBox*    option_time_spin = new QSpinBox;
    QComboBox    sf_type_combo;
    QComboBox    ss_code_combo;
    /* sf name ui */
    QRegExp           regExp("[A-Za-z0-9_]*");
    QRegExpValidator* validator = new QRegExpValidator(regExp, &name_edit);
    name_edit.setValidator(validator);
    name_edit.setMaxLength(20);
    name_edit.setText(sf_param.name);
    layout->addRow("Name:", &name_edit);

    /* sf_code ui */
    sf_code_combo.addItem("0x" + QString::number(sf_param.sf_code, 16));
    for (uint8_t i = 0; i < MAX_SF_NUM; i++) {
        if (!mainwindow->logic_view_class->sf_used_inf.sf_code[i].is_used) {
            sf_code_combo.addItem("0x"
                                  + QString::number(mainwindow->logic_view_class->sf_used_inf.sf_code[i].code, 16));
        }
    }
    sf_code_combo.setCurrentText("0x" + QString::number(sf_param.sf_code, 16));
    layout->addRow("Sf code:", &sf_code_combo);

    /* sf type ui */
    sf_type_combo.addItems(sf_type_str);
    for (int i = SAFE_TYPE_DECELERATION_DETECTION1; i < SAFE_TYPE_DECELERATION_DETECTION1 + MAX_DECELERATE_NUM; i++) {
        if (mainwindow->logic_view_class->sf_type_decelerate_isused[i - SAFE_TYPE_DECELERATION_DETECTION1]
            && sf_param.sf_type != i) {
            sf_type_combo.model()->setData(sf_type_combo.model()->index(i, 0), false, Qt::UserRole - 1);
        }
    }

    sf_type_combo.setCurrentIndex(sf_param.sf_type);
    layout->addRow("Sf type:", &sf_type_combo);

    /* ss code ui */
    for (uint8_t i = 0; i < mainwindow->condition_view_class->ss_info_list.count(); i++) {
        ss_code_combo.addItem("0x" + QString::number(mainwindow->condition_view_class->ss_info_list[i].ss_code, 16));
        if (mainwindow->condition_view_class->ss_info_list[i].ss_code == sf_param.ss_code) {
            ss_code_combo.setCurrentIndex(i);
        }
    }
    ss_code_combo.setCurrentText("0x" + QString::number(sf_param.ss_code, 16));
    layout->addRow("Ss code:", &ss_code_combo);

    /* delaytime ui */
    delay_time_spin->setMinimum(0);
    delay_time_spin->setMaximum(30000);
    delay_time_spin->setValue(sf_param.delay_time);
    layout->addRow("Delay time(ms):", delay_time_spin);

    /* optiontime ui */
    option_time_spin->setMinimum(10);
    option_time_spin->setMaximum(30000);
    option_time_spin->setValue(sf_param.option_time);
    layout->addRow("Option time(ms):", option_time_spin);
    /* 特殊处理relay类型 */
    if (sf_param.sf_type == SAFE_TYPE_RELAY_MONITOR) {
        layout->labelForField(option_time_spin)->setVisible(true);
        option_time_spin->setVisible(true);
        layout->labelForField(delay_time_spin)->setVisible(false);
        delay_time_spin->setVisible(false);
    } else {
        layout->labelForField(delay_time_spin)->setVisible(true);
        delay_time_spin->setVisible(true);
        layout->labelForField(option_time_spin)->setVisible(false);
        option_time_spin->setVisible(false);
    }
    /* sf type变化时，更改ui */
    connect(&sf_type_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        if (index == SAFE_TYPE_RELAY_MONITOR) {
            layout->labelForField(option_time_spin)->setVisible(true);
            option_time_spin->setVisible(true);
            layout->labelForField(delay_time_spin)->setVisible(false);
            delay_time_spin->setVisible(false);
        } else {
            layout->labelForField(delay_time_spin)->setVisible(true);
            delay_time_spin->setVisible(true);
            layout->labelForField(option_time_spin)->setVisible(false);
            option_time_spin->setVisible(false);
        }
    });
    QPushButton okButton("确定");
    layout->addRow(&okButton);
    QObject::connect(&okButton, &QPushButton::clicked, [&]() {
        mainwindow->logic_view_class->sf_used_inf.sf_code[sf_param.sf_code - SF_USER_CODE].is_used = false;
        sf_param.name                                                                              = name_edit.text();
        bool     ok;
        uint16_t sfcode = sf_code_combo.currentText().toInt(&ok, 16);
        if (ok) {
            sf_param.sf_code = sfcode;
        }
        if (sf_type_combo.currentIndex() != sf_param.sf_type) {
            if (sf_param.sf_type >= SAFE_TYPE_DECELERATION_DETECTION1
                && sf_param.sf_type < SAFE_TYPE_DECELERATION_DETECTION1 + MAX_DECELERATE_NUM) {
                mainwindow->logic_view_class
                    ->sf_type_decelerate_isused[sf_param.sf_type - SAFE_TYPE_DECELERATION_DETECTION1] = false;
            }
            if (sf_type_combo.currentIndex() >= SAFE_TYPE_DECELERATION_DETECTION1
                && sf_type_combo.currentIndex() < SAFE_TYPE_DECELERATION_DETECTION1 + MAX_DECELERATE_NUM) {
                mainwindow->logic_view_class
                    ->sf_type_decelerate_isused[sf_type_combo.currentIndex() - SAFE_TYPE_DECELERATION_DETECTION1] =
                    true;
            }
            sf_param.sf_type = sf_type_combo.currentIndex();
        }

        uint16_t sscode = ss_code_combo.currentText().toInt(&ok, 16);
        if (ok) {
            sf_param.ss_code = sscode;
        }
        sf_param.delay_time  = delay_time_spin->value();
        sf_param.option_time = option_time_spin->value();
        mainwindow->logic_view_class->sf_used_inf.sf_code[sf_param.sf_code - SF_USER_CODE].is_used = true;
        mainwindow->logic_view_class->sf_used_inf.sf_param[sf_param.sf_code - SF_USER_CODE]        = sf_param;
        mainwindow->logic_view_class->sf_used_inf.block_name[sf_param.sf_code - SF_USER_CODE] =
            block_attribute.other_name;
        sfname_label->setPlainText(sf_param.name);  // 设置字体大小
        sfname_label->setPos(this->boundingRect().center().x() - sfname_label->boundingRect().center().x(),
                             this->boundingRect().y() - sfname_label->boundingRect().height());
        block_attribute.func_string = block_attribute.other_name + "_" + sf_param.name + "_func()";
        dialog.close();
    });
    dialog.exec();
}

/**
 * @brief EXIT块的右键菜单
 */
void logic_block::exit_right_menu_setting()
{
    QDialog dialog;
    dialog.setModal(false);
    QFormLayout* layout          = new QFormLayout(&dialog);
    QSpinBox*    delay_time_spin = new QSpinBox;

    /* delaytime ui */
    delay_time_spin->setMinimum(0);
    delay_time_spin->setMaximum(30000);
    delay_time_spin->setValue(sf_param.delay_time);
    layout->addRow("Delay time(ms):", delay_time_spin);
    QPushButton okButton("确定");
    layout->addRow(&okButton);
    QObject::connect(&okButton, &QPushButton::clicked, [&]() {
        exit_delay_time = delay_time_spin->value();
        dialog.close();
    });
    dialog.exec();
}

/**
 * @brief 故障检测
 */
void logic_block::error_detect()
{
    if (block_mode != BLOCK_MODE_NORMAL) {
        return;
    }
#if 1
    QList<uint32_t> parent_list;
    error_info.clear();
    for (uint8_t i = 0; i < input_point_list.count(); i++) {
        if (input_point_list[i]->connect_is_created()) {
            block_error.input_error.value &= ~(0x01 << i);
            /* 统计各个输入节点的父节点列表 */
            parent_list.append(input_point_list[i]->parent_block_attribute.parent_id);
        } else {
            block_error.input_error.value |= (0x01 << i);
            error_info.append("Input" + QString::number(i) + " not connect\r\n\r\n");
        }
    }
    /* 更新父节点列表 */
    parent_list.append(block_attribute.self_id);
    if (parent_list != block_attribute.parent_id) {
        block_attribute.parent_id.clear();
        block_attribute.parent_id.append(parent_list);
    }
    for (uint8_t i = 0; i < output_point_list.count(); i++) {
        if (output_point_list[i]->get_connect_num() > 0) {
            block_error.output_error.value &= ~(0x01 << i);
        } else {
            block_error.output_error.value |= (0x01 << i);
            error_info.append("Output" + QString::number(i) + " not connect\r\n\r\n");
        }
    }
    /* 判断SF设置的ss是否还存在 */
    if (block_attribute.block_info.tool_type == TOOL_TYPE_LOGIC_SF) {
        block_error.other_error.error_bit.no_ss = 1;
        for (uint8_t i = 0; i < mainwindow->condition_view_class->ss_info_list.count(); i++) {
            if (sf_param.ss_code == sf_param.ss_code) {
                block_error.other_error.error_bit.no_ss = 0;
                break;
            }
        }
        if (sf_param.ss_code == DEFAULT_SS_CODE) {
            block_error.other_error.error_bit.no_ss = 0;
        }
        if (block_error.other_error.error_bit.no_ss == 1) {
            error_info.append("Sscode not exist\r\n\r\n");
        }
    }

    if (block_error.input_error.value != 0 || block_error.other_error.value != 0
        || block_error.output_error.value != 0) {
        QPen pen(QColor(255, 100, 110));
        pen.setWidth(4);
        this->setPen(pen);
    } else {
        QPen pen(QColor(50, 50, 50));
        pen.setWidth(2);
        this->setPen(pen);
        error_info.append("No error");
    }
#endif
}

/**
 * @brief 逻辑代码生成函数
 */
void logic_block::logic_string_generate()
{
    if (block_mode != BLOCK_MODE_NORMAL) {
        return;
    }
    int condi_size = mainwindow->logic_view_class->condition_block_list.size();
    if (block_error.input_error.value == 0) {
        switch (block_attribute.block_info.tool_type) {
        case TOOL_TYPE_LOGIC_AND:
        case TOOL_TYPE_LOGIC_OR: {
            block_attribute.logic_subcondi_string =
                "\r\nlocal result0 = " + input_point_list[0]->parent_block_attribute.func_string + "\r\n";
            block_attribute.logic_string =
                "set_emu_data("
                + QString::number(condi_size + mainwindow->logic_view_class->logic_block_list.indexOf(this))
                + ",(result0";
            for (uint8_t i = 1; i < input_point_list.count(); i++) {
                block_attribute.logic_subcondi_string +=
                    ("local result" + QString::number(i) + " = "
                     + input_point_list[i]->parent_block_attribute.func_string + "\r\n");
                block_attribute.logic_string +=
                    (lua_logic_keyword[block_attribute.block_info.tool_type - TOOL_TYPE_LOGIC_AND] + "result"
                     + QString::number(i));
            }
            block_attribute.logic_string += "))";
        } break;
        case TOOL_TYPE_LOGIC_NOT:
            block_attribute.logic_subcondi_string =
                "\r\nlocal result0 = " + input_point_list[0]->parent_block_attribute.func_string + "\r\n";
            block_attribute.logic_string =
                "set_emu_data("
                + QString::number(condi_size + mainwindow->logic_view_class->logic_block_list.indexOf(this)) + ",("
                + lua_logic_keyword[block_attribute.block_info.tool_type - TOOL_TYPE_LOGIC_AND] + " result0 ))";
            break;
        case TOOL_TYPE_LOGIC_SF:
            if (input_point_list[0]->connect_is_created()) {
                if (sf_param.sf_type >= SAFE_TYPE_DECELERATION_DETECTION1
                    && sf_param.sf_type < SAFE_TYPE_DECELERATION_DETECTION1 + MAX_DECELERATE_NUM) {
                    block_attribute.logic_subcondi_string =
                        "\r\nlocal result0 = decelerate_check("
                        + QString::number(sf_param.sf_type - SAFE_TYPE_DECELERATION_DETECTION1) + ","
                        + input_point_list[0]->parent_block_attribute.func_string + ")\r\n";
                } else {
                    block_attribute.logic_subcondi_string =
                        "\r\nlocal result0 = " + input_point_list[0]->parent_block_attribute.func_string + "\r\n";
                }
                block_attribute.logic_string =
                    "set_emu_data("
                    + QString::number(condi_size + mainwindow->logic_view_class->logic_block_list.indexOf(this))
                    + ",result0)";
            } else {
                block_attribute.logic_string.clear();
            }
            break;
        case TOOL_TYPE_LOGIC_EXIT:
            if (input_point_list[0]->connect_is_created()) {
                block_attribute.logic_subcondi_string =
                    "\r\nlocal result0 = " + input_point_list[0]->parent_block_attribute.func_string + "\r\n";
                block_attribute.logic_string =
                    "set_emu_data("
                    + QString::number(condi_size + mainwindow->logic_view_class->logic_block_list.indexOf(this))
                    + ",result0)";
            } else {
                block_attribute.logic_string.clear();
            }
            break;
        default:
            break;
        }
    } else {
        block_attribute.logic_string.clear();
    }

    for (int i = 0; i < output_point_list.count(); i++) {
        output_point_list[i]->send_block_attribute();
    }
}

/**
 * @brief 属性显示代码，不同类型块显示不同属性
 */
void logic_block::attribute_display()
{

    if (mainwindow->logic_view_class->attribute_display_id != block_attribute.self_id) {
        return;
    }
    attribute_description.clear();
    attribute_name.clear();
    attribute_name << "ID"
                   << "Name";
    attribute_description.append(QString::number(block_attribute.self_id));
    attribute_description.append(block_attribute.other_name);
    switch (block_attribute.block_info.tool_type) {
    case TOOL_TYPE_LOGIC_AND:
    case TOOL_TYPE_LOGIC_OR:
    case TOOL_TYPE_LOGIC_NOT:
        attribute_name << "Error";
        attribute_description.append(error_info);
        break;
    case TOOL_TYPE_LOGIC_SF:
        attribute_name << "SFname"
                       << "SFcode"
                       << "SFtype"
                       << "Sscode"
                       << "Delaytime"
                       << "Optiontime"
                       << "Error";
        attribute_description.append(sf_param.name);
        attribute_description.append("0x" + QString::number(sf_param.sf_code, 16));
        attribute_description.append(sf_type_str[sf_param.sf_type]);
        attribute_description.append("0x" + QString::number(sf_param.ss_code, 16));
        attribute_description.append(QString::number(sf_param.delay_time) + "ms");
        attribute_description.append(QString::number(sf_param.option_time) + "ms");
        attribute_description.append(error_info);
        break;
    case TOOL_TYPE_LOGIC_EXIT:
        attribute_name << "Delaytime";
        attribute_description.append(QString::number(exit_delay_time) + "ms");
        break;
    default:
        break;
    }
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

/**
 * @brief 块碰撞检测
 * @return
 */
bool logic_block::block_collison_detect()
{
    // 获取当前块的边界矩形
    QRectF currentRect = sceneBoundingRect();
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
 * @brief 仿真数据设置
 * @param res 仿真数据
 */
void logic_block::debug_data_set(bool res)
{
    if (block_mode == BLOCK_MODE_DEBUG) {
        if (res) {
            QPen pen(QColor(0, 255, 0));
            pen.setWidth(2);
            this->setPen(pen);
        } else {
            QPen pen(QColor(50, 50, 50));
            pen.setWidth(2);
            this->setPen(pen);
        }
        foreach (connect_block* item, output_point_list) {
            item->send_debug_data(res);
        }
    }
}

/* USER SLOTS */
void logic_block::update_state_slot()
{
    if (block_attribute.block_info.tool_type != TOOL_TYPE_LOGIC_EXIT) {
        error_detect();
    }
    logic_string_generate();
    attribute_display();
}

/* 事件  */

void logic_block::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    if (block_mode != BLOCK_MODE_NORMAL) {
        return;
    }
    if (settingsAction != nullptr) {
        right_menu_setting();
    }
}

void logic_block::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (block_mode != BLOCK_MODE_NORMAL) {
        return;
    }
    QPointF pos = mapToScene(event->pos());
    int     x   = qRound(pos.x() / 10) * 10;
    int     y   = qRound(pos.y() / 10) * 10;
    setPos(x - defaultWidth / 2, y - defaultHeight / 2);
}

void logic_block::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (block_mode != BLOCK_MODE_NORMAL) {
        return;
    }
    setCursor(Qt::ArrowCursor);  // 设置鼠标样式为箭头
    QGraphicsRectItem::mouseReleaseEvent(event);
    if (block_collison_detect()) {
        mainwindow->dispaly_status_message("此处已有其他块，禁止在此处放置", 3000);
        setPos(originalPos);
    }
    QBrush brush(QColor(200, 200, 10));
    setBrush(brush);
    setOpacity(1);
    foreach (QGraphicsItem* child, childItems()) {
        connect_block* connectBlock = dynamic_cast<connect_block*>(child);
        if (connectBlock) {
            connectBlock->position_change();
        }
    }
}

void logic_block::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        mainwindow->logic_view_class->attribute_display_id = block_attribute.self_id;
    }
    if (block_mode != BLOCK_MODE_NORMAL) {
        return;
    }
    setCursor(Qt::ClosedHandCursor);  // 设置鼠标样式为手掌抓起
    // 记录块的原始位置
    originalPos = pos();

    // 调用父类的事件处理函数
    QGraphicsRectItem::mousePressEvent(event);
}

QVariant logic_block::itemChange(GraphicsItemChange change, const QVariant& value)
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

void logic_block::keyPressEvent(QKeyEvent* event)
{
    if (block_mode != BLOCK_MODE_NORMAL) {
        return;
    }
    if (event->key() == Qt::Key_Delete) {
        block_delete();
    } else {
        QGraphicsItem::keyPressEvent(event);
    }
}
