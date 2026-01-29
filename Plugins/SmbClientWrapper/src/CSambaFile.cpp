#include "CSambaFile.h"

#include <fcntl.h>
#include <memory>
#include <chrono>
#include <thread>
#include <memory>

#include "CSambaCore.h"

CSambaFile::CSambaFile(CSambaClient* pClient)
    :CSambaIOStream(pClient),
    m_pSmbCtx(nullptr)
{
}

CSambaFile::~CSambaFile()
{
    Close();
}

bool CSambaFile::Open(const std::string& strPath)
{
    if (!m_pSmbCtx)
    {
        return Open(strPath, O_RDONLY);
    }
    else
    {
        if (m_pClient && m_pClient->GetSmbHandler())
        {
            CSambaClient::CSmbSafeGuard guard(m_pClient);
            CSambaCore::GetInstance().smb2_set_error((struct smb2_context*)(m_pClient->GetSmbHandler()), "the file maybe has opened!");
        }
    }
    return false;
}

const CSambaFileInfo& CSambaFile::GetFileInfo()
{
    return m_fInfo;
}

void CSambaFile::Close()
{
    if (!m_pClient) return;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return;

    if (m_pSmbCtx)
    {
        CSambaCore::GetInstance().smb2_close((struct smb2_context*)(m_pClient->GetSmbHandler()), (struct smb2fh*)m_pSmbCtx);
        m_pSmbCtx = nullptr;
    }
}

bool CSambaFile::OpenTruncate(const std::string& strPath)
{//smb2_truncate  smb2_ftruncate, nice
    if (!m_pSmbCtx)
    {
        return Open(strPath, O_WRONLY | O_CREAT | O_TRUNC);
    }
    else
    {
        if (m_pClient && m_pClient->GetSmbHandler())
        {
            CSambaClient::CSmbSafeGuard guard(m_pClient);
            CSambaCore::GetInstance().smb2_set_error((struct smb2_context*)(m_pClient->GetSmbHandler()), "the file maybe has opened!");
        }
    }
    return false;
}

