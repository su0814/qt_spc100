#ifndef CONFIG_PHOTO_H
#define CONFIG_PHOTO_H
#include "config/data_config.h"
#include "config_param.h"
#include "graphical_config_param.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QJsonObject>
#include <QtSvg>
#include <QtWidgets>

class MainWindow;
class config_photo : public QGraphicsSvgItem {
public:
    config_photo(const QString& filePath, QWidget* uiparent);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

public:
    module_param_t module_param;

public:
    void           module_param_init(void);
    QJsonObject    param_project_info(void);
    bool           param_project_parse(QJsonObject project);
    void           param_ss_set(uint8_t state);
    uint8_t        param_ss_get(void);
    module_param_t get_module_param(void);

private:
    void init(QWidget* uiparent);
    void config_rect_visible(QPointF pos);

private:
    config_param*        sys_config;
    config_param*        can_config;
    config_param*        ai_config;
    config_param*        di_config;
    config_param*        relay_config;
    config_param*        mos_config;
    config_param*        encode_config;
    config_param*        pi_config;
    QList<config_param*> config_rect_list;

protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
};

#endif  // CONFIG_PHOTO_H
