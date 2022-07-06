#include "CSambaClient.h"

#include <cstring>

#include "CSambaCore.h"

CSambaClient::CSambaClient()
    :m_pSmbCtx(nullptr),
    m_iTimeoutSeconds(2),
    m_bUserIsNull(true),
    m_bPwdIsNull(true)
{
}

CSambaClient::~CSambaClient()
{
    Disconnect();
}

void* CSambaClient::GetSmbHandler()
{
    return m_pSmbCtx;
}

void CSambaClient::SetUser(const char* pszVal)
{
    CSmbSafeGuard guard(this);
    if (nullptr == pszVal)
    {
        m_bUserIsNull = true;
    }
    else
    {
        m_bUserIsNull = false;
        m_strUser = pszVal;
    }
}

void CSambaClient::SetPwd(const char* pszVal)
{
    CSmbSafeGuard guard(this);
    if (nullptr == pszVal)
    {
        m_bPwdIsNull = true;
    }
    else
    {
        m_bPwdIsNull = false;
        m_strPwd = pszVal;
    }
}

void CSambaClient::SetServer(const std::string& strVal)
{
    CSmbSafeGuard guard(this);
    m_strServer = strVal;
}

void CSambaClient::SetShareDir(const std::string& strVal)
{
    CSmbSafeGuard guard(this);
    m_strShareDir = strVal;
}

void CSambaClient::SetTimeout(int iSeconds)
{
    CSmbSafeGuard guard(this);
    if (iSeconds <= 0)
    {
        iSeconds = 2;
    }
    m_iTimeoutSeconds = iSeconds;
    if (m_pSmbCtx)
    {
        CSambaCore::GetInstance().smb2_set_timeout((struct smb2_context*)m_pSmbCtx, iSeconds);
    }
}

std::string CSambaClient::GetLastErrorMsg()
{
    CSmbSafeGuard guard(this);
    if (nullptr != m_pSmbCtx)
    {
        std::string str(CSambaCore::GetInstance().smb2_get_error((struct smb2_context*)m_pSmbCtx));
        if (!m_strErr.empty())
        {
            str += m_strErr;
            m_strErr = "";
        }
        return str;
    }
    else
    {
        return m_strErr.empty()?"the samba init fail":m_strErr;
    }
}

bool CSambaClient::Connect()
{
    CSmbSafeGuard guard(this);
    if (m_pSmbCtx)
    {
        struct smb2_context* pCtx = (struct smb2_context*)m_pSmbCtx;
        if (CSambaCore::GetInstance().smb2_isconnect_share(pCtx) == 0)
        {
            return true;
        }
        CSambaCore::GetInstance().smb2_destroy_context(pCtx);
    }

    struct smb2_context* pCtx = CSambaCore::GetInstance().smb2_init_context();
    m_pSmbCtx = pCtx;

    if (pCtx)
    {
        CSambaCore::GetInstance().smb2_set_security_mode(pCtx, SMB2_NEGOTIATE_SIGNING_ENABLED);
        //CSambaCore::GetInstance().smb2_set_user(pCtx, m_strUser.c_str());
        CSambaCore::GetInstance().smb2_set_timeout(pCtx, m_iTimeoutSeconds);
        CSambaCore::GetInstance().smb2_set_password(pCtx, m_bPwdIsNull?nullptr:m_strPwd.c_str());

        return 0 == CSambaCore::GetInstance().smb2_connect_share(pCtx,
            m_strServer.c_str(),
            m_strShareDir.c_str(),
            m_bUserIsNull?nullptr:m_strUser.c_str());
    }
    else
    {
        m_strErr = "the samba context is not init";
    }
    return false;
}

void CSambaClient::Disconnect()
{
    CSmbSafeGuard guard(this);
    if (m_pSmbCtx)
    {
        struct smb2_context* pCtx = (struct smb2_context*)m_pSmbCtx;
        if (CSambaCore::GetInstance().smb2_isconnect_share(pCtx) == 0)
        {
            CSambaCore::GetInstance().smb2_disconnect_share(pCtx);
        }
        CSambaCore::GetInstance().smb2_destroy_context(pCtx);
        m_pSmbCtx = nullptr;
    }
}

bool CSambaClient::IsConnected()
{
    CSmbSafeGuard guard(this);
    if (m_pSmbCtx)
    {
        struct smb2_context* pctx = (struct smb2_context*)m_pSmbCtx;
        return CSambaCore::GetInstance().smb2_isconnect_share(pctx) == 0;
    }
    return false;
}

void CSambaClient::SetErrorMsg(int iRetCode, const std::string& str)
{
    CSmbSafeGuard guard(this);
    m_strErr = " code=" + std::to_string(iRetCode) + "," + strerror(-iRetCode);
    if (!str.empty())
    {
        m_strErr += "," + str;
    }
}
