#ifdef __arm__
#include <QCoreApplication>
#else
#include <QApplication>
#endif
#include <QSharedMemory>
#include <QTranslator>
#include <qicon.h>

#if (defined (Q_OS_LINUX) || defined (Q_OS_MAC))
#include <QDir>
#endif

#ifdef Q_OS_MAC
#include "MacosApi.h"
#include <QDebug>
#endif

#include "DobotLinkMain.h"
#include "DelayToQuit/DelayToQuit.h"
#include "Module/DSettings.h"
#include "Module/DLogger.h"

QSharedMemory g_sharedMemoryMain;

int main(int argc, char *argv[])
{
#if (defined (Q_OS_LINUX) || defined (Q_OS_MAC))
    QDir::setCurrent(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
#endif

#ifdef __arm__
    QCoreApplication app(argc, argv);
#else
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    QCoreApplication::setOrganizationName(QStringLiteral("Dobot"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("dobot.cc"));
    QCoreApplication::setApplicationName(QStringLiteral("DobotLink"));
    QApplication::setApplicationName(QStringLiteral("DobotLink"));
    QApplication::setOrganizationName(QStringLiteral("Dobot"));
    QApplication::setOrganizationDomain(QStringLiteral("dobot.cc"));
    QApplication::setWindowIcon(QIcon(QStringLiteral("://images/DobotLinkIcon.ico")));
    QApplication::setApplicationDisplayName(QStringLiteral("DobotLink"));

    QTranslator translator;
    QLocale locale;
    QString qmPath = ":/dobotlink.qm";
    if(locale.language() == QLocale::English)  //获取系统语言环境
    {
        qDebug() << "English system" ;
        qmPath = ":/dobotlink_en.qm";
    } else if(locale.language() == QLocale::Chinese )
    {
        qDebug() << "中文系统";
        qmPath = ":/dobotlink.qm";
    }
    if (DSettings::getInstance()->getIsLogging()) {
        DLogger::getInstance()->startLogging();
    }
    qDebug()<<"app path=====>"<<app.applicationDirPath();
    if (translator.load(qmPath)) {
        bool res = qApp->installTranslator(&translator);
        qDebug() << "app.installTranslator" << res;
    } else {
        qDebug() << "main translator.load() false";
    }

    /* make sure only one application is running */
    g_sharedMemoryMain.setKey("DobotLink_Running");    
    if (!g_sharedMemoryMain.create(1))
    {
        qDebug() << "the first time to create shared memory fail:"<<g_sharedMemoryMain.errorString();
        if (g_sharedMemoryMain.error()==QSharedMemory::AlreadyExists)
        {
            g_sharedMemoryMain.attach();
            g_sharedMemoryMain.detach();
            if (!g_sharedMemoryMain.create(1))
            {
                qDebug() << "the second time to create shared memory fail:"<<g_sharedMemoryMain.errorString();
                qDebug() << "app start fail,because the app is running!!!!!";
                DelayToQuit dQuit;
                dQuit.start();
                return app.exec();
            }
        }
    }

#endif
    DobotLinkMain mainWindow;
    //mainWindow.show();

#ifdef Q_OS_MAC
    if (MacosApi::disableAppNap()) {
        qDebug() << "Disabled App Nap feature.";
    } else {
        qDebug() << "Can not disable App Nap feature.";
    }
#endif
    return app.exec();
}

