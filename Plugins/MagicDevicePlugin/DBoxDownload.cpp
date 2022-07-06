#include "DBoxDownload.h"
#include <QStandardPaths>
#include <QDir>
#include <QProcess>
#include <QDebug>
#ifdef Q_OS_WIN
#include <fileapi.h>
#elif defined (Q_OS_MAC)

#endif

DBoxDownload::DBoxDownload(QObject* par) : QThread(par)
{
    m_isDownloading = false;
}

DBoxDownload::~DBoxDownload()
{
    qDebug() << __FUNCTION__;
}

bool DBoxDownload::searchDeviceDrive()
{
#ifdef Q_OS_WIN
    enum DRIVE_TYPE {
        UNKNOWN,            // 无法检测驱动器的类型
        ROOT_INVALID,       // 根目录无效
        DRIVE_REMOVEABLE,   // 可移动驱动器
        DRIVE_FIXED,        // 不可移动驱动器
        DRIVE_REMOTE,       // 网络驱动器
        DRIVE_CDROM,        // 光驱
        DRIVE_RAMDISK       // 虚拟驱动器
    };

    foreach (const QFileInfo &fileInfo, QDir::drives()) {
        QString driveName = fileInfo.absolutePath();
        qDebug() << "driveName" << driveName;
        LPCSTR driveName_char = reinterpret_cast<LPCSTR>(driveName.toLatin1().data());

        quint32 driveType = GetDriveTypeA(driveName_char);
        if (driveType == DRIVE_REMOVEABLE) {
            DWORD lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters;
            GetDiskFreeSpaceA(driveName_char,
                              reinterpret_cast<LPDWORD>(&lpSectorsPerCluster),
                              reinterpret_cast<LPDWORD>(&lpBytesPerSector),
                              reinterpret_cast<LPDWORD>(&lpNumberOfFreeClusters),
                              reinterpret_cast<LPDWORD>(&lpTotalNumberOfClusters));

            qDebug() << driveName << "drive type:" << driveType
                     << ", Sectors Per Cluster:" << lpSectorsPerCluster
                     << ", Bytes Per Sector:" << lpBytesPerSector
                     << ", Number Of Free Clusters:" << lpNumberOfFreeClusters
                     << ", Total Number Of Clusters:" << lpTotalNumberOfClusters;

            if (lpTotalNumberOfClusters < 500) {
                qDebug() << "Find MagicBox drive:" << driveName;

                m_boxDir.setPath(driveName);
                if (m_boxDir.exists("Script") && m_boxDir.exists("Draw")) {
                    qDebug() << __FUNCTION__ << "can found Script or Draw dir.";
                    return true;
                }
            }
        }
    }
#elif defined (Q_OS_MAC)
    QProcess process;

    process.start("diskutil list");
    process.waitForFinished();

    QString processRes;
    processRes.prepend(process.readAllStandardOutput());

    QStringList devList = processRes.split("/dev/disk");
    for (int i=0; i<devList.size(); i++)
    {
        qDebug() << devList.at(i);
        if (devList.at(i).contains("(external, physical):"))
        {
            QStringList devInfoList = devList.at(i).split(" ");
            qDebug() << devInfoList;
            for (int j=0; j<devInfoList.size(); j++)
            {
                qDebug() << devInfoList.at(j);
                if (!devInfoList.at(j).isEmpty())
                {
                    m_boxDir.setPath("/Volumes/" + devInfoList.at(j));
                    qDebug() << m_boxDir;
                    if (m_boxDir.exists("Script") && m_boxDir.exists("Draw"))
                    {
                        qDebug() << __FUNCTION__ << "can found Script or Draw dir.";
                        return true;
                    }
                }
            }
        }
    }
#endif

    // 补丁：有些情况下，BOX识别为“NO NAME”
    m_boxDir.setPath("/Volumes/NO NAME");
    if (m_boxDir.exists("Script") && m_boxDir.exists("Draw"))
    {
        return true;
    }

    // 补丁：有些情况下，BOX识别出带有特殊字符的卷标
    m_boxDir.setPath("/Volumes/PYBFLASH");
    if (m_boxDir.exists("Script") && m_boxDir.exists("Draw"))
    {
        return true;
    }

    qDebug() << __FUNCTION__ << "can not found box dir.";
    return false;
}

void DBoxDownload::setDownloadParams(QString fileName, QString code, quint64 id)
{
    m_fileName = fileName;
    m_code = code;
    m_id = id;
}

void DBoxDownload::run(){
    if (m_isDownloading == true) {
        qDebug() << "box is downloading, please try again later.";
        return;
    }

    m_handleID = m_id;

    if (searchDeviceDrive() == true) {
        m_boxPyFile.setFileName(m_boxDir.absoluteFilePath(m_fileName));

        m_isDownloading = true;

        qDebug() << __FUNCTION__ << m_boxPyFile;
        if (m_boxPyFile.open(QIODevice::WriteOnly |QIODevice::Text | QIODevice::Truncate)){
            QTextStream out(&m_boxPyFile);
            out.setGenerateByteOrderMark(false);
            out.setCodec("UTF-8");
            out << m_code;
            out.flush();

            m_boxPyFile.close();
            emit onDownloadFinished_signal(m_handleID, true);
        } else {
            qDebug() << __FUNCTION__ << m_boxPyFile.error() << m_boxPyFile.errorString();
            emit onDownloadFinished_signal(m_handleID, false);
        }
    } else {
        qDebug() << __FUNCTION__ << "can not found device";
        emit onDownloadFinished_signal(m_handleID, false);
    }
    m_isDownloading = false;
}
