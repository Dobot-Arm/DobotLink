#-------------------------------------------------
#
# DemoPlugin created by LYF 2019-01-29T19:45:00
#
#-------------------------------------------------

QT       -= gui
TEMPLATE = lib
DESTDIR = $$PWD/../../Output

win32:CONFIG(release, debug|release): TARGET = DemoPlugin
else:win32:CONFIG(debug, debug|release): TARGET = DemoPlugin_d

DEFINES += DEMO_PLUGIN_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
        DemoPlugin.cpp \
    DemoPacket.cpp

HEADERS += \
        DemoPlugin.h \
    DemoPacket.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

# Connect Dobot-Plugin-Interface Path
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Output/ -lDPluginInterface
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Output/ -lDPluginInterface_d

INCLUDEPATH += $$PWD/../../Output \
    $$PWD/../DPluginInterface

DEPENDPATH += $$PWD/../../Output
