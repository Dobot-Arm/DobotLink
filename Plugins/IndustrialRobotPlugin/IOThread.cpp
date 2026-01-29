#include "IOThread.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QThread>
#include <QQueue>
#include <QDateTime>
#include "DError/DError.h"


ReadThread::ReadThread(
        const quint64 id,
        const QString &fileName,
        QObject *parent):
    BaseThread(id, parent),
    m_fileName(fileName)
{
}

void ReadThread::exec()
{
    QFile file(m_fileName);
    QJsonValue value;
    QFileInfo info(file);
    if (!info.exists()) {
        qDebug() << "file don't exist." << file<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_NOT_EXIST);
    } else if (!file.open(QFile::ReadOnly)) {
        qDebug() << m_fileName<<"file cannot open." << file.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
    } else {
        QByteArray data = file.readAll();
        file.close();

        qint64 createTime = info.birthTime().toMSecsSinceEpoch();
        qint64 lastModifyTime = info.lastModified().toMSecsSinceEpoch();
        QJsonObject objTime;
        objTime.insert("createTimestamp", createTime);
        objTime.insert("lastModifyTimestamp", lastModifyTime);

        finish(NOERROR, data,objTime);
        qDebug() << m_fileName<<"file read ok, id:"<<m_id;
    }
}

WriteThread::WriteThread(
        const quint64 id,
        const QString &fileName,
        const QJsonValue &value,
        QObject *parent):
    BaseThread(id, parent),
    m_fileName(fileName)
{
    QJsonDocument doc;
    if (value.isObject()) {
        doc.setObject(value.toObject());
    } else if (value.isArray()) {
        doc.setArray(value.toArray());
    }
    m_bytes = doc.toJson();
}

WriteThread::WriteThread(
        const quint64 id,
        const QString &fileName,
        const QString &content,
        QObject *parent):
    BaseThread(id, parent),
    m_fileName(fileName),
    m_bytes(content.toUtf8())
{
}

void WriteThread::exec()
{
    QFile file(m_fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << file << "file cannot open." << file.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
    } else if (file.write(m_bytes) == -1) {
        file.close();
        qDebug() << file << "file cannot write." << file.errorString()<<" id:"<<m_id;
        finish( ERROR_INDUSTRY_FILE_CAN_NOT_WRITE);
    } else {
        file.close();
        finish(NOERROR);
        qDebug() << m_fileName<<"file write ok, id:"<<m_id;
    }
}

ChangeFileThread::ChangeFileThread(
        const quint64 id,
        const QString &fileName,
        const QString &key,
        const QJsonValue &value,
        QObject *parent):
    BaseThread(id, parent),
    m_fileName(fileName),
    m_key(key),
    m_value(value)
{
}

void ChangeFileThread::exec()
{
    QFile file(m_fileName);


    QFileInfo info(file);
    if (!info.exists()) {
        qDebug() << "file don't exist." << file<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_NOT_EXIST);
    } else if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file << "file cannot open." << file.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
    } else {
        QJsonParseError err;
        QJsonDocument jdc = QJsonDocument::fromJson(file.readAll(), &err);
        if (err.error != QJsonParseError::NoError || !jdc.isObject())
        {
            qDebug() << file << "file content is not json object." << err.errorString()<<" id:"<<m_id;
            file.close();
            finish(ERROR_INDUSTRY_JSONOBJECT);
            return ;
        }
        file.close();
        if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
            qDebug() << file << "file cannot open&truncate." << file.errorString()<<" id:"<<m_id;
            finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
        }

        QJsonObject obj = jdc.object();
        //修改key对应的value
        obj[m_key] = m_value;
        jdc.setObject(obj);
        file.seek(0);

        if (file.write(jdc.toJson()) == -1) {
            file.close();
            qDebug() << file << "file cannot write." << file.errorString()<<" id:"<<m_id;
            finish(ERROR_INDUSTRY_FILE_CAN_NOT_WRITE);
        } else {
            file.close();
            finish(NOERROR,QByteArray(),obj);
            qDebug() << m_fileName<<"file change ok, id:"<<m_id;
        }
    }
}

NewFileThread::NewFileThread(
        const quint64 id,
        const QString &fileName,
        const QJsonValue &value,
        QObject *parent):
    BaseThread(id, parent),
    m_fileName(fileName)
{
    QJsonDocument doc;
    if (value.isObject()) {
        doc.setObject(value.toObject());
    } else if (value.isArray()) {
        doc.setArray(value.toArray());
    }
    m_bytes = doc.toJson();
}

