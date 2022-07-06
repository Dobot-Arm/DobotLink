#ifndef DLOGGER_H
#define DLOGGER_H

#include <QObject>
#include <QMessageLogger>
#include <QFile>
#include <QDir>
#include <QMutex>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &message);

class DLogger : public QObject
{
    Q_OBJECT
public:
    static DLogger *getInstance();
    QMutex mMutex;

    QList<QString> msgHead;
    QFile *logFile;

    void cleanLogs();
    void timecleanLogs();
    QByteArray getLog();
signals:
    void logMessage_signal(QString msg);

public slots:
    void startLogging();
    void stopLogging();

    void openLogFile();
    bool openLogDir();
    void destroyLog_slot();

private:
    Q_DISABLE_COPY(DLogger)
    explicit DLogger(QObject *parent = nullptr);

    bool m_isLogging;
    QDir m_logPath;
};

#endif // DLOGGER_H
