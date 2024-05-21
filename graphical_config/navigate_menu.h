#ifndef NAVIGATE_MENU_H
#define NAVIGATE_MENU_H
#include "QObject"
#include <QTreeWidget>
#include <QWidget>
class MainWindow;
class navigate_menu : public QTreeWidget {
    Q_OBJECT
public:
    navigate_menu(QWidget* uparent, QWidget* parent = 0);
    virtual ~navigate_menu() {}
    MainWindow* mainwindow = nullptr;

private:
    void menu_init(void);

private:
    QList<QTreeWidgetItem*> menu_item_list;
private slots:
    void on_items_clicked(QTreeWidgetItem* item, int column);
};

#endif  // NAVIGATE_MENU_H
