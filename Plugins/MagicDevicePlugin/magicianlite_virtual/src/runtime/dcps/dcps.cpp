/**
 ***********************************************************************************************************************
 *
 * @author ZhangRan
 * @date   2021/06/20
 *
 * <h2><center>&copy; COPYRIGHT 2021 YUE JIANG TECHNOLOGY</center></h2>
 *
 ***********************************************************************************************************************
 */

#include "dcps.h"

#include <cstring>
#include <cassert>
#include <stdexcept>
#include <log/log.h>
#include "algorithm/variable.h"
#include "cmd_id.h"

#define Assert(x) assert(x)

extern uint32_t QueuedCmdTypeCalc(uint32_t protocolID);
extern bool QueuedCmdIsFinished(SysParams *sysParams);
extern void QueuedCmdFinishFlagClear(SysParams *sysParams);
extern void QueuedCmdFinishFlagRefresh(SysParams *sysParams);

/*
 *----------------------------------------------------------------------------------------------------------------------
 *                                              Packet
 *----------------------------------------------------------------------------------------------------------------------
 */

Packet::Packet()
{
    memset(buf_, 0, MAX_PACKET_SIZE);
    hdr_ = reinterpret_cast<PacketHeader*>(buf_);
}

Packet::Packet(uint8_t* buf)
{
    memcpy(buf_, buf, MAX_PACKET_SIZE);
    hdr_ = reinterpret_cast<PacketHeader*>(buf_);
}

uint8_t Packet::getID() const
{
    return hdr_->id;
}

void Packet::init(Packet* pkt)
{
    *hdr_ = *pkt->getHdr();
    hdr_->length = 2;
}

PacketHeader* Packet::getHdr() const
{
    return hdr_;
}

void Packet::serialize(uint8_t id, bool is_write, bool need_queue, void* payload, uint8_t length) const
{
    Assert(hdr_);
    Assert(hdr_->length < MAX_PACKET_SIZE - sizeof(PacketHeader));

    hdr_->eof1 = 0xAA;
    hdr_->eof2 = 0xAA;
    hdr_->id = id;
    hdr_->rw = is_write ? 1 : 0;
    hdr_->queue = need_queue ? 1 : 0;
    memcpy(hdr_->data, payload, length);
    hdr_->length = length + 2;
    hdr_->data[hdr_->length - 2] = checkSum(&hdr_->id, hdr_->length);
}

void Packet::serialize()
{
    Assert(hdr_);
    Assert(hdr_->length >= 2 && hdr_->length < MAX_PACKET_SIZE - sizeof(PacketHeader));

    hdr_->data[hdr_->length - 2] = checkSum(&hdr_->id, hdr_->length);
}

void Packet::setPayload(const void* buf, uint16_t length)
{
    Assert(hdr_);
    Assert(length < MAX_PACKET_SIZE - sizeof(PacketHeader));

    memcpy(hdr_->data, buf, length);
    hdr_->length = static_cast<uint8_t>(length + 2);
}

bool Packet::isWrite() const
{
    return hdr_->rw;
}

bool Packet::isQueue() const
{
    return hdr_->queue;
}

void* Packet::data() const
{
    Assert(hdr_);
    return hdr_;
}

void* Packet::payload() const
{
    return hdr_->data;
}

uint16_t Packet::length() const
{
    Assert(hdr_);
    return hdr_->length + 4;
}

uint8_t Packet::checkSum(const void* buf, uint8_t len)
{
    Assert(buf);

    uint8_t ret = 0;
    const auto* tmp = static_cast<const uint8_t*>(buf);
    for (uint8_t i = 0; i < len; i++)
        ret += tmp[i];

    return -ret;
}

/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------------------------------------------------
 */

DobotProtocolSlave::DobotProtocolSlave()
    : recv_state_(State::EOF1), check_sum_(0), recv_index_(0), recv_expect_length_(0)
{
#ifdef USE_VIRTUAL_SERIAL_PORT
    serial_port_ = new CVirtualSerialPort(this);
#else
    serial_port_ = new QSerialPort(this);
#endif
    msg_queue_ = new MsgQueue<Packet>(500);
}

void DobotProtocolSlave::init(const QString& strPortName)
{
    serial_port_->setBaudRate(115200);
#ifdef USE_VIRTUAL_SERIAL_PORT
    serial_port_->setPortName(strPortName);
#else
    serial_port_->setPortName("COM4");
    serial_port_->setDataBits(QSerialPort::Data8);
    serial_port_->setParity(QSerialPort::NoParity);
    serial_port_->setFlowControl(QSerialPort::NoFlowControl);
#endif
    if (!serial_port_->open(QIODevice::ReadWrite))
        throw std::logic_error(serial_port_->errorString().toStdString().c_str());

    connect(serial_port_, &QIODevice::readyRead, this, &DobotProtocolSlave::readyRecv);
    connect(this, &DobotProtocolSlave::recvPacket, this, &DobotProtocolSlave::packetProc);

    setLogLevel(LogLevel::LOG_INFO);
}

void DobotProtocolSlave::setPortName(const QString& strPortName)
{
#ifdef USE_VIRTUAL_SERIAL_PORT
    serial_port_->setPortName(strPortName);
#endif
}

