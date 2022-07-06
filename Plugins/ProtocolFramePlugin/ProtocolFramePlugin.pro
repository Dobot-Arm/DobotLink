QT -= gui

QT += core serialport network

TEMPLATE = lib
DEFINES += PROTOCOLFRAMEPLUGIN_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Controller.cpp \
    MoozApi.cpp \
    MyThread.cpp \
    ProtocolFramePacket.cpp \
    ProtocolFramePlugin.cpp


HEADERS += \
    Controller.h \
    DobotType.h \
    MoozApi.h \
    MyThread.h \
    ProtocolFramePacket.h \
    ProtocolFramePlugin.h


# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

# Connect Dobot-Plugin-Interface Path
win32{
CONFIG(release, debug|release): LIBS += -L$$PWD/../../Output/ -lDPluginInterface
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Output_d/ -lDPluginInterface_d
}

macx{
CONFIG(release, debug|release): LIBS += -L$$PWD/../../Output/ -lDPluginInterface.1.0.0
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Output_d/ -lDPluginInterface_d.1.0.0
}

include(Dobot3Source.pri)

INCLUDEPATH += $$PWD/../../Output \
    $$PWD/../DPluginInterface \
    $$PWD/DobotV3.0/source \

DEPENDPATH += $$PWD/../../Output

CONFIG(release, debug|release): {
DESTDIR = $$PWD/../../Output
TARGET = MOOZPlugin
} else:CONFIG(debug, debug|release): {
DESTDIR = $$PWD/../../Output_d
TARGET = MOOZPlugin_d
}
