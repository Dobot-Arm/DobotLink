#pragma once

#include <cstdint>
#include <smb2/libsmb2.h>
#include <smb2/smb2.h>
#include <mutex>

#include <QLibrary>
class QString;

class CSambaCore
{
    CSambaCore();
public:
    ~CSambaCore();
    CSambaCore(const CSambaCore&) = delete;
    CSambaCore(CSambaCore&&) = delete;
    CSambaCore& operator=(const CSambaCore&) = delete;
    CSambaCore& operator=(CSambaCore&&) = delete;

    static CSambaCore& GetInstance();

    bool LoadDll(const QString& strDll);
    bool IsLoaded();
    void UnloadDll();

    struct smb2_context *smb2_init_context(void);
    void smb2_destroy_context(struct smb2_context *smb2);
    void smb2_set_timeout(struct smb2_context *smb2, int seconds);
    const char *smb2_get_error(struct smb2_context *smb2);
    void smb2_set_error(struct smb2_context *smb2, const char *error_string, ...);

    void smb2_set_security_mode(struct smb2_context *smb2, uint16_t security_mode);
    void smb2_set_user(struct smb2_context *smb2, const char *user);
    void smb2_set_password(struct smb2_context *smb2, const char *password);
    void smb2_set_domain(struct smb2_context *smb2, const char *domain);

    int smb2_connect_share(struct smb2_context *smb2,
                           const char *server,
                           const char *share,
                           const char *user);
    int smb2_disconnect_share(struct smb2_context *smb2);
    int smb2_isconnect_share(struct smb2_context *smb2);

    struct smb2dir *smb2_opendir(struct smb2_context *smb2, const char *path);
    void smb2_closedir(struct smb2_context *smb2, struct smb2dir *smb2dir);
    struct smb2dirent *smb2_readdir(struct smb2_context *smb2,
                                    struct smb2dir *smb2dir);
    int smb2_mkdir(struct smb2_context *smb2, const char *path);
    int smb2_rmdir(struct smb2_context *smb2, const char *path);

    struct smb2fh *smb2_open(struct smb2_context *smb2, const char *path, int flags);
    int smb2_close(struct smb2_context *smb2, struct smb2fh *fh);
    int smb2_pread(struct smb2_context *smb2, struct smb2fh *fh,
                   uint8_t *buf, uint32_t count, uint64_t offset);
    int smb2_pwrite(struct smb2_context *smb2, struct smb2fh *fh,
                    const uint8_t *buf, uint32_t count, uint64_t offset);
    int smb2_read(struct smb2_context *smb2, struct smb2fh *fh,
                  uint8_t *buf, uint32_t count);
    int smb2_write(struct smb2_context *smb2, struct smb2fh *fh,
                   const uint8_t *buf, uint32_t count);
    int64_t smb2_lseek(struct smb2_context *smb2, struct smb2fh *fh,
                       int64_t offset, int whence, uint64_t *current_offset);
    int smb2_unlink(struct smb2_context *smb2, const char *path);

    int smb2_truncate(struct smb2_context *smb2, const char *path,
                      uint64_t length);
    int smb2_ftruncate(struct smb2_context *smb2, struct smb2fh *fh,
                       uint64_t length);

    int smb2_fstat(struct smb2_context *smb2, struct smb2fh *fh,
                   struct smb2_stat_64 *st);
    int smb2_stat(struct smb2_context *smb2, const char *path,
                  struct smb2_stat_64 *st);
    int smb2_rename(struct smb2_context *smb2, const char *oldpath,
                  const char *newpath);

private:
    void ResetFuncPtr();

private:
    QLibrary m_lib;
    std::mutex m_mtx;

    typedef struct smb2_context* (*pfn_smb2_init_context)();
    pfn_smb2_init_context pfn_init_context;

    typedef void (*pfn_smb2_destroy_context)(struct smb2_context *smb2);
    pfn_smb2_destroy_context pfn_destroy_context;

    typedef void (*pfn_smb2_set_timeout)(struct smb2_context *smb2, int seconds);
    pfn_smb2_set_timeout pfn_set_timeout;

    typedef const char *(*pfn_smb2_get_error)(struct smb2_context *smb2);
    pfn_smb2_get_error pfn_get_error;

    typedef void (*pfn_smb2_set_error)(struct smb2_context *smb2, const char *error_string, ...);
    pfn_smb2_set_error pfn_set_error;

