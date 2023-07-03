#ifndef MY_SERIALPORT_H
#define MY_SERIALPORT_H
#include "qserialport.h"
#include "transportcrc.h"
#include <QList>
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <qserialportinfo.h>

#define COMMUNICATION_ENCRYPTION_FLAG  1
#define COMMUNICATION_ENCRYPTION_CODE1 0X53
#define COMMUNICATION_ENCRYPTION_CODE2 0X75

enum {
    COMM_ENCRYPT_ENABLE,
    COMM_ENCRYPT_DISABLE,
};

class my_serialport : public QObject {
    Q_OBJECT
public:
    QList<QSerialPortInfo> portname_list;
    QSerialPort*           my_serial = new QSerialPort;
    transportcrc*          transport = new transportcrc;

public:
    my_serialport();
    void serch_port(void);
    int  open_port(QString portname, qint32 baudrate, QSerialPort::DataBits databits, QSerialPort::Parity parity,
                   QSerialPort::StopBits stopbits);
    void close_port(void);
    int  port_senddata(uint8_t* frame, int32_t length);
    int  port_sendframe(uint8_t* frame, int32_t length);
    void serial_senddata(QByteArray* data);
    void data_encrypt(uint8_t* frame, int32_t length);
    void data_decrypt(uint8_t* frame, int32_t length);
};

#endif  // MY_SERIALPORT_H
