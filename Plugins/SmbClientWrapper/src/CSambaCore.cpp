#include "CSambaCore.h"

#include <cstring>
#include <cstdarg>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include <QString>
#include <QDebug>

CSambaCore::CSambaCore()
{
#ifdef Q_OS_WIN
    WSAData wsd;
    WSAStartup(MAKEWORD(2, 2), &wsd);
#endif
    ResetFuncPtr();
}

CSambaCore::~CSambaCore()
{
    UnloadDll();
#ifdef Q_OS_WIN
    WSACleanup();
#endif
}

CSambaCore& CSambaCore::GetInstance()
{
    static CSambaCore obj;
    return obj;
}

bool CSambaCore::IsLoaded()
{
    std::lock_guard<std::mutex> guard(m_mtx);
    return m_lib.isLoaded();
}

bool CSambaCore::LoadDll(const QString& strDll)
{
    std::lock_guard<std::mutex> guard(m_mtx);
    if (m_lib.isLoaded())
    {
        return true;
    }
    m_lib.setFileName(strDll);
    if (m_lib.load())
    {
        pfn_init_context = (pfn_smb2_init_context)m_lib.resolve("smb2_init_context");
        QString s = m_lib.errorString();
        pfn_destroy_context = (pfn_smb2_destroy_context)m_lib.resolve("smb2_destroy_context");
        pfn_set_timeout = (pfn_smb2_set_timeout)m_lib.resolve("smb2_set_timeout");
        pfn_get_error = (pfn_smb2_get_error)m_lib.resolve("smb2_get_error");
        pfn_set_error = (pfn_smb2_set_error)m_lib.resolve("smb2_set_error");
        pfn_set_security_mode = (pfn_smb2_set_security_mode)m_lib.resolve("smb2_set_security_mode");
        pfn_set_user = (pfn_smb2_set_user)m_lib.resolve("smb2_set_user");
        pfn_set_password = (pfn_smb2_set_password)m_lib.resolve("smb2_set_password");
        pfn_set_domain = (pfn_smb2_set_domain)m_lib.resolve("smb2_set_domain");
        pfn_connect_share = (pfn_smb2_connect_share)m_lib.resolve("smb2_connect_share");
        pfn_disconnect_share = (pfn_smb2_disconnect_share)m_lib.resolve("smb2_disconnect_share");
        pfn_isconnect_share = (pfn_smb2_isconnect_share)m_lib.resolve("smb2_isconnect_share");
        pfn_opendir = (pfn_smb2_opendir)m_lib.resolve("smb2_opendir");
        pfn_closedir = (pfn_smb2_closedir)m_lib.resolve("smb2_closedir");
        pfn_readdir = (pfn_smb2_readdir)m_lib.resolve("smb2_readdir");
        pfn_mkdir = (pfn_smb2_mkdir)m_lib.resolve("smb2_mkdir");
        pfn_rmdir = (pfn_smb2_rmdir)m_lib.resolve("smb2_rmdir");
        pfn_open = (pfn_smb2_open)m_lib.resolve("smb2_open");
        pfn_close = (pfn_smb2_close)m_lib.resolve("smb2_close");
        pfn_pread = (pfn_smb2_pread)m_lib.resolve("smb2_pread");
        pfn_pwrite = (pfn_smb2_pwrite)m_lib.resolve("smb2_pwrite");
        pfn_read = (pfn_smb2_read)m_lib.resolve("smb2_read");
        pfn_write = (pfn_smb2_write)m_lib.resolve("smb2_write");
        pfn_lseek = (pfn_smb2_lseek)m_lib.resolve("smb2_lseek");
        pfn_unlink = (pfn_smb2_unlink)m_lib.resolve("smb2_unlink");
        pfn_truncate = (pfn_smb2_truncate)m_lib.resolve("smb2_truncate");
        pfn_ftruncate = (pfn_smb2_ftruncate)m_lib.resolve("smb2_ftruncate");
        pfn_fstat = (pfn_smb2_fstat)m_lib.resolve("smb2_fstat");
        pfn_stat = (pfn_smb2_stat)m_lib.resolve("smb2_stat");
        pfn_rename = (pfn_smb2_rename)m_lib.resolve("smb2_rename");
        pfn_get_max_read_size = (pfn_smb2_get_max_read_size)m_lib.resolve("smb2_get_max_read_size");
        pfn_get_max_write_size = (pfn_smb2_get_max_write_size)m_lib.resolve("smb2_get_max_write_size");
        return true;
    }
    else
    {
        qDebug()<<m_lib.errorString();
    }
    return false;
}

