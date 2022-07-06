#--------------------------------------------------------
#
# MicrobitPlugin created by LiuYufei 2019-01-29T19:45:00
#
#--------------------------------------------------------

QT       -= gui
TEMPLATE = lib

macx {
    CONFIG(release, debug|release): {
        DESTDIR = $$PWD/../../Output/DobotLink.app/Contents/Frameworks
    } else:CONFIG(debug, debug|release): {
        DESTDIR = $$PWD/../../Output_d/DobotLink_d.app/Contents/Frameworks
    }
} else {
    CONFIG(release, debug|release): {
        DESTDIR = $$PWD/../../Output
    } else:CONFIG(debug, debug|release): {
        DESTDIR = $$PWD/../../Output_d
    }
}

CONFIG(release, debug|release): {
    TARGET = MicrobitPlugin
} else:CONFIG(debug, debug|release): {
    TARGET = MicrobitPlugin_d
}

DEFINES += MICROBIT_PLUGIN_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
        MicrobitPlugin.cpp \
    MicrobitPacket.cpp

HEADERS += \
        MicrobitPlugin.h \
    MicrobitPacket.h

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
    CONFIG(release, debug|release) {
        LIBS += -L$$DESTDIR/ -lDPluginInterface.1.0.0
        QMAKE_POST_LINK += install_name_tool -change libDPluginInterface.1.dylib @rpath/libDPluginInterface.dylib $$DESTDIR/lib$${TARGET}.1.0.0.dylib
    } else:CONFIG(debug, debug|release) {
        LIBS += -L$$DESTDIR/ -lDPluginInterface_d.1.0.0
        QMAKE_POST_LINK += install_name_tool -change libDPluginInterface_d.1.dylib @rpath/libDPluginInterface_d.dylib $$DESTDIR/lib$${TARGET}.1.0.0.dylib
    }
}

INCLUDEPATH += $$PWD/../../Output \
    $$PWD/../DPluginInterface

DEPENDPATH += $$PWD/../../Output
