#ifndef PROJECT_DEBUG_H
#define PROJECT_DEBUG_H

#include "config/data_config.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QWidget>
class MainWindow;
class project_debug : public QWidget {
    Q_OBJECT
public:
    explicit project_debug(QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;

signals:

public slots:
};

#endif  // PROJECT_DEBUG_H
