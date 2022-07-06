#ifndef WEBBASE_H
#define WEBBASE_H

#include <QObject>
#include "MagicDevicePlugin.h"
class QCoreApplication;

typedef void (*pOnRecv)(const qint64 &id, const QJsonValue &recvValue);

class QThread;
class QCoreApplication;

class WebBase : public QObject
{
    Q_OBJECT
public:
    static WebBase *instance();
    ~WebBase();

    void setRecvCallback(pOnRecv onRecv){m_onRecv = onRecv;};
    qint64 send(const QString &type, const QString &method, const QJsonObject &sendObj);

private:
    inline void _initApp();
    inline void _initWebExec();
    void InitMagicDevicePlugin();

private:
    WebBase();
    quint16 m_wsport;
    qint64 m_id;

    pOnRecv m_onRecv;

    MagicDevicePlugin* m_pMagicDevicePlugin;
    QThread* m_pThdMagicDevicePlugin;

    QCoreApplication* m_mainApp;
};

#endif // WEBBASE_H
