#include "FileControllSmb.h"

#include "DError/DError.h"
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QDebug>
#include <QtConcurrent>

FileControllSmb::FileControllSmb(const QString &ip, QObject *parent):
    FileControll(ip,parent)
{
    m_ip = ip;
    m_strShareDir = "project";
    m_smbClient.reset(new CSambaClient);

    m_pool.setMaxThreadCount(1);
}

FileControllSmb::~FileControllSmb()
{
}

void FileControllSmb::readFile(const quint64 id, const QString &fileName, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strRealFilePath;
    int iPos = fileName.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strRealFilePath = fileName.mid(iPos+m_strShareDir.size());
    else strRealFilePath = fileName;

    QtConcurrent::run(&m_pool, [=]{
        ReadThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strRealFilePath.toStdString());
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            QByteArray array;
            for(char ch : data)
            {
                array.append(ch);
            }
            emit onFinish_signal(id, code, array, QJsonValue());
        };
        smb.exec();
    });
}

void FileControllSmb::writeFile(const quint64 id, const QString &fileName, const QJsonValue &value, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strRealFilePath;
    int iPos = fileName.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strRealFilePath = fileName.mid(iPos+m_strShareDir.size());
    else strRealFilePath = fileName;

    QJsonDocument doc;
    if (value.isObject()) {
        doc.setObject(value.toObject());
    } else if (value.isArray()) {
        doc.setArray(value.toArray());
    }
    QByteArray arr = doc.toJson();
    std::vector<char> data;
    data.reserve(arr.size());
    for(int i=0; i<arr.size(); ++i)
    {
        data.push_back(arr.at(i));
    }

    QtConcurrent::run(&m_pool, [=]{
        WriteThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strRealFilePath.toStdString());
        smb.SetContent(data);
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}

void FileControllSmb::writeFile(const quint64 id, const QString &fileName, const QString &content, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strRealFilePath;
    int iPos = fileName.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strRealFilePath = fileName.mid(iPos+m_strShareDir.size());
    else strRealFilePath = fileName;

    QByteArray arr = content.toUtf8();
    std::vector<char> data;
    data.reserve(arr.size());
    for(int i=0; i<arr.size(); ++i)
    {
        data.push_back(arr.at(i));
    }

    QtConcurrent::run(&m_pool, [=]{
        WriteThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strRealFilePath.toStdString());
        smb.SetContent(data);
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}

void FileControllSmb::changeFile(const quint64 id, const QString &fileName, const QString &key, const QJsonValue &value, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strRealFilePath;
    int iPos = fileName.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strRealFilePath = fileName.mid(iPos+m_strShareDir.size());
    else strRealFilePath = fileName;

    QtConcurrent::run(&m_pool, [=]{
        ChangeFileThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strRealFilePath.toStdString());
        smb.SetNewContent(key.toStdString(), value);
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}


void FileControllSmb::newFile(const quint64 id, const QString &fileName, const QJsonValue &value, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strRealFilePath;
    int iPos = fileName.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strRealFilePath = fileName.mid(iPos+m_strShareDir.size());
    else strRealFilePath = fileName;

    QJsonDocument doc;
    if (value.isObject()) {
        doc.setObject(value.toObject());
    } else if (value.isArray()) {
        doc.setArray(value.toArray());
    }
    QByteArray arr = doc.toJson();
    std::vector<char> data;
    data.reserve(arr.size());
    for(int i=0; i<arr.size(); ++i)
    {
        data.push_back(arr.at(i));
    }

    QtConcurrent::run(&m_pool, [=]{
        NewFileThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strRealFilePath.toStdString());
        smb.SetContent(data);
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}

void FileControllSmb::newFile(const quint64 id, const QString &fileName, const QString &content, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strRealFilePath;
    int iPos = fileName.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strRealFilePath = fileName.mid(iPos+m_strShareDir.size());
    else strRealFilePath = fileName;

    QByteArray arr = content.toUtf8();
    std::vector<char> data;
    data.reserve(arr.size());
    for(int i=0; i<arr.size(); ++i)
    {
        data.push_back(arr.at(i));
    }

    QtConcurrent::run(&m_pool, [=]{
        NewFileThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strRealFilePath.toStdString());
        smb.SetContent(data);
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}

void FileControllSmb::decodeFile(const quint64 id, const QString &fileName, const QString &content, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strRealFilePath;
    int iPos = fileName.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strRealFilePath = fileName.mid(iPos+m_strShareDir.size());
    else strRealFilePath = fileName;

    QtConcurrent::run(&m_pool, [=]{
        DecodeFileThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strRealFilePath.toStdString());
        smb.SetContent(content.toStdString());
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}

void FileControllSmb::newFolder(const quint64 id, const QString &url, const QString &folderName, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strPath = url+'/'+folderName;
    strPath.replace('\\','/');
    strPath.replace("//","/");
    if (strPath.endsWith('/'))
    {
        strPath.remove(strPath.length()-1,1);
    }
    if (strPath.startsWith('/'))
    {
        strPath = strPath.mid(1);
    }

    QString strRealFilePath;
    int iPos = strPath.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strRealFilePath = strPath.mid(iPos+m_strShareDir.size());
    else strRealFilePath = strPath;

    QtConcurrent::run(&m_pool, [=]{
        NewFolderThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strRealFilePath.toStdString());
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}

void FileControllSmb::newFolderRecursive(const quint64 id, const QString &strSubDirRelative, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strPath = strSubDirRelative;
    strPath.replace('\\','/');
    strPath.replace("//","/");
    if (strPath.endsWith('/'))
    {
        strPath.remove(strPath.length()-1,1);
    }

    QString strRealFilePath;
    int iPos = strPath.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strRealFilePath = strPath.mid(iPos+m_strShareDir.size());
    else strRealFilePath = strPath;

    QtConcurrent::run(&m_pool, [=]{
        CreateDirThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strRealFilePath.toStdString());
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}

void FileControllSmb::renameFolder(const quint64 id, const QString &folderName, const QString &newfolderName, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strOldPath = folderName;
    strOldPath.replace('\\','/');
    strOldPath.replace("//","/");
    if (strOldPath.endsWith('/'))
    {
        strOldPath.remove(strOldPath.length()-1,1);
    }
    int iPos = strOldPath.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strOldPath = strOldPath.mid(iPos+m_strShareDir.size());

    QString strNewPath = newfolderName;
    strNewPath.replace('\\','/');
    strNewPath.replace("//","/");
    if (strNewPath.endsWith('/'))
    {
        strNewPath.remove(strNewPath.length()-1,1);
    }
    iPos = strNewPath.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strNewPath = strNewPath.mid(iPos+m_strShareDir.size());

    QtConcurrent::run(&m_pool, [=]{
        RenameFolderThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strOldPath.toStdString());
        smb.SetNewNamePath(strNewPath.toStdString());
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}

void FileControllSmb::copyFolder(const quint64 id, const QString &url, const QString &folderName, const QString &newfolderName, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strOldPath = url+'/'+folderName;
    strOldPath.replace('\\','/');
    strOldPath.replace("//","/");
    if (strOldPath.endsWith('/'))
    {
        strOldPath.remove(strOldPath.length()-1,1);
    }
    int iPos = strOldPath.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strOldPath = strOldPath.mid(iPos+m_strShareDir.size());

    QString strNewPath = url+'/'+newfolderName;
    strNewPath.replace('\\','/');
    strNewPath.replace("//","/");
    if (strNewPath.endsWith('/'))
    {
        strNewPath.remove(strNewPath.length()-1,1);
    }
    iPos = strNewPath.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strNewPath = strNewPath.mid(iPos+m_strShareDir.size());

    QtConcurrent::run(&m_pool, [=]{
        CopyFolderThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strOldPath.toStdString());
        smb.SetNewPath(strNewPath.toStdString());
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}

void FileControllSmb::deleteFolder(const quint64 id, const QString &url, const QString &folderName, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strPath = url+'/'+folderName;
    strPath.replace('\\','/');
    strPath.replace("//","/");
    if (strPath.endsWith('/'))
    {
        strPath.remove(strPath.length()-1,1);
    }
    int iPos = strPath.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strPath = strPath.mid(iPos+m_strShareDir.size());

    QtConcurrent::run(&m_pool, [=]{
        DeleteFolderThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strPath.toStdString());
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}

void FileControllSmb::readFolder(const quint64 id, const QString &folderName)
{
    QJsonObject result;

    QRegExp reg("^((25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)\\.){3}(25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)$");
    if (m_ip.isEmpty() || !reg.exactMatch(m_ip))
    {
#ifdef __arm__
        Q_UNUSED(ip)
        QString root("/dobot/userdata");
#else
        QString root = QString("\\\\%1").arg(m_ip);
#endif
        QString r_folderName = folderName;
        if (!r_folderName.startsWith("/")) {
            r_folderName = "/" + r_folderName;
        }
        r_folderName = root+r_folderName;
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
    else
    {
        int timeout = 2;

        QString strPath = folderName;
        strPath.replace('\\','/');
        strPath.replace("//","/");
        if (strPath.endsWith('/'))
        {
            strPath.remove(strPath.length()-1,1);
        }
        int iPos = strPath.indexOf(QString(m_strShareDir.c_str()));
        if (iPos>=0) strPath = strPath.mid(iPos+m_strShareDir.size());

        QtConcurrent::run(&m_pool, [=]{
            ReadFolderThreadSmb smb(m_smbClient);
            smb.SetId(id);
            smb.SetServer(m_ip.toStdString());
            smb.SetShareDir(m_strShareDir);
            smb.SetUser(m_strUser);
            smb.SetPassword(m_strPwd);
            smb.SetTimeoutSeconds(timeout);
            smb.SetPath(strPath.toStdString());
            smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
                Q_UNUSED(data);
                emit onFinish_signal(id, code, QByteArray(), QJsonValue());
            };
            smb.OnFinishedResult = [this](uint64_t id, const std::list<
                                                                    std::tuple<std::string,//filename
                                                                                uint64_t>//lastmodifytime
                                                                    >& fileList){
                QJsonObject result;
                QString name;
                QDateTime time;
                for(const auto& v : fileList)
                {
                    name = QString(std::get<0>(v).c_str());
                    time = QDateTime::fromMSecsSinceEpoch(std::get<1>(v));
                    result.insert(name, time.toString("yyyy-MM-dd hh:mm:ss"));
                }
                emit onFinish_signal(id, NOERROR, QByteArray(), QJsonValue(result));
            };
            smb.exec();
        });
    }
}

void FileControllSmb::getFullFileNameList(const quint64 id, const QString& strDir,
                         const QStringList& lstFileNameFilter, int iDeepth, quint32 nTimeoutMillseconds)
{
    int timeout = nTimeoutMillseconds/1000;
    timeout = timeout ? timeout : 2;

    std::vector<std::string> vcFilter;
    foreach(QString str, lstFileNameFilter)
    {
        vcFilter.push_back(str.toStdString());
    }

    QString strPath = strDir;
    strPath.replace('\\','/');
    strPath.replace("//","/");
    if (strPath.endsWith('/'))
    {
        strPath.remove(strPath.length()-1,1);
    }
    int iPos = strPath.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strPath = strPath.mid(iPos+m_strShareDir.size());

    QtConcurrent::run(&m_pool, [=]{
        CGetFileListThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strPath.toStdString());
        smb.SetDeepth(iDeepth);
        smb.SetFileFilter(vcFilter);
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.OnFinishedResult = [this](uint64_t id, const std::list<std::string>& fileList){
            QStringList allFiles;
            for(auto f : fileList)
            {
                allFiles.push_back(QString(f.c_str()));
            }
            emit signalFinishedGetFileListResult(id, NOERROR, allFiles);
        };
        smb.exec();
    });
}

void FileControllSmb::DeleteFileName(const quint64 id, const QStringList& delFiles,quint32 nTimeoutMillseconds)
{
    int timeout = nTimeoutMillseconds/1000;
    timeout = timeout ? timeout : 2;

    std::vector<std::string> vcFile;
    vcFile.reserve(delFiles.size());
    foreach(QString strPath, delFiles)
    {
        strPath.replace('\\','/');
        strPath.replace("//","/");
        if (strPath.endsWith('/'))
        {
            strPath.remove(strPath.length()-1,1);
        }
        int iPos = strPath.indexOf(QString(m_strShareDir.c_str()));
        if (iPos>=0) strPath = strPath.mid(iPos+m_strShareDir.size());

        vcFile.push_back(strPath.toStdString());
    }

    QtConcurrent::run(&m_pool, [=]{
        DeleteFilesThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetDeleteFiles(vcFile);
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}

void FileControllSmb::pathIsExist(const quint64 id, const QString &path, quint32 timeout)
{
    timeout = timeout/1000;
    timeout = timeout ? timeout : 2;

    QString strRealFilePath;
    int iPos = path.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strRealFilePath = path.mid(iPos+m_strShareDir.size());
    else strRealFilePath = path;

    QtConcurrent::run(&m_pool, [=]{
        PathIsExistThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strRealFilePath.toStdString());
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.OnFinishedResult = [this](uint64_t id, bool isExist,bool isFile){
            QJsonObject obj;
            obj.insert("exist",isExist);
            obj.insert("isFile",isFile);
            emit onFinish_signal(id, NOERROR, QByteArray(), obj);
        };
        smb.exec();
    });
}

void FileControllSmb::copyFileFromLocaleToSmb(const quint64 id, const QString& strLocaleFile, const QString& strSmbFile, bool bIsTruncate, quint32 timeout)
{
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

    QString strRealFilePath;
    int iPos = strSmbFile.indexOf(QString(m_strShareDir.c_str()));
    if (iPos>=0) strRealFilePath = strSmbFile.mid(iPos+m_strShareDir.size());
    else strRealFilePath = strSmbFile;

    QtConcurrent::run(&m_pool, [=]{
        CopyFileLocaleToSMBThreadSmb smb(m_smbClient);
        smb.SetId(id);
        smb.SetServer(m_ip.toStdString());
        smb.SetShareDir(m_strShareDir);
        smb.SetUser(m_strUser);
        smb.SetPassword(m_strPwd);
        smb.SetTimeoutSeconds(timeout);
        smb.SetPath(strRealFilePath.toStdString());
        smb.SetLocalFile(strLocaleFile.toStdString());
        smb.SetTruncate(bIsTruncate);
        smb.OnFinished = [this](uint64_t id, int code, const std::vector<char>& data){
            Q_UNUSED(data);
            emit onFinish_signal(id, code, QByteArray(), QJsonValue());
        };
        smb.exec();
    });
}



