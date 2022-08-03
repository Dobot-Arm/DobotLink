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

CMyThreadPool* FileControll::m_pool = new CMyThreadPool(40);

FileControll::FileControll(const QString &ip, QObject *parent):
    QObject(parent)
{
    m_ip = ip;
}

FileControll::~FileControll()
{
    m_pool->Clear(this);
}

void FileControll::closureHandler(BaseThread *thread, quint64 id, quint32 timeout)
{
    timeout = timeout ? timeout : DEFAULT_TIMEOUT;
    //QPointer<BaseThread> thread(_thread);
    QTimer *timer(new QTimer(thread));
    timer->setSingleShot(true);
    timer->setInterval(timeout);

    connect(thread, &BaseThread::onFinished_signal, this, [=](quint64 _id, int code, QByteArray array,QJsonValue json) {
        Q_UNUSED(_id)
        if(!thread->property("isTimeout").toBool()) {
            emit onFinish_signal(id, code, array, json);
        }
        timer->stop();
    });
    connect(thread, &BaseThread::onBegin_signal, this, [=]{
        timer->start();
    });
    connect(timer, &QTimer::timeout, this, [=] {
        thread->setProperty("isTimeout", true);
        thread->disconnect();
        qDebug() << "ERROR_FILE_TIMEOUT";
        emit onFinish_signal(id, ERROR_INDUSTRY_FILE_TIMEOUT, QByteArray(),QJsonValue());
    });

    thread->setAutoDelete(true);
    m_pool->start(this,[thread](bool isCanceled){
        if (!isCanceled)
        {
            thread->run();
        }
        if (thread->autoDelete())
        {
            thread->deleteLater();
        }
    });
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


void FileControll::readFolder(const quint64 id, const QString &folderName, quint32 timeout)
{
    QString r_folderName = __getDobotPath(m_ip, folderName);
    qDebug() << __FUNCTION__ << r_folderName;
    ReadFolderThread *thread(new ReadFolderThread(id, r_folderName));
    closureHandler(thread, id, timeout);
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

    closureHandler(pFileThread, id, nTimeoutMillseconds);
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
    PathIsExistThread *thread(new PathIsExistThread(id, r_fileName));
    closureHandler(thread, id, timeout);
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
