#include "FileControll.h"

#include "DError/DError.h"
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <QEventLoop>
#include <QTimer>
#include <QSharedPointer>
#include <QPointer>
#include <QtConcurrent>

const QString MG400_DEFAULT_IP = "192.168.1.6";
const QString M1_DEFAULT_IP = "192.168.9.1";
const QString BASE_PORT = "22000";
const int DEFAULT_TIMEOUT = 2000;

//递归终止
QString __splicing()
{
    return "";
}

template<typename T, typename ...Types>
T __splicing(T first, Types ...paths)
{
    if (!first.startsWith("/")) {
        first = "/" + first;
    }

    T path = first + __splicing(paths...);
    return path;
}

template <typename ...T>
QString __getDobotPath(QString ip, T ...paths)
{
#ifdef __arm__
    Q_UNUSED(ip)
    QString root("/dobot/userdata");
#else
    QString root = QString("\\\\%1").arg(ip);
#endif
    QString path = root + __splicing(paths...);
    return path;
}


FileControll::FileControll(const QString &ip, QObject *parent):
    QObject(parent)
{
    m_ip = ip;
    m_pool.setMaxThreadCount(1);
}

FileControll::~FileControll()
{
}

void FileControll::closureHandler(BaseThread *_thread, quint64 id, quint32 timeout)
{
    timeout = timeout ? timeout : DEFAULT_TIMEOUT;
    QPointer<BaseThread> thread(_thread);
    QTimer *timer(new QTimer(_thread));
    timer->setSingleShot(true);
    timer->setInterval(timeout);

    connect(thread, &BaseThread::onFinished_signal, this, [=](quint64 _id, int code, QByteArray array) {
        Q_UNUSED(_id)
        if (!thread) return;
        if(!thread->property("isTimeout").toBool()) {
            emit onFinish_signal(id, code, array,QJsonValue());
        }
        delete thread;
    });
    connect(timer, &QTimer::timeout, this, [=] {
        if (!thread) return;
        if (m_pool.tryTake(thread)) {
            delete thread;
        } else {
            thread->setProperty("isTimeout", true);
        }
        qDebug() << "ERROR_FILE_TIMEOUT";
        emit onFinish_signal(id, ERROR_INDUSTRY_FILE_TIMEOUT, QByteArray(),QJsonValue());
    });
    timer->start();
    m_pool.start(thread);
}

void FileControll::readFile(const quint64 id, const QString &fileName, quint32 timeout)
{
    QString r_fileName =__getDobotPath(m_ip, fileName);

    ReadThread *thread(new ReadThread(id, r_fileName));
    closureHandler(thread, id, timeout);
}

void FileControll::writeFile(const quint64 id, const QString &fileName, const QJsonValue &value, quint32 timeout)
{
    QString w_fileName =__getDobotPath(m_ip, fileName);

    WriteThread *thread(new WriteThread(id, w_fileName, value));
    closureHandler(thread, id, timeout);
}

void FileControll::writeFile(const quint64 id, const QString &fileName, const QString &content, quint32 timeout)
{
    QString w_fileName =__getDobotPath(m_ip, fileName);
    qDebug() << __FUNCTION__ << w_fileName;

    WriteThread *thread(new WriteThread(id, w_fileName, content));
    closureHandler(thread, id, timeout);
}

void FileControll::changeFile(const quint64 id, const QString &fileName, const QString &key, const QJsonValue &value, quint32 timeout)
{
    QString w_fileName =__getDobotPath(m_ip, fileName);
    qDebug() << __FUNCTION__ << w_fileName;

    ChangeFileThread *thread(new ChangeFileThread(id, w_fileName, key, value));
    closureHandler(thread, id, timeout);
}


void FileControll::newFile(const quint64 id, const QString &fileName, const QJsonValue &value, quint32 timeout)
{
    QString w_fileName =__getDobotPath(m_ip, fileName);
    qDebug() << __FUNCTION__ << w_fileName;

    NewFileThread *thread(new NewFileThread(id, w_fileName, value));
    closureHandler(thread, id, timeout);
}

