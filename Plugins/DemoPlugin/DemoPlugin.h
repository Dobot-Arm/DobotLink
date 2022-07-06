#ifndef DEMOPLUGIN_H
#define DEMOPLUGIN_H

#include "DPluginInterface.h"

#include <QObject>
#include <QJsonObject>
#include "DemoPacket.h"

class DemoPlugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)

public:
    DemoPlugin(QObject *parent = nullptr);
    static const QString PluginID;

private:
    void handleDemoCommand(const QJsonObject &obj);

private slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj);
};

#endif // DEMOPLUGIN_H
