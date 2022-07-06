#ifndef FILECONTROLLSMB_H
#define FILECONTROLLSMB_H

#include <QObject>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>
#include <QThreadPool>

#include "FileControll.h"
#include "IOThreadSmb.h"

class FileControllSmb : public FileControll
{
    Q_OBJECT
public:
    FileControllSmb(const QString &ip, QObject *parent = nullptr);
    ~FileControllSmb();

    /* file */
    void readFile(const quint64 id, const QString &fileName, quint32 timeout=0) Q_DECL_OVERRIDE;
    void readFolder(const quint64 id, const QString &folderName) Q_DECL_OVERRIDE;
    void writeFile(const quint64 id, const QString &fileName, const QJsonValue &value, quint32 timeout=0) Q_DECL_OVERRIDE;
    void writeFile(const quint64 id, const QString &fileName, const QString &content, quint32 timeout=0) Q_DECL_OVERRIDE;
    void newFile(const quint64 id, const QString &fileName, const QString &content, quint32 timeout=0) Q_DECL_OVERRIDE;
    void newFile(const quint64 id, const QString &fileName, const QJsonValue &value, quint32 timeout=0) Q_DECL_OVERRIDE;
    void newFolder(const quint64 id, const QString &url, const QString &fileName, quint32 timeout=0) Q_DECL_OVERRIDE;
    void newFolderRecursive(const quint64 id, const QString &strSubDirRelative, quint32 timeout=0) Q_DECL_OVERRIDE;
    void decodeFile(const quint64 id, const QString &fileName, const QString &content, quint32 timeout=0) Q_DECL_OVERRIDE;
    void changeFile(const quint64 id, const QString &fileName, const QString &key, const QJsonValue &value, quint32 timeout=0) Q_DECL_OVERRIDE;
    void deleteFolder(const quint64 id, const QString &url, const QString &folderName, quint32 timeout=0) Q_DECL_OVERRIDE;
    void renameFolder(const quint64 id, const QString &folderName, const QString &newfolderName, quint32 timeout=0) Q_DECL_OVERRIDE;
    void copyFolder(const quint64 id, const QString &url, const QString &folderName, const QString &newfolderName, quint32 timeout=0) Q_DECL_OVERRIDE;
    void pathIsExist(const quint64 id, const QString &path, quint32 timeout=0) Q_DECL_OVERRIDE;
    void copyFileFromLocaleToSmb(const quint64 id, const QString& strLocaleFile, const QString& strSmbFile, bool bIsTruncate, quint32 timeout=0) Q_DECL_OVERRIDE;

    /*
    *功能：获取文件列表
    *参数：id-请求的标识id，
    *     strFolder-需要获取文件的目录
    *     lstFileNameFilter-文件过滤，表示需要被选择的文件
    *     iDeepth-遍历深度
    *     nTimeoutMillseconds-超时时间
    *返回值：无
    **/
    void getFullFileNameList(const quint64 id, const QString& strDir,
                             const QStringList& lstFileNameFilter, int iDeepth=0, quint32 nTimeoutMillseconds=0) Q_DECL_OVERRIDE;
    void DeleteFileName(const quint64 id, const QStringList& delFiles,quint32 nTimeoutMillseconds=0) Q_DECL_OVERRIDE;

private:
    std::string m_strShareDir;
    std::string m_strUser;
    std::string m_strPwd;
    std::shared_ptr<CSambaClient> m_smbClient;
};

#endif // FILECONTROLL_H
