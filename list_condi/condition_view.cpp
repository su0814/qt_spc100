#include "condition_view.h"
#include "mainwindow.h"
#include <QDebug>
#include <QJsonArray>
#include <QMenu>
#include <QStandardItemModel>
#define UPDATE_NAME_TIME (1000)
/**
 * @brief 条件视图构造函数，工程配置页面的条件设置
 * @param mparent
 * @param parent
 */
condition_view::condition_view(QWidget* mparent, QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )mparent;
    condition_tree_init();
    ss_table_init();
    connect(&update_tim, &QTimer::timeout, this, condition_name_update_slot);
}

/**
 * @brief ss列表初始化
 */
void condition_view::ss_table_init()
{
    /* 水平表头拉伸自适应宽度 */
    ui->tableWidget_ss->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    /* 垂直表头根据单元格内容调整大小 */
    ui->tableWidget_ss->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    /* 设置右键菜单为自定义菜单 */
    ui->tableWidget_ss->setContextMenuPolicy(Qt::CustomContextMenu);
    /* 连接右键菜单事件 */
    connect(ui->tableWidget_ss, &QTableWidget::customContextMenuRequested, this, ss_table_right_menu_slot);
    ui->tableWidget_ss->setRowCount(0);                                                             //设置行数
    ss_tabel_add_item(0xFF, mainwindow->config_view_class->config_photo_svg->param_ss_get() >> 1);  //添加默认的ss
}

/**
 * @brief 输入条件树形列表初始化
 */
