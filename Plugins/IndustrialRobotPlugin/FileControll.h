#ifndef FILECONTROLL_H
#define FILECONTROLL_H

#include <QObject>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>
#include <QThreadPool>
#include "IOThread.h"
#include "cmythreadpool.h"
class FileControll : public QObject
{
    Q_OBJECT
public:
    FileControll(const QString &ip, QObject *parent = nullptr);
    ~FileControll();

    virtual void setIpAddress(const QString &ip);
    QString getIpAddress() const{return m_ip;}

    /* file */
    virtual void readFile(const quint64 id, const QString &fileName, quint32 timeout=0);
    virtual void readFolder(const quint64 id, const QString &folderName, quint32 timeout=0);
    virtual void writeFile(const quint64 id, const QString &fileName, const QJsonValue &value, quint32 timeout=0);
    virtual void writeFile(const quint64 id, const QString &fileName, const QString &content, quint32 timeout=0);
    virtual void newFile(const quint64 id, const QString &fileName, const QString &content, quint32 timeout=0);
    virtual void newFile(const quint64 id, const QString &fileName, const QJsonValue &value, quint32 timeout=0);
    virtual void newFolder(const quint64 id, const QString &url, const QString &fileName, quint32 timeout=0);
    virtual void newFolderRecursive(const quint64 id, const QString &strSubDirRelative, quint32 timeout=0);
    virtual void decodeFile(const quint64 id, const QString &fileName, const QString &content, quint32 timeout=0);
    virtual void changeFile(const quint64 id, const QString &fileName, const QString &key, const QJsonValue &value, quint32 timeout=0);
    virtual void deleteFolder(const quint64 id, const QString &url, const QString &folderName, quint32 timeout=0);
    virtual void renameFolder(const quint64 id, const QString &folderName, const QString &newfolderName, quint32 timeout=0);
    virtual void copyFolder(const quint64 id, const QString &url, const QString &folderName, const QString &newfolderName, quint32 timeout=0);
    virtual void pathIsExist(const quint64 id, const QString &path, quint32 timeout=0);
    virtual void copyFileFromLocaleToSmb(const quint64 id, const QString& strLocaleFile, const QString& strSmbFile, bool bIsTruncate, quint32 timeout=0);
    /*
    *功能：获取文件列表
    *参数：id-请求的标识id，
    *     strFolder-需要获取文件的目录
    *     lstFileNameFilter-文件过滤，表示需要被选择的文件
    *     iDeepth-遍历深度
    *     nTimeoutMillseconds-超时时间
    *返回值：无
    **/
    virtual void getFullFileNameList(const quint64 id, const QString& strDir,
                             const QStringList& lstFileNameFilter, int iDeepth=0, quint32 nTimeoutMillseconds=0);
    virtual void DeleteFileName(const quint64 id, const QStringList& delFiles,quint32 nTimeoutMillseconds=0);

protected:
    QString m_ip;
    static CMyThreadPool* m_pool;

    inline void closureHandler(BaseThread *_thread, quint64 id, quint32 timeout);

signals:
    void onFinish_signal(quint64 id, int code, QByteArray array, QJsonValue value);
};

#endif // FILECONTROLL_H
