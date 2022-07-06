#--------------------------------------------------------
#
# IndustrialRobotPlugin created by LiuYufei 2020-05-07T19:31:00
#
#--------------------------------------------------------

QT       -= gui
QT       += core serialport network concurrent
TEMPLATE = lib

contains(QT_ARCH, i386) {
    MY_OUTPUT_DIR=Output
} else {
    MY_OUTPUT_DIR=Output
}

include("../SmbClientWrapper/SmbClientWrapper.pri")

macx {
    CONFIG(release, debug|release): {
        DESTDIR = $$PWD/../../$${MY_OUTPUT_DIR}/DobotLink.app/Contents/Frameworks
    } else:CONFIG(debug, debug|release): {
        DESTDIR = $$PWD/../../$${MY_OUTPUT_DIR}_d/DobotLink_d.app/Contents/Frameworks
    }
} else {
    CONFIG(release, debug|release): {
        DESTDIR = $$PWD/../../$${MY_OUTPUT_DIR}
    } else:CONFIG(debug, debug|release): {
        DESTDIR = $$PWD/../../$${MY_OUTPUT_DIR}_d
    }
}

CONFIG(release, debug|release): {
    TARGET = IndustrialRobotPlugin
} else:CONFIG(debug, debug|release): {
    TARGET = IndustrialRobotPlugin_d
}

DEFINES += MAGICIAN_PLUGIN_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    ../../DobotLink/DError/DError.cpp \
    ../../DobotLink/MessageCenter/DPacket.cpp \
    Device.cpp \
    FileControll.cpp \
    FileControllSmb.cpp \
    IOThread.cpp \
    IOThreadSmb.cpp \
    IndustrialRobotPlugin.cpp \
    Mobdebug.cpp \
    Module.cpp \
    RobotStatus.cpp

HEADERS += \
    ../../DobotLink/DError/DError.h \
    ../../DobotLink/MessageCenter/DPacket.h \
    Device.h \
    FileControll.h \
    FileControllSmb.h \
    IOThread.h \
    IOThreadSmb.h \
    IndustrialRobotPlugin.h \
    Mobdebug.h \
    Module.h \
    RobotStatus.h

unix:!android {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += \
    $$PWD/../../$${MY_OUTPUT_DIR} \
    $$PWD/../DPluginInterface \
    $$PWD/../../DobotLink \
    $$PWD/../SmbClientWrapper/src

DEPENDPATH += $$PWD/../../$${MY_OUTPUT_DIR}

# include($$PWD/curlnetwork/curlnetwork.pri)

# Connect Dobot-Plugin-Interface Path
win32{
CONFIG(release, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}/ -lDPluginInterface
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}_d/ -lDPluginInterface_d

LIBS += -lws2_32
}

macx {
    CONFIG(release, debug|release) {
        LIBS += -L$$DESTDIR/ -lDPluginInterface.1.0.0
        QMAKE_POST_LINK += install_name_tool -change libDPluginInterface.1.dylib @rpath/libDPluginInterface.dylib $$DESTDIR/lib$${TARGET}.1.0.0.dylib
    } else:CONFIG(debug, debug|release) {
        LIBS += -L$$DESTDIR/ -lDPluginInterface_d.1.0.0
        QMAKE_POST_LINK += install_name_tool -change libDPluginInterface_d.1.dylib @rpath/libDPluginInterface_d.dylib $$DESTDIR/lib$${TARGET}.1.0.0.dylib
    }
}

android{
CONFIG(release, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}/ -lDPluginInterface
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}_d/ -lDPluginInterface_d
}

android {
CONFIG(release, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}/ -lLuaMobile
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}_d/ -lLuaMobile_d

INCLUDEPATH += $$PWD/../../LuaMobile \
}

linux {
CONFIG(release, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}/ -lDPluginInterface
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}_d/ -lDPluginInterface_d
}

#linux {
#QMAKE_LFLAGS += "-Wl,-rpath=.,--enable-new-dtags"
#}
