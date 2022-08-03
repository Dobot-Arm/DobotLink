QT += testlib concurrent
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  \
    main.cpp \
    test_industrialrobot_filecontroll.cpp \
    test_industrialrobot_filecontroll_concurrency.cpp

CONFIG(release, debug|release): {
DESTDIR = $$PWD/../../Test_Output
} else:CONFIG(debug, debug|release): {
DESTDIR = $$PWD/../../Test_Output_d
}

INCLUDEPATH += \
$$PWD/../DobotLink \
$$PWD/../Plugins/IndustrialRobotPlugin \

SOURCES += \
$$PWD/../Plugins/IndustrialRobotPlugin/cmythreadpool.cpp \
$$PWD/../Plugins/IndustrialRobotPlugin/IOThread.cpp \
$$PWD/../Plugins/IndustrialRobotPlugin/FileControll.cpp \

HEADERS += \
$$PWD/../Plugins/IndustrialRobotPlugin/cmythreadpool.h \
$$PWD/../Plugins/IndustrialRobotPlugin/IOThread.h \
$$PWD/../Plugins/IndustrialRobotPlugin/FileControll.h \
    test_industrialrobot_filecontroll.h \
    test_industrialrobot_filecontroll_concurrency.h
