/**
 ***********************************************************************************************************************
 *
 * @author ZhangRan
 * @date   2021/06/09
 *
 * <h2><center>&copy; COPYRIGHT 2021 YUE JIANG TECHNOLOGY</center></h2>
 *
 ***********************************************************************************************************************
 */

#pragma once

#include <map>
#include <list>
#include <memory>
#include <cstdint>
#include "msg_queue.h"

#ifdef USE_VIRTUAL_SERIAL_PORT
#include "virtualport/VirtualSerialPort.h"
#else
#include <QSerialPort>
#endif

constexpr uint32_t MAX_PACKET_SIZE = 256;

#pragma pack(push)
#pragma pack(1)
struct PacketHeader
{
    uint8_t eof1;
    uint8_t eof2;
    uint8_t length;
    uint8_t id;
    uint8_t rw : 1;
    uint8_t queue : 1;
    uint8_t err : 1;
    uint8_t resv : 1;
    uint8_t dst : 2;    // 0 转发到 magician, 1 发给box自己的
    uint8_t dev : 2;
    uint8_t data[0];
};
#pragma pack(pop)

class Packet
{
private:
    PacketHeader* hdr_;
    uint8_t buf_[256];

public:
    uint64_t msg_id;

public:
    Packet();
    Packet(uint8_t* buf);

    void init(Packet* pkt);

    PacketHeader* getHdr() const;

    uint8_t getID() const;
    void serialize();
    void* data() const;
    void* payload() const;
    bool isWrite() const;
    bool isQueue() const;
    uint16_t length() const;
    void setPayload(const void* payload, uint16_t length);

    /**
     * serialize
     */
    void serialize(uint8_t id, bool is_write, bool need_queue, void* payload = nullptr, uint8_t length = 0) const;

private:
    static uint8_t checkSum(const void* buf, uint8_t len);
};

class DobotProtocolSlave;
class IDobotCmd
{
public:
    DobotProtocolSlave* slave_;

private:
    uint8_t id_;

public:
    explicit IDobotCmd(uint8_t id);
    virtual ~IDobotCmd() = default;

    uint8_t getID() const;

    virtual bool exec(const Packet* request, Packet* response) = 0;
};

class DobotCmdManage
{
private:
    std::map<uint8_t, IDobotCmd*> cmd_map_;

public:
    static DobotCmdManage* instance();

    void registerCmd(IDobotCmd* cmd);

    IDobotCmd* findCmd(uint8_t id);

private:
    DobotCmdManage() = default;
};

/**
 * DobotProtocolSlave
 */
class DobotProtocolSlave : public QObject
{
    Q_OBJECT

public:
    enum class State
    {
        EOF1 = 0,
        EOF2,
        LENGTH,
        REMAIN
    };

    static constexpr uint32_t MAX_PACKET_SIZE = 256;

private:
    State recv_state_;
    uint8_t check_sum_;
    uint8_t recv_index_;
    uint8_t recv_buf_[MAX_PACKET_SIZE]{};
    uint8_t recv_expect_length_;
    MsgQueue<Packet>* msg_queue_;
    std::shared_ptr<Packet> current_proc_packet_;

#ifdef USE_VIRTUAL_SERIAL_PORT
    CVirtualSerialPort* serial_port_;
#else
    QSerialPort* serial_port_;
#endif

public slots:
    void readyRecv();
    void packetProc(uint8_t* packet);

Q_SIGNALS:
    void recvPacket(uint8_t* packet);

public:
    static DobotProtocolSlave* instance();

    void init(const QString& strPortName="");

    /*
     * 改变虚拟串口号，只有在虚拟串口时才起作用
    */
    void setPortName(const QString& strPortName);

    void clearQueue();

    uint64_t getQueueIndex() const;

    uint64_t getLeftSpace() const;

    void doQueueCmd();

private:
    /**
     * Ctor
     */
    DobotProtocolSlave();
    void parse(uint8_t dat);
    uint64_t genIndexID();
};
