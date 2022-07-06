#pragma once

#include <list>

#include "CSambaClient.h"
#include "CSambaFileInfo.h"

class CSambaIOStream
{
protected:
    CSambaClient* m_pClient;

public:
    CSambaIOStream(CSambaClient* pClient);
    virtual ~CSambaIOStream();
    CSambaIOStream(const CSambaIOStream&) = delete;
    CSambaIOStream(CSambaIOStream&&) = delete;
    CSambaIOStream& operator=(const CSambaIOStream&) = delete;
    CSambaIOStream& operator=(CSambaIOStream&&) = delete;

    //open for read only
    virtual bool Open(const std::string& strPath) = 0;
    virtual void Close() = 0;
    //create for only write. if not exists,it will create.
    virtual bool Create(const std::string& strPath) = 0;
    virtual bool Delete(const std::string& strPath) = 0;

    virtual std::list<CSambaFileInfo> GetAllFiles();
    //Read file,Returns the number of bytes read, -1 means failed.
    virtual int Read(char* pBuffer, int iSize);
    virtual std::string ReadAllText();
    //Write file,Returns the number of bytes writed, -1 means failed.
    virtual int Write(const char* pszBuffer, int iSize);
    virtual bool WriteAllText(const std::string& str);

    bool IsExists(const std::string& strPath);
    static bool IsExists(CSambaClient* pClient, const std::string& strPath);

    bool Rename(const std::string& strOld, const std::string& strNew);
    static bool Rename(CSambaClient* pClient, const std::string& strOld, const std::string& strNew);

    //get the path information,the path maybe file or directory
    static bool GetPathInfo(CSambaClient* pClient, const std::string& strPath,CSambaFileInfo& info);
};

