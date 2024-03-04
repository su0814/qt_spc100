#include "coroutine_lua.h"
#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>
/**
 * @brief lua 线程类构造函数
 * @param mparent
 * @param parent
 */
coroutine_lua::coroutine_lua(QWidget* mparent, QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )mparent;
    coroutine_ui_init();
}

/**
 * @brief 初始化
 */
void coroutine_lua::coroutine_ui_init()
{
    ui->listWidget_coroutine->setContextMenuPolicy(Qt::CustomContextMenu);  //自定义右键菜单
    connect(ui->listWidget_coroutine, &QListWidget::itemChanged, this, coroutine_item_changeed);
    connect(ui->listWidget_coroutine, &QListWidget::itemClicked, this, coroutine_item_clicked);
    connect(ui->listWidget_coroutine, &QListWidget::customContextMenuRequested, this, coroutine_right_menu);
    connect(ui->listWidget_coroutine, &QListWidget::itemDoubleClicked, this, coroutine_item_edit);
    ui->listWidget_coroutine->setCurrentRow(0);
    connect(ui->textEdit_coroutine, &QPlainTextEdit::textChanged, this, coroutine_text_max_check_slot);
    ui->textEdit_coroutine->setEnabled(false);
}

/**
 * @brief 复位
 */
void coroutine_lua::coroutine_lua_reset()
{
    ui->listWidget_coroutine->clear();
    ui->textEdit_coroutine->clear();
    old_name.clear();
    coroutine_code.clear();
    coroutine_name.clear();
    ui->textEdit_coroutine->setEnabled(false);
}

/**
 * @brief 创建线程
 */
void coroutine_lua::coroutine_creat()
{
    QString name;
    if (ui->listWidget_coroutine->count() >= MAX_COROUTINE_NUM) {
        mainwindow->my_message_box("创建失败", "无法创建更多线程", false);
        return;
    }
    /* 线程名称数字尾缀循环使用 */
    for (uint8_t i = 0; i < MAX_COROUTINE_NUM; i++) {
        name = "coroutine" + QString::number(i + 1);
        if (coroutine_name.contains(name)) {
            continue;
        } else {
            break;
        }
    }

    QListWidgetItem* item = new QListWidgetItem(name);
    item->setFlags(item->flags() | Qt::ItemIsEditable);  //允许编辑
    item->setCheckState(Qt::Checked);
    ui->listWidget_coroutine->addItem(item);
    old_name.append(item->text());  //曾用名记录
    coroutine_code.append("");
    coroutine_name.append(item->text());                                             //记录当前名
    ui->listWidget_coroutine->setCurrentRow(ui->listWidget_coroutine->count() - 1);  //设置当前选中行
    ui->textEdit_coroutine->clear();
    ui->textEdit_coroutine->setEnabled(true);  //清空并使能线程编辑框
}

/**
 * @brief 线程删除
 */
void coroutine_lua::coroutine_delete()
{
    int              current_index = ui->listWidget_coroutine->currentRow();
    QListWidgetItem* currentItem   = ui->listWidget_coroutine->takeItem(current_index);
    delete currentItem;
    old_name.removeAt(current_index);  //曾用名列表移除
    coroutine_code.removeAt(current_index);
    coroutine_name.removeAt(current_index);
    /* 如果列表内还有其他线程，就显示其他线程的内容 */
    int new_index = ui->listWidget_coroutine->currentRow();
    if (new_index >= 0 && coroutine_code.count() > new_index) {
        ui->textEdit_coroutine->setPlainText(coroutine_code[new_index]);
    }
    /* 如果列表无线程，清空并禁止编辑线程编辑框 */
    if (ui->listWidget_coroutine->count() == 0) {
        ui->textEdit_coroutine->setEnabled(false);
        ui->textEdit_coroutine->clear();
    }
}

/**
 * @brief 生成线程的工程信息
 * @return 工程信息
 */
QJsonObject coroutine_lua::coroutine_lua_project_info()
{
    QJsonObject rootObject;
    for (uint8_t i = 0; i < ui->listWidget_coroutine->count(); i++) {
        QJsonObject      corObject;
        QListWidgetItem* item1 = ui->listWidget_coroutine->item(i);
        corObject["name"]      = item1->text();
        if (item1->checkState() == Qt::Unchecked) {
            corObject["checkstate"] = 0;
        } else {
            corObject["checkstate"] = 1;
        }
        corObject["code"]                            = coroutine_code[i];
        rootObject["coroutine" + QString::number(i)] = corObject;
    }
    rootObject["number"] = ui->listWidget_coroutine->count();
    return rootObject;
}

