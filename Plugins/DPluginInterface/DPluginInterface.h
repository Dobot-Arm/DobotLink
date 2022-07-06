#ifndef DPLUGININTERFACE_H
#define DPLUGININTERFACE_H

#if defined(DPLUGININTERFACE_LIBRARY)
#  define DPLUGININTERFACESHARED_EXPORT Q_DECL_EXPORT
#else
#  define DPLUGININTERFACESHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QtGlobal>
#include <QObject>
#include <QJsonObject>

class DPLUGININTERFACESHARED_EXPORT DPluginInterface : public QObject
{
    Q_OBJECT

public:
    DPluginInterface(QObject *parent = nullptr);
    virtual ~DPluginInterface();
    virtual QString getVersion() = 0;

signals:
    void pSendMessage_signal(QString id, QJsonObject obj);

public slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj) = 0;
};

QT_BEGIN_NAMESPACE
#define DPluginInterfaceIID "Dobot.plugin.interface"
Q_DECLARE_INTERFACE(DPluginInterface, DPluginInterfaceIID)
QT_END_NAMESPACE

#endif // DPLUGININTERFACE_H