void condition_view::condition_tree_init()
{
    // 隐藏表头
    other_name_edit_list.clear();
    ui->treeWidget_condi->setHeaderHidden(true);
    ui->treeWidget_condi->setColumnCount(2);  //列数
    /* DI */
    QTreeWidgetItem* topItem_di = new QTreeWidgetItem(ui->treeWidget_condi);
    topItem_di->setText(0, "DI");
    topItem_di->setCheckState(0, Qt::Unchecked);
    ui->treeWidget_condi->addTopLevelItem(topItem_di);
    for (uint8_t i = 0; i < di_resource.count(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(topItem_di);
        item->setText(0, di_resource[i]);
        item->setCheckState(0, Qt::Unchecked);
        QLineEdit* edit = new QLineEdit;
        edit->setMaxLength(25);
        edit->setMinimumWidth(40);
        edit->setMaximumWidth(250);
        QRegExp           regExp("[A-Za-z][A-Za-z0-9_]*");
        QRegExpValidator* validator = new QRegExpValidator(regExp, edit);
        edit->setValidator(validator);
        other_name_edit_list.append(edit);
        ui->treeWidget_condi->setItemWidget(item, 1, edit);
        di_item.append(item);
        if (i >= 8 && i <= 9) {
            item->setToolTip(0, di_resource[i] + "是" + ai_resource[i - 8] + "的复用，二者仅可选其一");
        } else if (i >= 10 && i <= 11) {
            item->setToolTip(0, di_resource[i] + "是" + pi_resource[i - 10] + "的复用，二者仅可选其一");
        }
    }
    /* AI */
    QTreeWidgetItem* topItem_ai = new QTreeWidgetItem(ui->treeWidget_condi);
    topItem_ai->setText(0, "AI");
    topItem_ai->setCheckState(0, Qt::Unchecked);
    ui->treeWidget_condi->addTopLevelItem(topItem_ai);
    for (uint8_t i = 0; i < ai_resource.count(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(topItem_ai);
        item->setText(0, ai_resource[i]);
        item->setCheckState(0, Qt::Unchecked);
        QLineEdit* edit = new QLineEdit;
        edit->setMaxLength(25);
        edit->setMinimumWidth(40);
        edit->setMaximumWidth(250);
        QRegExp           regExp("[A-Za-z][A-Za-z0-9_]*");
        QRegExpValidator* validator = new QRegExpValidator(regExp, edit);
        edit->setValidator(validator);
        other_name_edit_list.append(edit);
        ui->treeWidget_condi->setItemWidget(item, 1, edit);
        ai_item.append(item);
        item->setToolTip(0, di_resource[i + 8] + "是" + ai_resource[i] + "的复用，二者仅可选其一");
    }
    /* PI */
    QTreeWidgetItem* topItem_pi = new QTreeWidgetItem(ui->treeWidget_condi);
    topItem_pi->setText(0, "PI");
    topItem_pi->setCheckState(0, Qt::Unchecked);
    ui->treeWidget_condi->addTopLevelItem(topItem_pi);
    for (uint8_t i = 0; i < pi_resource.count(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(topItem_pi);
        item->setText(0, pi_resource[i]);
        item->setCheckState(0, Qt::Unchecked);
        QLineEdit* edit = new QLineEdit;
        edit->setMaxLength(25);
        edit->setMinimumWidth(40);
        edit->setMaximumWidth(250);
        QRegExp           regExp("[A-Za-z][A-Za-z0-9_]*");
        QRegExpValidator* validator = new QRegExpValidator(regExp, edit);
        edit->setValidator(validator);
        other_name_edit_list.append(edit);
        ui->treeWidget_condi->setItemWidget(item, 1, edit);
        pi_item.append(item);
        item->setToolTip(0, di_resource[i + 10] + "是" + pi_resource[i] + "的复用，二者仅可选其一");
    }
    /* QEP */
    QTreeWidgetItem* topItem_qep = new QTreeWidgetItem(ui->treeWidget_condi);
    topItem_qep->setText(0, "QEP");
    topItem_qep->setCheckState(0, Qt::Unchecked);
    ui->treeWidget_condi->addTopLevelItem(topItem_qep);

    for (uint8_t i = 0; i < qep_resource.count(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(topItem_qep);
        item->setText(0, qep_resource[i]);
        item->setCheckState(0, Qt::Unchecked);
        QLineEdit* edit = new QLineEdit;
        edit->setMaxLength(25);
        edit->setMinimumWidth(40);
        edit->setMaximumWidth(250);
        QRegExp           regExp("[A-Za-z][A-Za-z0-9_]*");
        QRegExpValidator* validator = new QRegExpValidator(regExp, edit);
        edit->setValidator(validator);
        other_name_edit_list.append(edit);
        ui->treeWidget_condi->setItemWidget(item, 1, edit);
        qep_item.append(item);
    }
    /* 根据默认安全参数复用情况来设置 */
    pi_item[TOOL_ID_PI1]->setDisabled(true);
    pi_item[TOOL_ID_PI2]->setDisabled(true);
    di_item[TOOL_ID_DI11]->setDisabled(true);
    di_item[TOOL_ID_DI12]->setDisabled(true);
    ai_item[TOOL_ID_AI1]->setDisabled(true);
    ai_item[TOOL_ID_AI2]->setDisabled(true);
    QObject::connect(ui->treeWidget_condi, &QTreeWidget::itemChanged, [&](QTreeWidgetItem* item, int column) {
        if (column == 0 && item != nullptr && item->parent() == nullptr) {  // 仅处理顶级项
            Qt::CheckState state = item->checkState(0);
            for (int i = 0; i < item->childCount(); ++i) {
                QTreeWidgetItem* childItem = item->child(i);
                if (childItem->isDisabled() == false) {
                    childItem->setCheckState(0, state);
                }
            }
        }
    });
}

/**
 * @brief 获取条件别名
 * @param type 条件类型
 * @param id 条件ID
 * @return 条件别名
 */
QString condition_view::condition_get_name(tool_type_e type, tool_id_e id)
{
    int num[4] = { INPUT_DI_RESOURCE_START, INPUT_AI_RESOURCE_START, INPUT_PI_RESOURCE_START,
                   INPUT_QEP_RESOURCE_START };
    if (type <= TOOL_TYPE_CONDI_QEP) {
        return (input_resource[type][id] + "_" + other_name_edit_list[num[type] + id]->text());
    } else {
        return (input_resource[TOOL_TYPE_CONDI_QEP + 1 + type - TOOL_TYPE_CONDI_BOOL][id]);
    }
}

/**
 * @brief 条件视图复位
 */
void condition_view::condition_view_reset()
{
    ui->treeWidget_condi->blockSignals(true);                           //禁用信号发送
    int topLevelItemCount = ui->treeWidget_condi->topLevelItemCount();  //获取顶层列表数量
    for (int i = 0; i < topLevelItemCount; i++) {
        QTreeWidgetItem* item = ui->treeWidget_condi->topLevelItem(i);
        item->setCheckState(0, Qt::Unchecked);
        int childCount = item->childCount();
        for (int j = 0; j < childCount; j++) {  //每个子项不选中
            QTreeWidgetItem* childItem = item->child(j);
            childItem->setCheckState(0, Qt::Unchecked);
            childItem->setDisabled(false);
        }
        item->setExpanded(false);
    }
    pi_item[TOOL_ID_PI1]->setDisabled(true);
    pi_item[TOOL_ID_PI2]->setDisabled(true);
    di_item[TOOL_ID_DI11]->setDisabled(true);
    di_item[TOOL_ID_DI12]->setDisabled(true);
    ai_item[TOOL_ID_AI1]->setDisabled(true);
    ai_item[TOOL_ID_AI2]->setDisabled(true);
    for (int i = 0; i < other_name_edit_list.count(); i++) {
        other_name_edit_list[i]->clear();  //清空别名
    }
    ui->treeWidget_condi->blockSignals(false);  //恢复信号发送
    /* reset ss table */
    for (int i = 0; i < ui->tableWidget_ss->rowCount(); i++) {
        ss_table_delete_item_combo(i);
    }
    ss_info_list.clear();
    ss_code = 0x20;
    ui->tableWidget_ss->clearContents();
    ui->tableWidget_ss->setRowCount(0);
    ss_tabel_add_item(0xFF, mainwindow->config_view_class->config_photo_svg->param_ss_get() >> 1);
}

/**
 * @brief 生成工程信息
 * @return 工程信息
 */
QJsonObject condition_view::condition_view_project_info()
{
    QJsonObject rootObject;

    /* input device resource */
    QJsonArray other_name;
    QJsonArray check_state;
    for (int i = 0; i < other_name_edit_list.count(); i++) {
        other_name.append(other_name_edit_list[i]->text());
    }
    rootObject[project_device_iothername] = other_name;
    int topLevelItemCount                 = ui->treeWidget_condi->topLevelItemCount();
    for (int i = 0; i < topLevelItemCount; i++) {
        QTreeWidgetItem* item = ui->treeWidget_condi->topLevelItem(i);
        if (item->checkState(0) == Qt::Unchecked) {
            check_state.append(0);
        } else {
            check_state.append(1);
        }
        int childCount = item->childCount();
        for (int j = 0; j < childCount; j++) {
            QTreeWidgetItem* childItem = item->child(j);
            if (childItem->checkState(0) == Qt::Unchecked) {
                check_state.append(0);
            } else {
                check_state.append(1);
            }
        }
        item->setExpanded(false);
    }
    rootObject[project_device_icheckstate] = check_state;
    /* output device resource */
    QJsonObject outputObject;
    outputObject["ss_number"] = ss_info_list.count() - 1;
    for (int i = 1; i < ss_info_list.count(); i++) {
        QJsonObject ssObject;
        ssObject["code"]                            = ss_info_list[i].ss_code;
        ssObject["relevant"]                        = ss_info_list[i].relevant_state;
        outputObject["ss" + QString::number(i - 1)] = ssObject;
    }
    rootObject[project_device_ossinfo] = outputObject;
    return rootObject;
}

/**
 * @brief 工程信息解析
 * @param project 工程信息
 * @return 解析结果
 */
bool condition_view::condition_view_project_parse(QJsonObject project)
{
#define OTHER_NAME_NUMBER  (20)
#define CHECK_STATE_NUMBER (24)
    if (project.isEmpty()) {
        return false;
    }
    /* read and set input resource */
    QJsonArray  other_name  = project[project_device_iothername].toArray();
    QJsonArray  check_state = project[project_device_icheckstate].toArray();
    QList<int>  check_state_array;
    QStringList other_name_array;

    if (other_name.count() != OTHER_NAME_NUMBER) {
        return false;
    }
    if (check_state.count() != CHECK_STATE_NUMBER) {
        return false;
    }

    for (const QJsonValue& value : check_state) {
        if (value.isDouble()) {
            check_state_array.append(value.toInt());
        } else {
            check_state_array.append(0);
        }
    }
    for (const QJsonValue& value : other_name) {
        if (value.isString()) {
            other_name_array.append(value.toString());
        } else {
            other_name_array.append("");
        }
    }
    int     topLevelItemCount = ui->treeWidget_condi->topLevelItemCount();  //获取顶层列表数量
    uint8_t othername_cnt     = 0;
    uint8_t checkstate_cnt    = 0;
    for (int i = 0; i < topLevelItemCount; i++) {
        QTreeWidgetItem* item = ui->treeWidget_condi->topLevelItem(i);
        if (check_state_array[checkstate_cnt++] == 0) {
            item->setCheckState(0, Qt::Unchecked);
        } else {
            item->setCheckState(0, Qt::Checked);
        }
        int childCount = item->childCount();
        for (int j = 0; j < childCount; j++) {  //每个子项不选中
            other_name_edit_list[othername_cnt]->setText(other_name_array[othername_cnt]);
            othername_cnt++;
            QTreeWidgetItem* childItem = item->child(j);
            childItem->setCheckState(0, Qt::Unchecked);
            if (check_state_array[checkstate_cnt++] == 0) {
                childItem->setCheckState(0, Qt::Unchecked);
            } else {
                childItem->setCheckState(0, Qt::Checked);
            }
        }
        item->setExpanded(false);
    }
    /* read and set ss */
    QJsonObject ss_info = project[project_device_ossinfo].toObject();
    int         ss_num  = ss_info["ss_number"].toInt();
    if (ss_num > 0) {
        for (int i = 0; i < ss_num; i++) {
            QJsonObject ssObject;
            ssObject     = ss_info["ss" + QString::number(i)].toObject();
            int code     = ssObject["code"].toInt();
            int relevant = ssObject["relevant"].toInt();
            ss_tabel_add_item(code, relevant);
        }
    }
    return true;
}

/**
 * @brief ss新增项目
 * @param code ss编码
 * @param relevant 相关设置
 */
void condition_view::ss_tabel_add_item(uint8_t code, uint8_t relevant)
{
    int row = ui->tableWidget_ss->rowCount();
    ui->tableWidget_ss->insertRow(row);

    QString styleSheet = "QComboBox {"
                         "    background-color: #E6E6FA;"
                         "    border: 1px solid #9370DB;"
                         "    padding: 2px;"
                         "    color: #000000;"
                         "}";
    /* 增加下拉框 */
    for (uint8_t i = 0; i < 6; i++) {
        QComboBox* comboBox = new QComboBox;
        comboBox->addItem("not_relevant");
        comboBox->addItem("relevant");
        comboBox->setStyleSheet(styleSheet);
        comboBox->setCurrentIndex(((relevant >> i) & 0x01));
        QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ss_table_combobox_change(int)));
        ui->tableWidget_ss->setCellWidget(row, i + 1, comboBox);
    }
    ss_info_t ss_info;
    if ((code >= SS_NUM_START && code < SS_NUM_START + MAX_SS_NUM) || code == DEFAULT_SS_CODE) {
        ss_info.relevant_state = relevant;
        ss_info.ss_code        = code;
    } else {
        /* ss code循环利用 */
        for (uint8_t i = SS_NUM_START; i < SS_NUM_START + MAX_SS_NUM; i++) {
            bool is_used = false;
            for (uint8_t j = 0; j < ss_info_list.count(); j++) {
                if (ss_info_list[j].ss_code == i) {
                    is_used = true;
                    break;
                }
            }
            if (is_used == false) {
                ss_info.relevant_state = relevant;
                ss_info.ss_code        = i;
                break;
            }
        }
    }
    QTableWidgetItem* item = new QTableWidgetItem("0x" + QString::number(ss_info.ss_code, 16));
    item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_ss->setItem(row, 0, item);
    ss_info_list.append(ss_info);
}

/**
 * @brief 默认的ss设置相关状态
 * @param state
 */
void condition_view::ss_default_set_state(uint8_t state)
{
    for (uint8_t col = 1; col < 7; col++) {
        QWidget* widget = ui->tableWidget_ss->cellWidget(0, col);
        if (widget != nullptr) {
            QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
            if (comboBox) {
                int id = ((state >> (col)) & (0x01));
                comboBox->setCurrentIndex(id);
            }
        }
    }
}

/**
 * @brief 右键菜单删除ss所在行的所有下拉框
 * @param row 删除行标
 */
void condition_view::ss_table_delete_item_combo(int row)
{
    for (uint8_t col = 1; col < 7; col++) {
        QWidget* widget = ui->tableWidget_ss->cellWidget(row, col);
        if (widget != nullptr) {
            QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
            if (comboBox != nullptr) {
                delete comboBox;
            }
        }
    }
    ui->tableWidget_ss->removeRow(row);
    ss_info_list.removeAt(row);
}

/* user slots */

/**
 * @brief 定时更新输入条件的别名-槽函数
 */
void condition_view::condition_name_update_slot()
{
    int num[4] = { INPUT_DI_RESOURCE_START, INPUT_AI_RESOURCE_START, INPUT_PI_RESOURCE_START,
                   INPUT_QEP_RESOURCE_START };
    for (int i = 0; i < INPUT_DI_RESOURCE_NUM; i++) {
        if (mainwindow->logic_tools_class->di_tools_list[i]->text()
            != (input_resource[TOOL_TYPE_CONDI_DI][i] + "_"
                + other_name_edit_list[num[TOOL_TYPE_CONDI_DI] + i]->text())) {
            mainwindow->logic_tools_class->di_tools_list[i]->set_name(
                (input_resource[TOOL_TYPE_CONDI_DI][i] + "_"
                 + other_name_edit_list[num[TOOL_TYPE_CONDI_DI] + i]->text()));
        }
    }
    for (int i = 0; i < INPUT_AI_RESOURCE_NUM; i++) {
        if (mainwindow->logic_tools_class->ai_tools_list[i]->text()
            != (input_resource[TOOL_TYPE_CONDI_AI][i] + "_"
                + other_name_edit_list[num[TOOL_TYPE_CONDI_AI] + i]->text())) {
            mainwindow->logic_tools_class->ai_tools_list[i]->set_name(
                (input_resource[TOOL_TYPE_CONDI_AI][i] + "_"
                 + other_name_edit_list[num[TOOL_TYPE_CONDI_AI] + i]->text()));
        }
    }
    for (int i = 0; i < INPUT_PI_RESOURCE_NUM; i++) {
        if (mainwindow->logic_tools_class->pi_tools_list[i]->text()
            != (input_resource[TOOL_TYPE_CONDI_PI][i] + "_"
                + other_name_edit_list[num[TOOL_TYPE_CONDI_PI] + i]->text())) {
            mainwindow->logic_tools_class->pi_tools_list[i]->set_name(
                (input_resource[TOOL_TYPE_CONDI_PI][i] + "_"
                 + other_name_edit_list[num[TOOL_TYPE_CONDI_PI] + i]->text()));
        }
    }
    for (int i = 0; i < INPUT_QEP_RESOURCE_NUM; i++) {
        if (mainwindow->logic_tools_class->qep_tools_list[i]->text()
            != (input_resource[TOOL_TYPE_CONDI_QEP][i] + "_"
                + other_name_edit_list[num[TOOL_TYPE_CONDI_QEP] + i]->text())) {
            mainwindow->logic_tools_class->qep_tools_list[i]->set_name(
                (input_resource[TOOL_TYPE_CONDI_QEP][i] + "_"
                 + other_name_edit_list[num[TOOL_TYPE_CONDI_QEP] + i]->text()));
        }
    }
}

/**
 * @brief 右键菜单新增ss - 槽函数
 */
void condition_view::ss_table_add_item_slot()
{
    if (ui->tableWidget_ss->rowCount() >= MAX_SS_NUM) {
        mainwindow->my_message_box("Creat ss fail", "ss 数量已达上限值", false);
    } else {
        ss_tabel_add_item(0, 0);
    }
}

/**
 * @brief 右键菜单删除ss-槽函数
 */
void condition_view::ss_table_delete_item_slot()
{
    if (ui->tableWidget_ss->currentRow() == 0) {
        mainwindow->my_message_box("不可删除", "默认的ss不可被删除", false);
        return;
    }
    ss_table_delete_item_combo(ui->tableWidget_ss->currentRow());
}

/**
 * @brief ss列表右键菜单-槽函数
 * @param pos 未使用
 */
void condition_view::ss_table_right_menu_slot(const QPoint& pos)
{
    Q_UNUSED(pos);
    QMenu    menu(ui->tableWidget_ss);
    QAction* action_add   = menu.addAction("新增");
    QAction* actiondelete = menu.addAction("删除");
    action_add->setIcon(QIcon(":/new/photo/photo/additem.png"));
    actiondelete->setIcon(QIcon(":/new/photo/photo/deleteitem.png"));
    connect(action_add, &QAction::triggered, this, ss_table_add_item_slot);
    connect(actiondelete, &QAction::triggered, this, ss_table_delete_item_slot);
    menu.exec(QCursor::pos());
}

/**
 * @brief ss列表的下拉框更改事件
 * @param index 未使用
 */
void condition_view::ss_table_combobox_change(int index)
{
    index               = index;
    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
    if (comboBox) {
        QModelIndex mindex = ui->tableWidget_ss->indexAt(comboBox->pos());
        int         row    = mindex.row();
        int         column = mindex.column();
        for (int i = 1; i < 7; i++) {
            QWidget* widget = ui->tableWidget_ss->cellWidget(row, i);
            if (widget != nullptr) {
                QComboBox* comboBox1 = qobject_cast<QComboBox*>(widget);
                if (comboBox1 == comboBox) {
                    column = i;
                    break;
                }
            }
        }
        if (comboBox->currentIndex() == SS_NOT_RELEVANT) {
            ss_info_list[row].relevant_state &= (~(0x01 << (column - 1)));
        } else {
            ss_info_list[row].relevant_state |= (0x01 << (column - 1));
        }
        if (row == 0) {
            mainwindow->config_view_class->config_photo_svg->param_ss_set(ss_info_list[row].relevant_state);
        }
    }
}