NewFileThread::NewFileThread(
        const quint64 id,
        const QString &fileName,
        const QString &content,
        QObject *parent):
    BaseThread(id, parent),
    m_fileName(fileName),
    m_bytes(content.toUtf8())
{
}

void NewFileThread::exec()
{
    QFile file(m_fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << m_fileName<<"file cannot open." << file.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
    } else if (file.write(m_bytes) == -1) {
        file.close();
        qDebug() << file << "file cannot write." << file.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_WRITE);
    } else {
        file.close();
        finish(NOERROR);
        qDebug() << m_fileName<<"file create ok, id:"<<m_id;
    }
}

DecodeFileThread::DecodeFileThread(
        const quint64 id,
        const QString &fileName,
        const QString &content,
        QObject *parent):
    BaseThread(id, parent),
    m_fileName(fileName),
    m_content(content)
{
}

void DecodeFileThread::exec()
{
    QByteArray array(m_content.toUtf8());
    QFile file(m_fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << m_fileName<<"file cannot open." << file.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
    } else if (file.write(QByteArray::fromBase64(array)) == -1) {
        file.close();
        qDebug() << m_fileName<<"file cannot write." << file.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_WRITE);
    } else {
        file.close();
        finish(NOERROR);
        qDebug() << m_fileName<<"file decode ok, id:"<<m_id;
    }
}

NewFolderThread::NewFolderThread(
        const quint64 id,
        const QString &url,
        const QString &folderName,
        const QDir &dir,
        QObject *parent):
    BaseThread(id, parent),
    m_url(url),
    m_folderName(folderName),
    m_dir(dir)
{
}

void NewFolderThread::exec()
{
    if (m_dir.exists(m_folderName)) {
        qDebug() << "folder already exist." << m_dir<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_ALREADY_EXIST);
    } else if (!m_dir.mkdir(m_folderName)) {
        qDebug() << "folder cannot create." << m_dir<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_CAN_NOT_CREATE);
    } else {
        finish(NOERROR);
        qDebug() << m_dir<<"folder create ok, id:"<<m_id;
    }
}

CreateDirThread::CreateDirThread(
        const quint64 id,
        const QString &strDir,
        QObject *parent):
    BaseThread(id, parent),
    m_strDir(strDir)
{
}

void CreateDirThread::exec()
{
    QDir dir;
    if (!dir.mkpath(m_strDir)) {
        qDebug() << "folder cannot create." << m_strDir<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_CAN_NOT_CREATE);
    } else {
        finish(NOERROR);
        qDebug() << m_strDir<<"dir create ok, id:"<<m_id;
    }
}

RenameFolderThread::RenameFolderThread(
        const quint64 id,
        const QString &folderName,
        const QString &newfolderName,
        QObject *parent):
    BaseThread(id, parent),
    m_folderName(folderName),
    m_newfolderName(newfolderName)
{
}

void RenameFolderThread::exec()
{
    QFile file(m_folderName);
    if (!file.exists()){
        finish(ERROR_INDUSTRY_FOLDER_NOT_EXIST);
    } else if (file.isOpen()) {
        qDebug() << file << "file is open, please close it." << file.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_HAD_OPENED);
    } else if (!file.rename(m_newfolderName)) {
        qDebug() << file << "file cannot rename." << file.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_CAN_NOT_RENAME);
    } else {
        finish(NOERROR);
        qDebug() << file << "folder rename ok, id:"<<m_id;
    }
}

ReadFolderThread::ReadFolderThread(
        const quint64 id,
        const QString &folderName,
        bool bIsOnlyFolder,
        QObject *parent):
    BaseThread(id, parent),
    m_folderName(folderName),
    m_bIsOnlyFolder(bIsOnlyFolder)
{
}

void ReadFolderThread::exec()
{
    QFile file(m_folderName);
    QDir dir(m_folderName);
    QStringList filter;
    QJsonObject result;
    QString name;
    QDateTime time;
    QFileInfoList fileInfo = dir.entryInfoList(filter);
    for (int i=0; i<fileInfo.count(); i++) {
        name = fileInfo.at(i).fileName();
        time = fileInfo.at(i).lastModified();
        if (m_bIsOnlyFolder)
        {
            if (fileInfo.at(i).isDir())
            {
                result.insert(name, time.toString("yyyy-MM-dd hh:mm:ss"));
            }
        }
        else
        {
            result.insert(name, time.toString("yyyy-MM-dd hh:mm:ss"));
        }
    }
    if (fileInfo.count()>=2){
        result.remove(".");
        result.remove("..");
    }
    finish(NOERROR, QByteArray(), result);
}

