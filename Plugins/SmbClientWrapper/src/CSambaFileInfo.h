#pragma once

#include <string>
#include <cstdint>

class CSambaFileInfo
{
public:
    std::string strName;
    uint64_t nSize;
    bool bIsFile;
    bool bIsDir;
    bool bIsLink;

    //elapsed millseconds from 1970-1-1 0:0:0
    uint64_t nCreateTime;
    uint64_t nLastAccessTime;
    uint64_t nLastModifyTime;
};

