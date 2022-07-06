#ifndef DOWNLOADPLUGIN_H
#define DOWNLOADPLUGIN_H

#include "DPluginInterface.h"

#include <QObject>
#include <QJsonObject>
#include <QMap>

#include "MessageCenter/DPacket.h"
#include "DDfufile.h"
#include "DMcuisp.h"
#include "DKflash.h"
#include "DMultiFunc.h"
#include "DGetVersion.h"
class DownloadPlugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)

public:
    static const QString PluginName;
    static const QString Version;

    DownloadPlugin(QObject *parent = nullptr);
    virtual ~DownloadPlugin() override;
    virtual QString getVersion() override;

private:
    DDfufile *m_dfufile;
    quint64 m_handlingId;
    quint16 m_handlingWsPort;
    QMap<quint64, DRequestPacket> m_requestPacketMap;

    DMcuisp *m_dmcuisp;
    DKflash *m_dkflash;
    DMultiFunc *m_dmultifunc;
    DGetVersion *m_dgetversioninfo;

    void handleDownloadCmd(const QJsonObject &obj);

    void handleDMcuisp(const QJsonObject &obj);

    void handleDKflash(const QJsonObject &obj);

    void stopProcess();

private slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj) override;
    void onFinish_slot(bool ok, quint64 id);
    void onStop_slot(quint64 id);
    void on_getVersionInfo_slot(QString res);
    void on_getVersion_slot(QString ver, QString length, QString md5, int cameraNum);
    void onErrorHandle_slot(quint64 id, int code, QString message);
};

#endif // DOWNLOADPLUGIN_H