CopyFolderThread::CopyFolderThread(
        const quint64 id,
        const QDir &dir,
        const QDir &fromDir,
        const QDir &toDir,
        const QString &folderName,
        const QString &newfolderName,
        QObject *parent):
    BaseThread(id, parent),
    m_dir(dir),
    m_fromDir(fromDir),
    m_toDir(toDir),
    m_folderName(folderName),
    m_newfolderName(newfolderName)
{
}

void CopyFolderThread::exec()
{
    QFile file(m_folderName);

    if (!m_fromDir.exists()) {
        finish(ERROR_INDUSTRY_FOLDER_NOT_EXIST);
        qDebug() << m_fromDir<<"folder not exist, id:"<<m_id;
    } else if (m_dir.exists(m_newfolderName)){
        finish(ERROR_INDUSTRY_FOLDER_ALREADY_EXIST);
        qDebug() << m_dir<<"folder not exist, id:"<<m_id;
    } else if (!m_dir.mkdir(m_newfolderName)) {
        finish(ERROR_INDUSTRY_FOLDER_CAN_NOT_CREATE);
        qDebug() << m_dir<<"folder make error, id:"<<m_id;
    } else if (file.isOpen()) {
        finish(ERROR_INDUSTRY_FILE_HAD_OPENED);
        qDebug() << file << "folder open error, id:"<<m_id;
    } else {
        QFileInfoList fileInfoList = m_fromDir.entryInfoList();
        foreach(QFileInfo fileInfo, fileInfoList)
        {
            if (fileInfo.fileName() == "." || fileInfo.fileName() == "..") {
                continue;
            }
            if (!QFile::copy(fileInfo.filePath(), m_toDir.filePath(fileInfo.fileName()))) {
                qDebug() << fileInfo.fileName();
                qDebug() << fileInfo.filePath();
                qDebug() << m_toDir.filePath(fileInfo.fileName());
                qDebug() << "file cannot copy"<<" id:"<<m_id;
                finish(ERROR_INDUSTRY_FILE_CAN_NOT_COPY);
                return;
            }
        }
        finish(NOERROR);
        qDebug() << "folder copy ok, id:"<<m_id;
    }
}

DeleteFolderThread::DeleteFolderThread(
        const quint64 id,
        const QString &url,
        const QString &folderName,
        const QString &path,
        const QDir &dir,
        QObject *parent):
    BaseThread(id, parent),
    m_url(url),
    m_folderName(folderName),
    m_dir(dir),
    m_path(path)
{
}

void DeleteFolderThread::exec()
{
    QFile file(m_path);

    if (!m_dir.exists(m_folderName)) {
        finish(ERROR_INDUSTRY_FOLDER_NOT_EXIST);
        qDebug() << m_dir<<"folder not exist, id:"<<m_id;
    } else if (file.isOpen()) {
        finish(ERROR_INDUSTRY_FILE_HAD_OPENED);
        qDebug() << file<<"folder is opened, id:"<<m_id;
    } else if (!m_dir.cd(m_folderName)) {
        finish(ERROR_INDUSTRY_FOLDER_CAN_NOT_ENTER);
        qDebug() << m_dir<<"folder cd failed, id:"<<m_id;
    } else if(!m_dir.removeRecursively()) {
        finish(ERROR_INDUSTRY_FOLDER_CAN_NOT_REMOVE);
        qDebug() << m_dir<<"folder remove failed, id:"<<m_id;
    } else {
        finish(NOERROR);
        qDebug() << m_dir<<"folder delete ok, id:"<<m_id;
    }
}

CGetFileListThread::CGetFileListThread(QObject *parent)
    :BaseThread(0,parent),
      m_iDeepth(0)
{
    m_lstFilter<<"*";
}

void CGetFileListThread::SetId(quint64 id)
{
    m_id = id;
}

void CGetFileListThread::SetDeepth(int iDeepth)
{
    m_iDeepth = iDeepth>=0 ? iDeepth : 0;
}

void CGetFileListThread::SetDir(const QString& strDir)
{
    m_strDir = strDir;
    if (!(m_strDir.endsWith('/')||m_strDir.endsWith('\\')))
    {
        m_strDir.append('/');
    }
}

void CGetFileListThread::SetFileFilter(const QStringList& lstFilter)
{
    m_lstFilter = lstFilter;
}

