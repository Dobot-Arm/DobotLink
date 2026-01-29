#ifndef IOTHREADSMB_H
#define IOTHREADSMB_H

#include <cstdint>
#include <functional>
#include <vector>
#include <string>
#include <list>
#include <QJsonValue>

#include <memory>
#include "CSambaClient.h"

class BaseThreadSmb
{
public:
    explicit BaseThreadSmb(const std::shared_ptr<CSambaClient>& sptr);
    virtual ~BaseThreadSmb(){};

    inline void SetId(uint64_t id)
    {
        m_id = id;
    }
    inline void SetServer(const std::string& str)
    {
        m_strIp = str;
    }
    inline void SetShareDir(const std::string& str)
    {
        m_strShareDir = str;
    }
    //如果不需要账号密码，则要传nullptr而不是空字符串
     inline void SetUser(const std::string& str)
    {
        m_strUser = str;
    }
    inline void SetPassword(const std::string& str)
    {
        m_strPwd = str;
    }
    inline void SetTimeoutSeconds(int iTimeout)
    {
        m_iTimeoutSeconds = iTimeout;
    }

    inline void SetPath(const std::string& strPath)
    {
        m_strPath = strPath;
    }

    std::function<void(uint64_t id, int code, const std::vector<char>& data, const std::string& strJson)> OnFinished;

    void exec()
    {
        run();
    }
protected:
    virtual void run() = 0;

    inline void finish(int code, const std::vector<char>& data={}, std::string strJson="")
    {
        if (nullptr != OnFinished)
        {
            OnFinished(m_id, code, data,strJson);
        }
    }

    uint64_t m_id;
    std::string m_strIp;
    std::string m_strShareDir;
    std::string m_strUser;
    std::string m_strPwd;
    int m_iTimeoutSeconds;

    std::string m_strPath;

    std::shared_ptr<CSambaClient> m_smbClient;
};

/* 读文件 */
class ReadThreadSmb : public BaseThreadSmb
{
public:
    explicit ReadThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){};
    ~ReadThreadSmb() {}

protected:
    void run();
};

/* 写文件 */
class WriteThreadSmb : public BaseThreadSmb
{
public:
    explicit WriteThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){};
    ~WriteThreadSmb() {}

    inline void SetContent(const std::vector<char>& str)
    {
        m_bytes = str;
    }

protected:
    void run();

private:
    std::vector<char> m_bytes;
};

/* 修改文件 */
class ChangeFileThreadSmb : public BaseThreadSmb
{
public:
    explicit ChangeFileThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){};
    ~ChangeFileThreadSmb() {}

    inline void SetNewContent(const std::string &strkey,const QJsonValue& strNewContent)
    {
        m_strKey = strkey;
        m_strContent = strNewContent;
    }

protected:
    void run();

private:
    std::string m_strKey;
    QJsonValue m_strContent;
};

/* 新建文件 */
class NewFileThreadSmb : public BaseThreadSmb
{
public:
    explicit NewFileThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){};
    ~NewFileThreadSmb() {}

    inline void SetContent(const std::vector<char>& str)
    {
        m_bytes = str;
    }

protected:
    void run();

private:
    std::vector<char> m_bytes;
};

/* base64解码文件 */
class DecodeFileThreadSmb : public BaseThreadSmb
{
public:
    explicit DecodeFileThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){};
    ~DecodeFileThreadSmb() {}

    inline void SetContent(const std::string& str)
    {
        m_strContent = str;
    }

protected:
    void run();

private:
    std::string m_strContent;
};

/* 新建文件夹 */
class NewFolderThreadSmb : public BaseThreadSmb
{
public:
    explicit NewFolderThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){};
    ~NewFolderThreadSmb() {}

protected:
    void run();
};

/* 新建目录，可以递归创建 */
class CreateDirThreadSmb : public BaseThreadSmb
{
public:
    explicit CreateDirThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){};
    ~CreateDirThreadSmb() {}

