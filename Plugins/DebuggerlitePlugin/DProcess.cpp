#include "DProcess.h"
#include <QTextCodec>
#include <QDebug>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>

const int NOTIFY_STDOUT_INTERVAL = 100;

DProcess::DProcess(const quint16 &wPort, QObject *parent):
    QObject(parent),
    m_wPort(wPort),
    m_waitId(0)
{
    static int dpid = 0;
    m_dpid = ++dpid;

    setWaitCmdFinished(false);

    m_proc = new QProcess(this);
    connect(m_proc, &QProcess::readyReadStandardOutput, this, &DProcess::slotProcStdout);
    connect(m_proc, &QProcess::readyReadStandardError, this, &DProcess::slotProcStderr);
    connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DProcess::slotProcFinish);
    connect(m_proc, &QProcess::started, this, &DProcess::slotProcStarted);

    m_tempFile = new QTemporaryFile("DobotLinkTemp", this);

    m_timerStdout = new QTimer(this);
    m_timerStdout->setInterval(NOTIFY_STDOUT_INTERVAL);
    m_timerStdout->setSingleShot(true);
    connect(m_timerStdout, &QTimer::timeout, [&](){
        if (m_stdoutDatas.size() > 0) {
            emit sigNotifyStdout(m_stdoutDatas);
            m_stdoutDatas = QJsonArray();
        }
        m_timerStdout->start();
    });
}

DProcess::~DProcess()
{
    setWaitCmdFinished(true);
    stop();
}


void DProcess::slotProcStdout()
{
    QProcess *process = static_cast<QProcess*>(sender());

    QByteArray data = process->readAllStandardOutput();
#ifdef Q_OS_WIN
    QString msg = QTextCodec::codecForName("GBK")->toUnicode(data);
#elif (defined (Q_OS_MAC) || defined (Q_OS_LINUX))
    QString msg = QTextCodec::codecForName("UTF8")->toUnicode(data);
#endif

    m_stdoutDatas.append(_msgObj(msg));
}

void DProcess::slotProcStderr()
{
    QProcess *process = static_cast<QProcess*>(sender());

    QByteArray data = process->readAllStandardError();
#ifdef Q_OS_WIN
    QString msg = QTextCodec::codecForName("GBK")->toUnicode(data);
#elif (defined (Q_OS_MAC) || defined (Q_OS_LINUX))
    QString msg = QTextCodec::codecForName("UTF8")->toUnicode(data);
#endif

    QJsonArray msgObjs;
    msgObjs.append(_msgObj(msg));

    emit sigNotifyStderr(msgObjs);
}

void DProcess::slotProcStarted()
{

}

void DProcess::slotProcFinish(int code, QProcess::ExitStatus status)
{
    qDebug() << __FUNCTION__ << code << status;
    m_timerStdout->stop();
    if (m_stdoutDatas.size() > 0) {
        emit sigNotifyStdout(m_stdoutDatas);
        m_stdoutDatas = QJsonArray();
    }

    QString msg = QString("Proccess[%1, %2] stoped.\r\n").arg(m_dpid).arg(m_pid);
    qDebug().noquote() << msg;

    QJsonArray msgObjs;
    msgObjs.append(_msgObj(msg));
    emit sigNotifyStdout(msgObjs);

    emit sigFinish();
}

