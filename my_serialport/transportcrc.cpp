#define MINPACKLEN 6
#define P_HEADER   0xAA
#define P_TAIL     0x55
#define P_CTRL     0xA5
#define P_FAIL     0
#define P_SUCCESS  1
#include "transportcrc.h"
#include "QDateTime"
#include "qdebug.h"
uint16_t transportcrc::crc16_ibm_step(uint16_t crc, uint8_t* data, int32_t length)
{
    uint8_t i;
    while (length--) {
        crc ^= *data++;
        for (i = 0; i < 8; ++i) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;  // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}
uint16_t transportcrc::crc16_ibm(uint8_t* data, int32_t length)
{
    return crc16_ibm_step(0xffff, data, length);
}
int32_t transportcrc::generateFrame(uint8_t* dataIn, int32_t dataInLen, uint8_t* dataOut, int32_t* dataOutLen)
{
    int32_t  i         = 0;
    int32_t  j         = 0;
    int32_t  maxOutLen = 0;
    uint16_t crc       = 0;
    uint8_t  crcu8[2];
    uint8_t  data;

    maxOutLen = *dataOutLen;
    if (!dataIn || !dataOut || maxOutLen < dataInLen + MINPACKLEN) {
        return -1;
    }

    dataOut[i++] = P_HEADER;  // 0xAA
    dataOut[i++] = P_HEADER;

    crc      = crc16_ibm(dataIn, dataInLen);
    crcu8[0] = (uint8_t)(crc & 0xff);
    crcu8[1] = (uint8_t)(crc >> 8);

    for (j = 0; j < dataInLen; j++) {
        data = dataIn[j];
        if (data == P_HEADER || data == P_TAIL || data == P_CTRL) {
            if (i >= maxOutLen) {
                return -2;
            }
            dataOut[i++] = P_CTRL;
        }
        if (i >= maxOutLen) {
            return -2;
        }
        dataOut[i++] = data;
    }

    for (j = 0; j < 2; j++) {
        data = crcu8[j];
        if (data == P_HEADER || data == P_TAIL || data == P_CTRL) {
            if (i >= maxOutLen) {
                return -2;
            }
            dataOut[i++] = P_CTRL;
        }
        if (i >= maxOutLen) {
            return -2;
        }
        dataOut[i++] = data;
    }

    if (i >= maxOutLen + 2) {
        return -2;
    }
    dataOut[i++] = P_TAIL;
    dataOut[i++] = P_TAIL;
    *dataOutLen  = i;
    return 0;
}

int32_t transportcrc::getDataFromFrame(uint8_t* dataIn, int32_t dataInLen, uint8_t* dataOut, uint32_t* dataOutLen)
{
    int      j = 0;
    int      i;
    uint16_t crc, crc_receive;
    for (i = 2; i < dataInLen - 2; i++, j++) {
        if (dataIn[i] == P_CTRL) {
            dataOut[j] = dataIn[++i];
        } else {
            dataOut[j] = dataIn[i];
        }
    }
    *dataOutLen = j - 2;
    if (*( int* )dataOutLen < 0) {
        return -2;
    }

    crc = crc16_ibm(dataOut, *dataOutLen);

    crc_receive = dataOut[*dataOutLen] + (dataOut[*dataOutLen + 1] << 8);

    if (crc == crc_receive) {
        return 0;
    } else {
        return -1;
    }
}
int32_t transportcrc::sendFrame(QByteArray* arr, uint8_t* frame, int32_t length)
{
    uint8_t  dataOut[TRANS_BUFF_LEN];
    int32_t  ret        = 0;
    uint32_t dataOutLen = sizeof(dataOut) / sizeof(dataOut[0]);
    ret                 = generateFrame(frame, length, dataOut, ( int32_t* )&dataOutLen);
    if (ret < 0) {
        return ret;
    }
    *arr = QByteArray(( char* )dataOut, dataOutLen);

    return ret;
}
void transportcrc::receiveByte(uint8_t byte)
{
    frameState_t state = frame_state;
    uint16_t     len   = flen;
    //    uint8_t frameData[RECV_BUFF_LEN] = { 0 };
    uint32_t frameDataLen = 0;
    int32_t  crcCheck;

    if (len >= sizeof(fbuf) / sizeof(fbuf[0])) {
        len   = 0;
        state = FRAME_STATE_IDLE;
    }

    switch (state) {
    case FRAME_STATE_IDLE:
        switch (byte) {
        case P_HEADER:
            fbuf[len++] = byte;
            state       = FRAME_STATE_FIRST_HEADER;
            break;
        case P_TAIL:
        default:
            break;
        }
        break;
    case FRAME_STATE_FIRST_HEADER:
        switch (byte) {
        case P_HEADER:
            fbuf[len++] = byte;
            state       = FRAME_STATE_SECOND_HEADER;
            break;
        case P_TAIL:
        default:
            len   = 0;
            state = FRAME_STATE_IDLE;
            break;
        }
        break;
    case FRAME_STATE_SECOND_HEADER:
        switch (byte) {
        case P_HEADER:
            state = FRAME_STATE_SECOND_HEADER;
            break;
        case P_TAIL:
            fbuf[len++] = byte;
            state       = FRAME_STATE_FIRST_TAIL;
            break;
        default:
            fbuf[len++] = byte;
            state       = FRAME_STATE_FRAME_DATA;
            break;
        }
        break;
    case FRAME_STATE_FRAME_DATA:
        switch (byte) {
        case P_HEADER:
            fbuf[len++] = byte;
            state       = FRAME_STATE_HEADER_IN_DATA;
            break;
        case P_TAIL:
            fbuf[len++] = byte;
            state       = FRAME_STATE_FIRST_TAIL;
            break;
        default:
            fbuf[len++] = byte;
            break;
        }
        break;
    case FRAME_STATE_FIRST_TAIL:
        switch (byte) {
        case P_HEADER:
            fbuf[len++] = byte;
            state       = FRAME_STATE_HEADER_IN_DATA;
            break;
        case P_TAIL:

            fbuf[len++] = byte;
            crcCheck    = getDataFromFrame(fbuf, len, frameData, &frameDataLen);
            if (0 == crcCheck) {
                onFrameDetected(frameData, frameDataLen);
            } else if (-1 == crcCheck) {
                onFrameDetected(frameData, frameDataLen | 0xFFFF0000);
            } else {
            }

            len   = 0;
            state = FRAME_STATE_IDLE;
            break;
        default:
            fbuf[len++] = byte;
            state       = FRAME_STATE_FRAME_DATA;
            break;
        }
        break;
    case FRAME_STATE_HEADER_IN_DATA:
        switch (byte) {
        case P_HEADER:
            len         = 0;
            fbuf[len++] = byte;
            fbuf[len++] = byte;
            state       = FRAME_STATE_SECOND_HEADER;
            break;
        case P_TAIL:
            fbuf[len++] = byte;
            state       = FRAME_STATE_FIRST_TAIL;
            break;
        default:
            fbuf[len++] = byte;
            state       = FRAME_STATE_FRAME_DATA;
            break;
        }
        break;
    default:
        break;
    }

    frame_state = state;
    flen        = len;
}
int transportcrc::onFrameDetected(uint8_t* frame, int32_t length)
{
    if ((length & 0xFFFF0000) == 0xFFFF0000) {
        emit signal_crc_error(frame, length);
        return -1;
    }
    emit signal_onFrameDetected(frame, length);
    return 0;
}
