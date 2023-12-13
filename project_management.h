#ifndef PROJECT_MANAGEMENT_H
#define PROJECT_MANAGEMENT_H

#include "config/data_config.h"
#include "ui_mainwindow.h"
#include <QWidget>
class MainWindow;
class project_management : public QWidget {
    Q_OBJECT
public:
    explicit project_management(QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

private:
    void                      project_save(void);
    project_management_info_t project_management_info;

private:
    void project_management_reset(void);
signals:

public slots:
    void project_creat_slot(void);

    void project_new_slot(void);
    void project_save_slot(void);
    void project_import_slot(void);
    void project_transmit_to_device_slot(void);

    void project_advanced_program_slot(int state);
};

#endif  // PROJECT_MANAGEMENT_H
