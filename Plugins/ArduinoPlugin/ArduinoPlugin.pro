#-------------------------------------------------------
#
# ArduinoPlugin created by LiuYufei 2019-01-29T19:45:00
#
#-------------------------------------------------------

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
    TARGET = ArduinoPlugin
} else:CONFIG(debug, debug|release): {
    TARGET = ArduinoPlugin_d
}

DEFINES += ARDUINO_PLUGIN_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    ArduinoPlugin.cpp \
    DAvrCompiler.cpp \
    DAvrUploader.cpp \
    ArduinoPacket.cpp


HEADERS += \
    ArduinoPlugin.h \
    DAvrCompiler.h \
    DAvrUploader.h \
    ArduinoPacket.h


unix {
    target.path = /usr/lib
    INSTALLS += target
}

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

INCLUDEPATH += $$PWD/../../$${MY_OUTPUT_DIR} \
    $$PWD/../DPluginInterface \
    $$PWD/../../DobotLink

DEPENDPATH += $$PWD/../../$${MY_OUTPUT_DIR}
