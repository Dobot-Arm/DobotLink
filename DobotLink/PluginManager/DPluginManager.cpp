#include "DPluginManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QMap>
#include <QPluginLoader>
#include <QFileInfo>
#include <QThread>
#include <QDirIterator>
#include <QDebug>

#include "DPluginInterface.h"
#include "MessageCenter/DPacket.h"
#include "Module/DSettings.h"

DPluginManager::DPluginManager(QObject *parent) : QObject(parent)
{
    handleNewPluginFiles();
}

/* before destroy DPluginManager, please run unloadAllPlugin first! */
DPluginManager::~DPluginManager()
{
    unloadAllPlugin();
//    qDebug() << "DPluginManager destroyed.";
}

void DPluginManager::loadDefaultPlugin()
{
    DSettings *settings = DSettings::getInstance();
    if (settings->getPluginLoaded("MagicDevicePlugin")) {
        loadPlugin("MagicDevicePlugin");
    }
    if (settings->getPluginLoaded("IndustrialRobotPlugin")) {
        loadPlugin("IndustrialRobotPlugin");
    }
    if (settings->getPluginLoaded("ArduinoPlugin")) {
        loadPlugin("ArduinoPlugin");
    }
    if (settings->getPluginLoaded("MicrobitPlugin")) {
        loadPlugin("MicrobitPlugin");
    }
    if (settings->getPluginLoaded("DownloadPlugin")) {
        loadPlugin("DownloadPlugin");
    }
    if (settings->getPluginLoaded("MagicianGoPlugin")) {
        loadPlugin("MagicianGoPlugin");
    }
    if (settings->getPluginLoaded("DynamicAlgorithmPlugin")){
        loadPlugin("DynamicAlgorithmPlugin");
    }
    if (settings->getPluginLoaded("DebuggerPlugin")){
        loadPlugin("DebuggerPlugin");
    }
    if (settings->getPluginLoaded("PyImageOMPlugin")){
        loadPlugin("PyImageOMPlugin");
    }
    if (settings->getPluginLoaded("CodingAgentPlugin")){
        loadPlugin("CodingAgentPlugin");
    }
    if (settings->getPluginLoaded("DebuggerlitePlugin")){
        loadPlugin("DebuggerlitePlugin");
    }
    if (settings->getPluginLoaded("RunCmdPlugin")){
        loadPlugin("RunCmdPlugin");
    }
    if (settings->getPluginLoaded("MagicianProJoystickPlugin")){
        loadPlugin("MagicianProJoystickPlugin");
    }


    qDebug() << "finish load plugin";

}

bool DPluginManager::loadPlugin(QString pluginName)
{
    QString fileNameStr = pluginName;

    if (!pluginName.endsWith("Plugin")) {
        qDebug() << "pluginName is incorrect.";
        return false;
    }

#ifdef Q_OS_WIN
    #ifdef QT_NO_DEBUG
        fileNameStr.append(".dll");
    #else
        fileNameStr.append("_d.dll");
    #endif

    QDir appDir(qApp->applicationDirPath());
    QString dllFilePath = appDir.absoluteFilePath(fileNameStr);

#elif defined (Q_OS_MAC)
    #ifdef QT_NO_DEBUG
        fileNameStr.append(".1.0.0.dylib");
    #else
        fileNameStr.append("_d.1.0.0.dylib");
    #endif

    QDir dir(qApp->applicationDirPath());
    dir.cdUp();
    dir.cd("Frameworks");
    QString dllFilePath = dir.absoluteFilePath(fileNameStr);
#elif defined (Q_OS_LINUX)
    #ifdef QT_NO_DEBUG
        fileNameStr.append(".so.1.0.0");
    #else
        fileNameStr.append("_d.so.1.0.0");
    #endif

    QDir dir(qApp->applicationDirPath());
    QString dllFilePath = dir.absoluteFilePath(fileNameStr);
#elif defined (Q_OS_ANDROID)
#ifdef QT_NO_DEBUG
    fileNameStr.append(".so");
#else
    fileNameStr.append("_d.so");
#endif
    QDir appDir(qApp->applicationDirPath());
    QString dllFilePath = appDir.absoluteFilePath(fileNameStr);
#endif
    bool ok = _pluginLoadDll(dllFilePath, pluginName);
    return ok;
}

