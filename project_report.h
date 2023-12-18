#ifndef PROJECT_REPORT_H
#define PROJECT_REPORT_H

#include "config/data_config.h"
#include "ui_mainwindow.h"
#include <QWidget>
class MainWindow;
class project_report : public QWidget {
    Q_OBJECT
public:
    explicit project_report(QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

private:
    void project_report_init(void);
    void project_table_init(void);
    void device_input_table_init(void);
    void device_output_table_init(void);
    void coroutine_table_init(void);
    void sf_table_init(void);

    void project_table_update(void);
    void device_input_table_update(void);
    void device_output_table_update(void);
    void coroutine_table_update(void);
    void sf_table_update(void);

public:
    void project_report_update(void);

public:
    input_resource_info_t input_resource_info;
signals:

public slots:
    void tab_changeed_slot(int index);
};

#endif  // PROJECT_REPORT_H