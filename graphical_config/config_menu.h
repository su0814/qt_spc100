#ifndef CONFIG_MENU_H
#define CONFIG_MENU_H
#include "config_element.h"
#include "graphical_config_param.h"
#include <QTreeWidget>
#include <QWidget>

class MainWindow;
class config_menu : public QTreeWidget {
public:
    config_menu(QWidget* uparent, QWidget* parent = 0);
    MainWindow* mainwindow = nullptr;
    void        startDrag(Qt::DropActions supportedActions) override;

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;

private:
    void load_menu(void);
    void json_menu_parse(QJsonObject object, QTreeWidgetItem* item);
};

#endif  // CONFIG_MENU_H
