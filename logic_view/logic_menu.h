#ifndef LOGIC_MENU_H
#define LOGIC_MENU_H
#include "graphical_config/graphical_config_param.h"
#include "logic_element.h"
#include <QTreeWidget>

class MainWindow;
class logic_menu : public QTreeWidget {
    Q_OBJECT
public:
    logic_menu(QWidget* parent = 0);

private:
    void init(void);
    void function_item_init(void);

public:
    logic_element* item_exist_detect(config_block_data_t data);
    logic_element* get_function_item(config_block_data_t data);

private:
    QTreeWidgetItem*      input_menu;
    QTreeWidgetItem*      function_menu;
    QTreeWidgetItem*      output_menu;
    QList<logic_element*> input_item_list;
    QList<logic_element*> function_item_list;
    QList<logic_element*> output_item_list;
    void                  startDrag(Qt::DropActions supportedActions) override;

signals:

public slots:
    void add_item_from_config_slot(config_block_data_t data);
    void remove_item_from_config_slot(config_block_data_t data);
    void name_change_slot(config_block_data_t data);
};

#endif  // LOGIC_MENU_H
