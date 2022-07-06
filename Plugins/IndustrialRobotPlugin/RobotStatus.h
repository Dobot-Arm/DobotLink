#ifndef ROBOTSTATUS_H
#define ROBOTSTATUS_H

#include <QObject>
#include <QJsonArray>
#include <QVector>
#include <QVariant>
#include <QJsonObject>

class RobotStatus
{
public:
    enum ControlMode {DISABLE, ENABLE, DRAG};
    enum CoordinateType {JOINT, CARTESIAN, TOOL};
    enum JogMode {JOG, STEP};
    enum AutoManualMode {AUTO, MANUAL};

    RobotStatus();

    QJsonObject getObjForSend() const;
    void setStatus(QJsonObject obj);

    QVariant getStatus(QString key);

    void setControlMode(ControlMode mode);
    void setControlMode(int mode);
    void setControlMode(QString mode);

    void setCoordinateMode(CoordinateType mode);
    void setCoordinateMode(int mode);
    void setCoordinateMode(QString mode);

    void setJogMode(JogMode mode);
    void setJogMode(int mode);
    void setJogMode(QString mode);

    void setToolCoordPattern(int pattern);
    void setUserCoordPattern(int pattern);

    void setAutoManualMode(AutoManualMode mode);
    void setAutoManualMode(int mode);
    void setAutoManualMode(QString mode);

    void clearAlarms();

    void setOutputs(bool en, QJsonArray outputs);

    void setisCollision(int isCollision);

private:
    QVariantMap m_dataMap;

    QString controlMode;
    QString coordinateMode;
    QString jogMode;
    int toolCoordinate;
    int userCoordinate;
    QString autoManualMode;

    bool alarmClear;
    bool outputEnable;
    bool hardware;

    QJsonArray alarmsArray;
    QJsonArray outputsArray;
    QJsonArray inputsArray;
    QJsonArray extendDOArray;
    QJsonArray extendDIArray;
    QJsonArray jointCoordinateArray;
    QJsonArray cartesianCoordinateArray;

    int isCollision;

    /***************************************/
    bool dragPlayback = false;
    int skinCollison = 0;
    QJsonArray skinValueArray;
    QJsonArray controlParamsArray;
    QJsonArray jointCurrentArray;
    QJsonArray jointVoltageArray;
    QJsonArray jointTempArray;
    QJsonArray endAIArray;
    QJsonArray gpioAIArray;

#if 0
    QVector<quint8> outputs;
    QVector<quint8> inputs;
    QVector<double> jointCoordinate;
    QVector<double> cartesianCoordinate;
#endif

    int armOrientation;

    void _updateStatusMap(QString key = QString());
};

#endif // ROBOTSTATUS_H
