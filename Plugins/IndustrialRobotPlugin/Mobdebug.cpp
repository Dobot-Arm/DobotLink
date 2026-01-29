#include "Mobdebug.h"
#include "DError/DError.h"
#include <QCoreApplication>
#include <QNetworkDatagram>
#include <QDebug>
#include <QTime>
#include <QtConcurrent>
#include <QSet>
#ifdef Q_OS_ANDROID
#include <moblistenworker.h>
#include <printerlistenworker.h>
#include <QThread>
#endif

#define MOBDEBUG_START_TIMEOUT 5000
#define MOBDEBUG_PROCESS_START_TIMEOUT 100
#define MOBDEBUG_SERVE_DELDAY 100


//一个DobotLink控制多台设备时，udp绑定的端口是固定的，所以这几个变量要共享
static QUdpSocket *g_pUdpPrint = nullptr;
static QUdpSocket *g_pUdp1Cursor = nullptr;
static QUdpSocket *g_pUdp2Cursor = nullptr;
static QUdpSocket *g_pUdpSpecial = nullptr;
static QUdpSocket *g_pUdpBlockHighlight = nullptr;

static QSet<Mobdebug*> g_AllMobdebug;

static bool isCanConvert2Utf8(const QByteArray& arr)
{
    int nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
    bool bAllAscii = true;
    for (int i=0; i<arr.size(); ++i)
    {
        unsigned char chr = static_cast<unsigned char>(arr[i]);
        //判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
        if (nBytes == 0 && (chr & 0x80) != 0) bAllAscii = false;
        if (nBytes == 0)
        {
            //如果不是ASCII码,应该是多字节符,计算字节数
            if (chr >= 0x80)
            {
                if (chr >= 0xFC && chr <= 0xFD) nBytes = 6;
                else if (chr >= 0xF8) nBytes = 5;
                else if (chr >= 0xF0) nBytes = 4;
                else if (chr >= 0xE0) nBytes = 3;
                else if (chr >= 0xC0) nBytes = 2;
                else return false;
                nBytes--;
            }
        }
        else
        {
            //多字节符的非首字节,应为 10xxxxxx
            if ((chr & 0xC0) != 0x80) return false;
            //减到为零为止
            nBytes--;
        }
    }
    //违返UTF8编码规则
    if (nBytes != 0) return false;
    if (bAllAscii)
    { //如果全部都是ASCII, 也是UTF8
        return true;
    }
    return true;
}

/*在脚本编程中，udp接收来的消息，可能包含中文字符，且不是utf8编码，
 * 结果这条消息内容打印到DobotLink界面上时，引起崩溃，这个是Qt本身的bug。
 * bugReport: https://bugreports.qt.io/browse/QTBUG-57180
 * 为了解决这个问题，折中方式，可以先判断接收到的内容是否为Utf8，不是就写成乱码
*/
static QString bytesToUtf8(const QByteArray& arr)
{
    if (arr.isEmpty()) return "";
    if (isCanConvert2Utf8(arr))
    {
        return QString::fromUtf8(arr);
    }
    return QString(5, '?');
}

Mobdebug::Mobdebug(QObject *parent) : QObject(parent)
{
    m_state = MODEBUG_IDLE;
    m_handlingId = 0;
    m_isStarting = false;
    m_timerMobdebug = new QTimer(this);
    m_timerMobdebug->setInterval(MOBDEBUG_START_TIMEOUT);
    m_timerMobdebug->setSingleShot(true);

    _processInit();
    _udpInit();

    if (m_process)
    {
        QString strLuaProgram = m_process->program();
        QtConcurrent::run([strLuaProgram]{
            QProcess *p = new QProcess();
            p->setProcessChannelMode(QProcess::MergedChannels);
            connect(p, &QProcess::readyRead, [p]{
                QTextCodec *codec = QTextCodec::codecForName("GBK");
                //qDebug().noquote().nospace()<<"QProcess::readyRead-->"<<codec->toUnicode(p->readAll());
            });
            connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),[](int code, QProcess::ExitStatus sta){
                //qDebug().noquote().nospace()<<"QProcess::finished-->"<<code<<","<<sta;
            });
            connect(p, &QProcess::errorOccurred, [](QProcess::ProcessError error){
                //qDebug().noquote().nospace()<<"QProcess::errorOccurred-->"<<error;
            });

            //先删除，防止太多重复的
            p->start("netsh advfirewall firewall delete rule name=\"lua\"");
            p->waitForFinished(5000);

            QThread::sleep(1);

            QString strTcpAdd = QString("netsh advfirewall firewall add rule name=\"lua\" dir=in "
                            "action=allow program=\"%1\" enable=yes profile=any "
                            "protocol=TCP localport= 8172")
                            .arg(QDir::toNativeSeparators(strLuaProgram));

            qDebug().noquote().nospace()<<strTcpAdd<<'\n';

            p->start(strTcpAdd);
            p->waitForFinished(5000);
            p->deleteLater();
        });
    }
}

