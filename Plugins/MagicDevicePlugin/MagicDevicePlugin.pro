#--------------------------------------------------------
#
# MagicDevicePlugin created by LiuYufei 2019-09-16T19:31:00
#
#--------------------------------------------------------

QT       -= gui
QT       += core

contains(QT_ARCH, i386) {
    MY_OUTPUT_DIR=Output
} else {
    MY_OUTPUT_DIR=Output
}


!wasm {
    include("./magicianlite_virtual/VirtualSerialPort.pri")
    QT       += serialport network
    TEMPLATE = lib
}

macx {
    CONFIG(release, debug|release): {
        DESTDIR = $$PWD/../../$${MY_OUTPUT_DIR}/DobotLink.app/Contents/Frameworks
    } else:CONFIG(debug, debug|release): {
        DESTDIR = $$PWD/../../$${MY_OUTPUT_DIR}_d/DobotLink_d.app/Contents/Frameworks
    }
} else {
    wasm {
        CONFIG(release, debug|release): {
            DESTDIR = $$PWD
        } else:CONFIG(debug, debug|release): {
            DESTDIR = $$PWD
        }
    } else {
        CONFIG(release, debug|release): {
            DESTDIR = $$PWD/../../$${MY_OUTPUT_DIR}
        } else:CONFIG(debug, debug|release): {
            DESTDIR = $$PWD/../../$${MY_OUTPUT_DIR}_d
        }
    }
}

CONFIG(release, debug|release): {
    TARGET = MagicDevicePlugin
} else:CONFIG(debug, debug|release): {
    TARGET = MagicDevicePlugin_d
}

CONFIG += c++11

DEFINES += MAGICIAN_PLUGIN_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    ../../DobotLink/DError/DError.cpp \
    ../../DobotLink/MessageCenter/DPacket.cpp \
    DBoxDownload.cpp \
    DProtocolInterface.cpp \
    MagicDevicePlugin.cpp \
    ActionTimerManager.cpp \
    MagicDevice.cpp \
    Protocol2/DP2Packet.cpp \
    Protocol3/CameraApi.cpp \
    Protocol3/CameraCalibration.cpp \
    Protocol3/DP3Packet.cpp \
    Protocol3/DProtocol3.cpp \
    Protocol3/GeneralApi.cpp \
    Protocol3/MagicBoxApi.cpp \
    Protocol3/MagicianGoApi.cpp \
    MessageHandler.cpp \
    Protocol2/DM1Protocol.cpp \
    Protocol2/DMagicianProtocol.cpp \
    Protocol2/DProtocol2.cpp \
    Protocol3/P3FrameAdapter.cpp

HEADERS += \
    ../../DobotLink/DError/DError.h \
    ../../DobotLink/MessageCenter/DPacket.h \
    DBoxDownload.h \
    DProtocolInterface.h \
    MagicDevicePlugin.h\
    MagicDevice_p.h \
    Protocol2/DP2Packet.h \
    Protocol3/CameraApi.h \
    Protocol3/CameraCalibration.h \
    Protocol3/DP3Packet.h \
    Protocol3/DProtocol3.h \
    Protocol3/DobotType.h \
    Protocol3/GeneralApi.h \
    Protocol3/MagicBoxApi.h \
    Protocol3/MagicianGoApi.h \
    Protocol2/DProtocol2.h \
    Protocol3/P3FrameAdapter.h \
    ActionTimerManager.h \
    MagicDevice.h \
    MessageHandler.h \
    Protocol2/DM1Protocol.h \
    Protocol2/DMagicianProtocol.h \
    Types.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += \
    $$PWD/../../$${MY_OUTPUT_DIR} \
    $$PWD/../DPluginInterface \
    $$PWD/../../DobotLink\
    $$PWD/DobotV3.0/DobotLink_DobotV3.0/source \
    $$PWD/Protocol2 \
    $$PWD/Protocol3

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

android{
    CONFIG(release, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}/ -lDPluginInterface
    else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../$${MY_OUTPUT_DIR}_d/ -lDPluginInterface_d
}

INCLUDEPATH += \
     $$PWD/DobotV3.0/DobotLink_DobotV3.0/source

SOURCES += \
    DobotV3.0/DobotLink_DobotV3.0/source/DobotV3CmdInter.c \
    DobotV3.0/DobotLink_DobotV3.0/source/DobotV3Format.c \
    DobotV3.0/DobotLink_DobotV3.0/source/DobotV3Func.c \
    DobotV3.0/DobotLink_DobotV3.0/source/ProtocolFrame.c \
    DobotV3.0/DobotLink_DobotV3.0/source/GeneralCmdRules.c\
    DobotV3.0/DobotLink_DobotV3.0/source/MGoCmdRules.c \
    DobotV3.0/DobotLink_DobotV3.0/source/MoozCmdRules.c \
    DobotV3.0/DobotLink_DobotV3.0/source/K210ArmCmdRules.c \
    DobotV3.0/DobotLink_DobotV3.0/source/K210CarCmdRules.c \
    DobotV3.0/DobotLink_DobotV3.0/source/MBoxCmdRules.c \

HEADERS += \
    DobotV3.0/DobotLink_DobotV3.0/source/DobotV3CmdInter.h \
    DobotV3.0/DobotLink_DobotV3.0/source/DobotV3Format.h \
    DobotV3.0/DobotLink_DobotV3.0/source/DobotV3Func.h \
    DobotV3.0/DobotLink_DobotV3.0/source/ProtocolFrame.h \
    DobotV3.0/DobotLink_DobotV3.0/source/GeneralCmdRules.h \
    DobotV3.0/DobotLink_DobotV3.0/source/MGoCmdRules.h \
    DobotV3.0/DobotLink_DobotV3.0/source/MoozCmdRules.h \
    DobotV3.0/DobotLink_DobotV3.0/source/DobotV3API.h \
    DobotV3.0/DobotLink_DobotV3.0/source/K210ArmCmdRules.h \
    DobotV3.0/DobotLink_DobotV3.0/source/K210CarCmdRules.h \
    DobotV3.0/DobotLink_DobotV3.0/source/MBoxCmdRules.h \

wasm {
    INCLUDEPATH += /Users/jomar/Documents/Dev/emsdk/upstream/emscripten/system/include
    
    INCLUDEPATH -= $$PWD/../../$${MY_OUTPUT_DIR} \
                   $$PWD/../DPluginInterface

    HEADERS +=  WebAssembly/EmSerialPort.h \
                WebAssembly/Marcs.h \
                WebAssembly/WebBase.h

    SOURCES +=  WebAssembly/EmSerialPort.cpp  \
                WebAssembly/WebBase.cpp  \
                WebAssembly/WrapperCommon.cpp  \
                WebAssembly/WrapperMagician.cpp 

     MOC_DIR = $$PWD/temp/moc
     OBJECTS_DIR = $$PWD/temp/obj
#    QMAKE_CFLAGS += -s PTHREAD_POOL_SIZE=6 \
#                    -s TOTAL_MEMORY=4GB

#    QMAKE_CXXFLAGS += -s PTHREAD_POOL_SIZE=6 \
#                      -s TOTAL_MEMORY=4GB

    QMAKE_LFLAGS += -s ASYNCIFY \
#                    -s PTHREAD_POOL_SIZE=6 \
#                    -s TOTAL_MEMORY=4GB

}