protected:
    void run();
};

/* 重命名文件夹 */
class RenameFolderThreadSmb: public BaseThreadSmb
{
public:
    explicit RenameFolderThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){};
    ~RenameFolderThreadSmb() {}

    inline void SetNewNamePath(const std::string& strNew)
    {
        m_strNewNamePath = strNew;
    }

protected:
    void run();

private:
    std::string m_strNewNamePath;
};

/* 复制文件夹 */
class CopyFolderThreadSmb : public BaseThreadSmb
{
public:
    explicit CopyFolderThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){};
    ~CopyFolderThreadSmb() {}

    inline void SetNewPath(const std::string& strNew)
    {
        m_strNewPath = strNew;
    }

protected:
    void run();

private:
    std::string m_strNewPath;
};

/* 删除文件夹 */
class DeleteFolderThreadSmb : public BaseThreadSmb
{
public:
    explicit DeleteFolderThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){};
    ~DeleteFolderThreadSmb() {}

protected:
    void run();
};

class ReadFolderThreadSmb : public BaseThreadSmb
{
public:
    explicit ReadFolderThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){OnFinishedResult=nullptr;};
    ~ReadFolderThreadSmb() {}

    std::function<void(uint64_t id, const std::list<
                            std::tuple<std::string,//filename
                                       uint64_t,//lastmodifytime
                                        bool//isFile
                                        >
                       >& fileList)> OnFinishedResult;

protected:
    void run();
};

/* 获取某个目录以及子目录下的所有文件 */
class CGetFileListThreadSmb : public BaseThreadSmb
{
public:
    explicit CGetFileListThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){OnFinishedResult=nullptr;};
    ~CGetFileListThreadSmb() {}

    inline void SetDeepth(int iDeepth)
    {
        m_iDeepth = iDeepth>=0 ? iDeepth : 0;
    }
    inline void SetFileFilter(const std::vector<std::string>& lstFilter)
    {
        m_lstFilter = lstFilter;
    }

    std::function<void(uint64_t id, const std::list<std::string>& fileList)> OnFinishedResult;

protected:
    void run();

private:
    std::vector<std::string> m_lstFilter;//过滤项
    int m_iDeepth; //遍历的深度，0-表示遍历当前目录，1-表示遍历到第一级子目录，2-表示遍历到第二级子目录，依次类推
};

/* 删除文件 */
class DeleteFilesThreadSmb : public BaseThreadSmb
{
public:
    explicit DeleteFilesThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){};
    ~DeleteFilesThreadSmb() {}

    inline void SetDeleteFiles(const std::vector<std::string>& delFiles)
    {
        m_delFiles = delFiles;
    }

protected:
    void run();

private:
    std::vector<std::string> m_delFiles;
};

/* 目录或文件是否存在 */
class PathIsExistThreadSmb : public BaseThreadSmb
{
public:
    explicit PathIsExistThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){OnFinishedResult=nullptr;};
    ~PathIsExistThreadSmb() {}

    std::function<void(uint64_t id, bool isExist,bool isFile, int64_t nFileSize)> OnFinishedResult;

protected:
    void run();
};

/* 将本地文件写到Samba */
class CopyFileLocaleToSMBThreadSmb : public BaseThreadSmb
{
public:
    explicit CopyFileLocaleToSMBThreadSmb(const std::shared_ptr<CSambaClient>& sptr):BaseThreadSmb(sptr){};
    ~CopyFileLocaleToSMBThreadSmb() {}

    inline void SetLocalFile(const std::string& strLocalPath)
    {
        m_strLocalFile = strLocalPath;
    }

    inline void SetTruncate(bool bIsTruncate)
    {
        m_bIsTruncate = bIsTruncate;
    }

protected:
    void run();
private:
    std::string m_strLocalFile;
    bool m_bIsTruncate;
};

#endif // IOTHREAD_H
