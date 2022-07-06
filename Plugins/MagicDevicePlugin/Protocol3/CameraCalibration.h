#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H

#include <QObject>
#include <QJsonObject>
#include <QMap>
#include <QTimer>
#include <QJsonArray>
#include <QTimer>
#include <QMap>
#include <QProcess>
#include <QDir>

#include "DP3Packet.h"
extern "C" {
#include "DobotV3API.h"
}
class CameraCalibration: public QObject
{
    Q_OBJECT
public:
    static CameraCalibration *instance();

    CameraCalibration();
    virtual ~CameraCalibration();

    static const QString processName;

    void GOManualCalibration(uint32_t seqNum, QString april_list, QString device_list);
    void WriteBoxFile(uint32_t seqNum);

private:
    static CameraCalibration *m_staticCameraCalibration;
    uint32_t m_seqNum;
    QProcess *m_process;
    QDir m_workSpaceDir;
    QString m_processPath;
    QDir logDir;
    QString log_path;
    QByteArray readarray;

    void _processInit();
    void _processSetArgument(QString src, QString dest);
    void _stopProcess();

signals:
    void receiveData_signal(bool isTimeOut, QJsonObject jsonObj, quint32 seqNum);

public slots:
    void onProcessReadyRead_slot();
};

#endif // CAMERACALIBRATION_H
