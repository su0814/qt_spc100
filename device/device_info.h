#ifndef DEVICE_INFO_H
#define DEVICE_INFO_H
#include <QHeaderView>
#include <QTableWidget>

class device_info {
public:
    device_info();

public:
    static void spc100_info_table_creat(QTableWidget* table);

private:
    static void table_header_creat(QTableWidget* table, QString device_name);
};

#endif  // DEVICE_INFO_H
