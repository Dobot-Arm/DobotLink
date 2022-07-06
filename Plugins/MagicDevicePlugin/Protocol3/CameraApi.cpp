#include "CameraApi.h"
#include "P3FrameAdapter.h"
#include <QTimer>
#include <QJsonArray>

CameraApi *CameraApi::m_staticCameraApi = new CameraApi();

CameraApi *CameraApi::instance()
{
    return m_staticCameraApi;
}

CameraApi::CameraApi()
{
    connect(this, &CameraApi::sendCmd_signal, P3FrameAdapter::instance(), &P3FrameAdapter::sendCmd_slot);

    P3FrameAdapter::instance()->registerCarCameraCB(CBGetCameraAngleId, GetCarCameraAngle_CB);
    P3FrameAdapter::instance()->registerCarCameraCB(CBGetCameraColorId, GetCarCameraColor_CB);
    P3FrameAdapter::instance()->registerCarCameraCB(CBGetCameraObjId, GetCarCameraObj_CB);
    P3FrameAdapter::instance()->registerCarCameraCB(CBGetCameraTagId, GetCarCameraTag_CB);
    P3FrameAdapter::instance()->registerCarCameraCB(CBCameraCalibrationModeId, CarCameraCalibrationMode_CB);
    P3FrameAdapter::instance()->registerCarCameraCB(CBCameraCalibrationModeId, CarCameraCalibrationMode_CB);
    P3FrameAdapter::instance()->registerCarCameraCB(CBCameraRunModelId, CarCameraRunModel_CB);
    P3FrameAdapter::instance()->registerCarCameraCB(CBCameraRunModelId, CarCameraRunModel_CB);

    P3FrameAdapter::instance()->registerArmCameraCB(CBGetCameraAngleId, GetArmCameraAngle_CB);
    P3FrameAdapter::instance()->registerArmCameraCB(CBGetCameraColorId, GetArmCameraColor_CB);
    P3FrameAdapter::instance()->registerArmCameraCB(CBGetCameraObjId, GetArmCameraObj_CB);
    P3FrameAdapter::instance()->registerArmCameraCB(CBGetCameraTagId, GetArmCameraTag_CB);
    P3FrameAdapter::instance()->registerArmCameraCB(CBCameraCalibrationModeId, ArmCameraCalibrationMode_CB);
    P3FrameAdapter::instance()->registerArmCameraCB(CBCameraCalibrationModeId, ArmCameraCalibrationMode_CB);
    P3FrameAdapter::instance()->registerArmCameraCB(CBCameraRunModelId, ArmCameraRunModel_CB);
    P3FrameAdapter::instance()->registerArmCameraCB(CBCameraRunModelId, ArmCameraRunModel_CB);
}

CameraApi::~CameraApi()
{

}

void CameraApi::GetArmCameraAngle(uint32_t seqNum){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x21;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = GetCameraAngleId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);

}


void CameraApi::GetArmCameraColor(uint32_t seqNum){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x21;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = GetCameraColorId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);

}


void CameraApi::GetArmCameraObj(uint32_t seqNum){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x21;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = GetCameraObjId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);

}


void CameraApi::GetArmCameraTag(uint32_t seqNum){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x21;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = GetCameraTagId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);

}