    typedef void (*pfn_smb2_set_security_mode)(struct smb2_context *smb2, uint16_t security_mode);
    pfn_smb2_set_security_mode pfn_set_security_mode;

    typedef void (*pfn_smb2_set_user)(struct smb2_context *smb2, const char *user);
    pfn_smb2_set_user pfn_set_user;

    typedef void (*pfn_smb2_set_password)(struct smb2_context *smb2, const char *password);
    pfn_smb2_set_password pfn_set_password;

    typedef void (*pfn_smb2_set_domain)(struct smb2_context *smb2, const char *domain);
    pfn_smb2_set_domain pfn_set_domain;

    typedef int (*pfn_smb2_connect_share)(struct smb2_context *smb2,const char *server,const char *share,const char *user);
    pfn_smb2_connect_share pfn_connect_share;

    typedef int (*pfn_smb2_disconnect_share)(struct smb2_context *smb2);
    pfn_smb2_disconnect_share pfn_disconnect_share;

    typedef int (*pfn_smb2_isconnect_share)(struct smb2_context *smb2);
    pfn_smb2_isconnect_share pfn_isconnect_share;

    typedef struct smb2dir *(*pfn_smb2_opendir)(struct smb2_context *smb2, const char *path);
    pfn_smb2_opendir pfn_opendir;

    typedef void (*pfn_smb2_closedir)(struct smb2_context *smb2, struct smb2dir *smb2dir);
    pfn_smb2_closedir pfn_closedir;

    typedef struct smb2dirent *(*pfn_smb2_readdir)(struct smb2_context *smb2,struct smb2dir *smb2dir);
    pfn_smb2_readdir pfn_readdir;

    typedef int (*pfn_smb2_mkdir)(struct smb2_context *smb2, const char *path);
    pfn_smb2_mkdir pfn_mkdir;

    typedef int (*pfn_smb2_rmdir)(struct smb2_context *smb2, const char *path);
    pfn_smb2_rmdir pfn_rmdir;

    typedef struct smb2fh *(*pfn_smb2_open)(struct smb2_context *smb2, const char *path, int flags);
    pfn_smb2_open pfn_open;

    typedef int (*pfn_smb2_close)(struct smb2_context *smb2, struct smb2fh *fh);
    pfn_smb2_close pfn_close;

    typedef int (*pfn_smb2_pread)(struct smb2_context *smb2, struct smb2fh *fh, uint8_t *buf, uint32_t count, uint64_t offset);
    pfn_smb2_pread pfn_pread;

    typedef int (*pfn_smb2_pwrite)(struct smb2_context *smb2, struct smb2fh *fh,const uint8_t *buf, uint32_t count, uint64_t offset);
    pfn_smb2_pwrite pfn_pwrite;

    typedef int (*pfn_smb2_read)(struct smb2_context *smb2, struct smb2fh *fh,uint8_t *buf, uint32_t count);
    pfn_smb2_read pfn_read;

    typedef int (*pfn_smb2_write)(struct smb2_context *smb2, struct smb2fh *fh,const uint8_t *buf, uint32_t count);
    pfn_smb2_write pfn_write;

    typedef int64_t (*pfn_smb2_lseek)(struct smb2_context *smb2, struct smb2fh *fh,int64_t offset, int whence, uint64_t *current_offset);
    pfn_smb2_lseek pfn_lseek;

    typedef int (*pfn_smb2_unlink)(struct smb2_context *smb2, const char *path);
    pfn_smb2_unlink pfn_unlink;

    typedef int (*pfn_smb2_truncate)(struct smb2_context *smb2, const char *path,uint64_t length);
    pfn_smb2_truncate pfn_truncate;

    typedef int (*pfn_smb2_ftruncate)(struct smb2_context *smb2, struct smb2fh *fh,uint64_t length);
    pfn_smb2_ftruncate pfn_ftruncate;

    typedef int (*pfn_smb2_fstat)(struct smb2_context *smb2, struct smb2fh *fh,struct smb2_stat_64 *st);
    pfn_smb2_fstat pfn_fstat;

    typedef int (*pfn_smb2_stat)(struct smb2_context *smb2, const char *path,struct smb2_stat_64 *st);
    pfn_smb2_stat pfn_stat;

    typedef int (*pfn_smb2_rename)(struct smb2_context *smb2, const char *oldpath,const char *newpath);
    pfn_smb2_rename pfn_rename;
};

