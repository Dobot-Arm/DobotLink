SOURCES += \
    $$PWD/CurlNetworkManager.cpp \
    $$PWD/CurlNetworkRequest.cpp \
    $$PWD/CurlNetworkReply.cpp

HEADERS += \
    $$PWD/CurlNetworkManager.h \
    $$PWD/CurlNetworkRequest.h \
    $$PWD/CurlNetworkReply.h

win32: LIBS += -L$$PWD/lib/ -lcurldll
macx: LIBS += -L$$PWD/lib_mac/ -lcurl.4
android{
    contains(ANDROID_TARGET_ARCH,arm64-v8a){
       LIBS += -L$$PWD/lib_android/ -lcurl
    }
    contains(ANDROID_TARGET_ARCH,armeabi-v7a){
      LIBS += -L$$PWD/lib_android_armeabi_v7a/ -lcurl
    }
}

INCLUDEPATH += $$PWD/curl
DEPENDPATH += $$PWD/curl
