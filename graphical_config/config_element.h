#ifndef CONFIG_ELEMENT_H
#define CONFIG_ELEMENT_H
#include "graphical_config_param.h"
#include <QJsonObject>
#include <QPainter>
#include <QTreeWidget>

class config_element : public QWidget, public QTreeWidgetItem {
public:
    config_element(QJsonObject object, QTreeWidget* parent = nullptr);
    config_element(QJsonObject object, QTreeWidgetItem* parent = nullptr);

public:
    element_data_t* config_data(void);

private:
    element_data_t element_data;
    void           json_config_prase(QJsonObject object);
};

#endif  // CONFIG_ELEMENT_H
