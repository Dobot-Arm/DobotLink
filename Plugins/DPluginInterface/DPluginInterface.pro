#----------------------------------------------------------
#
# DPluginInterface created by LiuYufei 2019-01-29T18:00:00
#
#----------------------------------------------------------

QT       -= gui
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
    TARGET = DPluginInterface
} else:CONFIG(debug, debug|release): {
    TARGET = DPluginInterface_d
}

DEFINES += DPLUGININTERFACE_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
        DPluginInterface.cpp

HEADERS += \
        DPluginInterface.h

unix:!android {
    target.path = /usr/lib
    INSTALLS += target
}

#MOC_DIR = $$PWD/../../temp/moc
#RCC_DIR = $$PWD/../../temp/rcc
#UI_DIR = $$PWD/../../temp/ui
#OBJECTS_DIR = $$PWD/../../temp/obj

#linux {
#QMAKE_LFLAGS += "-Wl,-rpath=.,--enable-new-dtags"
#}