void DPluginManager::loadAllPlugin()
{
    QString appPath = qApp->applicationDirPath();

#ifdef Q_OS_WIN
    QFileInfoList fileInfoList = _getAllFileInfo(appPath);
    foreach (const QFileInfo &fileInfo, fileInfoList) {
        if (fileInfo.fileName().contains("Plugin.dll")) {
            _pluginLoadDll(fileInfo.absoluteFilePath(), QString(fileInfo.fileName()).remove(".dll"));
        } else if (fileInfo.fileName().contains("Plugin_d.dll")) {
            _pluginLoadDll(fileInfo.absoluteFilePath(), QString(fileInfo.fileName()).remove("_d.dll"));
        }
    }

#elif defined (Q_OS_MAC)
    QDir resourceDir(appPath);
    resourceDir.cdUp();
    resourceDir.cd("Resources");

    QFileInfoList fileInfoList = _getAllFileInfo(resourceDir.absolutePath());
    foreach (const QFileInfo &fileInfo, fileInfoList) {
        if (fileInfo.fileName().contains("Plugin.1.0.0.dylib")) {
            _pluginLoadDll(fileInfo.absoluteFilePath(), QString(fileInfo.fileName()).remove(0, 3).remove(".1.0.0.dylib"));
        } else if (fileInfo.fileName().contains("Plugin_d.1.0.0.dylib")) {
            _pluginLoadDll(fileInfo.absoluteFilePath(), QString(fileInfo.fileName()).remove(0, 3).remove("_d.1.0.0.dylib"));
        }
    }
#elif defined (Q_OS_LINUX)
//    QDir resourceDir(appPath);
//    resourceDir.cdUp();
//    resourceDir.cd("Resources");

//    QFileInfoList fileInfoList = _getAllFileInfo(resourceDir.absolutePath());
    QFileInfoList fileInfoList = _getAllFileInfo(appPath);
    foreach (const QFileInfo &fileInfo, fileInfoList) {
        if (fileInfo.fileName().contains("Plugin.so.1.0.0")) {
            _pluginLoadDll(fileInfo.absoluteFilePath(), QString(fileInfo.fileName()).remove(0, 3).remove(".1.0.0.dylib"));
        } else if (fileInfo.fileName().contains("Plugin_d.so.1.0.0")) {
            _pluginLoadDll(fileInfo.absoluteFilePath(), QString(fileInfo.fileName()).remove(0, 3).remove("_d.1.0.0.dylib"));
        }
    }
#elif defined (Q_OS_ANDROID)
    QFileInfoList fileInfoList = _getAllFileInfo(appPath);
    foreach (const QFileInfo &fileInfo, fileInfoList) {
        if (fileInfo.fileName().contains("Plugin.so")) {
            _pluginLoadDll(fileInfo.absoluteFilePath(), QString(fileInfo.fileName()).remove(".so"));
        } else if (fileInfo.fileName().contains("Plugin_d.so")) {
            _pluginLoadDll(fileInfo.absoluteFilePath(), QString(fileInfo.fileName()).remove("_d.so"));
        }
    }
#endif
}

bool DPluginManager::_pluginLoadDll(QString dllPath, QString pluginName)
{
//    qDebug() << "load dll:" << dllPath << "name:" << pluginName;

    QPluginLoader *pluginLoader = new QPluginLoader(dllPath, this);

    if (pluginLoader->isLoaded()) {
        qDebug() << "plugin is already loaded.";
        return true;
    }

    if (pluginLoader->load() == true) {
        QObject *plugin = pluginLoader->instance();
        if (plugin) {
            auto interface = qobject_cast<DPluginInterface *>(plugin);
            if (interface) {
                DSettings *settings = DSettings::getInstance();
                m_pluginVersionMap.insert(pluginName, interface->getVersion());
                settings->setVersion(pluginName, interface->getVersion());
                qDebug().noquote() << QString("load %1 version:%2. --(OK)").arg(pluginName).arg(interface->getVersion());

                emit onLoadPluginFinish_signal(pluginName, true);

                QThread *thread = new QThread();
                connect(thread, &QThread::finished, thread, &QThread::deleteLater);

                interface->moveToThread(thread);
                thread->start();

                m_PluginThreadMap.insert(pluginName, thread);

                connect(this, &DPluginManager::sendPluginMsg_signal, interface, &DPluginInterface::pReceiveMassage_slot);
                connect(interface, &DPluginInterface::pSendMessage_signal, this, &DPluginManager::receivePluginMessage_slot);
            }
        }
        m_PluginLoaderMap.insert(pluginName, pluginLoader);
    } else {
        qDebug() << QString("load %1 Error:%2").arg(pluginName).arg(pluginLoader->errorString());
        return false;
    }
    return true;
}

