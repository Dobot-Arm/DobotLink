#-------------------------------------------------
#
# Project created by QtCreator 2019-03-22T09:45:23
#
#-------------------------------------------------

#QT       += core gui serialport
CONFIG += c++11

INCLUDEPATH += $$PWD/../../smbclient/include

SOURCES += \
    $$PWD/src/CSambaCore.cpp \
    $$PWD/src/CSambaClient.cpp \
    $$PWD/src/CSambaDirectory.cpp \
    $$PWD/src/CSambaFile.cpp \
    $$PWD/src/CSambaFileInfo.cpp \
    $$PWD/src/CSambaIOStream.cpp

HEADERS += \
    $$PWD/../../smbclient/include/dllconfig.h \
    $$PWD/../../smbclient/include/libsmb2-private.h \
    $$PWD/../../smbclient/include/smb2/libsmb2.h \
    $$PWD/../../smbclient/include/smb2/smb2.h \
    $$PWD/../../smbclient/include/smb2/libsmb2-dcerpc.h \
    $$PWD/../../smbclient/include/smb2/libsmb2-dcerpc-lsa.h \
    $$PWD/../../smbclient/include/smb2/libsmb2-dcerpc-srvsvc.h \
    $$PWD/../../smbclient/include/smb2/libsmb2-raw.h \
    $$PWD/../../smbclient/include/smb2/smb2-errors.h \
    $$PWD/src/CSambaCore.h \
    $$PWD/src/CSambaClient.h \
    $$PWD/src/CSambaDirectory.h \
    $$PWD/src/CSambaFile.h \
    $$PWD/src/CSambaFileInfo.h \
    $$PWD/src/CSambaIOStream.h