void CSambaCore::UnloadDll()
{
    std::lock_guard<std::mutex> guard(m_mtx);
    if  (m_lib.isLoaded())
    {
        m_lib.unload();
    }
    ResetFuncPtr();
}

void CSambaCore::ResetFuncPtr()
{
    pfn_init_context = NULL;
    pfn_destroy_context = NULL;
    pfn_set_timeout = NULL;
    pfn_get_error = NULL;
    pfn_set_error = NULL;
    pfn_set_security_mode = NULL;
    pfn_set_user = NULL;
    pfn_set_password = NULL;
    pfn_set_domain = NULL;
    pfn_connect_share = NULL;
    pfn_disconnect_share = NULL;
    pfn_isconnect_share = NULL;
    pfn_opendir = NULL;
    pfn_closedir = NULL;
    pfn_readdir = NULL;
    pfn_mkdir = NULL;
    pfn_rmdir = NULL;
    pfn_open = NULL;
    pfn_close = NULL;
    pfn_pread = NULL;
    pfn_pwrite = NULL;
    pfn_read = NULL;
    pfn_write = NULL;
    pfn_lseek = NULL;
    pfn_unlink = NULL;
    pfn_truncate = NULL;
    pfn_ftruncate = NULL;
    pfn_fstat = NULL;
    pfn_stat = NULL;
    pfn_rename = NULL;
    pfn_get_max_write_size = NULL;
    pfn_get_max_read_size = NULL;
}

struct smb2_context * CSambaCore::smb2_init_context()
{
    if (pfn_init_context) return pfn_init_context();
    return nullptr;
}

void CSambaCore::smb2_destroy_context(struct smb2_context *smb2)
{
    if (pfn_init_context) pfn_destroy_context(smb2);
}

void CSambaCore::smb2_set_timeout(struct smb2_context *smb2, int seconds)
{
    if (pfn_set_timeout) pfn_set_timeout(smb2,seconds);
}

const char *CSambaCore::smb2_get_error(struct smb2_context *smb2)
{
    if (pfn_get_error) return pfn_get_error(smb2);
    return "";
}

void CSambaCore::smb2_set_error(struct smb2_context *smb2, const char *error_string, ...)
{
    if (pfn_set_error)
    {
        char szBuf[256] = "";
        va_list arg;
        va_start(arg, error_string);
        vsnprintf(szBuf, 255, error_string, arg);
        va_end(arg);
        pfn_set_error(smb2, "%s", szBuf);
    }
}

void CSambaCore::smb2_set_security_mode(struct smb2_context *smb2, uint16_t security_mode)
{
    if (pfn_set_security_mode) pfn_set_security_mode(smb2, security_mode);
}

void CSambaCore::smb2_set_user(struct smb2_context *smb2, const char *user)
{
    if (pfn_set_user) pfn_set_user(smb2, user);
}

void CSambaCore::smb2_set_password(struct smb2_context *smb2, const char *password)
{
    if (pfn_set_password) pfn_set_password(smb2, password);
}

void CSambaCore::smb2_set_domain(struct smb2_context *smb2, const char *domain)
{
    if (pfn_set_domain) pfn_set_domain(smb2, domain);
}

int CSambaCore::smb2_connect_share(struct smb2_context *smb2,
                       const char *server,
                       const char *share,
                       const char *user)
{
    if (pfn_connect_share) return pfn_connect_share(smb2, server,share,user);
    return -1;
}

int CSambaCore::smb2_disconnect_share(struct smb2_context *smb2)
{
    if (pfn_disconnect_share) return pfn_disconnect_share(smb2);
    return -1;
}

int CSambaCore::smb2_isconnect_share(struct smb2_context *smb2)
{
    if (pfn_isconnect_share) return pfn_isconnect_share(smb2);
    return -1;
}

struct smb2dir *CSambaCore::smb2_opendir(struct smb2_context *smb2, const char *path)
{
    if (pfn_opendir) return pfn_opendir(smb2, path);
    return nullptr;
}

