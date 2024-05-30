#include "logic_element.h"
#include <QDebug>

logic_element::logic_element(config_block_data_t data, QTreeWidgetItem* parent)
    : QTreeWidgetItem(parent)
{
    set_config_data(data);
    setIcon(0, QIcon(data.pixmap));
    this->setSizeHint(0, QSize(50, 50));
    setData(0, Qt::UserRole, "element");
}

bool logic_element::is_exist(config_block_data_t data)
{
    if ((data.config_param_data.model_iotype == config_block_data.config_param_data.model_iotype)
        && (data.config_param_data.model_type == config_block_data.config_param_data.model_type)
        && (data.config_param_data.model_id == config_block_data.config_param_data.model_id)
        && (data.config_param_data.source_mcu == config_block_data.config_param_data.source_mcu)) {
        return true;
    }
    if (data.config_param_data.model_iotype == MODEL_TYPE_INPUT
        && data.config_param_data.model_type == MODEL_INPUT_QEP) {
        return false;
    }
    if (config_block_data.config_param_data.cat3_model != nullptr) {
        if ((config_block_data.safe_level == SAFE_LEVEL_CAT3)) {
            config_block_data_t cat3_data = config_block_data.config_param_data.cat3_model->get_block_data();
            if ((cat3_data.config_param_data.model_iotype == data.config_param_data.model_iotype)
                && (cat3_data.config_param_data.model_type == data.config_param_data.model_type)
                && (cat3_data.config_param_data.model_id == data.config_param_data.model_id)
                && (cat3_data.config_param_data.source_mcu == data.config_param_data.source_mcu)) {
                return true;
            }
        }
    }

    return false;
}

config_block_data_t* logic_element::get_config_data()
{
    return &config_block_data;
}

void logic_element::set_config_data(config_block_data_t data)
{
    config_block_data = data;
    if (data.config_param_data.model_iotype != MODEL_TYPE_LOGIC) {
        if (data.name.isEmpty()) {
            setText(0, data.type_name + "." + data.source_name + data.suffix);
        } else {
            setText(0, data.name + "." + data.source_name + data.suffix);
        }
    } else {
        setText(0, data.source_name);
    }

    setToolTip(0, this->text(0));
}

void logic_element::set_config_name(QString name)
{
    config_block_data.name = name;
    if (config_block_data.name.isEmpty()) {
        setText(0, config_block_data.type_name + "." + config_block_data.source_name + config_block_data.suffix);
    } else {
        setText(0, config_block_data.name + "." + config_block_data.source_name + config_block_data.suffix);
    }
}
