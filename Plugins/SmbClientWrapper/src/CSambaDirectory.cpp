#include "CSambaDirectory.h"

#include <cstring>
#include <stack>

#include "CSambaCore.h"

CSambaDirectory::CSambaDirectory(CSambaClient* pClient)
    :CSambaIOStream(pClient),
    m_pSmbCtx(nullptr)
{
}

CSambaDirectory::~CSambaDirectory()
{
    Close();
}

bool CSambaDirectory::Open(const std::string& strPath)
{
    if (!m_pClient) return false;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return false;

    if (!m_pSmbCtx)
    {
        struct smb2dir* pctx = CSambaCore::GetInstance().smb2_opendir((struct smb2_context*)(m_pClient->GetSmbHandler()),
            strPath.c_str());
        m_pSmbCtx = pctx;
        return (nullptr != pctx);
    }
    return false;
}

void CSambaDirectory::Close()
{
    if (!m_pClient) return ;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return ;

    if (m_pSmbCtx)
    {
        CSambaCore::GetInstance().smb2_closedir((struct smb2_context*)(m_pClient->GetSmbHandler()), (struct smb2dir*)m_pSmbCtx);
        m_pSmbCtx = nullptr;
    }
}

bool CSambaDirectory::Create(const std::string& strPath)
{
    if (!m_pClient) return false;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return false;

    struct smb2_context* pctx = (struct smb2_context*)(m_pClient->GetSmbHandler());
    int iRet = CSambaCore::GetInstance().smb2_mkdir(pctx, strPath.c_str());
    if (0 != iRet)
    {
        m_pClient->SetErrorMsg(iRet);
    }
    return 0 == iRet;
}

bool CSambaDirectory::Delete(const std::string& strPath)
{
    if (!m_pClient) return false;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return false;

    struct smb2_context* pctx = (struct smb2_context*)(m_pClient->GetSmbHandler());
    int iRet = CSambaCore::GetInstance().smb2_rmdir(pctx, strPath.c_str());
    if (0 != iRet)
    {
        m_pClient->SetErrorMsg(iRet);
    }
    return 0 == iRet;
}

std::list<CSambaFileInfo> CSambaDirectory::GetAllFiles()
{
    std::list<CSambaFileInfo> allFiles;

    if (!m_pSmbCtx)
    {
        return allFiles;
    }

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    struct smb2_context* pctx = (struct smb2_context*)(m_pClient->GetSmbHandler());
    struct smb2dir* pdir = (struct smb2dir*)m_pSmbCtx;
    struct smb2dirent* dirent = CSambaCore::GetInstance().smb2_readdir(pctx, pdir);
    while (NULL != dirent)
    {
        if (strcmp(".", dirent->name) != 0 &&
            strcmp("..", dirent->name) != 0)
        {
            CSambaFileInfo info;
            info.strName = dirent->name;
            info.nSize = dirent->st.smb2_size;
            info.bIsFile = (SMB2_TYPE_FILE == dirent->st.smb2_type);
            info.bIsDir = (SMB2_TYPE_DIRECTORY == dirent->st.smb2_type);
            info.bIsLink = (SMB2_TYPE_LINK == dirent->st.smb2_type);
            info.nCreateTime = dirent->st.smb2_btime * 1000 + dirent->st.smb2_btime_nsec / 1000000;
            info.nLastAccessTime = dirent->st.smb2_atime * 1000 + dirent->st.smb2_atime_nsec / 1000000;
            info.nLastModifyTime = dirent->st.smb2_mtime * 1000 + dirent->st.smb2_mtime_nsec / 1000000;
            allFiles.push_back(info);
        }

        dirent = CSambaCore::GetInstance().smb2_readdir(pctx, pdir);
    }
    return allFiles;
}

bool CSambaDirectory::CreateRecursive(const std::string& strPath)
{
    if (!m_pClient) return false;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return false;

    if (IsExists(strPath))
    {
        return true;
    }

    std::string strPathDir;
    int iPos =  0;
    do
    {
        iPos = strPath.find('/',iPos+1);
        if (iPos < 0)
        {
            strPathDir = strPath;
        }
        else
        {
            strPathDir = strPath.substr(0, iPos);
        }
        if (!IsExists(strPathDir))
        {
            if (!Create(strPathDir))
            {
                return false;
            }
        }
    }while (iPos>=0);
    return true;
}

bool CSambaDirectory::DeleteRecursive(const std::string& strPath)
{
    if (!m_pClient) return false;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return false;

    struct smb2_context* pctx = (struct smb2_context*)(m_pClient->GetSmbHandler());

    std::stack<std::string> allDirs;
    allDirs.push(strPath);
    while (allDirs.size()>0)
    {
        std::string strTmp = allDirs.top();

        struct smb2dir* pdir = CSambaCore::GetInstance().smb2_opendir(pctx,strTmp.c_str());
        if (nullptr == pdir)
        {
            return false;
        }

        int iSubDirCount = 0;
        struct smb2dirent* dirent = CSambaCore::GetInstance().smb2_readdir(pctx, pdir);
        while (NULL != dirent)
        {
            if (strcmp(".", dirent->name) != 0 &&
                strcmp("..", dirent->name) != 0)
            {
                std::string strDir = strTmp+'/'+dirent->name;
                if (SMB2_TYPE_DIRECTORY == dirent->st.smb2_type)
                {
                    ++iSubDirCount;
                    allDirs.push(strDir);
                }
                else
                {
                    if (0 != CSambaCore::GetInstance().smb2_unlink(pctx, strDir.c_str()))
                    {
                        CSambaCore::GetInstance().smb2_closedir(pctx, pdir);
                        return false;
                    }
                }
            }
            dirent = CSambaCore::GetInstance().smb2_readdir(pctx, pdir);
        }
        CSambaCore::GetInstance().smb2_closedir(pctx, pdir);

        if (0 == iSubDirCount)
        {
            allDirs.pop();
            if ( 0!= CSambaCore::GetInstance().smb2_rmdir(pctx, strPath.c_str()))
            {
                return false;
            }
        }
    }
    return true;
}
