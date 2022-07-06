#include "DGetVersion.h"
#include "DError/DError.h"
#include <QCoreApplication>
#include <QFile>
#include <QDebug>
DGetVersion::DGetVersion(QObject *parent) : QObject(parent)
{
    m_md5 = "";
    m_length = "";
}

DGetVersion::~DGetVersion()
{

}

bool DGetVersion::_VersionFileInfo(quint64 id, QString device){
    QString appPath =QCoreApplication::applicationDirPath().remove("_d");
    m_fileDir.setPath(appPath);

#ifdef Q_OS_WIN
    if (m_fileDir.exists("firmware_info")) {
        m_fileDir.cd("firmware_info");
#else
    QString firmwareInfoPath = "../Resources/firmware_info";
    if (m_fileDir.exists(firmwareInfoPath)) {
        m_fileDir.cd(firmwareInfoPath);
#endif
    } else {
        emit on_getVersionErr_signal(id, ERROR_FIRMWARE_NOT_EXIST, "firmware don't exist !");
        return false;
    }
    QStringList fileFilters;
    fileFilters << "*.txt";
    QStringList files = m_fileDir.entryList(fileFilters, QDir::Files|QDir::Readable, QDir::Name);

    QString typeName = device;

    foreach (QString file, files) {
        if (file == (typeName + ".txt")){
            m_fileName = m_fileDir.absoluteFilePath(file);
        }
    }

    return true;
}

void DGetVersion::getVersionInfo(quint64 id, QString device, QString version)
{
    if(_VersionFileInfo(id, device)) {
        _devicefilter(id, device, version);
    }
}

bool DGetVersion::_devicefilter(quint64 id, QString device, QString version)
{
    QString res;
    QString Ver;
    if (device == "Magician"){
        Ver = "Version" + version;
        res = getInfo(Ver);
    } else if (device == "MagicianLite") {
        Ver = "Ver" + version;
        res = getInfo(version);
    } else if (device == "MagicBox") {
        Ver = "Ver" + version;
        res = getInfo(version);
    } else if (device == "Camera") {
        Ver = "Ver" + version;
        res = getInfo(version);
    } else if (device == "MagicianGO") {
        Ver = "Ver" + version;
        res = getInfo(version);
    } else {
        qDebug() << "invalid device !";
        emit on_getVersionErr_signal(id, ERROR_FIRMWARE_INVALID_DEVICE, "invalid device!");
        return false;
    }
    emit on_getVersionInfo_signal(res);
    return true;
}

bool DGetVersion::_getLatestVersion(quint64 id, QString device, QString &version)
{
    version = "";

    if (!_VersionFileInfo(id, device)) return false;

    QString typeName;
    // 根据固件文件获取版本号
    if (device == "Magician"){
        typeName = "DobotMCU";
    } else if (device == "MagicianLite") {
        typeName = "MagicianLite";
    } else if (device == "MagicBox") {
        typeName = "MagicBox";
    } else if (device == "CameraCar") {
        typeName = "AicamCar";
    } else if (device == "CameraArm") {
        typeName = "AicamEndstop";
    } else if (device == "MagicianGO") {
        typeName = "CarTemp";
    }
    QString appPath =QCoreApplication::applicationDirPath().remove("_d");
    m_firmwareDir.setPath(appPath);

#ifdef Q_OS_WIN
    if (m_firmwareDir.exists("firmware")) {
        m_firmwareDir.cd("firmware");
#else
    QString firmwarePath = "../Resources/firmware";
    if (m_firmwareDir.exists(firmwarePath)) {
        m_firmwareDir.cd(firmwarePath);
#endif
    } else {
        emit on_getVersionErr_signal(id, ERROR_FIRMWARE_NOT_EXIST, "firmware don't exist!");
        return false;
    }

    QStringList fileFilters;
    QStringList files = m_firmwareDir.entryList(fileFilters, QDir::Files|QDir::Readable, QDir::Name);

    foreach (QString file, files) {
        if (file.startsWith(typeName)){
            m_firmwareName = file;
        }
    }
    // 删除后缀
    int lastPoint = m_firmwareName.lastIndexOf(".");
    QString fileNameNoExt = m_firmwareName.left(lastPoint);

    QStringList list = fileNameNoExt.split("_");
    QString ver = list[1];
    if ((device == "MagicianLite" or device == "MagicBox") and list.length()>4){
        m_length = list[3];
        m_md5 = list[4];
    }
    if ((device.startsWith("Camera")) and list.length() > 2) {
        m_CameraNum = list[2].toInt();
    }
    ver.remove(0,1);
    version = ver;
    return true;
}

void DGetVersion::getLatestVersionInfo(quint64 id, QString device)
{
    QString ver("");
    if (!_getLatestVersion(id, device, ver)) return;
    _devicefilter(id, device, ver);
}

void DGetVersion::getLatestVersion(quint64 id, QString device)
{
    QString ver("");
    if (!_getLatestVersion(id, device, ver)) return;
    emit on_getVersion_signal(ver, m_length, m_md5, m_CameraNum);
}

QString DGetVersion::getInfo(QString Ver)
{
    QFile file (m_fileName);
    QString res;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QString line;
        QTextStream in(&file);  //用文件构造流
        bool flag = false;
        while (!in.atEnd()) {
            line = in.readLine();
            while(!line.isNull())
            {
                if (line.contains(Ver, Qt::CaseInsensitive)){ //筛选版本号
                    flag = true;
                }
                while(line != "" and flag){
                    res.append(line+"\r\n");
                    line = in.readLine();
                }
                flag = false;
                break;
            }
        }
    }
    file.close();
    return res;
}







