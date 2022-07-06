// Add C++ includes here
#ifndef __arm__
#include "Views/DAboutUsDialog.h"
#include "Views/DLocalMonitorForm.h"
#include "Views/DRemoteMonitorForm.h"
#include "Views/DUpgradeDialog.h"
#include "ui_DobotLinkMain.h"
// Qt includes
#include <QMessageBox>
#include <QScrollBar>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QWidget>
#include <QApplication>
#include <QSerialPort>
#include <QSerialPortInfo>
#endif

#ifdef Q_OS_ANDROID
#include <luamobile.h>
#include <QThread>
#include <printerlistenworker.h>
#endif
// Qt includes
#include <QSettings>
#include <QDir>
#include <QLocale>
#include <QDate>
#include <QTimer>
#include <QDebug>
#include <QStandardPaths>
#include <QJsonObject>
#include <QJsonValue>
#include <QProcess>
#include <QMap>
#include <QObject>
#include <QWebSocket>
#include <QString>
#include <QMessageLogger>
#include <QFile>
#include <QFileInfoList>
#include <QMutex>
#include <QVariant>
#include <QCoreApplication>
#include <QUdpSocket>
#include <QtGlobal>
#include <QList>

// #include <QtCore> //请谨慎包含
// #include <QtGui>  //请谨慎包含

// other
#include "DPluginInterface.h"
#include "Module/DNetworkManager.h"



// Other