Mobdebug::~Mobdebug()
{
    udpClose();
}

void Mobdebug::_processInit()
{

#ifdef __arm__
    QDir dir("/dobot/bin/bin");
#else
    QString appPath = QCoreApplication::applicationDirPath().remove("_d");
    qDebug()<<"mobdebug appPath:"<<appPath;
    QDir dir(appPath);
#ifdef Q_OS_MAC
    dir.cdUp();
    dir.cd("Resources");
#endif
    if (dir.exists("tool")) {
        dir.cd("tool");
#ifdef Q_OS_WIN
        dir.cd("Lua");
#elif defined (Q_OS_MAC)
        dir.cd("lua-mac");
#elif defined (Q_OS_LINUX)
        dir.cd("lua-linux");
#endif
    } else {
        qDebug() << "work space error:" << dir;
    }
#endif

    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setWorkingDirectory(dir.absolutePath());
    connect(m_process, &QProcess::started, this, &Mobdebug::onProcessStarted_slot);
    connect(m_process, &QProcess::readyRead, this, &Mobdebug::onProcessReadyRead_slot);
    connect(m_process, &QProcess::errorOccurred, this, &Mobdebug::onProcessErrorOccurred_slot);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &Mobdebug::onProcessFinished_slot);
    connect(m_process, &QProcess::stateChanged, this, &Mobdebug::onProcessStateChanged);

#ifdef Q_OS_WIN
    QString program = "Lua.exe";
#elif defined (Q_OS_MAC)
    QString program = "lua";
#elif defined (Q_OS_ANDROID)
    QString program = "lua";
#elif defined (Q_OS_LINUX)
    QString program = "lua";
#endif

    if (dir.exists(program)) {
        m_process->setProgram(dir.absoluteFilePath(program));
    } else {
#ifdef Q_OS_ANDROID
        printerWorker =new PrinterListenWorker();
        QThread *printListenThread =new QThread();
        printerWorker->setLuaMobile(luaMobile);
        printerWorker->moveToThread(printListenThread);
        //operate信号发射后启动线程工作
        connect(this, SIGNAL(operate(const int)), printerWorker, SLOT(doListenWork(int)));
        //该线程结束时销毁
        connect(printListenThread, &QThread::finished, printerWorker, &QObject::deleteLater);
        //线程结束后发送信号，对结果进行处理
        connect(printerWorker, SIGNAL(hasInNeedPrint(const char *)), this, SLOT(handlePrintRes(const char*)));
        //启动线程
        printListenThread->start();

        luaMobile =*new LuaMobile();
        luaMobile.qLuaDoString("print(package.path)");
        luaMobile.qLuaDoString("package.path ='/usr/local/share/lua/5.3/?.lua;/usr/local/share/lua/5.3/?/init.lua;/usr/local/lib/lua/5.3/?.lua;/usr/local/lib/lua/5.3/?/init.lua;./?.lua;./?/init.lua;/storage/emulated/0/LuaQt/?.lua'");
#endif
        qDebug() << "Lua:" << program << "path is not correct.";
    }

}

