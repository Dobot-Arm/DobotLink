#pragma once

#include "CSambaIOStream.h"

class CSambaDirectory : public CSambaIOStream
{
public:
    CSambaDirectory(CSambaClient* pClient);
    virtual ~CSambaDirectory();

    bool Open(const std::string& strPath) override;
    void Close() override;
    bool Create(const std::string& strPath) override;
    bool Delete(const std::string& strPath) override;

    std::list<CSambaFileInfo> GetAllFiles() override;

    bool CreateRecursive(const std::string& strPath);
    bool DeleteRecursive(const std::string& strPath);

private:
    void* m_pSmbCtx;//struct smb2dir*
};

