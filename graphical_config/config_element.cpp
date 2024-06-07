#include "config_element.h"
#include <QDebug>

config_element::config_element(QJsonObject object, QTreeWidget* parent)
    : QTreeWidgetItem(parent)
{
    json_config_prase(object);
}

config_element::config_element(QJsonObject object, QTreeWidgetItem* parent)
    : QTreeWidgetItem(parent)
{
    json_config_prase(object);
}

element_data_t* config_element::config_data()
{
    return (&element_data);
}

void config_element::json_config_prase(QJsonObject object)
{
    int type          = object["type"].toInt();
    element_data.type = type;
    switch (type) {
    case KEY_TYPE_ELEMENT:
        element_data.model_iotype = object["modeliotype"].toInt();
        element_data.model_type   = object["modeltype"].toInt();
        element_data.cat          = object["cat"].toInt();
        element_data.name         = object["name"].toString();
        element_data.normal_state = object["ns"].toInt();
        element_data.pixmap       = object["pixmap"].toString();
        setIcon(0, QIcon(QPixmap(element_data.pixmap)));
        this->setSizeHint(0, QSize(50, 50));
        setData(0, Qt::UserRole, "element");
        break;
    }
}
