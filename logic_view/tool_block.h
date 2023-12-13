#ifndef TOOL_BLOCK_H
#define TOOL_BLOCK_H
#include "config/data_config.h"
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QObject>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
class tool_block : public QToolButton {
    Q_OBJECT
public:
    tool_block(QString iconName, tool_type_e type, tool_id_e id, QWidget* parent = nullptr);

    void mousePressEvent(QMouseEvent* event) override;

protected:
    tool_info_t tool_info;

private:
    QDrag*  drag = nullptr;
    QTimer* drag_timer;

public:
    void    set_name(QString Name);
    QString get_name(void);
signals:

public slots:
    void drag_moveover_detect_slot(void);
};

#endif  // TOOL_BLOCK_H