void FileControll::newFile(const quint64 id, const QString &fileName, const QString &content, quint32 timeout)
{
    QString w_fileName =__getDobotPath(m_ip, fileName);
    qDebug() << __FUNCTION__ << w_fileName;

    NewFileThread *thread(new NewFileThread(id, w_fileName, content));
    closureHandler(thread, id, timeout);
}

void FileControll::decodeFile(const quint64 id, const QString &fileName, const QString &content, quint32 timeout)
{
    QString w_fileName =__getDobotPath(m_ip, fileName);
    qDebug() << __FUNCTION__ << w_fileName;

    DecodeFileThread *thread(new DecodeFileThread(id, w_fileName, content));
    closureHandler(thread, id, timeout);
}

void FileControll::newFolder(const quint64 id, const QString &url, const QString &folderName, quint32 timeout)
{
    QDir dir;
    dir.setPath(__getDobotPath(m_ip, url));
    qDebug() << __FUNCTION__ << dir;

    NewFolderThread *thread(new NewFolderThread(id, url, folderName, dir));
    closureHandler(thread, id, timeout);
}

void FileControll::newFolderRecursive(const quint64 id, const QString &strSubDirRelative, quint32 timeout)
{
    QString str = __getDobotPath(m_ip, strSubDirRelative);
    if (!str.endsWith('/'))
    {
        str += '/';
    }
    qDebug() << __FUNCTION__ << str;

    CreateDirThread *thread(new CreateDirThread(id, str));
    closureHandler(thread, id, timeout);
}

void FileControll::renameFolder(const quint64 id, const QString &folderName, const QString &newfolderName, quint32 timeout)
{
    QString re_folderName =__getDobotPath(m_ip, folderName);
    QString ne_folderName =__getDobotPath(m_ip, newfolderName);
    qDebug() << __FUNCTION__ << re_folderName;

    RenameFolderThread *thread(new RenameFolderThread(id, re_folderName, ne_folderName));
    closureHandler(thread, id, timeout);
}

void FileControll::copyFolder(const quint64 id, const QString &url, const QString &folderName, const QString &newfolderName, quint32 timeout)
{
    QDir dir, fromDir, toDir;
    dir.setPath(__getDobotPath(m_ip, url));
    fromDir.setPath(__getDobotPath(m_ip, url, folderName));
    toDir.setPath(__getDobotPath(m_ip, url, newfolderName));

    CopyFolderThread *thread(new CopyFolderThread(id, dir, fromDir, toDir, folderName, newfolderName));
    closureHandler(thread, id, timeout);
}

void FileControll::deleteFolder(const quint64 id, const QString &url, const QString &folderName, quint32 timeout)
{
    QDir dir;
    QString re_folderName =__getDobotPath(m_ip, url, folderName);
    dir.setPath(__getDobotPath(m_ip, url));

    DeleteFolderThread *thread(new DeleteFolderThread(id, url, folderName, re_folderName, dir));
    closureHandler(thread, id, timeout);
}

void FileControll::setIpAddress(const QString &ip)
{
    m_ip = ip;
}


void FileControll::readFolder(const quint64 id, const QString &folderName)
{
    QString r_folderName = __getDobotPath(m_ip, folderName);
    qDebug() << __FUNCTION__ << r_folderName;
    QtConcurrent::run(&m_pool,[=]{
        QFile file(r_folderName);
        QDir dir(r_folderName);
        QStringList filter;
        QJsonObject result;
        QString name;
        QDateTime time;
        QFileInfoList fileInfo = dir.entryInfoList(filter);
        for (int i=0; i<fileInfo.count(); i++) {
            name = fileInfo.at(i).fileName();
            time = fileInfo.at(i).lastModified();
            result.insert(name, time.toString("yyyy-MM-dd hh:mm:ss"));
        }
        if (fileInfo.count()>=2){
            result.remove(".");
            result.remove("..");
        }
        emit onFinish_signal(id, NOERROR, QByteArray(), QJsonValue(result));
    });
}

