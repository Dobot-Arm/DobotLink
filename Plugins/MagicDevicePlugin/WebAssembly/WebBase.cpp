#include "WebBase.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <QCoreApplication>
#include <emscripten/bind.h>
#include <emscripten.h>
#include <QJsonDocument>
#include "Config.h"

static void ProcessEvents(int ms)
{
    if (ms <= 0)
        ms = 1;
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, ms);
}

EMSCRIPTEN_BINDINGS(WebBase)
{
    emscripten::function("ProcessEvents", ProcessEvents);
}

WebBase::WebBase() : QObject(nullptr),
                     m_id(0),
                     m_onRecv(nullptr)
{
    qDebug()<<"the WebBase is running"<<QThread::currentThreadId();

    static quint16 _wsport(12345);
    m_wsport = _wsport++;

    _initApp();

    _initWebExec();

    InitMagicDevicePlugin();
}

WebBase::~WebBase()
{
    qDebug()<<"WebBase::~WebBase";
    if (nullptr != m_pThdMagicDevicePlugin)
    {
        if (m_pThdMagicDevicePlugin->isRunning())
        {
            m_pThdMagicDevicePlugin->quit();
            m_pThdMagicDevicePlugin->wait(200);
        }
        delete m_pThdMagicDevicePlugin;
    }
    if (nullptr != m_pMagicDevicePlugin)
    {
        delete m_pMagicDevicePlugin;
    }
    if (nullptr != m_mainApp)
    {
        m_mainApp->quit();
        delete m_mainApp;
    }
}

void WebBase::_initApp()
{
    int argc = 0;
    char** argv = nullptr;
    m_mainApp = new QCoreApplication(argc, argv);
}

void WebBase::_initWebExec()
{
    qDebug().noquote() << QString("Setting Qt exec params, time: %1, interval: %2").arg(QT_EVENT_PROCESS_TIME).arg(QT_EVENT_PROCESS_INTERVAL);
    EM_ASM({
        setInterval(async() => {
            await Module.ProcessEvents($0);
        }, $1);
    }, QT_EVENT_PROCESS_TIME, QT_EVENT_PROCESS_INTERVAL);
}


void WebBase::InitMagicDevicePlugin()
{
    // 注意：MagicDevicePlugin不能在运行在主线程中
    //      1. qt事件处理函数嵌套在JS事件循环中，串口混编部分使用了await语法糖，百分百会造成死锁
    //      2. qt的定时器很容易造成多个JS事件同时等待的问题
    m_pMagicDevicePlugin = new MagicDevicePlugin();
    m_pThdMagicDevicePlugin = new QThread();
    m_pMagicDevicePlugin->moveToThread(m_pThdMagicDevicePlugin);
    m_pThdMagicDevicePlugin->start();

    connect(m_pMagicDevicePlugin, &MagicDevicePlugin::pSendMessage_signal, this, [this](QString module, QJsonObject obj){
        Q_UNUSED(module);
        if (!obj.contains("WSport")) {
            qWarning() << "Invaild json obj:" << obj;
            return;
        }
        if (obj.value("WSport") == m_wsport) {
            qDebug() << QString("<<%1:[send]").arg(m_wsport) << obj;
            QJsonValue recvValue;
            if (obj.contains("result")) {
                recvValue = obj.value("result");
            } else if (obj.contains("error")) {
                recvValue = obj.value("error");
            }
            if (m_onRecv) m_onRecv(obj.value("id").toDouble(), recvValue);
        }
    });
}

WebBase *WebBase::instance()
{
    qDebug()<<"WebBase::instance,thid="<<QThread::currentThreadId();
    static WebBase *_instance(nullptr);
    if (!_instance) {
        _instance = new WebBase();
    }
    return _instance;
}

qint64 WebBase::send(const QString &type, const QString &method, const QJsonObject &sendObj)
{
    qint64 _id = ++m_id;

    QJsonObject obj;
    obj.insert("WSport", m_wsport);
    obj.insert("id", _id);
    obj.insert("method", QString("dobotlink.%1.%2").arg(type).arg(method));
    obj.insert("params", sendObj);
    qDebug()<<"thdid:"<<QThread::currentThreadId() << QString(">>%1:[receive]").arg(m_wsport) << obj;

    static QString module("MagicDevice");
    QMetaObject::invokeMethod(m_pMagicDevicePlugin,
                              "pReceiveMassage_slot",
                              Qt::QueuedConnection,
                              Q_ARG(QString, module),
                              Q_ARG(QJsonObject, obj));
    return _id;
}
