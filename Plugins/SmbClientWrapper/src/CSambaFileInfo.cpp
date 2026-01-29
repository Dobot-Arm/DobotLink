#include "CSambaFileInfo.h"

void CSambaFileInfo::Clear()
{
    strName = "";
    nSize = 0;
    bIsFile = false;
    bIsDir = false;
    bIsLink = false;

    //elapsed millseconds from 1970-1-1 0:0:0
    nCreateTime = 0;
    nLastAccessTime = 0;
    nLastModifyTime = 0;
}