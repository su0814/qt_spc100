#ifndef TRANSPORTCRC_H
#define TRANSPORTCRC_H
#include <QObject>
#define TRANS_BUFF_LEN 1024
#define RECV_BUFF_LEN  1024
typedef enum frameState {
    FRAME_STATE_IDLE,
    FRAME_STATE_FIRST_HEADER,
    FRAME_STATE_SECOND_HEADER,
    FRAME_STATE_FRAME_DATA,
    FRAME_STATE_FIRST_TAIL,
    FRAME_STATE_HEADER_IN_DATA
} frameState_t;

class transportcrc : public QObject {
    Q_OBJECT
public:
    transportcrc() {}
    ~transportcrc() {}

public:
    frameState_t frame_state = FRAME_STATE_IDLE;
    uint8_t      fbuf[RECV_BUFF_LEN];
    uint16_t     flen;
    uint8_t      frameData[RECV_BUFF_LEN] = { 0 };

public:
    int (*send_data)(uint8_t* frame, int32_t length);
    uint16_t crc16_ibm_step(uint16_t crc, uint8_t* data, int32_t length);
    uint16_t crc16_ibm(uint8_t* data, int32_t length);
    int32_t  generateFrame(uint8_t* dataIn, int32_t dataInLen, uint8_t* dataOut, int32_t* dataOutLen);
    int32_t  getDataFromFrame(uint8_t* dataIn, int32_t dataInLen, uint8_t* dataOut, uint32_t* dataOutLen);
    int32_t  sendFrame(QByteArray* arr, uint8_t* frame, int32_t length);
    void     receiveByte(uint8_t byte);
    int      onFrameDetected(uint8_t* frame, int32_t length);

signals:
    void signal_onFrameDetected(uint8_t* frame, int32_t length);
    void signal_crc_error(uint8_t* frame, int32_t length);
};
#endif  // TRANSPORTCRC_H
