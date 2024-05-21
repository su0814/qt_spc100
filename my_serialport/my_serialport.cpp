#include "my_serialport.h"
#include "config/def.h"
#include "qdebug.h"
#include "qmessagebox.h"
#include <QObject>
#include <QThread>
my_serialport::my_serialport() {}
void my_serialport::serch_port(void)
{
    if (!portname_list.isEmpty()) {
        portname_list.clear();
    }
    portname_list = QSerialPortInfo ::availablePorts();
}
int my_serialport::open_port(QString portname, qint32 baudrate, QSerialPort::DataBits databits,
                             QSerialPort::Parity parity, QSerialPort::StopBits stopbits)
{
    foreach (const QSerialPortInfo& serialportinfo, portname_list) {
        if (portname == serialportinfo.portName()) {
            if (serialportinfo.isBusy() == true || serialportinfo.isNull() == true) {
                QMessageBox::warning(NULL, "温馨提示", "串口不存在或被占用", QMessageBox::Yes, QMessageBox::Yes);
                return -1;
            }
            my_serial->clear();
            my_serial->setPortName(serialportinfo.portName());
            my_serial->setBaudRate(baudrate);
            my_serial->setDataBits(databits);
            my_serial->setParity(parity);
            my_serial->setStopBits(stopbits);
            my_serial->setDataTerminalReady(true);
            my_serial->setRequestToSend(true);
            my_serial->setFlowControl(QSerialPort::NoFlowControl);
            my_serial->open(QIODevice::ReadWrite);

            return 0;
        }
    }
    return -2;
}

void my_serialport::close_port(void)
{
    if (my_serial->isOpen()) {
        my_serial->close();
    }
}

void my_serialport::data_encrypt(uint8_t* frame, int32_t length)
{
    if (!frame) {
        return;
    }
    for (int32_t i = 0; i < length; i++) {
        if (frame[i] != 0x00 && frame[i] != COMMUNICATION_ENCRYPTION_CODE1) {
            frame[i] ^= COMMUNICATION_ENCRYPTION_CODE1;
        }
        frame[i] ^= COMMUNICATION_ENCRYPTION_CODE2;
    }
}

void my_serialport::data_decrypt(uint8_t* frame, int32_t length)
{
    if (!frame) {
        return;
    }
    for (int32_t i = 0; i < length; i++) {
        frame[i] ^= COMMUNICATION_ENCRYPTION_CODE2;
        if (frame[i] != 0x00 && frame[i] != COMMUNICATION_ENCRYPTION_CODE1) {
            frame[i] ^= COMMUNICATION_ENCRYPTION_CODE1;
        }
    }
}

int my_serialport::port_senddata(uint8_t* frame, int32_t length)
{
    QByteArray data = QByteArray(( char* )frame, length);
    serial_senddata(&data);
    return 0;
}

int my_serialport::port_transmisson_sendframe(uint8_t* frame, int32_t length)
{
    QByteArray data;
    transport->sendFrame(&data, frame, length);
    serial_senddata(&data);
    return 0;
}

int my_serialport::port_cmd_sendframe(uint8_t id, uint8_t type, uint8_t cmd, uint8_t sub, uint8_t* frame,
                                      uint16_t length)
{
    uint8_t  data[1024] = { 0 };
    uint16_t i          = 0;
    data[i++]           = id;
    data[i++]           = pack_seq;
    data[i++]           = pack_seq >> 8;
    data[i++]           = pack_seq >> 16;
    data[i++]           = pack_seq >> 24;
    data[i++]           = type;
    data[i++]           = cmd;
    data[i++]           = sub;
    data[i++]           = (length);
    data[i++]           = (length) >> 8;
    if (frame != NULL) {
        memcpy(&data[i], frame, length);
        i += length;
    }
    return port_transmisson_sendframe(data, i);
}

void my_serialport::serial_senddata(QByteArray* data)
{
    if (COMMUNICATION_ENCRYPTION_FLAG) {
        data_encrypt(( uint8_t* )data->data(), data->length());
    }
    my_serial->write(data->data(), data->length());
}
