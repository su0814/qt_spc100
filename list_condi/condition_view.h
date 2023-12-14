#ifndef CONDITION_VIEW_H
#define CONDITION_VIEW_H
#include "config/data_config.h"
#include "config/param_config.h"
#include "ui_mainwindow.h"
#include <QJsonObject>
#include <QObject>
#include <QTimer>
#include <QWidget>
class MainWindow;
class condition_view : public QWidget {
    Q_OBJECT
public:
    condition_view(QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

protected:
    /* about ss */
    uint8_t ss_code = 0x20;

public:
    QTimer            update_tim;
    QList<ss_info_t>  ss_info_list;
    QList<QLineEdit*> other_name_edit_list;
    void              condition_view_reset(void);
    QJsonObject       condition_view_project_info(void);
    bool              condition_view_project_parse(QJsonObject project);
    QString           condition_get_name(tool_type_e type, tool_id_e id);

protected:
    void condition_tree_init(void);
    /* ss相关 */
    void ss_table_init(void);
    void ss_table_delete_item_combo(int row);

    void ss_tabel_add_item(uint8_t code, uint8_t relevant);
public slots:
    /* ss相关 */
    void ss_table_add_item_slot(void);
    void ss_table_delete_item_slot(void);
    void ss_table_right_menu_slot(const QPoint& pos);
    void ss_table_combobox_change(int index);
    void condition_name_update_slot(void);
};

#endif  // CONDITION_VIEW_H