void Mobdebug::_udpInit()
{
    if (!g_pUdpPrint)
    {
        g_pUdpPrint = new QUdpSocket();
        g_pUdpPrint->setProperty("id", "ClientMsg");
        connect(g_pUdpPrint, &QUdpSocket::readyRead, &Mobdebug::readPendingClientMsg_slot);
    }

    if (!g_pUdp1Cursor)
    {
        g_pUdp1Cursor = new QUdpSocket(this);
        g_pUdp1Cursor->setProperty("id", "1stCursorMsg");
        connect(g_pUdp1Cursor, &QUdpSocket::readyRead, &Mobdebug::readPending1stCursorMsg_slot);
    }

    if (!g_pUdp2Cursor)
    {
        g_pUdp2Cursor = new QUdpSocket(this);
        g_pUdp2Cursor->setProperty("id", "2ndCursorMsg");
        connect(g_pUdp2Cursor, &QUdpSocket::readyRead, &Mobdebug::readPending2ndCursorMsg_slot);
    }

    if (!g_pUdpSpecial)
    {
        g_pUdpSpecial = new QUdpSocket(this);
        g_pUdpSpecial->setProperty("id", "SpecialMsg");
        connect(g_pUdpSpecial, &QUdpSocket::readyRead, &Mobdebug::readPendingSpecialMsg_slot);
    }

    if (!g_pUdpBlockHighlight)
    {
        g_pUdpBlockHighlight = new QUdpSocket(this);
        g_pUdpBlockHighlight->setProperty("id", "BlockHighlightId");
        connect(g_pUdpBlockHighlight, &QUdpSocket::readyRead, &Mobdebug::readBlockHighlightId_slot);
    }
}

bool Mobdebug::start(quint64 id)
{
    qDebug() << __FUNCTION__ << "start" << m_process->program();
    m_handlingId = id;
#ifndef Q_OS_ANDROID
    QStringList arguments;
#ifdef __arm__
    arguments << "-e" << "package.cpath = \"/dobot/bin/lib/?.so;\"..package.cpath;package.path = \"/dobot/bin/lua/?.lua;\"..package.path;require('mobdebug').listen()";
#else
    arguments << "-e" << "require('mobdebug').listen()";
#endif

    m_process->setArguments(arguments);
    m_process->start();
    bool ok = m_process->waitForStarted(MOBDEBUG_PROCESS_START_TIMEOUT);
    qDebug() << __FUNCTION__ << "end" << ok;
    m_isStarting = true;
    return ok;
#else
    printerWorker->setRun(true);
    QThread *listenThread=new QThread();
    MobListenWorker *worker=new MobListenWorker();
    worker->setLuaMobile(luaMobile);
    worker->moveToThread(listenThread);
    //operate信号发射后启动线程工作
    connect(this, SIGNAL(operate(const int)), worker, SLOT(doWork(int)));
    //该线程结束时销毁
    connect(listenThread, &QThread::finished, worker, &QObject::deleteLater);
    //线程结束后发送信号，对结果进行处理
    connect(worker, SIGNAL(finishResult(bool)), this, SLOT(handleResults(bool)));
    //启动线程
    listenThread->start();
    emit operate(0);
    emit operate(1);
//    qDebug()<<"mobDir is exists:"<<mobDir.exists();
//    luaMobile.qLuaDoString("print(package.path)");
//    luaMobile.qLuaDoString("print(package.cpath)");
//    QString cmd="require(\"mobdebug\").listen('0.0.0.0',8172)";
//    bool ok=luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
    return true;
#endif
}

