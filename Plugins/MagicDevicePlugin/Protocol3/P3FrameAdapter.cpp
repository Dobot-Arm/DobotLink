#include "P3FrameAdapter.h"

// PS： 静态变量意味着不能同时使用多个实例，尤其要注意
P3FrameAdapter *P3FrameAdapter::sm_instance = nullptr;
QList<QByteArray> P3FrameAdapter::sm_recviveBytes;

P3FrameAdapter *P3FrameAdapter::instance()
{
    if (!sm_instance) {
        sm_instance = new P3FrameAdapter();
        sm_instance->m_thread = new QThread();
        sm_instance->moveToThread(sm_instance->m_thread);
        sm_instance->m_timerExec->moveToThread(sm_instance->m_thread);
        sm_instance->m_timerCNT->moveToThread(sm_instance->m_thread);
        sm_instance->m_thread->start();
        connect(sm_instance->m_thread, &QThread::started, [](){
            sm_instance->m_timerExec->start();
            sm_instance->m_timerCNT->start();
        });
    }
    return sm_instance;
}

P3FrameAdapter::P3FrameAdapter(QObject *parent):
    QObject(parent),
    m_thread(nullptr)
{
    qRegisterMetaType<DobotV3Format_Packet>("DobotV3Format_Packet");
    qRegisterMetaType<DobotV3_SendData>("DobotV3_SendData");
    qRegisterMetaType<DobotV3_ReadData>("DobotV3_ReadData");
    qRegisterMetaType<DobotV3CmdInter_CB>("DobotV3CmdInter_CB");
    qRegisterMetaType<SDobotV3CmdInterCBParm>("SDobotV3CmdInterCBParm");

    m_timerExec = new QTimer(this);
    m_timerExec->setSingleShot(true);
    m_timerExec->setInterval(5);
    connect(m_timerExec, &QTimer::timeout, this, &P3FrameAdapter::execTimeout_slot);

    m_timerCNT = new QTimer(this);
    m_timerCNT->setSingleShot(true);
    m_timerCNT->setInterval(10);
    connect(m_timerCNT, &QTimer::timeout, this, &P3FrameAdapter::cntTimeout_slot);

    DobotV3_SetSendParm(3, 4000);
    m_linkPath = DobotV3_RegisterLink(sendData, readData);
}

void P3FrameAdapter::execTimeout_slot()
{
    DobotV3_Exec();
    m_timerExec->start();
}

void P3FrameAdapter::cntTimeout_slot()
{
    DobotV3_TimeCNT(10);
    m_timerCNT->start();
}

P3FrameAdapter::~P3FrameAdapter()
{
    m_thread->quit();

    DobotV3_DestoryLink(m_linkPath);
    if (m_timerExec->isActive()) {
        m_timerExec->stop();
    }
    if (m_timerCNT->isActive()) {
        m_timerCNT->stop();
    }
    sm_recviveBytes.clear();
}

void P3FrameAdapter::setComPars_slot(quint8 resendTimesMax, quint32 commTimeout)
{
    DobotV3_SetSendParm(resendTimesMax, commTimeout);
}

void P3FrameAdapter::registerGeneralCB(quint16 id, DobotV3CmdInter_CB cb)
{
    General_RegisterCB(id, cb);
}

void P3FrameAdapter::registerMagicianGoCB(quint16 id, DobotV3CmdInter_CB cb)
{
    MGo_RegisterCB(id, cb);
}

void P3FrameAdapter::registerCarCameraCB(quint16 id, DobotV3CmdInter_CB cb)
{
    K210Car_RegisterCB(id, cb);
}

void P3FrameAdapter::registerArmCameraCB(quint16 id, DobotV3CmdInter_CB cb)
{
    K210Arm_RegisterCB(id, cb);
}

void P3FrameAdapter::registerMagicBoxCB(quint16 id, DobotV3CmdInter_CB cb)
{
    MBox_RegisterCB(id, cb);
}

// 数据往前端发
void P3FrameAdapter::addRawData_slot(QByteArray data)
{
    sm_recviveBytes.append(data);
}

//注册的读函数，数据网前端发
uint16_t P3FrameAdapter::readData(uint8_t *dataArray, uint16_t dataLength)
{
    Q_UNUSED(dataLength)
    //获取数据dataArray,读取dataLength长度，返回真正长度
    if (!sm_recviveBytes.length()){
        return 0;
    }

    QByteArray _byteArry = sm_recviveBytes.at(0);
    uint16_t realLength = static_cast<uint16_t>(_byteArry.length());
    if (realLength) {
        memcpy(dataArray, reinterpret_cast<uint8_t *>(_byteArry.data()), realLength);
        sm_recviveBytes.takeFirst();
    }

    return realLength;
}

// 数据往设备发
void P3FrameAdapter::sendCmd_slot(DobotV3Format_Packet pPacket, quint16 payloadLen, bool isAck)
{
    DobotV3_SendCmd(m_linkPath, &pPacket, payloadLen, isAck);
}

//注册的写函数，数据往设备发
void P3FrameAdapter::sendData(uint8_t *dataArray, uint16_t dataLength)
{
    QByteArray data(reinterpret_cast<char *>(dataArray), dataLength);

    // *****恶心的处理方式，小心处理，P3协议栈接口问题************
    QByteArray seqNumBytes = data.mid(6, 4);
    quint32 *seqNum = reinterpret_cast<quint32*>(seqNumBytes.data());
    // *****恶心的处理方式，小心处理，P3协议栈接口问题************

    emit sm_instance->sendRawData_signal(*seqNum, data);
}
