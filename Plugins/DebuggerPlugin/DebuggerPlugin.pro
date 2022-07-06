QT -= gui
QT += core serialport

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
    TARGET = DebuggerPlugin
} else:CONFIG(debug, debug|release): {
    TARGET = DebuggerPlugin_d
}

DEFINES += DEBUGGERPlugin_LIBRARY

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
    ../../DobotLink/DError/DError.cpp \
    ../../DobotLink/MessageCenter/DPacket.cpp \
    debuggerplugin.cpp

HEADERS += \
    ../../DobotLink/DError/DError.h \
    ../../DobotLink/MessageCenter/DPacket.h \
    debuggerplugin.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target


INCLUDEPATH += \
    $$PWD/../../$${MY_OUTPUT_DIR} \
    $$PWD/../DPluginInterface \
    $$PWD/../../DobotLink

DEPENDPATH += $$PWD/../../$${MY_OUTPUT_DIR}

# Connect Dobot-Plugin-Interface Path
win32{
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
