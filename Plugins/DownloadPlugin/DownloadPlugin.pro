#------------------------------------------------------
#
# DownloadPlugin created by LiuYufei 2019-05-9T10:08:00
#
#------------------------------------------------------

QT       -= gui
QT       += core serialport
TEMPLATE = lib

contains(QT_ARCH, i386) {
    MY_OUTPUT_DIR=Output
} else {
    MY_OUTPUT_DIR=Output
}

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
    TARGET = DownloadPlugin
} else:CONFIG(debug, debug|release): {
    TARGET = DownloadPlugin_d
}

DEFINES += DEMO_PLUGIN_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    ../../DobotLink/DError/DError.cpp \
    ../../DobotLink/MessageCenter/DPacket.cpp \
    DDfufile.cpp \
    DGetVersion.cpp \
    DKflash.cpp \
    DMcuisp.cpp \
    DMultiFunc.cpp \
    DownloadPlugin.cpp

HEADERS += \
    ../../DobotLink/DError/DError.h \
    ../../DobotLink/MessageCenter/DPacket.h \
    DDfufile.h \
    DGetVersion.h \
    DKflash.h \
    DMcuisp.h \
    DMultiFunc.h \
    DownloadPlugin.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += \
    $$PWD/../../$${MY_OUTPUT_DIR} \
    $$PWD/../DPluginInterface \
    $$PWD/../../DobotLink

DEPENDPATH += $$PWD/../../$${MY_OUTPUT_DIR}


# Connect Dobot-Plugin-Interface Path
win32 {
CONFIG(release, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}/ -lDPluginInterface
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}_d/ -lDPluginInterface_d
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

android {
CONFIG(release, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}/ -lDPluginInterface
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}_d/ -lDPluginInterface_d
}




