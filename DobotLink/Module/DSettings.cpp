#include "DSettings.h"

#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

DSettings *DSettings::getInstance()
{
    static DSettings *instance = nullptr;
    if (instance == nullptr) {
        instance = new DSettings();
    }
    return instance;
}

DSettings::DSettings(QObject *parent) : QObject(parent)
{
    QString appPath = QCoreApplication::applicationDirPath().remove(QRegExp("_d$"));
    QDir dir(appPath);

#ifdef Q_OS_WIN
    m_settings = new QSettings(dir.absoluteFilePath("dobotlink.ini"), QSettings::IniFormat, this);
#elif defined (Q_OS_MAC)
    m_settings = new QSettings(dir.absoluteFilePath("dobotlink.plist"), QSettings::NativeFormat, this);
#elif defined (Q_OS_LINUX)
    m_settings = new QSettings(dir.absoluteFilePath("dobotlink.ini"), QSettings::NativeFormat, this);
#elif defined (Q_OS_ANDROID)
    m_settings = new QSettings("Dobot", "DobotLink", this);
    m_settings->setDefaultFormat(QSettings::IniFormat);
#endif
    m_settings->setIniCodec("UTF-8");

    defaultValueInit();
}

void DSettings::defaultValueInit()
{
    if (!contains("version", "main")) {
        setValue("version", "main", "x");
    }
    if (!contains("version", "MagicDevicePlugin")) {
        setValue("version", "MagicDevicePlugin", "x");
    }
    if (!contains("version", "IndustrialRobotPlugin")) {
        setValue("version", "IndustrialRobotPlugin", "x");
    }
    if (!contains("version", "DownloadPlugin")) {
        setValue("version", "DownloadPlugin", "x");
    }
    if (!contains("version", "MicrobitPlugin")) {
        setValue("version", "MicrobitPlugin", "x");
    }
    if (!contains("version", "ArduinoPlugin")) {
        setValue("version", "ArduinoPlugin", "x");
    }

    if (!contains("config", "isLogging")) {
        setValue("config", "isLogging", true);
    }
    if (!contains("config", "isPrintable")) {
        setValue("config", "isPrintable", true);
    }
    if (!contains("config", "showSystemTrayOnStart")) {
        setValue("config", "showSystemTrayOnStart", true);
    }
    if (!contains("config", "showOnTop")) {
        setValue("config", "showOnTop", true);
    }

    if (!contains("plugin", "MagicDevicePlugin")) {
        setValue("plugin", "MagicDevicePlugin", true);
    }
    if (!contains("plugin", "IndustrialRobotPlugin")) {
        setValue("plugin", "IndustrialRobotPlugin", true);
    }
    if (!contains("plugin", "ArduinoPlugin")) {
        setValue("plugin", "ArduinoPlugin", false);
    }
    if (!contains("plugin", "MicrobitPlugin")) {
        setValue("plugin", "MicrobitPlugin", false);
    }
    if (!contains("plugin", "DownloadPlugin")) {
        setValue("plugin", "DownloadPlugin", true);
    }
    if (!contains("plugin", "DynamicAlgorithmPlugin")) {
        setValue("plugin", "DynamicAlgorithmPlugin", true);
    }
    if (!contains("plugin", "DebuggerPlugin")){
        setValue("plugin", "DebuggerPlugin", true);
    }
    if (!contains("plugin", "PyImageOMPlugin")){
        setValue("plugin", "PyImageOMPlugin", true);
    }
    if (!contains("plugin", "CodingAgentPlugin")){
        setValue("plugin", "CodingAgentPlugin", false);
    }
    if (!contains("plugin", "DebuggerlitePlugin")){
        setValue("plugin", "DebuggerlitePlugin", true);
    }
    if (!contains("plugin", "RunCmdPlugin")){
        setValue("plugin", "RunCmdPlugin", true);
    }
    if (!contains("plugin", "MagicianProJoystickPlugin")){
        setValue("plugin", "MagicianProJoystickPlugin", false);
    }
}

void DSettings::setValue(QString node, QString key, QVariant value)
{
    m_settings->setValue(QString("/%1/%2").arg(node).arg(key), value);
}

QVariant DSettings::getValue(QString node, QString key, QVariant defaultValue)
{
    QVariant value = m_settings->value(QString("/%1/%2").arg(node).arg(key), defaultValue);
    return value;
}

bool DSettings::contains(QString node, QString key)
{
    return m_settings->contains(QString("/%1/%2").arg(node).arg(key));
}

void DSettings::setVersion(QString section, QString version)
{
    setValue("version", section, version);
}

QString DSettings::getVersion(QString section)
{
    return getValue("version", section, QString("x")).toString();
}

/* /config/isLogging */
bool DSettings::getIsLogging()
{
    return getValue("config", "isLogging", false).toBool();
}

void DSettings::setIsLogging(bool on)
{
    setValue("config", "isLogging", on);
}

/* /config/isLogging */
bool DSettings::getIsPrintable()
{
    return getValue("config", "isPrintable", false).toBool();
}

void DSettings::setIsPrintable(bool on)
{
    setValue("config", "isPrintable", on);
}

/* /config/showOnTop */
bool DSettings::getShowTopOn()
{
    return getValue("config", "showOnTop", false).toBool();
}

void DSettings::setShowTopOn(bool on)
{
    setValue("config", "showOnTop", on);
}

/* /config/showSystemTrayOnStart */
bool DSettings::getShowSystemTray()
{
    return getValue("config", "showSystemTrayOnStart", true).toBool();
}

void DSettings::setShowSystemTray(bool on)
{
    setValue("config", "showSystemTrayOnStart", on);
}

/* /plugin/ */
void DSettings::setPluginLoaded(QString pluginName, bool on)
{
    setValue("plugin", pluginName, on);
}

bool DSettings::getPluginLoaded(QString pluginName)
{
    return getValue("plugin", pluginName, false).toBool();
}




