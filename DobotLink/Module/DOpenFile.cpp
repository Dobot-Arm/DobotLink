#include "DOpenFile.h"

#include <QCoreApplication>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

DOpenFile *DOpenFile::getInstance()
{
    static DOpenFile *instance = nullptr;
    if (instance == nullptr) {
        instance = new DOpenFile();
    }
    return instance;
}

DOpenFile::DOpenFile(QObject *parent) : QObject(parent)
{
//    _processInit();
}

void DOpenFile::_processInit()
{
    m_vscodeProcess = new QProcess(this);
    m_vscodeProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_vscodeProcess->setProgram("code.cmd");

    connect(m_vscodeProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DOpenFile::onProcessFinished_slot);
    connect(m_vscodeProcess, &QProcess::errorOccurred, this, &DOpenFile::errorHandle_slot);
}

void DOpenFile::openFileWithVSCode(QString fileName)
{
    QDir dir(QCoreApplication::applicationDirPath());

    QStringList arguments;
    arguments << dir.absoluteFilePath(fileName);

    m_vscodeProcess->setArguments(arguments);
    m_vscodeProcess->startDetached();
}

void DOpenFile::openFileWithDesktopServices(QString fileName)
{
    QDir dir(QCoreApplication::applicationDirPath());

#ifdef Q_OS_MAC
    dir.cdUp();
    dir.cd("Resources");
#endif

    qDebug() << __FUNCTION__ << dir.absoluteFilePath(fileName);
    if (dir.exists("readme")) {
        dir.cd("readme");
        QUrl url = QUrl::fromLocalFile(dir.absoluteFilePath(fileName));
        QDesktopServices::openUrl(url);
    }
}

void DOpenFile::onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "finished" << exitCode << exitStatus;
}

void DOpenFile::errorHandle_slot(QProcess::ProcessError error)
{
    qDebug() << "DOpenFileWithVSCode: ERROR:" << error;
}
