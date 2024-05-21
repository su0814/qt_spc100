#ifndef CONFIG_DEVICE_FAULT_H
#define CONFIG_DEVICE_FAULT_H
#include "device/device_fault.h"
#include <QGraphicsRectItem>
#include <QTimer>
class config_device_fault : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    config_device_fault(int x, int y, int w, int h, QGraphicsItem* parent = nullptr);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

public:
    void set_fault_info(module_fault_t a_fault_info, module_fault_t b_fault_info);

private:
    device_fault         device_fault_dialog;
    QGraphicsPixmapItem* error_pixmap;
    QGraphicsPixmapItem* warning_pixmap;
    QTimer               error_flash_timer;
    QTimer               warning_flash_timer;
private slots:
    void update_fault_state(bool error, bool warning);
    void error_flash_slot(void);
    void warning_flash_slot(void);
};

#endif  // CONFIG_DEVICE_FAULT_H
