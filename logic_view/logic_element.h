#ifndef LOGIC_ELEMENT_H
#define LOGIC_ELEMENT_H
#include "graphical_config/config_param.h"
#include "graphical_config/graphical_config_param.h"
#include <QTreeWidget>

class logic_element : public QTreeWidgetItem {
public:
    logic_element(config_block_data_t data, QTreeWidgetItem* parent = nullptr);

private:
    config_block_data_t config_block_data;

public:
    bool                 is_exist(config_block_data_t data);
    void                 set_config_data(config_block_data_t data);
    config_block_data_t* get_config_data(void);
};

#endif  // LOGIC_ELEMENT_H
