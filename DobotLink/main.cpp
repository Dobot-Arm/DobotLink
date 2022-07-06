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

#define SINGLEAPPLICATION

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
    if (translator.load(qmPath)) {
        bool res = qApp->installTranslator(&translator);
        qDebug() << "app.installTranslator" << res;
    } else {
        qDebug() << "main translator.load() false";
    }

#ifdef SINGLEAPPLICATION
    /* make sure only one application is running */
    QSharedMemory shared("DobotLink_Running");
    if (shared.attach()) {
        DelayToQuit dQuit;
        dQuit.start();
        return app.exec();
    }
    shared.create(1);
#endif 

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

