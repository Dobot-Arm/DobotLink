#-------------------------------------------------
#
# Project created by QtCreator 2019-03-22T09:45:23
#
#-------------------------------------------------

#QT       += core gui serialport
CONFIG += c++11

DEFINES += USE_VIRTUAL_SERIAL_PORT

INCLUDEPATH += \
    $$PWD/src \
    $$PWD/src/runtime \
    $$PWD/src/algorithm \
    $$PWD/src/dcps \
    $$PWD/src/log \
    $$PWD/src/mm

SOURCES += \
    #$$PWD/src/virtualport/MonitorUI.cpp \
    #$$PWD/src/virtualport/MySafeUI.cpp \
    $$PWD/src/virtualport/VirtualSerialPort.cpp \
    $$PWD/src/virtualport/VirtualSerialPortInfo.cpp \
    $$PWD/src/virtualport/VirtualSerialPortManager.cpp \
    $$PWD/src/virtualport/VSPServer.cpp \
    $$PWD/src/algorithm/alarms.cpp \
    $$PWD/src/algorithm/moveCircle.cpp \
    $$PWD/src/algorithm/playBack.cpp \
    $$PWD/src/algorithm/rtKinematicCalc.cpp \
    $$PWD/src/algorithm/teach.cpp \
    $$PWD/src/algorithm/variable.cpp \
    $$PWD/src/mm/profile.cpp \
    $$PWD/src/runtime/dcps/cmd.cpp \
    $$PWD/src/runtime/dcps/dcps.cpp \
    $$PWD/src/runtime/dcps/semaphore.cpp \
    $$PWD/src/runtime/log/log.cpp

HEADERS += \
    #$$PWD/src/virtualport/MonitorUI.h \
    #$$PWD/src/virtualport/MySafeUI.h \
    $$PWD/src/virtualport/VirtualSerialPort.h \
    $$PWD/src/virtualport/VirtualSerialPortInfo.h \
    $$PWD/src/virtualport/VirtualSerialPortManager.h \
    $$PWD/src/virtualport/VSPServer.h \
    $$PWD/src/algorithm/BitMacro.h \
    $$PWD/src/algorithm/alarms.h \
    $$PWD/src/algorithm/moveCircle.h \
    $$PWD/src/algorithm/playBack.h \
    $$PWD/src/algorithm/rtKinematicCalc.h \
    $$PWD/src/algorithm/symbol.h \
    $$PWD/src/algorithm/teach.h \
    $$PWD/src/algorithm/type.h \
    $$PWD/src/algorithm/variable.h \
    $$PWD/src/mm/profile.h \
    $$PWD/src/mm/ParaComm.h \
    $$PWD/src/mm/RobotInfo.h \
    $$PWD/src/runtime/dcps/dcps.h \
    $$PWD/src/runtime/dcps/semaphore.h \
    $$PWD/src/runtime/log/log.h





