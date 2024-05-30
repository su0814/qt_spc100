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
    void           set_output_repeat_disable(int id, bool state);
    void           set_input_repeat_disable(int id, bool state);
    void           set_output_repeat_name(int id, QString name);
    void           set_input_repeat_name(int id, QString name);
    bool           input_repeater_is_disable(int id);
    void           logic_menu_reset(void);

private:
    QTreeWidgetItem*      input_menu;
    QTreeWidgetItem*      function_menu;
    QTreeWidgetItem*      output_menu;
    QTreeWidgetItem*      output_repeater;
    QTreeWidgetItem*      output_spc100;
    QTreeWidgetItem*      input_repeater;
    QTreeWidgetItem*      input_spc100;
    QTreeWidgetItem*      input_const;
    QList<logic_element*> input_item_list;
    QList<logic_element*> input_const_item_list;
    QList<logic_element*> function_item_list;
    QList<logic_element*> output_item_list;
    QList<logic_element*> input_repeater_item_list;
    QList<logic_element*> output_repeater_item_list;
    void                  startDrag(Qt::DropActions supportedActions) override;

signals:

public slots:
    void add_item_from_config_slot(config_block_data_t data);
    void remove_item_from_config_slot(config_block_data_t data);
    void name_change_slot(config_block_data_t data);
};

#endif  // LOGIC_MENU_H
