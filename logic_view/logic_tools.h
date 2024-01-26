#ifndef LOGIC_TOOLS_H
#define LOGIC_TOOLS_H

#include "config/data_config.h"
#include "tool_block.h"
#include "ui_mainwindow.h"
#include <QWidget>
enum {
    TOOL_DI_ITEM_NUM = 0,
    TOOL_AI_ITEM_NUM,
    TOOL_PI_ITEM_NUM,
    TOOL_QEP_ITEM_NUM,
};

class MainWindow;
class logic_tools : public QWidget {
    Q_OBJECT
public:
    explicit logic_tools(QWidget* mparent, QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

protected:
    void tools_ui_init(void);

public:
    QList<tool_block*> di_tools_list;
    QList<tool_block*> ai_tools_list;
    QList<tool_block*> pi_tools_list;
    QList<tool_block*> qep_tools_list;
    QList<tool_block*> logic_tools_list;

public:
    void logic_tools_reset(void);

signals:

public slots:
    void condi_tree_changed(QTreeWidgetItem* item, int column);
};

#endif  // LOGIC_TOOLS_H
