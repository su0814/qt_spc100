#ifndef CONFIG_MENU_H
#define CONFIG_MENU_H
#include <QTreeWidget>
#include <QWidget>

enum {
    KEY_TYPE_MENU = 1,
    KEY_TYPE_ELEMENT,
};

class MainWindow;
class config_menu {
public:
    config_menu(QWidget* parent = 0);
    MainWindow* mainwindow = nullptr;

private:
    void load_menu(void);
    void json_menu_parse(QJsonObject object, QTreeWidgetItem* item);
};

#endif  // CONFIG_MENU_H
