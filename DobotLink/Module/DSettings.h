#ifndef DSETTINGS_H
#define DSETTINGS_H

#include <QObject>
#include <QVariant>

class QSettings;
class DSettings : public QObject
{
    Q_OBJECT
public:
    static DSettings *getInstance();

    void setValue(QString node, QString key, QVariant value);
    QVariant getValue(QString node, QString key, QVariant defaultValue = QVariant());
    bool contains(QString node, QString key);

    void setVersion(QString section, QString version);
    QString getVersion(QString section);

    bool getIsLogging();
    void setIsLogging(bool on);

    bool getIsPrintable();
    void setIsPrintable(bool on);

    bool getShowTopOn();
    void setShowTopOn(bool on);

    bool getShowSystemTray();
    void setShowSystemTray(bool on);

    bool getPluginLoaded(QString pluginName);
    void setPluginLoaded(QString pluginName, bool on);

private:
    Q_DISABLE_COPY(DSettings)
    explicit DSettings(QObject *parent = nullptr);

    void defaultValueInit();

    QSettings *m_settings;
};

#endif // DSETTINGS_H
