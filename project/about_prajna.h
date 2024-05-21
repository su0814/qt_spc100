#ifndef ABOUT_PRAJNA_H
#define ABOUT_PRAJNA_H

#include "ui_mainwindow.h"
#include <QWidget>

class MainWindow;
class about_prajna : public QWidget {
    Q_OBJECT
public:
    explicit about_prajna(QWidget* mparent, QWidget* parent = nullptr);
    Ui::MainWindow* ui         = nullptr;
    MainWindow*     mainwindow = nullptr;
signals:

private slots:
    void about_prajnasafe_message_slot(void);
    void about_configtool_manual_file_slot(void);
    void about_usermanual_file_slot(void);
};

#endif  // ABOUT_PRAJNA_H
