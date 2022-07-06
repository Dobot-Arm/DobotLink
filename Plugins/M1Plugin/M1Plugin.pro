#--------------------------------------------------
#
# M1Plugin created by LiuYufei 2019-01-29T19:45:00
#
#--------------------------------------------------

QT       -= gui
QT       += serialport
TEMPLATE = lib
DESTDIR = $$PWD/../../Output

CONFIG(release, debug|release): {
DESTDIR = $$PWD/../../Output
TARGET = M1Plugin
} else:CONFIG(debug, debug|release): {
DESTDIR = $$PWD/../../Output_d
TARGET = M1Plugin_d
}

DEFINES += MAGICIANPLUGIN_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
        M1Plugin.cpp \
    M1Controller.cpp \
    M1Packet.cpp \
    M1WaitForFinish.cpp

HEADERS += \
        M1Plugin.h \
    M1Controller.h \
    M1Packet.h \
    M1WaitForFinish.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

# Connect Dobot-Plugin-Interface Path
win32 {
CONFIG(release, debug|release): LIBS += -L$$PWD/../../Output/ -lDPluginInterface
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Output_d/ -lDPluginInterface_d
}

macx {
CONFIG(release, debug|release): LIBS += -L$$PWD/../../Output/ -lDPluginInterface.1.0.0
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Output_d/ -lDPluginInterface_d.1.0.0
}

INCLUDEPATH += $$PWD/../../Output \
    $$PWD/../DPluginInterface

DEPENDPATH += $$PWD/../../Output

# Connect DobotDll Path
win32 {
CONFIG(release, debug|release): LIBS += -L$$PWD/DobotDll/ -lDobotDllM1
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/DobotDll/ -lDobotDllM1_d
}

macx {
CONFIG(release, debug|release): LIBS += -L$$PWD/DobotDll/ -lDobotDllM1.1.0.0
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/DobotDll/ -lDobotDllM1_d.1.0.0
}

INCLUDEPATH += $$PWD/DobotDll
DEPENDPATH += $$PWD/DobotDll