bool CSambaFile::Copy(const std::string& strOldPath, const std::string& strNewPath)
{
    if (!IsExists(strOldPath) || IsExists(strNewPath))
    {
        return false;
    }
    CSambaFile fr(m_pClient);
    CSambaFile fw(m_pClient);

    if (!fr.Open(strOldPath) || !fw.OpenTruncate(strNewPath))
    {
        return false;
    }

    const int iLimitedValue=3e6;
    const int nBufferMaxSize = fr.m_fInfo.nSize>iLimitedValue?iLimitedValue:fr.m_fInfo.nSize;
    std::unique_ptr<char[]> szBuffer(new char[nBufferMaxSize]);

    int iTotalRead = fr.Read(szBuffer.get(), nBufferMaxSize);
    while (iTotalRead > 0)
    {
        int iTotalWrite = 0;
        while (iTotalWrite < iTotalRead)
        {
            int iw = fw.Write(szBuffer.get()+iTotalWrite, iTotalRead-iTotalWrite);
            if (iw > 0)
            {
                iTotalWrite += iw;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            else
            {
                return false;
            }
        }
        iTotalRead = fr.Read(szBuffer.get(), nBufferMaxSize);
    }

    return true;
}

bool CSambaFile::Open(const std::string& strPath, int iOpenMode)
{
    if (!m_pClient) return false;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return false;

    struct smb2fh* pctx = CSambaCore::GetInstance().smb2_open((struct smb2_context*)(m_pClient->GetSmbHandler()),
        strPath.c_str(), iOpenMode);

    m_pSmbCtx = pctx;

    m_fInfo.Clear();

    if (pctx)
    {
        struct smb2_stat_64 st;
        if (0 == CSambaCore::GetInstance().smb2_fstat((struct smb2_context*)(m_pClient->GetSmbHandler()), pctx, &st))
        {
            size_t npos = strPath.find_last_of("/\\");
            if (npos != std::string::npos)
            {
                m_fInfo.strName = strPath.substr(npos + 1);
            }
            else
            {
                m_fInfo.strName = strPath;
            }
            m_fInfo.nSize = st.smb2_size;
            m_fInfo.bIsFile = (SMB2_TYPE_FILE == st.smb2_type);
            m_fInfo.bIsDir = (SMB2_TYPE_DIRECTORY == st.smb2_type);
            m_fInfo.bIsLink = (SMB2_TYPE_LINK == st.smb2_type);
            m_fInfo.nCreateTime = st.smb2_btime * 1000 + st.smb2_btime_nsec / 1000000;
            m_fInfo.nLastAccessTime = st.smb2_atime * 1000 + st.smb2_atime_nsec / 1000000;
            m_fInfo.nLastModifyTime = st.smb2_mtime * 1000 + st.smb2_mtime_nsec / 1000000;
        }
    }
    return (nullptr != pctx);
}

uint64_t CSambaFile::SetFilePointer(uint64_t nOffset, int iWhence)
{
    if (!m_pClient) return -1;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return -1;

    struct smb2_context* pctx = (struct smb2_context*)(m_pClient->GetSmbHandler());
    struct smb2fh* pfh = (struct smb2fh*)m_pSmbCtx;

    uint64_t iCurrentOffset = 0;
    if (CSambaCore::GetInstance().smb2_lseek(pctx, pfh, nOffset, iWhence, &iCurrentOffset) < 0)
    {
        return -1;
    }
    return iCurrentOffset;
}

bool CSambaFile::Create(const std::string& strPath)
{
    if (m_pSmbCtx)
    {
        if (m_pClient && m_pClient->GetSmbHandler())
        {

            CSambaClient::CSmbSafeGuard guard(m_pClient);
            CSambaCore::GetInstance().smb2_set_error((struct smb2_context*)(m_pClient->GetSmbHandler()),
                "create fail,the CSambaFile maybe open a another file");
        }
        return false;
    }
    return Open(strPath, O_WRONLY | O_EXCL | O_CREAT);
}

bool CSambaFile::Delete(const std::string& strPath)
{
    if (!m_pClient) return false;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return false;

    struct smb2_context* pctx = (struct smb2_context*)(m_pClient->GetSmbHandler());
    int iRet = CSambaCore::GetInstance().smb2_unlink(pctx, strPath.c_str());
    if (0 != iRet)
    {
        m_pClient->SetErrorMsg(iRet);
    }
    return 0 == iRet;
}

int CSambaFile::Read(char* pBuffer, int iSize)
{
    if (!m_pClient || !m_pSmbCtx) return -1;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return -1;

    struct smb2_context* pctx = (struct smb2_context*)(m_pClient->GetSmbHandler());
    struct smb2fh* pfh = (struct smb2fh*)m_pSmbCtx;

    return CSambaCore::GetInstance().smb2_read(pctx, pfh, (uint8_t*)pBuffer, iSize);//
}

std::string CSambaFile::ReadAllText()
{
    if (!m_pClient || !m_pSmbCtx || 0==m_fInfo.nSize) return "";

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return "";

    struct smb2_context* pctx = (struct smb2_context*)(m_pClient->GetSmbHandler());
    struct smb2fh* pfh = (struct smb2fh*)m_pSmbCtx;
    
    uint64_t iCurrentOffset = 0;
    if (CSambaCore::GetInstance().smb2_lseek(pctx, pfh, 0, SEEK_SET, &iCurrentOffset) < 0)
    {
        return "";
    }

    const int iLimitedValue=3e6;
    const int nBufferMaxSize = m_fInfo.nSize>iLimitedValue?iLimitedValue:m_fInfo.nSize;
    std::string strRet;
    strRet.reserve(m_fInfo.nSize+10);

    std::unique_ptr<char[]> szBuffer(new char[nBufferMaxSize]);

    uint64_t iTotalRead = 0;
    int iRet = CSambaCore::GetInstance().smb2_read(pctx, pfh, (uint8_t*)szBuffer.get(), nBufferMaxSize);
    while (iRet > 0 && iTotalRead < m_fInfo.nSize)
    {
        iTotalRead += iRet;
        strRet.append(szBuffer.get(), iRet);
        iRet = CSambaCore::GetInstance().smb2_read(pctx, pfh, (uint8_t*)szBuffer.get(), nBufferMaxSize);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return strRet;
}

int CSambaFile::Write(const char* pszBuffer, int iSize)
{
    if (nullptr == pszBuffer || iSize <= 0)
    {
        return 0;
    }
    if (!m_pClient || !m_pSmbCtx) return -1;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return -1;

    struct smb2_context* pctx = (struct smb2_context*)(m_pClient->GetSmbHandler());
    struct smb2fh* pfh = (struct smb2fh*)m_pSmbCtx;

    return CSambaCore::GetInstance().smb2_write(pctx, pfh, (const uint8_t*)pszBuffer, iSize);
}

bool CSambaFile::WriteAllText(const std::string& str)
{
    if (str.empty()) return 0;
    if (!m_pClient || !m_pSmbCtx) return -1;

    CSambaClient::CSmbSafeGuard guard(m_pClient);
    if (!m_pClient->GetSmbHandler()) return -1;

    struct smb2_context* pctx = (struct smb2_context*)(m_pClient->GetSmbHandler());
    struct smb2fh* pfh = (struct smb2fh*)m_pSmbCtx;

    const char* psz = str.c_str();
    int iSize = str.size();
    do
    {
        int iRet = CSambaCore::GetInstance().smb2_write(pctx, pfh, (const uint8_t*)psz, iSize);
        if (iRet <= 0)
        {
            return false;
        }
        psz += iRet;
        iSize -= iRet;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }while (iSize>0);
    return true;
}
