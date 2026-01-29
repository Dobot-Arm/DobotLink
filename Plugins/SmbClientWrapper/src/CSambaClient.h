#pragma once

#include <string>
#include <mutex>

class CSambaClient
{
public:
    class CSmbSafeGuard
    {
        CSambaClient* m_p;
    public:
        CSmbSafeGuard(CSambaClient* p):m_p(p)
        {
            m_p->m_mtx.lock();
        }
        ~CSmbSafeGuard()
        {
            m_p->m_mtx.unlock();
        }
    };
    friend class CSafeGuard;

public:
    CSambaClient();
    ~CSambaClient();
    CSambaClient(const CSambaClient&) = delete;
    CSambaClient(CSambaClient&&) = delete;
    CSambaClient& operator=(const CSambaClient&) = delete;
    CSambaClient& operator=(CSambaClient&&) = delete;

    void* GetSmbHandler();

    void SetUser(const char* pszVal);
    void SetPwd(const char* pszVal);
    void SetServer(const std::string& strVal);
    void SetShareDir(const std::string& strVal);
    void SetTimeout(int iSeconds);

    std::string GetLastErrorMsg();
    void SetErrorMsg(int iRetCode, const std::string& str="");

    bool Connect();
    void Disconnect();
    bool IsConnected();

    int GetMaxReadSize();
    int GetMaxWriteSize();

private:
    std::recursive_mutex m_mtx;
    void* m_pSmbCtx; //struct smb2_context*

    bool m_bUserIsNull;
    bool m_bPwdIsNull;
    std::string m_strUser;
    std::string m_strPwd;
    std::string m_strServer;
    std::string m_strShareDir;
    int m_iTimeoutSeconds;

    std::string m_strErr;
};

