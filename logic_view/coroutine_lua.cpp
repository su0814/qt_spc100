#include "coroutine_lua.h"
#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>
coroutine_lua::coroutine_lua(QWidget* parent)
    : QWidget(parent)
{
    ui         = MainWindow::my_ui->ui;
    mainwindow = ( MainWindow* )parent;
    coroutine_ui_init();
}

void coroutine_lua::coroutine_ui_init()
{
    ui->listWidget_coroutine->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget_coroutine, &QListWidget::itemChanged, this, coroutine_item_changeed);
    connect(ui->listWidget_coroutine, &QListWidget::itemClicked, this, coroutine_item_clicked);
    connect(ui->listWidget_coroutine, &QListWidget::customContextMenuRequested, this, coroutine_right_menu);
    connect(ui->listWidget_coroutine, &QListWidget::itemDoubleClicked, this, coroutine_item_edit);
    ui->listWidget_coroutine->setCurrentRow(0);
    connect(ui->textEdit_coroutine, &QPlainTextEdit::textChanged, this, coroutine_text_max_check_slot);
}

void coroutine_lua::coroutine_lua_reset()
{
    ui->listWidget_coroutine->clear();
    ui->textEdit_coroutine->clear();
    old_name.clear();
    coroutine_code.clear();
    coroutine_id            = 1;
    current_coroutine_index = 0;
}

void coroutine_lua::coroutine_creat()
{
    if (ui->listWidget_coroutine->count() >= MAX_COROUTINE_NUM) {
        mainwindow->my_message_box("Creat fail", "Unable to create more coroutine", false);
        return;
    }
    QListWidgetItem* item = new QListWidgetItem("coroutine" + QString::number(coroutine_id));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setCheckState(Qt::Checked);
    ui->listWidget_coroutine->addItem(item);
    old_name.append(item->text());
    coroutine_code.append("");
    coroutine_name.append(item->text());
    coroutine_id++;
    if (ui->listWidget_coroutine->count() > 1) {
        coroutine_code[ui->listWidget_coroutine->currentRow()] = ui->textEdit_coroutine->toPlainText();
    }
    ui->listWidget_coroutine->setCurrentRow(ui->listWidget_coroutine->count() - 1);
    ui->textEdit_coroutine->clear();
    current_coroutine_index = ui->listWidget_coroutine->count() - 1;
}

void coroutine_lua::coroutine_delete()
{
    int              current_index = ui->listWidget_coroutine->currentRow();
    QListWidgetItem* currentItem   = ui->listWidget_coroutine->takeItem(current_index);
    delete currentItem;
    old_name.removeAt(current_index);
    coroutine_code.removeAt(current_index);
    coroutine_name.removeAt(current_index);
    int new_index = ui->listWidget_coroutine->currentRow();
    ui->textEdit_coroutine->setPlainText(coroutine_code[new_index]);
    current_coroutine_index = new_index;
}

QJsonObject coroutine_lua::coroutine_lua_project_info()
{
    if (ui->listWidget_coroutine->count() > 0) {
        coroutine_code[current_coroutine_index] = ui->textEdit_coroutine->toPlainText();
    }
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
    rootObject["number"]      = ui->listWidget_coroutine->count();
    rootObject["coroutineid"] = ( int )coroutine_id;
    return rootObject;
}

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
    coroutine_id = project["coroutineid"].toInt();
    return true;
}

/* user slots */

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
            mainwindow->my_message_box("Duplicate Name", "Each coroutine must have a unique name.", false);
            return;
        }
    }
    coroutine_name[row] = new_name;
}

void coroutine_lua::coroutine_item_clicked(QListWidgetItem* item)
{
    int row = ui->listWidget_coroutine->row(item);
    if (row == current_coroutine_index) {
        return;
    }
    coroutine_code[current_coroutine_index] = ui->textEdit_coroutine->toPlainText();
    ui->textEdit_coroutine->clear();
    ui->textEdit_coroutine->setPlainText(coroutine_code[row]);
    current_coroutine_index = row;
}

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
    }
}