void DobotProtocolSlave::packetProc(uint8_t* packet)
{
    Packet request(packet);

    DobotCmdManage* mgr = DobotCmdManage::instance();
    IDobotCmd* cmd = mgr->findCmd(request.getID());
    if (cmd == nullptr)
    {
        DOBOT_WARN("unsupported cmd : %d", request.getID());
    }
    else
    {
        Packet response;
        response.init(&request);

        if (request.isQueue())
        {
            DOBOT_WARN("push queue cmd : %d", request.getID());
            auto msg_ptr = std::make_shared<Packet>(packet);
            uint64_t index = genIndexID();
            msg_ptr->msg_id = index;
            msg_queue_->push(msg_ptr);
            response.setPayload(&index, sizeof(index));
            DOBOT_WARN("return queue index %ld", index);
        }
        else
        {
            cmd->slave_ = this;
            if (!cmd->exec(&request, &response))
            {
                DOBOT_ERR("cmd %d process failed", cmd->getID());
            }
        }

        response.serialize();
        serial_port_->write((const char*)response.data(), response.length());

//        printf("send data : ");
//        for (int32_t i = 0; i < response.length(); i++)
//        {
//            printf("0x%02hhX ", ((const char*)response.data())[i]);
//        }
//        printf("\n\n");
    }
}

void DobotProtocolSlave::readyRecv()
{
    QByteArray dat = serial_port_->readAll();

    char* buf = dat.data();
//    printf("\n\nrecv data : ");
//    for (int32_t i = 0; i < dat.size(); i++)
//    {
//        printf("0x%02hhX ", buf[i]);
//    }
//    printf("\n");

    for (int32_t i = 0; i < dat.size(); i++)
    {
        parse(buf[i]);
    }
}

void DobotProtocolSlave::parse(uint8_t dat)
{
    switch (recv_state_)
    {
        case State::EOF1:
            if (dat == 0xAA)
            {
                recv_index_ = 0;
                recv_buf_[recv_index_++] = dat;
                recv_state_ = State::EOF2;
            }
            else
            {
                DOBOT_ERR("recv invalid data");
            }
            break;

        case State::EOF2:
            if (dat == 0xAA)
            {
                recv_buf_[recv_index_++] = dat;
                recv_state_ = State::LENGTH;
            }
            else
            {
                recv_state_ = State::EOF1;
                DOBOT_ERR("recv invalid data");
            }
            break;

        case State::LENGTH:
            if (dat < 2 || dat > 250)
            {
                recv_state_ = State::EOF1;
                DOBOT_ERR("Invalid length");
            }
            else
            {
                check_sum_ = 0;
                recv_expect_length_ = dat + 4;
                recv_buf_[recv_index_++] = dat;
                recv_state_ = State::REMAIN;
            }
            break;

        case State::REMAIN:
            check_sum_ += dat;
            recv_buf_[recv_index_++] = dat;
            if (recv_index_ == recv_expect_length_)
            {
                if (check_sum_ == 0)
                {
                    emit recvPacket(recv_buf_);
                }
                else
                {
                    DOBOT_ERR("invalid data");
                }

                recv_state_ = State::EOF1;
            }
            break;
    }
}

void DobotProtocolSlave::doQueueCmd()
{
    SysParams *sysParams = &gSysParams;
    if(QueuedCmdIsFinished(sysParams) == false) {
        QueuedCmdFinishFlagRefresh(sysParams);
        return;
    }
    if (!finishFlag)
        return;

    if (!gSysParams.runQueuedCmd)
        return;

    if (msg_queue_->getLength() > 0)
    {
        std::shared_ptr<Packet> request;
        try{
            request = msg_queue_->pop();
        }catch(...){}
        if (!request.get()){
            return;
        }
        current_proc_packet_ = request;

        DobotCmdManage* mgr = DobotCmdManage::instance();
        IDobotCmd* cmd = mgr->findCmd(request->getID());
        if (cmd == nullptr)
        {
            DOBOT_WARN("unsupported cmd : %d", request->getID());
        }
        else
        {
            Packet response;

            cmd->slave_ = this;
            gSysParams.queuedCmdType = (QueuedCmdType)QueuedCmdTypeCalc(cmd->getID());
            if (!cmd->exec(request.get(), &response))
            {
                DOBOT_ERR("cmd %d process failed", cmd->getID());
            }
            QueuedCmdFinishFlagClear(&gSysParams);
        }
    }
}

DobotProtocolSlave* DobotProtocolSlave::instance()
{
    static DobotProtocolSlave slave;
    return &slave;
}

uint64_t DobotProtocolSlave::genIndexID()
{
    static uint64_t idx = 1;
    idx++;
    return idx;
}

uint64_t DobotProtocolSlave::getQueueIndex() const
{
    uint64_t currentIndex = -1;
    if (finishFlag && current_proc_packet_)
        currentIndex = current_proc_packet_->msg_id;
    if(QueuedCmdIsFinished(&gSysParams) == false)
    {
        currentIndex -= 1;
    }
    return currentIndex;
}

void DobotProtocolSlave::clearQueue()
{
    msg_queue_->clean();
}

uint64_t DobotProtocolSlave::getLeftSpace() const
{
    return msg_queue_->getSize() - msg_queue_->getLength();
}

/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------------------------------------------------
 */

IDobotCmd::IDobotCmd(uint8_t id) : id_(id)
{
    DobotCmdManage::instance()->registerCmd(this);
}

uint8_t IDobotCmd::getID() const
{
    return id_;
}

/*
 *----------------------------------------------------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------------------------------------------------
 */
DobotCmdManage* DobotCmdManage::instance()
{
    static DobotCmdManage mgr;
    return &mgr;
}

void DobotCmdManage::registerCmd(IDobotCmd* cmd)
{
    cmd_map_[cmd->getID()] = cmd;
}

IDobotCmd* DobotCmdManage::findCmd(uint8_t id)
{
    auto iter = cmd_map_.find(id);
    if (iter == cmd_map_.end())
        return nullptr;
    else
        return iter->second;
}