void FileControll::getFullFileNameList(const quint64 id, const QString& strDir,
                         const QStringList& lstFileNameFilter, int iDeepth, quint32 nTimeoutMillseconds)
{
    QString strDirectory =__getDobotPath(m_ip, strDir);

    auto pFileThread = new CGetFileListThread();
    pFileThread->SetId(id);
    pFileThread->SetDir(strDirectory);
    pFileThread->SetFileFilter(lstFileNameFilter);
    pFileThread->SetDeepth(iDeepth);

    //closureHandler(pFileThread, id, nTimeoutMillseconds);
    nTimeoutMillseconds = nTimeoutMillseconds ? nTimeoutMillseconds : DEFAULT_TIMEOUT;
    QPointer<CGetFileListThread> thdGuard(pFileThread);
    QTimer* pTimer = new QTimer(pFileThread);
    pTimer->setSingleShot(true);
    pTimer->setInterval(nTimeoutMillseconds);

    connect(thdGuard, &CGetFileListThread::signalFinishedResult, this, [=](quint64 id, QStringList fileList) {
        if (!thdGuard) return;
        if(!thdGuard->property("isTimeout").toBool())
        {
            for(auto itr = fileList.begin(); itr != fileList.end(); ++itr)
            {
                *itr = strDir+itr->mid(strDirectory.length());
            }
            emit signalFinishedGetFileListResult(id, NOERROR, fileList);
        }
        delete thdGuard;
    });
    connect(pTimer, &QTimer::timeout, this, [=] {
        if (!thdGuard) return;
        if (m_pool.tryTake(thdGuard)) {
            delete thdGuard;
        } else {
            thdGuard->setProperty("isTimeout", true);
        }
        qDebug() << "ERROR_FILE_TIMEOUT";
        emit signalFinishedGetFileListResult(id, ERROR_INDUSTRY_FILE_TIMEOUT, QStringList());
    });
    pTimer->start();
    m_pool.start(thdGuard);
}

void FileControll::DeleteFileName(const quint64 id, const QStringList& delFiles,quint32 nTimeoutMillseconds)
{
    QString strDirectory =__getDobotPath(m_ip);
    DeleteFilesThread *thread(new DeleteFilesThread(id, strDirectory, delFiles));
    closureHandler(thread, id, nTimeoutMillseconds);
}

void FileControll::pathIsExist(const quint64 id, const QString &path, quint32 timeout)
{
    QString r_fileName =__getDobotPath(m_ip, path);

    auto pFileThread = new PathIsExistThread(id, r_fileName);

    timeout = timeout ? timeout : DEFAULT_TIMEOUT;
    QPointer<PathIsExistThread> thdGuard(pFileThread);
    QTimer* pTimer = new QTimer(pFileThread);
    pTimer->setSingleShot(true);
    pTimer->setInterval(timeout);

    connect(thdGuard, &PathIsExistThread::signalFinishedResult, this, [=](quint64 id, bool bIsExist, bool bIsFile) {
        if (!thdGuard) return;
        if(!thdGuard->property("isTimeout").toBool())
        {
            QJsonObject obj;
            obj.insert("exist",bIsExist);
            obj.insert("isFile",bIsFile);
            emit onFinish_signal(id, NOERROR, QByteArray(), obj);
        }
        delete thdGuard;
    });
    connect(pTimer, &QTimer::timeout, this, [=] {
        if (!thdGuard) return;
        if (m_pool.tryTake(thdGuard)) {
            delete thdGuard;
        } else {
            thdGuard->setProperty("isTimeout", true);
        }
        qDebug() << "ERROR_FILE_TIMEOUT";
        emit onFinish_signal(id, ERROR_INDUSTRY_FILE_TIMEOUT, QByteArray(),QJsonValue());
    });
    pTimer->start();
    m_pool.start(thdGuard);
}

void FileControll::copyFileFromLocaleToSmb(const quint64 id, const QString& strLocaleFile, const QString& strSmbFile, bool bIsTruncate, quint32 timeout)
{
    QString strRemoteFile =__getDobotPath(m_ip, strSmbFile);
    QFileInfo info(strLocaleFile);
    if (0 == timeout)
    {
        if (info.size()>=104857600)
        {//100M
            timeout = 120000;//120s
        }
        else if (info.size()>=52428800)
        {//50M
            timeout = 60000;//60s
        }
        else
        {
            timeout = 30000;
        }
    }
    CopyFileLocaleToSMBThread *thread(new CopyFileLocaleToSMBThread(id, strLocaleFile, strRemoteFile));
    thread->setTruncate(bIsTruncate);
    closureHandler(thread, id, timeout);
}