bool Mobdebug::udpOpen(const QString& strDeviceIpAddress)
{
    bool bOk = true;

    if (QUdpSocket::SocketState::BoundState != g_pUdpPrint->state())
    {
#ifdef __arm__
        if (g_pUdpPrint->bind(QHostAddress::Any, 5000)) {
#else
        if (g_pUdpPrint->bind(5000)) {
#endif
            qDebug() << "bind 5000 sucess.";
        } else {
            qDebug() << "bind 5000 failed.";
            bOk = false;
        }
    }

    if (QUdpSocket::SocketState::BoundState != g_pUdp1Cursor->state()) {
#ifdef __arm__
        if (g_pUdp1Cursor->bind(QHostAddress::Any, 5001)) {
#else
        if (g_pUdp1Cursor->bind(5001)) {
#endif
            qDebug() << "bind 5001 sucess.";
        } else {
            qDebug() << "bind 5001 failed.";
            bOk = false;
        }
    }

    if (QUdpSocket::SocketState::BoundState != g_pUdp2Cursor->state()) {
#ifdef __arm__
        if (g_pUdp2Cursor->bind(QHostAddress::Any, 5002)) {
#else
        if (g_pUdp2Cursor->bind(5002)) {
#endif
            qDebug() << "bind 5002 sucess.";
        } else {
            qDebug() << "bind 5002 failed.";
            bOk = false;
        }
    }

    if (QUdpSocket::SocketState::BoundState != g_pUdpSpecial->state()) {
#ifdef __arm__
        if (g_pUdpSpecial->bind(QHostAddress::Any, 5003)) {
#else
        if (g_pUdpSpecial->bind(5003)) {
#endif
            qDebug() << "bind 5003 sucess.";
        } else {
            qDebug() << "bind 5003 failed.";
            bOk = false;
        }
    }

    if (QUdpSocket::SocketState::BoundState != g_pUdpBlockHighlight->state()) {
#ifdef __arm__
        if (g_pUdpBlockHighlight->bind(QHostAddress::Any, 5004)) {
#else
        if (g_pUdpBlockHighlight->bind(5004)) {
#endif
            qDebug() << "bind 5004 sucess.";
        } else {
            qDebug() << "bind 5004 failed.";
            bOk = false;
        }
    }

    //1. 成功打开了所有udp后，就需要将 this保存起来，当closeudp时，就要删掉this，当所有this都删完了，就真的关掉所有udp
    //2. udp收到消息时，要判断ip是否为 m_strDeviceIpAddress，如果是，则消息发出去，否则消息不发，不然就出现消息串台了。
    m_strDeviceIpAddress = strDeviceIpAddress;
    g_AllMobdebug.insert(this);

    return bOk;
}

void Mobdebug::udpClose()
{
    g_AllMobdebug.remove(this);
    if (g_AllMobdebug.isEmpty())
    {
        if (QUdpSocket::SocketState::BoundState == g_pUdpPrint->state())
        {
            g_pUdpPrint->abort();
        }
        if (QUdpSocket::SocketState::BoundState == g_pUdp1Cursor->state())
        {
            g_pUdp1Cursor->abort();
        }
        if (QUdpSocket::SocketState::BoundState == g_pUdp2Cursor->state())
        {
            g_pUdp2Cursor->abort();
        }
        if (QUdpSocket::SocketState::BoundState == g_pUdpSpecial->state())
        {
            g_pUdpSpecial->abort();
        }
        if (QUdpSocket::SocketState::BoundState == g_pUdpBlockHighlight->state())
        {
            g_pUdpBlockHighlight->abort();
        }
    }

    m_strDeviceIpAddress = "";
}

void Mobdebug::mo_run(quint64 id)
{
    qDebug() << __FUNCTION__;
#ifndef Q_OS_ANDROID
    _processWrite("run", id);
#else
    QString cmd="require(\"mobdebug\").cmd('run')";
    qDebug()<<luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
#endif
}

void Mobdebug::mo_suspend(quint64 id)
{
    qDebug() << __FUNCTION__;
#ifndef Q_OS_ANDROID
    _processWrite("suspend", id);
#else
    QString cmd="require(\"mobdebug\").cmd('suspend')";
    qDebug()<<luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
#endif
}

void Mobdebug::mo_step(quint64 id)
{
    qDebug() << __FUNCTION__;
#ifndef Q_OS_ANDROID
    _processWrite("step", id);
#else
    QString cmd="require(\"mobdebug\").cmd('step')";
    qDebug()<<luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
#endif
}

void Mobdebug::mo_over(quint64 id)
{
    qDebug() << __FUNCTION__;
#ifndef Q_OS_ANDROID
    _processWrite("over", id);
#else
    QString cmd="require(\"mobdebug\").cmd('over')";
    qDebug()<<luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
    printerWorker->setRun(false);
#endif
}

void Mobdebug::mo_eval(QString exp, quint64 id)
{

    qDebug() << __FUNCTION__;
#ifndef Q_OS_ANDROID
    _processWrite("eval " + exp, id);
#else
    QString cmd="require(\"mobdebug\").cmd('eval "+exp+"')";
    qDebug()<<luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
#endif
}

void Mobdebug::mo_exec(QString stmt, quint64 id)
{
    qDebug() << __FUNCTION__;
#ifndef Q_OS_ANDROID
    _processWrite("exec " + stmt, id);
#else
    QString cmd="require(\"mobdebug\").cmd('exec "+stmt+"')";
    qDebug()<<luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
#endif
}

void Mobdebug::mo_setb(QString file, int line, quint64 id)
{
    qDebug() << __FUNCTION__;
#ifndef Q_OS_ANDROID
    _processWrite(QString("setb %1 %2").arg(file).arg(line), id);
#else
    QString cmd="require(\"mobdebug\").cmd('"+QString("setb %1 %2").arg(file).arg(line)+"')";
    qDebug()<<luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
#endif
}

void Mobdebug::mo_delb(QString file, int line, quint64 id)
{
    qDebug() << __FUNCTION__;
#ifndef Q_OS_ANDROID
    _processWrite(QString("delb %1 %2").arg(file).arg(line), id);
#else
    QString cmd="require(\"mobdebug\").cmd('"+QString("delb %1 %2").arg(file).arg(line)+"')";
    qDebug()<<luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
#endif
}

void Mobdebug::mo_delallb(quint64 id)
{
    qDebug() << __FUNCTION__;
#ifndef Q_OS_ANDROID
    _processWrite("delallb", id);
#else
    QString cmd="require(\"mobdebug\").cmd('delallb')";
    qDebug()<<luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
#endif
}

void Mobdebug::mo_stack(quint64 id)
{
    qDebug() << __FUNCTION__;
#ifndef Q_OS_ANDROID
    _processWrite("stack", id);
#else
    QString cmd="require(\"mobdebug\").cmd('stack')";
    qDebug()<<luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
#endif
}

void Mobdebug::mo_help(quint64 id)
{
    qDebug() << __FUNCTION__;
#ifndef Q_OS_ANDROID
    _processWrite("help", id);
#else
    QString cmd="require(\"mobdebug\").cmd('help')";
    qDebug()<<luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
#endif
}

void Mobdebug::mo_exit(quint64 id)
{
    qDebug() << __FUNCTION__;
    _killProcess();
    //抛出状态改变信号
    this->m_state = MODEBUG_STOP;
    emit onModebugStateChanged_signal(m_state, id);
    emit onFinish_signal(id);
}

void Mobdebug::_processWrite(QString cmd, quint64 id)
{
    if (id > 0) {
        if (m_process == nullptr) {
            emit onErrorOccured_signal(id, ERROR_MOBDEBUG_PROCESS_NOT_RUNNING);
        }

        if (m_process->state() != QProcess::Running) {
            emit onErrorOccured_signal(id, ERROR_MOBDEBUG_PROCESS_NOT_RUNNING);
        }
    }

    m_handlingId = id;
    m_process->write((cmd + "\r\n").toUtf8());
}

//![process slot]
void Mobdebug::onProcessStarted_slot()
{
    qDebug() << __FUNCTION__;
}

void Mobdebug::onProcessReadyRead_slot()
{
    QString readStr = QString::fromLatin1(m_process->readAll());
    emit onModebugMessage_signal(readStr);
    qDebug() << ">>[process]" << readStr;

    // mobdebug bind failed
    if (readStr.contains("Can't bind")) {
        _killProcess();
        this->m_state = MODEBUG_STOP;
        emit onErrorOccured_signal(m_handlingId, ERROR_MOBDEBUG_TCP_BIND_FAILED);
        return;
    }

    if (readStr.contains("Server started")) {
        m_state = MODEBUG_STARTED;
        emit onModebugStateChanged_signal(m_state, m_handlingId);
    }
    if (readStr.contains("Waitting client")) {
        m_state = MODEBUG_LISTENING;
        QTimer::singleShot(MOBDEBUG_SERVE_DELDAY, this, [=]() {
            if (!m_isStarting) {
                return;
            }
            emit onModebugStateChanged_signal(m_state, m_handlingId);
            m_timerMobdebug->start();
            connect(m_timerMobdebug, &QTimer::timeout, [=]() {
                qDebug() << "waitting client timeout!!!!!!!!";
                if (!m_isStarting) {
                    return;
                }
                m_isStarting = false;
                this->m_state = MODEBUG_STOP;
                _killProcess();
                emit onErrorOccured_signal(m_handlingId, ERROR_MOBDEBUG_CLIENT_TIMEOUT);
            });
        });
    }
    if (readStr.contains("Client connected")) {
        if (!m_isStarting) {
            return;
        }
        m_isStarting = false;
        m_timerMobdebug->stop();
        m_state = MODEBUG_CONNECTED;
        emit onModebugStateChanged_signal(m_state, m_handlingId);
        emit onFinish_signal(m_handlingId);
    }
    if (readStr.contains("Comm: run")) {
        m_state = MODEBUG_RUNNING;
        emit onModebugStateChanged_signal(m_state, m_handlingId);
        emit onFinish_signal(m_handlingId);
    }
    if (readStr.contains("Comm: suspend")) {
            m_state = MODEBUG_SUSPENDED;
            emit onModebugStateChanged_signal(m_state, m_handlingId);
    }
    if (readStr.contains("Server closed")) {
        m_state = MODEBUG_STOP;
        emit onModebugStateChanged_signal(m_state, m_handlingId);
        emit onFinish_signal(m_handlingId);
    }
}

#ifdef Q_OS_ANDROID
void Mobdebug::handlePrintRes(const char * res){
    QString readStr = res;
    emit onModebugMessage_signal(readStr);
    qDebug().noquote()<<"has print read Str:\n"<<readStr;

    if (readStr.contains("Server started")) {
        m_state = MODEBUG_STARTED;
        emit onModebugStateChanged_signal(m_state, m_handlingId);
    }
    if (readStr.contains("Waitting client")) {
        m_state = MODEBUG_LISTENING;
        emit onModebugStateChanged_signal(m_state, m_handlingId);
    }
    if (readStr.contains("Client connected")) {
        m_state = MODEBUG_CONNECTED;
        emit onModebugStateChanged_signal(m_state, m_handlingId);
        emit onFinish_signal(m_handlingId);
    }
    if (readStr.contains("Comm: run")) {
        m_state = MODEBUG_RUNNING;
        emit onModebugStateChanged_signal(m_state, m_handlingId);
        emit onFinish_signal(m_handlingId);
    }
    if (readStr.contains("Server closed")) {
        m_state = MODEBUG_STOP;
        emit onModebugStateChanged_signal(m_state, m_handlingId);
        emit onFinish_signal(m_handlingId);
    }

}
#endif

void Mobdebug::onProcessErrorOccurred_slot(QProcess::ProcessError error)
{
    qDebug() << Q_FUNC_INFO << error;
}

void Mobdebug::onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << Q_FUNC_INFO << exitCode << exitStatus;
    if (m_isStarting) {
        m_isStarting = false;
        emit onErrorOccured_signal(m_handlingId, ERROR_MOBDEBUG_PROCESS_CRASHED);
    }
}

void Mobdebug::onProcessStateChanged(QProcess::ProcessState newState)
{
    qDebug() << __FUNCTION__ << newState;
}

//![udp slot]
void Mobdebug::readPendingClientMsg_slot()
{
    QUdpSocket *socket = g_pUdpPrint;
    if (socket) {
        while (socket->hasPendingDatagrams()) {
            QNetworkDatagram datagram = socket->receiveDatagram();
            QString message = bytesToUtf8(datagram.data());
            QHostAddress host = QHostAddress(datagram.senderAddress().toIPv4Address());
            quint16 iPort = datagram.senderPort();
            qDebug() << "ip" << host.toString();
            qDebug() << "[udp]"<< iPort << ": " << message;
            // 过滤
#ifdef __arm__
            if (host.toString() == QHostAddress("::ffff:192.168.1.255%8").toString()){
                emit readPendingClientMsg_signal(socket->localPort(), message);
            }
#else
            QString strIp = host.toString();
            foreach(Mobdebug* pDebug, g_AllMobdebug)
            {
                if (strIp == pDebug->m_strDeviceIpAddress)
                {
                    emit pDebug->readPendingClientMsg_signal(socket->localPort(), message);
                }
            }
#endif
        }

    }
}

void Mobdebug::readPending1stCursorMsg_slot()
{
    QUdpSocket *socket = g_pUdp1Cursor;
    if (socket) {
        while (socket->hasPendingDatagrams()) {
            QNetworkDatagram datagram = socket->receiveDatagram();
            QString message = bytesToUtf8(datagram.data());
            QHostAddress host = QHostAddress(datagram.senderAddress().toIPv4Address());
            quint16 iPort = datagram.senderPort();
            qDebug() << "ip" << host.toString();
            qDebug() << "[udp]"<< iPort << ": " << message;
            // 过滤
#ifdef __arm__
            if (host.toString() == QHostAddress("::ffff:192.168.1.255%8").toString()){
                emit readPending1stCursorMsg_signal(socket->localPort(), message);
            }
#else
            QString strIp = host.toString();
            foreach(Mobdebug* pDebug, g_AllMobdebug)
            {
                if (strIp == pDebug->m_strDeviceIpAddress)
                {
                    emit pDebug->readPending1stCursorMsg_signal(socket->localPort(), message);
                }
            }
#endif
        }
    }
}

void Mobdebug::readPending2ndCursorMsg_slot()
{
    QUdpSocket *socket = g_pUdp2Cursor;
    if (socket) {
        while (socket->hasPendingDatagrams()) {
            QNetworkDatagram datagram = socket->receiveDatagram();
            QString message = bytesToUtf8(datagram.data());
            QHostAddress host = QHostAddress(datagram.senderAddress().toIPv4Address());
            quint16 iPort = datagram.senderPort();
            qDebug() << "ip" << host.toString();
            qDebug() << "[udp]"<< iPort << ": " << message;
            // 过滤
#ifdef __arm__
            if (host.toString() == QHostAddress("::ffff:192.168.1.255%8").toString()){
                emit readPending2ndCursorMsg_signal(socket->localPort(), message);
            }
#else
            QString strIp = host.toString();
            foreach(Mobdebug* pDebug, g_AllMobdebug)
            {
                if (strIp == pDebug->m_strDeviceIpAddress)
                {
                    emit pDebug->readPending2ndCursorMsg_signal(socket->localPort(), message);
                }
            }
#endif
        }
    }
}

void Mobdebug::readPendingSpecialMsg_slot()
{
    QUdpSocket *socket = g_pUdpSpecial;
    if (socket) {
        while (socket->hasPendingDatagrams()) {
            QNetworkDatagram datagram = socket->receiveDatagram();
            QString message = bytesToUtf8(datagram.data());
            QHostAddress host = QHostAddress(datagram.senderAddress().toIPv4Address());
            quint16 iPort = datagram.senderPort();
            qDebug() << "ip" << host.toString();
            qDebug() << "[udp]"<< iPort << ": " << message;
            // 过滤
#ifdef __arm__
            if (host.toString() == QHostAddress("::ffff:192.168.1.255%8").toString()){
                emit readPendingSpecialMsg_signal(socket->localPort(), message);
            }
#else
            QString strIp = host.toString();
            foreach(Mobdebug* pDebug, g_AllMobdebug)
            {
                if (strIp == pDebug->m_strDeviceIpAddress)
                {
                    emit pDebug->readPendingSpecialMsg_signal(socket->localPort(), message);
                }
            }
#endif
        }
    }
}

void Mobdebug::readBlockHighlightId_slot()
{
    QUdpSocket *socket = g_pUdpBlockHighlight;
    if (socket) {
        while (socket->hasPendingDatagrams()) {
            QNetworkDatagram datagram = socket->receiveDatagram();
            QString message = bytesToUtf8(datagram.data());
            QHostAddress host = QHostAddress(datagram.senderAddress().toIPv4Address());
            quint16 iPort = datagram.senderPort();
            qDebug() << "ip" << host.toString();
            qDebug() << "[udp]"<< iPort << ": " << message;
            // 过滤
#ifdef __arm__
            if (datagram.destinationAddress().toString() == QHostAddress("::ffff:192.168.1.255%8").toString()){
                emit readBlockHighlightId_signal(socket->localPort(), message);
            }
#else
            QString strIp = host.toString();
            foreach(Mobdebug* pDebug, g_AllMobdebug)
            {
                if (strIp == pDebug->m_strDeviceIpAddress)
                {
                    emit pDebug->readBlockHighlightId_signal(socket->localPort(), message);
                }
            }
#endif
        }
    }
}

void Mobdebug::sleep(int msec)
{
    QTime reachTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < reachTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
    }
}

void Mobdebug::_killProcess()
{
    qDebug() << __FUNCTION__;
#ifdef Q_OS_ANDROID
    QString cmd="require(\"mobdebug\").clear()";
    qDebug()<<luaMobile.qLuaDoString(cmd.toUtf8().toStdString().data());
    printerWorker->setRun(false);
#elif defined __arm__
    _processWrite("exit");
//    m_process->kill();
    m_process->waitForFinished();
#else
    //关闭子进程
    m_process->kill();
    m_process->waitForFinished();
#endif
    m_process->terminate();
}