void CSambaCore::smb2_closedir(struct smb2_context *smb2, struct smb2dir *smb2dir)
{
    if (pfn_closedir) pfn_closedir(smb2,smb2dir);
}

struct smb2dirent *CSambaCore::smb2_readdir(struct smb2_context *smb2,
                                struct smb2dir *smb2dir)
{
    if (pfn_readdir) return pfn_readdir(smb2,smb2dir);
    return nullptr;
}

int CSambaCore::smb2_mkdir(struct smb2_context *smb2, const char *path)
{
    if (pfn_mkdir) return pfn_mkdir(smb2,path);
    return -1;
}

int CSambaCore::smb2_rmdir(struct smb2_context *smb2, const char *path)
{
    if (pfn_rmdir) return pfn_rmdir(smb2,path);
    return -1;
}

struct smb2fh *CSambaCore::smb2_open(struct smb2_context *smb2, const char *path, int flags)
{
    if (pfn_open) return pfn_open(smb2,path,flags);
    return nullptr;
}

int CSambaCore::smb2_close(struct smb2_context *smb2, struct smb2fh *fh)
{
    if (pfn_close) return pfn_close(smb2,fh);
    return -1;
}

int CSambaCore::smb2_pread(struct smb2_context *smb2, struct smb2fh *fh,
               uint8_t *buf, uint32_t count, uint64_t offset)
{
    if (pfn_pread) return pfn_pread(smb2,fh,buf,count,offset);
    return -1;
}

int CSambaCore::smb2_pwrite(struct smb2_context *smb2, struct smb2fh *fh,
                const uint8_t *buf, uint32_t count, uint64_t offset)
{
    if (pfn_pwrite) return pfn_pwrite(smb2,fh,buf,count,offset);
    return -1;
}

int CSambaCore::smb2_read(struct smb2_context *smb2, struct smb2fh *fh,
              uint8_t *buf, uint32_t count)
{
    if (pfn_read) return pfn_read(smb2,fh,buf,count);
    return -1;
}

int CSambaCore::smb2_write(struct smb2_context *smb2, struct smb2fh *fh,
               const uint8_t *buf, uint32_t count)
{
    if (pfn_write) return pfn_write(smb2,fh,buf,count);
    return -1;
}

int64_t CSambaCore::smb2_lseek(struct smb2_context *smb2, struct smb2fh *fh,
                   int64_t offset, int whence, uint64_t *current_offset)
{
    if (pfn_lseek) return pfn_lseek(smb2,fh,offset,whence,current_offset);
    return -1;
}

int CSambaCore::smb2_unlink(struct smb2_context *smb2, const char *path)
{
    if (pfn_unlink) return pfn_unlink(smb2,path);
    return -1;
}

int CSambaCore::smb2_truncate(struct smb2_context *smb2, const char *path,
                  uint64_t length)
{
    if (pfn_truncate) return pfn_truncate(smb2,path,length);
    return -1;
}

int CSambaCore::smb2_ftruncate(struct smb2_context *smb2, struct smb2fh *fh,
                   uint64_t length)
{
    if (pfn_ftruncate) return pfn_ftruncate(smb2,fh,length);
    return -1;
}

int CSambaCore::smb2_fstat(struct smb2_context *smb2, struct smb2fh *fh,
               struct smb2_stat_64 *st)
{
    if (pfn_fstat) return pfn_fstat(smb2,fh,st);
    return -1;
}

int CSambaCore::smb2_stat(struct smb2_context *smb2, const char *path,
              struct smb2_stat_64 *st)
{
    if (pfn_stat) return pfn_stat(smb2,path,st);
    return -1;
}

int CSambaCore::smb2_rename(struct smb2_context *smb2, const char *oldpath,
              const char *newpath)
{
    if (pfn_rename) return pfn_rename(smb2,oldpath,newpath);
    return -1;
}

uint32_t CSambaCore::smb2_get_max_read_size(smb2_context *smb2)
{
    if (pfn_get_max_read_size) return pfn_get_max_read_size(smb2);
    return 0;
}

uint32_t CSambaCore::smb2_get_max_write_size(smb2_context *smb2)
{
    if (pfn_get_max_write_size) return pfn_get_max_write_size(smb2);
    return 0;
}

