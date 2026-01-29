#include "IOThreadSmb.h"
#include <QDebug>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QRegExp>
#include <QFile>
#include <QDeadlineTimer>
#include <QThread>
#include "DError/DError.h"

#include <algorithm>
#include <queue>

#include "CSambaCore.h"
#include "CSambaDirectory.h"
#include "CSambaFile.h"

//CSambaClient BaseThreadSmb::m_smbClient;
BaseThreadSmb::BaseThreadSmb(const std::shared_ptr<CSambaClient>& sptr):
    m_smbClient(sptr)
{
    OnFinished = nullptr;
    m_iTimeoutSeconds = 2;

    if (!CSambaCore::GetInstance().IsLoaded())
    {
        QString strDll = QCoreApplication::applicationDirPath()+QDir::separator()+QString("smb2");
        CSambaCore::GetInstance().LoadDll(strDll);
    }
}

#define SMB_CHECK_AND_CONNECT()                                           \
if (!m_smbClient->IsConnected())                                           \
{                                                                         \
    m_smbClient->SetServer(m_strIp);                                       \
    m_smbClient->SetShareDir(m_strShareDir);                               \
    m_smbClient->SetUser(nullptr);                                       \
    m_smbClient->SetPwd(nullptr);                                         \
    m_smbClient->Connect();                                                \
}                                                                         \
if (!m_smbClient->IsConnected())                                           \
{                                                                         \
    qDebug()<<"connect smb fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id; \
    finish(ERROR_INDUSTRY_FILE_TIMEOUT);                                  \
    return ;                                                              \
}                                                                         \
m_smbClient->SetTimeout(m_iTimeoutSeconds);

#define SMBV2_WRITE_FILE_BATCH(smbObj,writeData)                                          \
int iMaxSend = m_smbClient->GetMaxWriteSize();                                            \
if (iMaxSend <= 0 || iMaxSend > 30720){                                                   \
    iMaxSend = 30720;                                                                     \
}                                                                                         \
const char* pszData = writeData.data();                                                   \
const int iTotalSize = writeData.size();                                                  \
int iHasWrite = 0;                                                                        \
m_smbClient->SetTimeout(2);                                                               \
QDeadlineTimer deadline(m_iTimeoutSeconds*1000+500);                                      \
do{                                                                                       \
    if (iTotalSize-iHasWrite < iMaxSend){                                                 \
        iMaxSend = iTotalSize-iHasWrite;                                                  \
    }                                                                                     \
    int iWritten = smbObj.Write(pszData+iHasWrite,iMaxSend);                              \
    if (iWritten <= 0){                                                                   \
        qDebug()<<"write file fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id; \
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_WRITE);                                          \
        return ;                                                                            \
    }                                                                                       \
    iHasWrite += iWritten;                                                                  \
    QThread::msleep(1);                                                                     \
}while(iHasWrite<iTotalSize && !deadline.hasExpired());                                     \
if (iHasWrite < iTotalSize){                                                                \
    qDebug()<<"write file timeout,id:"<<m_id;                                               \
    finish(ERROR_INDUSTRY_FILE_TIMEOUT);                                                    \
    return ;                                                                                \
}

#define SMBV2_WRITE_FILE_BATCH2(smbObj,writeData) \
int iMaxSend = 3e6; \
const char* pszData = writeData.data();                                                   \
const int iTotalSize = writeData.size();                                                  \
int iHasWrite = 0;                                                                        \
m_smbClient->SetTimeout(m_iTimeoutSeconds);                                               \
do{                                                                                       \
    if (iTotalSize-iHasWrite < iMaxSend){                                                 \
        iMaxSend = iTotalSize-iHasWrite;                                                  \
    }                                                                                     \
    int iWritten = smbObj.Write(pszData+iHasWrite,iMaxSend);                              \
    if (iWritten <= 0){                                                                   \
        qDebug()<<"write file fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id; \
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_WRITE);                                          \
        return ;                                                                            \
    }                                                                                       \
    iHasWrite += iWritten;                                                                  \
    QThread::msleep(1);                                                                     \
}while(iHasWrite<iTotalSize);

void ReadThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();

    CSambaFile smb(m_smbClient.get());
    if (!smb.Open(m_strPath))
    {
        qDebug()<<m_strPath.c_str()<<"read file fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
        return ;
    }
    std::string strValue = smb.ReadAllText().c_str();
    smb.Close();
    std::vector<char> data(strValue.begin(),strValue.end());

    qint64 createTime = smb.GetFileInfo().nCreateTime;
    qint64 lastModifyTime = smb.GetFileInfo().nLastModifyTime;
    QJsonObject objTime;
    objTime.insert("createTimestamp", createTime);
    objTime.insert("lastModifyTimestamp", lastModifyTime);
    QJsonDocument doc(objTime);
    QString strJson(doc.toJson());

    finish(NOERROR, data, strJson.toStdString());
}

void WriteThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();

    CSambaFile smb(m_smbClient.get());
    if (!smb.OpenTruncate(m_strPath))
    {
        qDebug()<<m_strPath.c_str()<<"open file fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
        return ;
    }
    if (m_bytes.size() > 0)
    {
        SMBV2_WRITE_FILE_BATCH2(smb, m_bytes);
    }
    smb.Close();
    finish(NOERROR);
}

void ChangeFileThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();

    //read
    CSambaFile smb(m_smbClient.get());
    if (!smb.Open(m_strPath))
    {
        qDebug()<<m_strPath.c_str()<<"read file fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
        return ;
    }
    QByteArray data(smb.ReadAllText().c_str());
    smb.Close();

    QJsonParseError err;
    QJsonDocument jdc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !jdc.isObject())
    {
        qDebug() << m_strPath.c_str() << "file content is not json object." << err.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_JSONOBJECT);
        return ;
    }
    QJsonObject obj = jdc.object();
    //修改key对应的value
    obj[QString(m_strKey.c_str())] = m_strContent;
    jdc.setObject(obj);
    data = jdc.toJson();

    //write
    if (!smb.OpenTruncate(m_strPath))
    {
        qDebug()<<m_strPath.c_str()<<"open file fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
        return ;
    }
    if (data.size() > 0)
    {
        SMBV2_WRITE_FILE_BATCH2(smb, data);
    }
    smb.Close();

    std::vector<char> dataTmp(data.begin(),data.end());
    finish(NOERROR, dataTmp);
}

void NewFileThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();

    CSambaFile smb(m_smbClient.get());
    if (!smb.OpenTruncate(m_strPath))
    {
        qDebug()<<m_strPath.c_str()<<"open file fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
        return ;
    }
    if (m_bytes.size()>0)
    {
        SMBV2_WRITE_FILE_BATCH2(smb, m_bytes);
    }
    smb.Close();
    finish(NOERROR);
}

void DecodeFileThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();

    CSambaFile smb(m_smbClient.get());
    if (!smb.OpenTruncate(m_strPath))
    {
        qDebug()<<m_strPath.c_str()<<"open file fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
        return ;
    }
    if (!m_strContent.empty())
    {
        QByteArray bytes = QByteArray::fromBase64(QByteArray(m_strContent.c_str()));
        SMBV2_WRITE_FILE_BATCH2(smb, bytes);
    }
    smb.Close();
    finish(NOERROR);
}

void NewFolderThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();

    CSambaDirectory smb(m_smbClient.get());
    if (smb.IsExists(m_strPath))
    {
        qDebug() <<m_strPath.c_str()<<"folder already exist." << m_strPath.c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_ALREADY_EXIST);
        return ;
    }
    if (!smb.Create(m_strPath))
    {
        qDebug()<<m_strPath.c_str()<<"create dir fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_CAN_NOT_CREATE);
        return ;
    }
    smb.Close();
    finish(NOERROR);
}

void CreateDirThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();

    std::transform(m_strPath.begin(),m_strPath.end(),m_strPath.begin(), [](char ch){
        if ('\\' == ch) return '/';
        return ch;
    });
    CSambaDirectory smb(m_smbClient.get());
    if (!smb.CreateRecursive(m_strPath))
    {
        qDebug()<<m_strPath.c_str()<<" create dir fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_CAN_NOT_CREATE);
        return;
    }
    finish(NOERROR);
    return ;
}

void RenameFolderThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();

    CSambaDirectory smb(m_smbClient.get());
    if (!smb.IsExists(m_strPath))
    {
        finish(ERROR_INDUSTRY_FOLDER_NOT_EXIST);
        return;
    }
    if (!smb.Rename(m_strPath, m_strNewNamePath))
    {
        qDebug()<<m_strPath.c_str()<<"file rename "<<m_strNewNamePath.c_str()<<"fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_CAN_NOT_RENAME);
        return;
    }
    finish(NOERROR);
}

void CopyFolderThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();

    CSambaDirectory smb(m_smbClient.get());
    if (!smb.IsExists(m_strPath))
    {
        qDebug()<<m_strPath.c_str()<<" dir not exist,"<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_NOT_EXIST);
        return;
    }
    if (smb.IsExists(m_strNewPath))
    {
        qDebug()<<m_strNewPath.c_str()<<" the new dir is exist,"<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_ALREADY_EXIST);
        return;
    }
    if (!smb.CreateRecursive(m_strNewPath))
    {
        qDebug()<<m_strNewPath.c_str()<<" the new dir create fail:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_CAN_NOT_CREATE);
        return;
    }
    if (!smb.Open(m_strPath))
    {
        qDebug()<<m_strPath.c_str()<<" can not open:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_CAN_NOT_ENTER);
        return;
    }

    CSambaFile file(m_smbClient.get());
    std::list<CSambaFileInfo> allFile = smb.GetAllFiles();
    for(const auto& dir : allFile)
    {
        if (!dir.bIsFile)
        {
            continue ;
        }
        std::string strFrom = m_strPath+'/'+dir.strName;
        std::string strTo = m_strNewPath+'/'+dir.strName;
        if (!file.Copy(strFrom, strTo))
        {
            finish(ERROR_INDUSTRY_FILE_CAN_NOT_COPY);
            return ;
        }
    }
    finish(NOERROR);
}

void DeleteFolderThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();

    CSambaDirectory smb(m_smbClient.get());
    if (!smb.IsExists(m_strPath))
    {
        qDebug()<<m_strPath.c_str()<<" dir not exist,"<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_NOT_EXIST);
        return;
    }
    if (!smb.DeleteRecursive(m_strPath))
    {
        qDebug()<<m_strPath.c_str()<<" can not delete:"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FOLDER_CAN_NOT_REMOVE);
        return ;
    }
    finish(NOERROR);
}

void ReadFolderThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();

    std::list<std::tuple<std::string,//filename
                        uint64_t,//lastmodifytime
                         bool//isFile
                        >
             > fileList;

    CSambaDirectory smb(m_smbClient.get());
    if (smb.Open(m_strPath))
    {
        std::list<CSambaFileInfo> all = smb.GetAllFiles();
        for (const auto& f : all)
        {
            if (f.bIsDir)
            {
                fileList.push_back(std::make_tuple(f.strName, f.nLastModifyTime, false));
            }
            else if (f.bIsFile)
            {
                fileList.push_back(std::make_tuple(f.strName, f.nLastModifyTime, true));
            }
        }
        smb.Close();
    }

    if (OnFinishedResult != nullptr)
    {
        OnFinishedResult(m_id, fileList);
    }
}

void CGetFileListThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();

    std::list<std::string> lstResult;

    std::queue<std::list<std::string>> allSubDirs;
    allSubDirs.push(std::list<std::string>({m_strPath}));

    int iDeepth = m_iDeepth;

    while (iDeepth>=0 && allSubDirs.size()>0)
    {
        std::list<std::string> subDirs = allSubDirs.front();
        allSubDirs.pop();

        std::list<std::string> tmpDirs;
        for (auto strDir : subDirs)
        {
            CSambaDirectory smb(m_smbClient.get());
            if (smb.Open(strDir))
            {
                std::list<CSambaFileInfo> all = smb.GetAllFiles();
                for (const auto& f : all)
                {
                    if (f.bIsDir)
                    {
                        tmpDirs.push_back(strDir+'/'+f.strName);
                    }
                    else if (f.bIsFile)
                    {
                        lstResult.push_back(strDir+'/'+f.strName);
                    }
                }
                smb.Close();
            }
        }

        --iDeepth;
        if (tmpDirs.size()>0 && iDeepth>=0)
        {
            allSubDirs.push(tmpDirs);
        }
    }

    if (!m_lstFilter.empty())
    {
        for(auto str : m_lstFilter)
        {
            QRegExp reg(QString(str.c_str()), Qt::CaseSensitive, QRegExp::Wildcard);

            auto itr = lstResult.begin();
            while(itr != lstResult.end())
            {
                if (reg.exactMatch(QString(itr->c_str())))
                {
                    ++itr;
                }
                else
                {
                    itr = lstResult.erase(itr);
                }
            }
            if (lstResult.empty())
            {
                break;
            }
        }
    }
    if (OnFinishedResult != nullptr)
    {
        OnFinishedResult(m_id, lstResult);
    }
}

void DeleteFilesThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();
    for(const std::string& str : m_delFiles)
    {
        CSambaFile smb(m_smbClient.get());
        if (!smb.Delete(str))
        {
            qDebug()<<"delete failed:"<<str.c_str()<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        }
    }
    finish(NOERROR);
}

void PathIsExistThreadSmb::run()
{
    SMB_CHECK_AND_CONNECT();
    CSambaFileInfo info;
    bool bIsExist = CSambaIOStream::GetPathInfo(m_smbClient.get(), m_strPath, info);
    if (nullptr != OnFinishedResult)
    {
        OnFinishedResult(m_id, bIsExist,info.bIsFile, bIsExist?info.nSize:0);
    }
}

void CopyFileLocaleToSMBThreadSmb::run()
{
    QFile localFile(QString::fromStdString(m_strLocalFile));
    if (!localFile.exists())
    {
        qDebug() << localFile.fileName() << "file cannot copy.because the file has not exist!"<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_NOT_EXIST);
        return ;
    }
    if (!localFile.open(QFile::ReadOnly))
    {
        qDebug() << localFile.fileName() << " file cannot open,"<<localFile.errorString()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
        return ;
    }

    /**************************************************************************/
    SMB_CHECK_AND_CONNECT();
    if (!m_bIsTruncate)
    {
        if (CSambaIOStream::IsExists(m_smbClient.get(), m_strPath))
        {
            qDebug() << m_strPath.c_str() << "file cannot copy.because the file has already exist!"<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
            finish(ERROR_INDUSTRY_FILE_CAN_NOT_COPY);
            return ;
        }
    }
    CSambaFile smbFile(m_smbClient.get());
    if (!smbFile.OpenTruncate(m_strPath))
    {
        qDebug()<<"open failed:"<<m_strPath.c_str()<<m_smbClient->GetLastErrorMsg().c_str()<<" id:"<<m_id;
        finish(ERROR_INDUSTRY_FILE_CAN_NOT_OPEN);
        return ;
    }

    const int iBufferSize = 1024*1024;
    std::unique_ptr<char[]> uptr(new char[1024*1024]);
    m_smbClient->SetTimeout(2);
    QDeadlineTimer deadTime(m_iTimeoutSeconds*1000+500);
    do
    {
        const int iRead = localFile.read(uptr.get(), iBufferSize);
        if (0 == iRead)
        {
            finish(NOERROR);
            return ;
        }
        else if (iRead < 0)
        {
            qDebug() << localFile.fileName() << " file cannot read,"<<localFile.errorString()<<" id:"<<m_id;
            finish(ERROR_INDUSTRY_FILE_CAN_NOT_WRITE);
            return ;
        }
        QByteArray writeData(uptr.get(), iRead);
        SMBV2_WRITE_FILE_BATCH2(smbFile, writeData);
    }while(!deadTime.hasExpired());

    qDebug()<<"write file timeout,id:"<<m_id;
    finish(ERROR_INDUSTRY_FILE_TIMEOUT);
}

