#ifndef COROUTINE_LUA_H
#define COROUTINE_LUA_H

#include "ui_mainwindow.h"
#include <QJsonObject>
#include <QWidget>

class MainWindow;
class coroutine_lua : public QWidget {
    Q_OBJECT
public:
    explicit coroutine_lua(QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

private:
    QStringList old_name;
    uint32_t    coroutine_id = 1;

private:
    void coroutine_ui_init(void);

public:
    QStringList coroutine_code;
    QStringList coroutine_name;

public:
    void        coroutine_creat(void);
    void        coroutine_delete(void);
    void        coroutine_lua_reset(void);
    QJsonObject coroutine_lua_project_info(void);
    bool        coroutine_lua_project_parse(QJsonObject project);
signals:

public slots:
    void coroutine_item_changeed(QListWidgetItem* item);
    void coroutine_item_clicked(QListWidgetItem* item);
    void coroutine_item_edit(QListWidgetItem* item);
    void coroutine_right_menu(const QPoint& pos);
    void coroutine_text_max_check_slot(void);
};

#endif  // COROUTINE_LUA_H
