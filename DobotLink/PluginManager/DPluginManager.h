#ifndef DPLUGINMANAGER_H
#define DPLUGINMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QMap>
#include <QFileInfoList>

class QThread;
class QPluginLoader;

class DPluginManager : public QObject
{
    Q_OBJECT
public:
    explicit DPluginManager(QObject *parent = nullptr);
    ~DPluginManager();

    void loadDefaultPlugin();

    bool loadPlugin(QString pluginName);
    void loadAllPlugin();
    void unloadPlugin(QString pluginName);
    void unloadAllPlugin();
    bool sendMassage(QString pluginName, QJsonObject obj);
    QJsonObject getLoadedPluginInfo();

private:
    QMap<QString, QPluginLoader*> m_PluginLoaderMap;
    QMap<QString, QThread*> m_PluginThreadMap;
    QMap<QString, QString> m_pluginVersionMap;

    bool _pluginLoadDll(QString dllPath, QString pluginName);
    QFileInfoList _getAllFileInfo(QString path);

    void handleNewPluginFiles();

signals:
    /* DPluginManager <-> MessageCenter */
    void receiveMessage_signal(QString id, QJsonObject obj);
    void onLoadPluginFinish_signal(QString name, bool isLoaded);

    /* DPluginManager <-> Plugin */
    void sendPluginMsg_signal(QString id, QJsonObject obj);

private slots:
    void receivePluginMessage_slot(QString id, QJsonObject obj);
};

#endif // DPLUGINMANAGER_H