QFileInfoList DPluginManager::_getAllFileInfo(QString path)
{
    QDir dir(path);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folderList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0; i != folderList.size(); i++) {
        QString name = folderList.at(i).absoluteFilePath();
        QFileInfoList childFileList = _getAllFileInfo(name);
        fileList.append(childFileList);
    }
    return fileList;
}

void DPluginManager::handleNewPluginFiles()
{
    QDir appDir(qApp->applicationDirPath());

    QDir pluginDir(appDir);
    QStringList nameFilters;
    nameFilters << "{$$}lib*.a" << "{$$}*.dll";

    QStringList newFiles;
    QDirIterator it(appDir.path(), nameFilters, QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
        it.next();
        if (it.fileName().startsWith("{$$}")) {
            QString fileName = pluginDir.absoluteFilePath(it.fileName());
            newFiles.append(fileName);
        }
    }

    foreach (const QString &fileName, newFiles) {
        QFileInfo newFile(fileName);
        QString oldFileName = newFile.fileName().remove("{$$}");
        QFileInfo oldFile(pluginDir, oldFileName);

        if (oldFile.exists()) {
            if (QFile::remove(oldFile.fileName())) {
                bool ok = QFile::rename(newFile.fileName(), oldFile.fileName());
                if (ok) {
                    qDebug() << "new plugin file is updated. fileName:" << oldFile.fileName();
                }
            }
        }
    }
}

void DPluginManager::unloadPlugin(QString pluginName)
{

    QThread *thread = m_PluginThreadMap.value(pluginName);
    if (thread) {
        thread->quit();
        thread->wait(500);
        if (thread->isFinished()) {
            qDebug() << pluginName << "thread quit finished.";
        } else {
            qDebug() << pluginName << "thread can not quit. timeout";
        }
    }
    m_pluginVersionMap.remove(pluginName);
    m_PluginThreadMap.remove(pluginName);
//    m_PluginLoaderMap.remove(pluginName);

    QPluginLoader *pluginLoader = m_PluginLoaderMap.value(pluginName);
    if (pluginLoader) {
        if (pluginLoader->unload() == true) {
            qDebug() << pluginName << "plugin unloaded.";
            emit onLoadPluginFinish_signal(pluginName, false);
        }
    }
    m_PluginLoaderMap.remove(pluginName);


}

void DPluginManager::unloadAllPlugin()
{
    foreach (const QString &pluginName, m_PluginLoaderMap.keys()) {
        unloadPlugin(pluginName);
    }
}

bool DPluginManager::sendMassage(QString pluginName, QJsonObject obj)
{
    if (pluginName == "ALL" || m_PluginLoaderMap.contains(pluginName + "Plugin")) {
        emit sendPluginMsg_signal(pluginName, obj);
        return true;
    } else {
        qDebug() << "No plugin called:" << pluginName;
    }
    return false;
}

QJsonObject DPluginManager::getLoadedPluginInfo()
{
    QJsonObject resObj;
    foreach (const QString &pluginName, m_pluginVersionMap.keys()) {
        resObj.insert(pluginName, m_pluginVersionMap.value(pluginName));
    }
    return resObj;
}


/* SLOT */
/* 收到插件发来的消息，转发给 massage center */
void DPluginManager::receivePluginMessage_slot(QString id, QJsonObject obj)
{
//    qDebug() << "pluginManager get massage:" << "id:" << id << "get:" << obj;
    emit receiveMessage_signal(id, obj);
}

