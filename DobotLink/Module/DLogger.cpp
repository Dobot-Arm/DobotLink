#include "DLogger.h"

#include <QTextStream>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QFileInfo>
#include <QUrl>
#include <QMutexLocker>
#include <QTextCodec>
#include <QDebug>
int const static DAYS_CLEANLOGS = -15;
DLogger *DLogger::getInstance()
{
    static DLogger *instance = nullptr;
    if (instance == nullptr) {
        instance = new DLogger();
    }
    return instance;
}

DLogger::DLogger(QObject *parent) : QObject(parent)
{
    m_isLogging = false;
    logFile = new QFile();

    msgHead << "Debug   " << "Warning " << "Critical" << "Fatal   " << "Info    ";

    /* log-file-path setting */
#ifdef Q_OS_WIN
    m_logPath.setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    m_logPath.cd("AppData");
    m_logPath.cd("Roaming");
#elif defined (Q_OS_MAC)
    m_logPath.setPath(QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation));
#elif defined (Q_OS_LINUX)
#ifdef __arm__
    m_logPath.setPath("/var");
    m_logPath.cd("log");
#else
    m_logPath.setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    if (!m_logPath.exists("log")) {
        if(!m_logPath.mkdir("log")) {
            qDebug() << "Can not create \"log\" floder.";
        }
    }
    m_logPath.cd("log");
#endif
#endif

    if (!m_logPath.exists("DobotLink")) {
        if(!m_logPath.mkdir("DobotLink")) {
            qDebug() << "Can not create \"DobotLink\" floder.";
        }
    }
    if (m_logPath.cd("DobotLink")) {
        QString dataTime = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
        QString fileName = QString("DL_%1.log").arg(dataTime);
        logFile->setFileName(m_logPath.absoluteFilePath(fileName));
    }
}

void DLogger::destroyLog_slot()
{
    stopLogging();

    if (logFile->isOpen()) {
        logFile->close();
        qDebug() << "Close log file";
    }
    delete logFile;
}

void DLogger::cleanLogs()
{
    if(!m_logPath.path().contains("DobotLink")) return;

    QDir dir(m_logPath);
    QStringList nameFilters;
    nameFilters << "*.log";
    QStringList files =  dir.entryList(nameFilters, QDir::Files|QDir::Writable, QDir::Name);

    foreach (QString file, files) {
        if (file != logFile->fileName()) {
            QString path = dir.absolutePath() + "/" + file;
            qDebug() << "delete file:" << path;
            QFile::remove(path);
        }
    }
}

/* 定期清理日志 */
void DLogger::timecleanLogs()
{
    if(!m_logPath.path().contains("DobotLink")) return;

    QDir dir(m_logPath);
    QString path = dir.absolutePath();
    QString filepath;
    QStringList filter;
    filter << "*.log";
    QFileInfoList fileInfo = dir.entryInfoList(filter);
    int filedate;
    int dietime = QDateTime::currentDateTime().addDays(DAYS_CLEANLOGS).toString("yyyyMMdd").toInt();
    for (int i=0; i<fileInfo.count(); i++) {
        filedate = fileInfo.at(i).lastModified().toString("yyyyMMdd").toInt();
        // 如果该文件的时间小于一个月前的时间，则删除
        if (filedate < dietime) {
            filepath = path + "/" + fileInfo.at(i).fileName();
            qDebug() << "delete overdue file: " << filepath;
            QFile::remove(filepath);
        }
    }
}

QByteArray DLogger::getLog()
{
   return logFile->readAll();
}

void DLogger::startLogging()
{
    if (m_isLogging) {
        return;
    }

    if (logFile->isOpen()) {
        m_isLogging = true;
        qDebug() << "Start logging at path:" << logFile->fileName();
        qInstallMessageHandler(myMessageOutput);
    } else if (logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        m_isLogging = true;
        qDebug() << "Start logging at path:" << logFile->fileName();
        qInstallMessageHandler(myMessageOutput);
    } else {
        qWarning() << "Open log File Failed:" << logFile->fileName();
    }
}

void DLogger::stopLogging()
{
    qDebug() << "stop logging.";

    m_isLogging = false;
    qInstallMessageHandler(nullptr);

    if (logFile->isOpen()) {
        logFile->close();
    }
}

void DLogger::openLogFile()
{
    QFileInfo fileInfo(logFile->fileName());
    if (fileInfo.exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    } else {
        qDebug() << "log file is Not exist.";
    }
}

bool DLogger::openLogDir()
{
    bool ok = QDesktopServices::openUrl(QUrl::fromLocalFile(m_logPath.absolutePath()));
    return ok;
}


void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    DLogger *logger = DLogger::getInstance();

    QString msg;
    if (type == QtWarningMsg) {
        msg.append("[!WARNING]");
    }
    msg.append(message);

    emit logger->logMessage_signal(msg);

    if (!logger->logFile->isOpen()) {
        return;
    }

    QMutexLocker locker(&logger->mMutex);

    QTextStream textStream(logger->logFile);
    QString messageText;
    QString currentDateTime;

#ifdef QT_NO_DEBUG
    /* simple print */
    Q_UNUSED(context)

    currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    messageText = QString("%1|%2\n").arg(currentDateTime).arg(msg);
#else
    /* details print */
    currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    messageText = QString("%1 | %6 | %2:%3, %4 | %5\n")
            .arg(logger->msgHead.at(type))
            .arg(context.file)
            .arg(context.line)
            .arg(context.function)
            .arg(message)
            .arg(currentDateTime);
#endif

    textStream << messageText.toUtf8();
    logger->logFile->flush();
}

#if 0
    QString appPath = QCoreApplication::applicationDirPath(); //app 路径
    QString otherPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation); //系统路径
    QString otherPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    SetFileAttributes((LPCWSTR)(filePath.unicode()), FILE_ATTRIBUTE_HIDDEN);    //#include "windows.h" 隐藏文件
#endif