void CameraApi::SetArmCameraCalibrationMode(uint32_t seqNum, uint8_t isEnableCali){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x21;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = CameraCalibrationModeId;
    uint16_t payloadLen = sizeof(uint8_t);
    memcpy(&m_packFrame.payloadAndCRC[0], &isEnableCali, sizeof(uint8_t));
    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void CameraApi::GetArmCameraCalibrationMode(uint32_t seqNum){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x21;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = CameraCalibrationModeId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void CameraApi::SetArmCameraRunModel(uint32_t seqNum, uint8_t runModelIndex){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x21;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = CameraRunModelId;
    uint16_t payloadLen = sizeof(uint8_t);
    memcpy(&m_packFrame.payloadAndCRC[0], &runModelIndex, sizeof(uint8_t));
    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void CameraApi::GetArmCameraRunModel(uint32_t seqNum){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x21;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = CameraRunModelId;
    uint16_t payloadLen = 0;
    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

// 回调函数
void CameraApi::GetArmCameraAngle_CB(DobotV3CmdInterCBParm packet){
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {

        isTimeout = false;

        uint8_t count;
        memcpy(&count, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
        resObj.insert("count", count);

        uint8_t crc;
        memcpy(&crc, &packet.packet.payloadAndCRC[1], sizeof (uint8_t));
        resObj.insert("crc", crc);

        uint8_t angle;
        memcpy(&angle, &packet.packet.payloadAndCRC[3], sizeof (uint8_t));
        resObj.insert("angle", angle);
    }

    emit m_staticCameraApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}

void CameraApi::GetArmCameraColor_CB(DobotV3CmdInterCBParm packet){
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {

        isTimeout = false;

        uint8_t count;
        memcpy(&count, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
        resObj.insert("count", count);

//        uint8_t crc;
//        memcpy(&crc, &packet.packet.payloadAndCRC[1], sizeof (uint8_t));
//        resObj.insert("crc", crc);

        int index = 2;
        QJsonArray array;

        for (int i=0; i<count; i++) {
            colorobj obj_t;
            memcpy(&obj_t, &packet.packet.payloadAndCRC[index], sizeof (obj_t));
            QJsonObject jsonobj_t;

            jsonobj_t.insert("x", static_cast<int>(obj_t.x));
            jsonobj_t.insert("y", static_cast<int>(obj_t.y));
            jsonobj_t.insert("w", static_cast<int>(obj_t.w));
            jsonobj_t.insert("h", static_cast<int>(obj_t.h));
            jsonobj_t.insert("id", static_cast<int>(obj_t.id));

            array.append(jsonobj_t);
            index = index + 5;


        }

        resObj.insert("color_obj", array);
    }

    emit m_staticCameraApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}


void CameraApi::GetArmCameraObj_CB(DobotV3CmdInterCBParm packet){
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {

        isTimeout = false;

        uint8_t count;
        memcpy(&count, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
        resObj.insert("count", count);

//        uint8_t crc;
//        memcpy(&crc, &packet.packet.payloadAndCRC[1], sizeof (uint8_t));
//        resObj.insert("crc", crc);

        int index = 2;
        QJsonArray array;

        for (int i=0; i<count; i++) {
            dlobj obj_t;
            memcpy(&obj_t, &packet.packet.payloadAndCRC[index], sizeof (obj_t));
            QJsonObject jsonobj_t;

            jsonobj_t.insert("x", static_cast<int>(obj_t.x));
            jsonobj_t.insert("y", static_cast<int>(obj_t.y));
            jsonobj_t.insert("w", static_cast<int>(obj_t.w));
            jsonobj_t.insert("h", static_cast<int>(obj_t.h));
            jsonobj_t.insert("id", static_cast<int>(obj_t.id));


            array.append(jsonobj_t);
            index = index + 5;


        }
        //测试用例，直接抛所有的结果
        resObj.insert("dl_obj", array);
    }

    emit m_staticCameraApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}


void CameraApi::GetArmCameraTag_CB(DobotV3CmdInterCBParm packet){
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {

        isTimeout = false;

        uint8_t count;
        memcpy(&count, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
        resObj.insert("count", count);

        uint8_t crc;
        memcpy(&crc, &packet.packet.payloadAndCRC[1], sizeof (uint8_t));
        resObj.insert("crc", crc);

        int index = 2;
        int rotid = 7;
        QJsonArray array;

        for (int i=0; i<count; i++) {
            Obj_t obj_t;
            memcpy(&obj_t, &packet.packet.payloadAndCRC[index], sizeof (obj_t));
            QJsonObject jsonobj_t;

            jsonobj_t.insert("x", static_cast<int>(obj_t.x));
            jsonobj_t.insert("y", static_cast<int>(obj_t.y));
            jsonobj_t.insert("w", static_cast<int>(obj_t.w));
            jsonobj_t.insert("h", static_cast<int>(obj_t.h));
            jsonobj_t.insert("id", static_cast<int>(obj_t.id));
            float rot;
            memcpy(&rot, &packet.packet.payloadAndCRC[rotid], sizeof (float));
            rot = (rot/3.1415926)*180.0;
            jsonobj_t.insert("rot", rot);

            array.append(jsonobj_t);
            index = index + 9;
            rotid = rotid + 9;

        }

        resObj.insert("aptag_obj", array);

    }

    emit m_staticCameraApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}

void CameraApi::SetArmCameraCalibrationMode_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticCameraApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}


void CameraApi::ArmCameraCalibrationMode_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        if (packet.packet.rwType == 0){
            isTimeOut = false;
            uint8_t isEnableCali;
            memcpy(&isEnableCali, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
            resObj.insert("isEnableCali", isEnableCali);
        }
        else {
            resObj.insert("result", "true");
        }
    }

    emit m_staticCameraApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void CameraApi::SetArmCameraRunModel_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticCameraApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void CameraApi::ArmCameraRunModel_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        if (packet.packet.rwType == 0){
            uint8_t runModelIndex;
            memcpy(&runModelIndex, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
            resObj.insert("runModelIndex", runModelIndex);
        } else {
            resObj.insert("result", "true");
        }
    }

    emit m_staticCameraApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void CameraApi::GetCarCameraAngle(uint32_t seqNum){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x20;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = GetCameraAngleId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);

}


void CameraApi::GetCarCameraColor(uint32_t seqNum){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x20;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = GetCameraColorId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);

}


void CameraApi::GetCarCameraObj(uint32_t seqNum){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x20;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = GetCameraObjId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);

}


void CameraApi::GetCarCameraTag(uint32_t seqNum){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x20;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = GetCameraTagId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);

}

void CameraApi::SetCarCameraCalibrationMode(uint32_t seqNum, uint8_t isEnableCali){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x20;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = CameraCalibrationModeId;
    uint16_t payloadLen = sizeof(uint8_t);
    memcpy(&m_packFrame.payloadAndCRC[0], &isEnableCali, sizeof(uint8_t));
    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void CameraApi::GetCarCameraCalibrationMode(uint32_t seqNum){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x20;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = CameraCalibrationModeId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void CameraApi::SetCarCameraRunModel(uint32_t seqNum, uint8_t runModelIndex){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x20;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = CameraRunModelId;
    uint16_t payloadLen = sizeof(uint8_t);
    memcpy(&m_packFrame.payloadAndCRC[0], &runModelIndex, sizeof(uint8_t));
    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void CameraApi::GetCarCameraRunModel(uint32_t seqNum){
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x20;
    m_packFrame.cmdSet = 0x20;
    m_packFrame.cmdId = CameraRunModelId;
    uint16_t payloadLen = 0;
    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

// 回调函数
void CameraApi::GetCarCameraAngle_CB(DobotV3CmdInterCBParm packet){
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {

        isTimeout = false;

        uint8_t count;
        memcpy(&count, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
        resObj.insert("count", count);

//        uint8_t crc;
//        memcpy(&crc, &packet.packet.payloadAndCRC[1], sizeof (uint8_t));
//        resObj.insert("crc", crc);

        uint8_t angle;
        memcpy(&angle, &packet.packet.payloadAndCRC[2], sizeof (uint8_t));
        resObj.insert("angle", angle);
    }

    emit m_staticCameraApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}



void CameraApi::GetCarCameraColor_CB(DobotV3CmdInterCBParm packet){
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {

        isTimeout = false;

        uint8_t count;
        memcpy(&count, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
        resObj.insert("count", count);

//        uint8_t crc;
//        memcpy(&crc, &packet.packet.payloadAndCRC[1], sizeof (uint8_t));
//        resObj.insert("crc", crc);

        int index = 2;
        QJsonArray array;

        for (int i=0; i<count; i++) {
            colorobj obj_t;
            memcpy(&obj_t, &packet.packet.payloadAndCRC[index], sizeof (obj_t));
            QJsonObject jsonobj_t;

            jsonobj_t.insert("x", static_cast<int>(obj_t.x));
            jsonobj_t.insert("y", static_cast<int>(obj_t.y));
            jsonobj_t.insert("w", static_cast<int>(obj_t.w));
            jsonobj_t.insert("h", static_cast<int>(obj_t.h));
            jsonobj_t.insert("id", static_cast<int>(obj_t.id));

            array.append(jsonobj_t);
            index = index + 5;


        }

        resObj.insert("color_obj", array);
    }

    emit m_staticCameraApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}


void CameraApi::GetCarCameraObj_CB(DobotV3CmdInterCBParm packet){
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {

        isTimeout = false;

        uint8_t count;
        memcpy(&count, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
        resObj.insert("count", count);

//        uint8_t crc;
//        memcpy(&crc, &packet.packet.payloadAndCRC[1], sizeof (uint8_t));
//        resObj.insert("crc", crc);

        int index = 2;
        QJsonArray array;

        for (int i=0; i<count; i++) {
            dlobj obj_t;
            memcpy(&obj_t, &packet.packet.payloadAndCRC[index], sizeof (obj_t));
            QJsonObject jsonobj_t;

            jsonobj_t.insert("x", static_cast<int>(obj_t.x));
            jsonobj_t.insert("y", static_cast<int>(obj_t.y));
            jsonobj_t.insert("w", static_cast<int>(obj_t.w));
            jsonobj_t.insert("h", static_cast<int>(obj_t.h));
            jsonobj_t.insert("id", static_cast<int>(obj_t.id));

            array.append(jsonobj_t);
            index = index + 5;


        }
        resObj.insert("dl_obj", array);
    }

    emit m_staticCameraApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}


void CameraApi::GetCarCameraTag_CB(DobotV3CmdInterCBParm packet){
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {


        isTimeout = false;

        uint8_t count;
        memcpy(&count, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
        resObj.insert("count", count);

        int index = 2;
        int rotid = 7;
        QJsonArray array;

        for (int i=0; i<count; i++) {
            Obj_t obj_t;
            memcpy(&obj_t, &packet.packet.payloadAndCRC[index], sizeof (obj_t));
            QJsonObject jsonobj_t;

            jsonobj_t.insert("x", static_cast<int>(obj_t.x));
            jsonobj_t.insert("y", static_cast<int>(obj_t.y));
            jsonobj_t.insert("w", static_cast<int>(obj_t.w));
            jsonobj_t.insert("h", static_cast<int>(obj_t.h));
            jsonobj_t.insert("id", static_cast<int>(obj_t.id));
            float rot;
            memcpy(&rot, &packet.packet.payloadAndCRC[rotid], sizeof (float));
            rot = (rot/3.1415926)*180.0;
            jsonobj_t.insert("rot", rot);


            array.append(jsonobj_t);
            index = index + 9;
            rotid = rotid + 9;

        }

        resObj.insert("aptag_obj", array);

    }

    emit m_staticCameraApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}


void CameraApi::SetCarCameraCalibrationMode_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticCameraApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}


void CameraApi::CarCameraCalibrationMode_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;

    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {
        isTimeOut = false;
        if (packet.packet.rwType == 0){
            isTimeOut = false;
            uint8_t isEnableCali;
            memcpy(&isEnableCali, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
            resObj.insert("isEnableCali", isEnableCali);
        }
        else {
            resObj.insert("result", "true");
        }
    }

    emit m_staticCameraApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}


void CameraApi::SetCarCameraRunModel_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticCameraApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void CameraApi::CarCameraRunModel_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        if (packet.packet.rwType == 0){
            uint8_t runModelIndex;
            memcpy(&runModelIndex, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
            resObj.insert("runModelIndex", runModelIndex);
        } else {
            resObj.insert("result", "true");
        }
    }

    emit m_staticCameraApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