bool DProcess::prepare(const QString &b64Script, const QString &portName, const QString &token, const QString &host)
{
    if (isRunning()) {
        qDebug() << __FUNCTION__ << "process is running";
        return false;
    }

    QByteArray base64Bytes = b64Script.toUtf8();
    QString script =  QString(QByteArray::fromBase64(base64Bytes));

    QString strOldPath = QDir::currentPath();
    QDir::setCurrent(qApp->applicationDirPath());
    qDebug().noquote() << __FUNCTION__ << "temp file location:" << QDir::currentPath();
    if (!m_tempFile->open())  {
        qDebug() << __FUNCTION__ << m_tempFile->error() << m_tempFile->errorString();
        return false;
    }
    m_tempFile->write(script.toUtf8());
    m_tempFile->close();
    QDir::setCurrent(strOldPath);


    QStringList arguments;
    arguments << "-u" << m_tempFile->fileName() << portName << token << host;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

#ifdef Q_OS_MAC
    QString pathPy = qApp->applicationDirPath() + "/../Resources/tool/py38";
#elif (defined (Q_OS_WIN) || defined (Q_OS_LINUX))
    QString pathPy = qApp->applicationDirPath() + "/tool/py38";
#endif

    QString pathCustom = pathPy + "/custom";

#ifdef Q_OS_WIN
    QString cmd = pathPy + "/python.exe";
    env.insert("PATH", qApp->applicationDirPath() + ";" + env.value("PATH"));
    env.insert("PYTHONPATH", pathCustom + ";" + env.value("PYTHONPATH"));
#elif defined (Q_OS_MAC)
    QString cmd = pathPy + "/bin/python";
    env.insert("PATH", qApp->applicationDirPath() + ":" + env.value("PATH"));
    env.insert("PYTHONPATH", pathCustom + ":" + env.value("PYTHONPATH"));
#elif defined (Q_OS_LINUX)
    QString cmd = pathPy + "/python";
    env.insert("PATH", qApp->applicationDirPath() + ":" + env.value("PATH"));
    env.insert("PYTHONPATH", pathCustom + ":" + env.value("PYTHONPATH"));
    env.insert("PYTHONPATH", pathPy + "/Lib/site-packages:" + env.value("PYTHONPATH"));
#endif

    m_proc->setProcessEnvironment(env);
    m_proc->setProgram(cmd);
    m_proc->setArguments(arguments);
    m_proc->setWorkingDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

    return true;
}

bool DProcess::start()
{
    if (isRunning()) {
        qDebug() << __FUNCTION__ << "process is running";
        return true;
    }

    setWaitCmdFinished(false);

    m_pid = 0;
    m_proc->start();
    qDebug().noquote() << QString("new process[%1, %2] starting...").arg(m_dpid).arg(m_proc->processId());
    qDebug().noquote() << "program:" << m_proc->program();
    qDebug().noquote() << "arguments:" << m_proc->arguments();
    qDebug().noquote() << "working directory:" << m_proc->workingDirectory();
    qDebug().noquote() << "environment:" << m_proc->environment();

    bool isSuccess = m_proc->waitForStarted(1000);
    if (isSuccess) {
        m_pid = m_proc->processId();
        QString msg = QString("Script running on proccess[%1, %2]...").arg(m_dpid).arg(m_proc->processId());
        qDebug().noquote() << msg;

        QJsonArray msgObjs;
        msgObjs.append(_msgObj(msg));
        emit sigNotifyStdout(msgObjs);

        m_timerStdout->start();
    } else {
        qDebug().noquote() << m_proc->errorString();
    }

    return isSuccess;
}

bool DProcess::stop()
{
    if (!isRunning()) {
        qDebug() << __FUNCTION__ << "process is not running";
        return true;
    }

    setWaitCmdFinished(true);

    // 针对Win系统kill进程不能触发finish槽的情况
    QEventLoop loop;
    QTimer timerWaitFinish;
    timerWaitFinish.setInterval(200);
    connect(&timerWaitFinish, &QTimer::timeout, [&]() {
        slotProcFinish(0, QProcess::NormalExit);
        loop.quit();
    });
    connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, [&](){
        timerWaitFinish.stop();
        loop.quit();
    });

    QProcess p;
    QString cmd;
    QStringList arguments;
#ifdef Q_OS_WIN
    cmd = "taskkill";
    arguments << "/T" << "/F" << "/PID" << QString::number(m_proc->processId());
#elif (defined (Q_OS_MAC) || defined (Q_OS_LINUX))
    cmd = "kill";
    arguments << "-9" << QString::number(m_proc->processId());
# else
    return false;
#endif

    qDebug() << __FUNCTION__ << cmd << arguments;

    p.execute(cmd, arguments);
    p.close();

    loop.exec();
    return true;
}

void DProcess::stopProcess()
{
    if (m_proc)
    {
        m_proc->kill();
        m_proc->terminate();
    }
}

void DProcess::forceStopProcessAll()
{

#ifdef Q_OS_WIN
        QProcess p;
        QString c = QString("taskkill /im python.exe /f /t");
        p.execute(c);
        p.waitForFinished();
        p.close();
#elif defined (Q_OS_MAC)
        QProcess p;
        QString c = QString("pkill -f python");
        p.execute(c);
        p.waitForFinished();
        p.close();
#endif
}

bool DProcess::write(const QString &cmd)
{
    if (!isRunning()) {
        qDebug() << __FUNCTION__ << "process is not running";
        return false;
    }

    qint64 flag = m_proc->write(cmd.toLocal8Bit());
    return flag != -1;
}
