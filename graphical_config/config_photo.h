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

private:
    void init(QWidget* uiparent);
    void config_rect_visible(QPointF pos);

private:
    config_param*        a_ai1;
    config_param*        a_ai2;
    config_param*        b_ai1;
    config_param*        b_ai2;
    config_param*        a_di1;
    config_param*        a_di2;
    config_param*        a_di3;
    config_param*        a_di4;
    config_param*        a_di5;
    config_param*        a_di6;
    config_param*        a_di7;
    config_param*        a_di8;
    config_param*        b_di1;
    config_param*        b_di2;
    config_param*        b_di3;
    config_param*        b_di4;
    config_param*        b_di5;
    config_param*        b_di6;
    config_param*        b_di7;
    config_param*        b_di8;
    config_param*        a_relay1;
    config_param*        a_relay2;
    config_param*        b_relay1;
    config_param*        b_relay2;
    config_param*        mos1;
    config_param*        mos2;
    config_param*        mos3;
    config_param*        mos4;
    config_param*        qep1;
    config_param*        qep2;
    config_param*        a_pi1;
    config_param*        a_pi2;
    config_param*        b_pi1;
    config_param*        b_pi2;
    config_param*        pi_qep1;
    config_param*        pi_qep2;
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
