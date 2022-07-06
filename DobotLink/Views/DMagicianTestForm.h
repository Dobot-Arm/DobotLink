#ifndef DMAGICIANTESTFORM_H
#define DMAGICIANTESTFORM_H

#include <QWidget>
#include <QJsonObject>
#include <QButtonGroup>
#include <QMap>

#include "DDownloadDialog.h"
#include "DSendUserPacketDialog.h"

namespace Ui {
class DMagicianTestForm;
}

class QWebSocket;
class QTcpSocket;
class DMagicianTestForm : public QWidget
{
    Q_OBJECT

public:
    explicit DMagicianTestForm(QWidget *parent = nullptr);
    ~DMagicianTestForm();

private:
    enum DeviceType {
        DEVICE_TYPE_MAGICIAN,
        DEVICE_TYPE_MAGICIANLITE,
        DEVICE_TYPE_MAGICBOX,
        DEVICE_TYPE_M1
    } m_deviceType;

    enum EndEffectorType {
        END_TYPE_CUSTOM,
        END_TYPE_SUCTIONCUP,
        END_TYPE_GRIPPER,
        END_TYPE_LASER,
        END_TYPE_PEN
    };

    Ui::DMagicianTestForm *ui;

    DDownloadDialog *m_downloadDialog;
    DSendUserPacketDialog *m_sendUserPacketDialog;

    QWebSocket *m_websocket;

    QButtonGroup *m_jogGroupBtn;
    QButtonGroup *m_coordinateGroupBtn;
    QButtonGroup *m_extensionGroupBtn;

    quint64 m_id;
    QJsonObject m_baseObj;
    QMap<double, QString> m_functionIDMap;
    QString m_portName;
    bool m_isJoint;
    bool isConnected;

    void _controlPanelInit();
    void _groupButtonInit();
    void _setDeviceControlEnable(bool en);
    QString _getRPCMethod(QString method);

    void _sendMessgeObj(QJsonObject &obj);
    void _handleWSMessageReceived(QJsonObject obj);

    void setJOGCmd(int cmd);
    void setEndEffectorParams(EndEffectorType type);
    void setDeviceWithLCmd(bool on, int version = 1);
    void setSuctionCupEnable(bool enable, bool on);
    void setGripperEnable(bool enable, bool on);

    /* handle websocket reply message */
    void _handleSearchCmd(QJsonArray resultArr);
    void _handleGetPoseCmd(QJsonObject resultObj);
    void _handleDobotConnectCmd();
    void _handleDobotDisConnectCmd();
    void _handleGetAlarmCmd(QJsonObject resultObj);
    void _handleDownloadCmd();

    int GetfixJointCmd(int cmd);

private slots:
    /* panel jog button slot */
    void panelBtnPressed_slot(int id);
    void panelBtnReleased_slot(int id);

    /* radiobox change slot */
    void changeCoordinates_slot(int id);

    /* extension function slot */
    void extensionGroupBtnClicked_slot(int id);
    void extensionGroupBtnPressed_slot(int id);
    void extensionGroupBtnReleased_slot(int id);

    /* websocket slot */
    void onConnected_slot();
    void onMessageReceive_slot(QString message);

    /* button slot */
    void on_btn_wsConnect_clicked();
    void on_btn_search_clicked();
    void on_btn_devConnect_clicked();
    void on_btn_home_clicked();
    void on_btn_sendPacket_clicked();
    void on_btn_userSendPacket_clicked();
    void on_btn_getpose_clicked();
    void on_btn_statusUpdate_clicked();
    void on_btn_statusReset_clicked();

    /* checkbox slot */
    void on_comboBox_SuctionCup_stateChanged(int arg1);
    void on_comboBox_Gripper_stateChanged(int arg1);
    void on_comboBox_SlidingRail_stateChanged(int arg1);
    void on_comboBox_functionName_currentTextChanged(const QString &arg1);
    void on_comboBox_devType_currentIndexChanged(const QString &arg1);

    /* download slot */
    void downloadCode_slot(QString code);

    /* send user defined message */
    void sendUserMessage_slot(QString text);

    /* Switch language */
    void SwitchLanguage_slot(QString language);
};

#endif // DMAGICIANTESTFORM_H
