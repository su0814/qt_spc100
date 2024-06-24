#ifndef PROJECT_START_DIALOG_H
#define PROJECT_START_DIALOG_H

#include <QDialog>
class MainWindow;
namespace Ui {
class project_start_dialog;
}

class project_start_dialog : public QDialog {
    Q_OBJECT

public:
    explicit project_start_dialog(QWidget* mparent, QWidget* parent = 0);
    ~project_start_dialog();

private:
    MainWindow* mainwindow = nullptr;
private slots:

    void on_pushButton_new_pro_clicked();

    void on_pushButton_import_pro_clicked();

    void on_pushButton_skip_clicked();

private:
    Ui::project_start_dialog* ui;
};

#endif  // PROJECT_START_DIALOG_H
