#ifndef PROJECT_REPORT_H
#define PROJECT_REPORT_H
#include <QGridLayout>
#include <QWidget>
class MainWindow;
class project_report {
public:
    project_report(QWidget* mparent);
    MainWindow* mainwindow = nullptr;

private:
private:
    void project_report_clear(QGridLayout* layout);
    void project_info_creat(QGridLayout* layout, int* line);
    void config_overview_creat(QGridLayout* layout, int* line);
    void safety_param_creat(QGridLayout* layout, int* line);
    void wiring_diagram_creat(QGridLayout* layout, int* line);

public:
    void project_report_update(void);
    void project_report_save();
};

#endif  // PROJECT_REPORT_H
