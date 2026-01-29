#ifndef IOTHREAD_H
#define IOTHREAD_H
#include <QRunnable>
#include <QObject>
#include <QJsonObject>
#include <QDir>
#include <QFile>
#include <QDebug>


class BaseThread: public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit BaseThread(quint64 id, QObject *parent = nullptr):
        QObject(parent),
        QRunnable(),
        m_id(id)
    {
        setAutoDelete(false);
    }
    virtual ~BaseThread() {}

    void run() Q_DECL_OVERRIDE{
        emit onBegin_signal();
        exec();
    };

    virtual void exec()=0;
signals:
    void onFinished_signal(quint64 id, int code, QByteArray data, QJsonValue json);
    void onBegin_signal();

protected:
    inline void finish(int code, QByteArray data = QByteArray(), QJsonValue json=QJsonValue()) {
        emit onFinished_signal(m_id, code, data,json);
    }

    quint64 m_id;
};

/* 读文件 */
class ReadThread : public BaseThread
{
public:
    explicit ReadThread(
            const quint64 id,
            const QString &fileName,
            QObject *parent = nullptr);
    ~ReadThread() {}
    void exec();
private:
    QString m_fileName;
};

/* 写文件 */
class WriteThread : public BaseThread
{
public:
    explicit WriteThread(
            const quint64 id,
            const QString &fileName,
            const QJsonValue &value,
            QObject *parent=nullptr);
    explicit WriteThread(
            const quint64 id,
            const QString &fileName,
            const QString &content,
            QObject *parent=nullptr);
    ~WriteThread() {}
    void exec();
private:
    QString m_fileName;
    QByteArray m_bytes;
};

/* 修改文件 */
class ChangeFileThread : public BaseThread
{
public:
    explicit ChangeFileThread(
            const quint64 id,
            const QString &fileName,
            const QString &key,
            const QJsonValue &value,
            QObject *parent=nullptr);
    ~ChangeFileThread() {}
    void exec();
private:
    QString m_fileName;
    QString m_key;
    QJsonValue m_value;
};

/* 新建文件 */
class NewFileThread : public BaseThread
{
public:
    explicit NewFileThread(
            const quint64 id,
            const QString &fileName,
            const QJsonValue &value,
            QObject *parent = nullptr);
    explicit NewFileThread(
            const quint64 id,
            const QString &fileName,
            const QString &content,
            QObject *parent = nullptr);
    ~NewFileThread() {}
    void exec();
private:
    QString m_fileName;
    QByteArray m_bytes;
};

/* base64解码文件 */
class DecodeFileThread : public BaseThread
{
public:
    explicit DecodeFileThread(
            const quint64 id,
            const QString &fileName,
            const QString &content,
            QObject *parent = nullptr);
    ~DecodeFileThread() {}
    void exec();
private:
    QString m_fileName;
    QString m_content;
};

/* 新建文件夹 */
class NewFolderThread : public BaseThread
{
public:
    explicit NewFolderThread(
            const quint64 id,
            const QString &url,
            const QString &folderName,
            const QDir &dir,
            QObject *parent = nullptr);
    ~NewFolderThread() {}
    void exec();
private:
    QString m_url;
    QString m_folderName;
    QDir m_dir;
};

/* 新建目录，可以递归创建 */
class CreateDirThread : public BaseThread
{
public:
    explicit CreateDirThread(
            const quint64 id,
            const QString &strDir,
            QObject *parent = nullptr);
    ~CreateDirThread() {}
    void exec();

private:
    QString m_strDir;
};

/* 重命名文件夹 */
class RenameFolderThread: public BaseThread
{
public:
    explicit RenameFolderThread(
            const quint64 id,
            const QString &folderName,
            const QString &newfolderName,
            QObject *parent = nullptr);
    ~RenameFolderThread() {}
    void exec();
private:
    QString m_folderName;
    QString m_newfolderName;
};

class ReadFolderThread: public BaseThread
{
public:
    explicit ReadFolderThread(
            const quint64 id,
            const QString &folderName,
            bool bIsOnlyFolder,
            QObject *parent = nullptr);
    ~ReadFolderThread() {}
    void exec();
private:
    QString m_folderName;
    bool m_bIsOnlyFolder;
};

/* 复制文件夹 */
class CopyFolderThread : public BaseThread
{
public:
    explicit CopyFolderThread(
            const quint64 id,
            const QDir &dir,
            const QDir &fromDir,
            const QDir &toDir,
            const QString &folderName,
            const QString &newfolderName,
            QObject *parent = nullptr);
    ~CopyFolderThread() {}
    void exec();
private:
    QDir m_dir;
    QDir m_fromDir;
    QDir m_toDir;
    QString m_folderName;
    QString m_newfolderName;
};

/* 删除文件夹 */
class DeleteFolderThread : public BaseThread
{
public:
    explicit DeleteFolderThread(
            const quint64 id,
            const QString &url,
            const QString &folderName,
            const QString &path,
            const QDir &dir,
            QObject *parent = nullptr);
    ~DeleteFolderThread() {}
    void exec();
private:
    QString m_url;
    QString m_folderName;
    QDir m_dir;
    QString m_path;
};


/* 获取某个目录以及子目录下的所有文件 */
class CGetFileListThread : public BaseThread
{
    Q_OBJECT
public:
    explicit CGetFileListThread(QObject *parent = nullptr);
    ~CGetFileListThread() {}

    void SetId(quint64 id);
    void SetDeepth(int iDeepth);
    void SetDir(const QString& strDir);
    void SetFileFilter(const QStringList& lstFilter);

    void exec() override;

private:
    QString m_strDir; //访问的目录
    QStringList m_lstFilter;//过滤项
    int m_iDeepth; //遍历的深度，0-表示遍历当前目录，1-表示遍历到第一级子目录，2-表示遍历到第二级子目录，依次类推
};

/* 删除文件 */
class DeleteFilesThread : public BaseThread
{
public:
    explicit DeleteFilesThread(
            const quint64 id,
            const QString& strUrl,
            const QStringList& delFiles,
            QObject *parent = nullptr);
    ~DeleteFilesThread() {}
    void exec();

private:
    QString m_strUrl;
    QStringList m_delFiles;
};

/* 目录或文件是否存在 */
class PathIsExistThread : public BaseThread
{
    Q_OBJECT
public:
    explicit PathIsExistThread(
            const quint64 id,
            const QString& strUrl,
            QObject *parent = nullptr);
    ~PathIsExistThread() {}
    void exec();

private:
    QString m_url;
};

/* 将本地文件写到Samba */
class CopyFileLocaleToSMBThread : public BaseThread
{
public:
    explicit CopyFileLocaleToSMBThread(
            const quint64 id,
            const QString &strLocalFile,
            const QString &strSmbFile,
            QObject *parent = nullptr);
    ~CopyFileLocaleToSMBThread() {}
    void exec();
    inline void setTruncate(bool bIsTruncate){m_bIsTruncate = bIsTruncate;}
private:
    QString m_strLocalFile;
    QString m_strSmbFile;
    bool m_bIsTruncate;
};
#endif // IOTHREAD_H