/**
 * @brief 解析工程信息，重建线程列表
 * @param project 工程信息
 * @return
 */
bool coroutine_lua::coroutine_lua_project_parse(QJsonObject project)
{
    int num = project["number"].toInt();
    for (int i = 0; i < num; i++) {
        if (!project[project_object_coroutine + QString::number(i)].isObject()) {
            continue;
        }
        QJsonObject corobject = project[project_object_coroutine + QString::number(i)].toObject();
        coroutine_creat();
        ui->listWidget_coroutine->item(i)->setText(corobject["name"].toString());
        if (corobject["checkstate"].toInt() == 0) {
            ui->listWidget_coroutine->item(i)->setCheckState(Qt::Unchecked);
        }
        coroutine_code[i] = corobject["code"].toString();
        ui->textEdit_coroutine->setPlainText(coroutine_code[i]);
    }
    return true;
}

/* user slots */

/**
 * @brief 线程重命名后的回调函数-槽函数
 * @param item 项目
 */
void coroutine_lua::coroutine_item_changeed(QListWidgetItem* item)
{
    int     row      = ui->listWidget_coroutine->row(item);
    QString new_name = item->text();
    for (int i = 0; i < ui->listWidget_coroutine->count(); i++) {
        if (ui->listWidget_coroutine->item(i) == item) {
            continue;
        }
        if (ui->listWidget_coroutine->item(i)->text() == new_name) {
            item->setText(old_name[row]);
            mainwindow->my_message_box("命名重复", "当前已有其他线程使用此名称", false);
            return;
        }
    }
    coroutine_name[row] = new_name;
}

/**
 * @brief 线程点击事件
 * @param item 被点击的项目
 */
void coroutine_lua::coroutine_item_clicked(QListWidgetItem* item)
{
    int row = ui->listWidget_coroutine->row(item);
    ui->textEdit_coroutine->setPlainText(coroutine_code[row]);
}

/**
 * @brief 右键菜单
 * @param pos
 */
void coroutine_lua::coroutine_right_menu(const QPoint& pos)
{
    QMenu    menu(ui->listWidget_coroutine);
    QAction* action_add    = menu.addAction("新增");
    QAction* action_delete = menu.addAction("删除");
    action_add->setIcon(QIcon(":/new/photo/photo/additem.png"));
    action_delete->setIcon(QIcon(":/new/photo/photo/deleteitem.png"));
    QAction* selectedAction = menu.exec(ui->listWidget_coroutine->mapToGlobal(pos));
    if (selectedAction == action_add) {
        coroutine_creat();
    } else if (selectedAction == action_delete) {
        coroutine_delete();
    }
}

/**
 * @brief 线程名称编辑-槽函数
 * @param item
 */
void coroutine_lua::coroutine_item_edit(QListWidgetItem* item)
{
    ui->listWidget_coroutine->editItem(item);
    QLineEdit* editor = ui->listWidget_coroutine->itemWidget(item) ?
                            qobject_cast<QLineEdit*>(ui->listWidget_coroutine->itemWidget(item)) :
                            new QLineEdit();
    editor->setMaxLength(20);
    QRegExp regex("^[A-Za-z][A-Za-z0-9_]*$");
    editor->setValidator(new QRegExpValidator(regex, editor));
}

/**
 * @brief 线程最大字符两检测互调函数-槽函数
 */
void coroutine_lua::coroutine_text_max_check_slot()
{
    QString cur_text = ui->textEdit_coroutine->toPlainText();
    int     length   = cur_text.count();
    if (length > COROUTINE_CODE_MAX_LENGTH) {
        int         position   = ui->textEdit_coroutine->textCursor().position();
        QTextCursor textCursor = ui->textEdit_coroutine->textCursor();
        cur_text.truncate(COROUTINE_CODE_MAX_LENGTH);
        ui->textEdit_coroutine->setPlainText(cur_text);
        textCursor.setPosition(position - (length - COROUTINE_CODE_MAX_LENGTH));
        ui->textEdit_coroutine->setTextCursor(textCursor);
    } else {
        int new_index = ui->listWidget_coroutine->currentRow();
        if (new_index >= 0 && coroutine_code.count() > new_index) {
            coroutine_code[new_index] = cur_text;
        }
    }
}
