#---------------------------------------------------
#
# DobotLink created by LiuYufei 2019-01-29T17:30:00
# Where there is a will, there is a way.
#
#---------------------------------------------------

QT       += core
QT       += network websockets serialport
!linux-arm-gnueabi-g++ {
QT       += gui widgets
QT       += multimedia multimediawidgets
}

contains(QT_ARCH, i386) {
    MY_OUTPUT_DIR=Output
} else {
    MY_OUTPUT_DIR=Output
}

CONFIG          += precompile_header
PRECOMPILED_HEADER=$$PWD/stable.h

CONFIG(release, debug|release): {
TARGET = DobotLink
} else:CONFIG(debug, debug|release): {
TARGET = DobotLink_d
}
TEMPLATE = app

# DESTDIR
CONFIG(release, debug|release): DESTDIR = $$PWD/../$${MY_OUTPUT_DIR}
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../$${MY_OUTPUT_DIR}_d

CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    Module/DNetworkManager.cpp \
    Upgrade/DUpgrade.cpp \
    main.cpp \
    DobotLinkMain.cpp \
    DError/DError.cpp \
    DelayToQuit/DelayToQuit.cpp \
    MessageCenter/DMessageCenter.cpp \
    MessageCenter/DPacket.cpp \
    PluginManager/DPluginManager.cpp \
    Module/DLogger.cpp \
    Module/DSerialPort.cpp \
    Module/DOpenFile.cpp \
    Module/DSettings.cpp \
    Module/DTcpSocketServer.cpp \
    Module/DWebSocketServer.cpp \

HEADERS += \
    DobotLinkMain.h \
    DError/DError.h \
    DelayToQuit/DelayToQuit.h \
    MessageCenter/DMessageCenter.h \
    MessageCenter/DPacket.h \
    Module/DNetworkManager.h \
    PluginManager/DPluginManager.h \
    Module/DLogger.h \
    Module/DSerialPort.h \
    Module/DOpenFile.h \
    Module/DSettings.h \
    Module/DTcpSocketServer.h \
    Module/DWebSocketServer.h \
    Upgrade/DUpgrade.h \
    stable.h

!linux-arm-gnueabi-g++ {
SOURCES += \
    Views/DDownloadTestForm.cpp \
    Views/DUpgradeDialog.cpp \
    Views/DfuDownloadTestForm.cpp \
    Views/DAboutUsDialog.cpp \
    Views/DArduinoTestForm.cpp \
    Views/DMicrobitTestForm.cpp \
    Views/DSerialTestForm.cpp \
    Views/DMagicianTestForm.cpp \
    Views/DDownloadDialog.cpp \
    Views/DSendUserPacketDialog.cpp \
    Views/DLocalMonitorForm.cpp \
    Views/DRemoteMonitorForm.cpp

HEADERS += \
    Views/DAboutUsDialog.h \
    Views/DArduinoTestForm.h \
    Views/DDownloadTestForm.h \
    Views/DMicrobitTestForm.h \
    Views/DSerialTestForm.h \
    Views/DMagicianTestForm.h \
    Views/DDownloadDialog.h \
    Views/DSendUserPacketDialog.h \
    Views/DLocalMonitorForm.h \
    Views/DRemoteMonitorForm.h \
    Views/DUpgradeDialog.h \
    Views/DfuDownloadTestForm.h

FORMS += \
    DobotLinkMain.ui \
    Views/DAboutUsDialog.ui \
    Views/DArduinoTestForm.ui \
    Views/DDownloadDialog.ui \
    Views/DDownloadTestForm.ui \
    Views/DLocalMonitorForm.ui \
    Views/DMicrobitTestForm.ui \
    Views/DRemoteMonitorForm.ui \
    Views/DSendUserPacketDialog.ui \
    Views/DSerialTestForm.ui \
    Views/DMagicianTestForm.ui \
    Views/DUpgradeDialog.ui \
    Views/DfuDownloadTestForm.ui
}

RESOURCES += \
    resource/translation.qrc \
    resource/image.qrc


TRANSLATIONS += resource/dobotlink.ts \
                resource/dobotlink_en.ts
RC_FILE = resource/myapp.rc
ICON = dobotlink.icns


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


# Connect Dobot-Plugin-Interface Path
win32 {
CONFIG(release, debug|release): LIBS += -L$$PWD/../$${MY_OUTPUT_DIR}/ -lDPluginInterface
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../$${MY_OUTPUT_DIR}_d/ -lDPluginInterface_d
}

macx {
    LIBS += -framework Foundation
    HEADERS += $$PWD/MacosApi.h
    OBJECTIVE_SOURCES += $$PWD/MacosApi.mm
    QMAKE_INFO_PLIST =$$PWD/Info.plist
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../$${MY_OUTPUT_DIR}/DobotLink.app/Contents/Frameworks/ -lDPluginInterface.1.0.0
        QMAKE_POST_LINK += install_name_tool -change libDPluginInterface.1.dylib @rpath/libDPluginInterface.dylib $$DESTDIR/DobotLink.app/Contents/MacOS/Dobotlink
    } else:CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../$${MY_OUTPUT_DIR}_d/DobotLink_d.app/Contents/Frameworks -lDPluginInterface_d.1.0.0
        QMAKE_POST_LINK += install_name_tool -change libDPluginInterface_d.1.dylib @rpath/libDPluginInterface_d.dylib $$DESTDIR/DobotLink_d.app/Contents/MacOS/Dobotlink_d
    }
}

linux {
CONFIG(release, debug|release): LIBS += -L$$PWD/../$${MY_OUTPUT_DIR}/ -lDPluginInterface
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../$${MY_OUTPUT_DIR}_d/ -lDPluginInterface_d
}

android {
CONFIG(release, debug|release): LIBS += -L$$PWD/../$${MY_OUTPUT_DIR}/ -lDPluginInterface
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../$${MY_OUTPUT_DIR}_d/ -lDPluginInterface_d
}




INCLUDEPATH += $$PWD/../$${MY_OUTPUT_DIR} \
    $$PWD/../Plugins/DPluginInterface

DEPENDPATH += $$PWD/../$${MY_OUTPUT_DIR}

#MOC_DIR = $$PWD/../temp/moc
#RCC_DIR = $$PWD/../temp/rcc
#UI_DIR = $$PWD/../temp/ui
#OBJECTS_DIR = $$PWD/../temp/obj

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_EXTRA_LIBS = \
        F:/git_workspace/dobotlinkpro/DobotLinkPro/DobotLink/../Plugins/MagicDevicePlugin/MagicDevice/libMagicDevice_d.so

}

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/../Plugins/MagicDevicePlugin/MagicDevice_armeabi_v7a/libMagicDevice_d.so
}

#linux {
#QMAKE_LFLAGS += "-Wl,-rpath=.,--enable-new-dtags"
#}