void CGetFileListThread::exec()
{
    QStringList lstResult;

    QQueue<QStringList> allSubDirs;
    allSubDirs.enqueue(QStringList(m_strDir));

    int iDeepth = m_iDeepth;

    while (iDeepth>=0 && allSubDirs.size()>0)
    {
        QStringList subDirs = allSubDirs.dequeue();

        QStringList tmpDirs;
        foreach (auto strDir , subDirs)
        {
            if (!(strDir.endsWith('/')||strDir.endsWith('\\')))
            {
                strDir.append('/');
            }
            qDebug()<<__FUNCTION__<<",folder is:"<<strDir;
            QDir dir(strDir);
            QFileInfoList infoList = dir.entryInfoList(m_lstFilter,QDir::Files|QDir::Dirs);
            foreach (const auto& info , infoList)
            {
                if (info.isDir() && !info.fileName().startsWith('.'))
                {
                    tmpDirs.append(strDir+info.fileName());
                }
                else if (info.isFile())
                {
                    lstResult.append(strDir+info.fileName());
                }
            }
        }

        --iDeepth;
        if (tmpDirs.size()>0 && iDeepth>=0)
        {
            allSubDirs.enqueue(tmpDirs);
        }
    }

    QJsonArray arr;
    foreach(const auto& file, lstResult)
    {
        arr.append(file);
    }
    finish(NOERROR, QByteArray(), arr);
}

DeleteFilesThread::DeleteFilesThread(
            const quint64 id,
            const QString& strUrl,
            const QStringList& delFiles,
            QObject *parent)
            :BaseThread(id, parent),
            m_strUrl(strUrl),
            m_delFiles(delFiles)
{
}

void DeleteFilesThread::exec()
{
    foreach(QString strFile, m_delFiles)
    {
        QFile file(m_strUrl+strFile);
        if (!file.remove())
        {
            qDebug()<<"delete failed:"<<file.fileName()<<file.errorString()<<" id:"<<m_id;
        }
    }
    finish(NOERROR);
    qDebug() << "delete all files ok, id:"<<m_id;
}

PathIsExistThread::PathIsExistThread(
            const quint64 id,
            const QString& strUrl,
            QObject *parent)
            :BaseThread(id, parent),
            m_url(strUrl)
{
}

void PathIsExistThread::exec()
{
    QFileInfo info(m_url);
    bool bOk = info.exists();

    QJsonObject obj;
    obj.insert("exist",bOk);
    obj.insert("isFile",info.isFile());
    obj.insert("fileSize",bOk?info.size():0);
    finish(NOERROR, QByteArray(), obj);
}

CopyFileLocaleToSMBThread::CopyFileLocaleToSMBThread(const quint64 id,
                                                     const QString &strLocalFile,
                                                     const QString &strSmbFile,
                                                     QObject *parent)
                                                    :BaseThread(id, parent),
                                                      m_strLocalFile(strLocalFile),
                                                      m_strSmbFile(strSmbFile)
{
    m_bIsTruncate = false;
}

void CopyFileLocaleToSMBThread::exec()
{
    QFile file(m_strLocalFile);
    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << m_strLocalFile << "file cannot open,"<<file.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
        return ;
    }

    bool bCreated = false;
    QFile fileSmb(m_strSmbFile);
    if (m_bIsTruncate)
    {
        bCreated = fileSmb.open(QFile::WriteOnly|QFile::Truncate);
    }
    else
    {
        bCreated = fileSmb.open(QFile::WriteOnly|QFile::NewOnly);
    }
    if (!bCreated)
    {
        qDebug() << m_strSmbFile << "file cannot create,"<<fileSmb.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
        return ;
    }

    std::unique_ptr<char[]> uptr(new char[1024]);
    do
    {
        const qint64 iRead = file.read(uptr.get(), 1024);
        if (iRead < 0)
        {
            qDebug() <<"file cannot read,"<<file.errorString()<<" id:"<<m_id;
            finish(ERROR_INDUSTRY_FILE_CAN_NOT_COPY);
            return ;
        }
        if (iRead == 0)
        {
            break;
        }
        qint64 hasWrite = 0;
        while (hasWrite < iRead)
        {
            qint64 iWrite = fileSmb.write(uptr.get(), iRead-hasWrite);
            if (iWrite <= 0)
            {
                qDebug() <<fileSmb<<"file cannot write,"<<fileSmb.errorString()<<" id:"<<m_id;
                finish(ERROR_INDUSTRY_FILE_CAN_NOT_COPY);
                return ;
            }
            hasWrite += iWrite;
        }
    }while(true);
    finish(NOERROR);
    qDebug() << "copy file locale to smb ok, id:"<<m_id;
}



