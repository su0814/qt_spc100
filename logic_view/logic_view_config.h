#ifndef LOGIC_VIEW_CONFIG_H
#define LOGIC_VIEW_CONFIG_H
#include "QStyleOptionTab"
#include "config/data_config.h"
#include "graphical_config/graphical_config_param.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QPainter>
#include <QProxyStyle>
#define LOGIC_BLOCK_MAX_NUM (16)
enum {
    ACTION_NONE   = 0,
    ACTION_SET    = 0x01,
    ACTION_DELETE = 0X02,
};

typedef struct {
    QList<uint32_t>      parent_id;
    int                  uid;
    int                  order_id;
    QString              source_function    = "";  //输入的源函数
    QString              logic_function     = "";  //输入的逻辑
    QString              function_name      = "";  //函数名称
    int                  input_reverse_data = 0;
    config_block_data_t* config_block_data;
} attribute_t;

typedef struct {
    uint32_t value = 0;
    uint8_t  dir   = 0;
} condition_right_set_t;

/* 复制信息 */
typedef struct {
    int                 type;
    int                 width;
    int                 height;
    QPointF             offset_pos;
    config_block_data_t block_data;
    QJsonObject         block_info;
} block_copy_data_t;

typedef struct {
    int                    input_block_id;
    int                    input_point_id;
    connect_point_iotype_e input_point_type;
    int                    output_block_id;
    int                    output_point_id;
    connect_point_iotype_e output_point_type;
} line_copy_data_t;

extern const QStringList connect_point_data_type_label;

class MyProxyStyle : public QProxyStyle {
public:
    using QProxyStyle::QProxyStyle;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
                     const QWidget* widget = nullptr) const override
    {
        if (element == CE_TabBarTabLabel) {
            // 在这里进行标签标题的绘制
            const QStyleOptionTab* tabOption = qstyleoption_cast<const QStyleOptionTab*>(option);
            if (tabOption) {
                QRect       allRect = tabOption->rect;
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                painter->drawText(allRect, tabOption->text, option);
                return;
            }
        }
        QProxyStyle::drawControl(element, option, painter, widget);
    }
};
#endif  // LOGIC_VIEW_CONFIG_H
