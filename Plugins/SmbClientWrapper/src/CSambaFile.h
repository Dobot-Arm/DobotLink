#pragma once

#include "CSambaIOStream.h"

class CSambaFile : public CSambaIOStream
{
public:
    CSambaFile(CSambaClient* pClient);
    virtual ~CSambaFile();

    bool Open(const std::string& strPath) override;
    void Close() override;
    bool Create(const std::string& strPath) override;
    bool Delete(const std::string& strPath) override;

    int Read(char* pBuffer, int iSize) override;
    std::string ReadAllText() override;
    int Write(const char* pszBuffer, int iSize) override;
    bool WriteAllText(const std::string& str) override;

    //open file for write, and trancate content. create file if not exists
    bool OpenTruncate(const std::string& strPath);

    //copy file
    bool Copy(const std::string& strOldPath, const std::string& strNewPath);

    /*
    func: Move the file pointer to the nPos position
    params: nOffset-Moved offset position
            iWhence-where to start the offset, the value is SEEK_SET,SEEK_CUR,SEEK_END
    return: The current pointer is relative to the beginning of the file. If it is -1, the flag is wrong.
    */
    uint64_t SetFilePointer(uint64_t nOffset, int iWhence);

    const CSambaFileInfo& GetFileInfo();

private:
    bool Open(const std::string& strPath, int iOpenMode);

private:
    void* m_pSmbCtx; //struct smb2fh*;
    CSambaFileInfo m_fInfo;
};

