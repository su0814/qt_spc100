#ifndef PROJECT_REPORT_H
#define PROJECT_REPORT_H
#include "graphical_config/graphical_config_param.h"
#include <QGridLayout>
#include <QWidget>
typedef enum {
    BASEMAP_AI = 0,
    BASEMAP_DI_UP,
    BASEMAP_DI_DOWN,
    BASEMAP_PI,
    BASEMAP_QEP,
    BASEMAP_RELAY,
    BASEMAP_MOS,
} basemap_id_e;
class MainWindow;
class project_report {
public:
    project_report(QWidget* mparent);
    MainWindow* mainwindow = nullptr;

private:
    int left_element_x;
    int power_y[2];
    int ai_y[4];
    int di_y[16];
    int right_element_x;
    int relay_y[8];
    int mos_y[4];
    int qep_y[8];
    int piqep_y[8];
    int pi_y[4];

    QList<int> title_row;

    int left_distance  = 0;
    int right_distance = 0;

private:
    void    project_report_clear(QGridLayout* layout);
    void    project_info_creat(QGridLayout* layout, int* line);
    void    config_overview_creat(QGridLayout* layout, int* line);
    void    safety_param_creat(QGridLayout* layout, int* line);
    QPixmap wiring_basemap_baseinput_creat(safe_level_e safety_level, QPixmap element_pixmap, QString name,
                                           bool used = true, bool power = false, source_mcu_e channel = SOURCE_MCU_A);
    QPixmap wiring_basemap_qep_creat(safe_level_e safety_level, QPixmap element_pixmap, QString name, bool used = true);
    QPixmap wiring_basemap_relay_creat(safe_level_e safety_level, QString name, int contactors_num,
                                       source_mcu_e channel = SOURCE_MCU_A, bool used = true);
    QPixmap wiring_basemap_mos_creat(QString name, QString pixmap, bool used = true);
    void    wiring_diagram_creat(QGridLayout* layout, int* line);

public:
    void project_report_update(void);
    void project_report_save();
};

#endif  // PROJECT_REPORT_H
