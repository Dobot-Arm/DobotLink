#include "CSambaIOStream.h"

#include "CSambaCore.h"

CSambaIOStream::CSambaIOStream(CSambaClient* pClient)
    :m_pClient(pClient)
{
}

CSambaIOStream::~CSambaIOStream()
{
}

std::list<CSambaFileInfo> CSambaIOStream::GetAllFiles()
{
    return {};
}

int CSambaIOStream::Read(char* pBuffer, int iSize)
{
    (void)pBuffer;
    (void)iSize;
    return 0;
}

std::string CSambaIOStream::ReadAllText()
{
    return "";
}

int CSambaIOStream::Write(const char* pszBuffer, int iSize)
{
    (void)pszBuffer;
    (void)iSize;
    return 0;
}

bool CSambaIOStream::WriteAllText(const std::string& str)
{
    return false;
}

bool CSambaIOStream::IsExists(const std::string& strPath)
{
    if (!m_pClient) return false;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return false;

    struct smb2_stat_64 st;

    struct smb2_context* pctx = (struct smb2_context*)(m_pClient->GetSmbHandler());
    int iRet = CSambaCore::GetInstance().smb2_stat(pctx, strPath.c_str(), &st);
    if (0 != iRet)
    {
        m_pClient->SetErrorMsg(iRet);
    }
    return 0 == iRet;
}

bool CSambaIOStream::IsExists(CSambaClient* pClient, const std::string& strPath)
{
    if (!pClient) return false;

    CSambaClient::CSmbSafeGuard guard(pClient);
    if (!pClient->GetSmbHandler()) return false;

    struct smb2_stat_64 st;

    struct smb2_context* pctx = (struct smb2_context*)(pClient->GetSmbHandler());
    int iRet = CSambaCore::GetInstance().smb2_stat(pctx, strPath.c_str(), &st);
    if (0 != iRet)
    {
        pClient->SetErrorMsg(iRet);
    }
    return 0 == iRet;
}

bool CSambaIOStream::Rename(const std::string& strOld, const std::string& strNew)
{
    if (!m_pClient) return false;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return false;

    struct smb2_context* pctx = (struct smb2_context*)(m_pClient->GetSmbHandler());
    int iRet = CSambaCore::GetInstance().smb2_rename(pctx, strOld.c_str(), strNew.c_str());
    if (0 != iRet)
    {
        m_pClient->SetErrorMsg(iRet);
    }
    return 0 == iRet;
}

bool CSambaIOStream::Rename(CSambaClient* pClient, const std::string& strOld, const std::string& strNew)
{
    if (!pClient) return false;

    CSambaClient::CSmbSafeGuard guard(pClient);
    if (!pClient->GetSmbHandler()) return false;

    struct smb2_context* pctx = (struct smb2_context*)(pClient->GetSmbHandler());
    int iRet = CSambaCore::GetInstance().smb2_rename(pctx, strOld.c_str(), strNew.c_str());
    if (0 != iRet)
    {
        pClient->SetErrorMsg(iRet);
    }
    return 0 == iRet;
}

bool CSambaIOStream::GetPathInfo(CSambaClient* pClient, const std::string& strPath,CSambaFileInfo& info)
{
    if (!pClient) return false;
    CSambaClient::CSmbSafeGuard guard(pClient);
    if (!pClient->GetSmbHandler()) return false;

    struct smb2_stat_64 st;

    struct smb2_context* pctx = (struct smb2_context*)(pClient->GetSmbHandler());
    int iRet = CSambaCore::GetInstance().smb2_stat(pctx, strPath.c_str(), &st);
    if (0 != iRet)
    {
        pClient->SetErrorMsg(iRet);
    }
    else
    {
        info.nSize = st.smb2_size;
        info.bIsFile = (SMB2_TYPE_FILE == st.smb2_type);
        info.bIsDir = (SMB2_TYPE_DIRECTORY == st.smb2_type);
        info.bIsLink = (SMB2_TYPE_LINK == st.smb2_type);
        info.nCreateTime = st.smb2_btime * 1000 + st.smb2_btime_nsec / 1000000;
        info.nLastAccessTime = st.smb2_atime * 1000 + st.smb2_atime_nsec / 1000000;
        info.nLastModifyTime = st.smb2_mtime * 1000 + st.smb2_mtime_nsec / 1000000;
    }
    return 0 == iRet;
}
