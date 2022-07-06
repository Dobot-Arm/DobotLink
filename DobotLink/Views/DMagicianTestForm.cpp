#include "DMagicianTestForm.h"
#include "ui_DMagicianTestForm.h"

#include <QWebSocket>
#include <QTcpSocket>
#include <QPixmap>
#include <QBitmap>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QTranslator>
#include "MessageCenter/DMessageCenter.h"

DMagicianTestForm::DMagicianTestForm(QWidget *parent) : QWidget(parent), ui(new Ui::DMagicianTestForm)
{
    ui->setupUi(this);

    m_id = 0;
    isConnected = false;
    m_isJoint = false;
    m_id = 0;
    m_baseObj.insert("jsonrpc", "2.0");

    ui->groupBox_device->setEnabled(false);
    _setDeviceControlEnable(false);
    ui->label_rpcMethod->setText(QString("method : dobotlink.%1.")
                                 .arg(ui->comboBox_devType->currentText()));

    _controlPanelInit();
    _groupButtonInit();

    m_websocket = new QWebSocket();
    m_websocket->setParent(this);
    connect(m_websocket, &QWebSocket::connected,
            this, &DMagicianTestForm::onConnected_slot);
    connect(m_websocket, &QWebSocket::textMessageReceived,
            this, &DMagicianTestForm::onMessageReceive_slot);

    m_downloadDialog = new DDownloadDialog(this);
    connect(m_downloadDialog, &DDownloadDialog::downloadCode_signal,
            this, &DMagicianTestForm::downloadCode_slot);

    m_sendUserPacketDialog = new DSendUserPacketDialog(this);
    connect(m_sendUserPacketDialog, &DSendUserPacketDialog::sendUserMessage_signal,
            this, &DMagicianTestForm::sendUserMessage_slot);

    DMessageCenter *msgCenter = DMessageCenter::getInstance();
    connect(msgCenter, &DMessageCenter::SwitchLanguage_signal,
            this, &DMagicianTestForm::SwitchLanguage_slot);

}

DMagicianTestForm::~DMagicianTestForm()
{
    m_websocket->abort();
    delete ui;
}

void DMagicianTestForm::_controlPanelInit()
{
    QPixmap maskUp("://images/panel/mask_up.png");
    ui->btn_y_up->setMask(maskUp.mask());

    QPixmap maskDown("://images/panel/mask_down.png");
    ui->btn_y_down->setMask(maskDown.mask());

    QPixmap maskLeft("://images/panel/mask_left.png");
    ui->btn_x_left->setMask(maskLeft.mask());

    QPixmap maskRight("://images/panel/mask_right.png");
    ui->btn_x_right->setMask(maskRight.mask());

    QPixmap maskUp2("://images/panel/mask_up2.png");
    ui->btn_z_up->setMask(maskUp2.mask());

    QPixmap maskDown2("://images/panel/mask_down2.png");
    ui->btn_z_down->setMask(maskDown2.mask());
}

void DMagicianTestForm::_groupButtonInit()
{
    m_jogGroupBtn = new QButtonGroup(this);
    m_jogGroupBtn->addButton(ui->btn_x_left, 1);
    m_jogGroupBtn->addButton(ui->btn_x_right, 2);
    m_jogGroupBtn->addButton(ui->btn_y_up, 3);
    m_jogGroupBtn->addButton(ui->btn_y_down, 4);
    m_jogGroupBtn->addButton(ui->btn_z_up, 5);
    m_jogGroupBtn->addButton(ui->btn_z_down, 6);
    m_jogGroupBtn->addButton(ui->btn_r0, 7);
    m_jogGroupBtn->addButton(ui->btn_r1, 8);
    connect(m_jogGroupBtn, QOverload<int>::of(&QButtonGroup::buttonPressed),
            this, &DMagicianTestForm::panelBtnPressed_slot);
    connect(m_jogGroupBtn, QOverload<int>::of(&QButtonGroup::buttonReleased),
            this, &DMagicianTestForm::panelBtnReleased_slot);

    m_coordinateGroupBtn = new QButtonGroup(this);
    m_coordinateGroupBtn->addButton(ui->rBtn_cartesian, 1);
    m_coordinateGroupBtn->addButton(ui->rBtn_joint, 2);
    connect(m_coordinateGroupBtn, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &DMagicianTestForm::changeCoordinates_slot);

    m_extensionGroupBtn = new QButtonGroup(this);
    m_extensionGroupBtn->addButton(ui->btn_suck_open, 1);
    m_extensionGroupBtn->addButton(ui->btn_suck_close, 2);
    m_extensionGroupBtn->addButton(ui->btn_grip_open, 3);
    m_extensionGroupBtn->addButton(ui->btn_grip_close, 4);
    m_extensionGroupBtn->addButton(ui->btn_L_left, 5);
    m_extensionGroupBtn->addButton(ui->btn_L_right, 6);
    m_extensionGroupBtn->addButton(ui->btn_download, 10);
    connect(m_extensionGroupBtn, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &DMagicianTestForm::extensionGroupBtnClicked_slot);
    connect(m_extensionGroupBtn, QOverload<int>::of(&QButtonGroup::buttonPressed),
            this, &DMagicianTestForm::extensionGroupBtnPressed_slot);
    connect(m_extensionGroupBtn, QOverload<int>::of(&QButtonGroup::buttonReleased),
            this, &DMagicianTestForm::extensionGroupBtnReleased_slot);
}

void DMagicianTestForm::_sendMessgeObj(QJsonObject &obj)
{
    QJsonDocument document;
    document.setObject(obj);

    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    if (m_websocket->isValid()) {
        m_websocket->sendTextMessage(QString(byteArray));
        m_id++;
        ui->label_id->setText(QString("id: %1").arg(m_id));
    }
}

/* 设置功能区使能 */
void DMagicianTestForm::_setDeviceControlEnable(bool en)
{
    if (en == false) {
        ui->groupBox_pose->setEnabled(false);
        ui->groupBox_extend->setEnabled(false);
        ui->groupBox_packet->setEnabled(false);
        ui->groupBox_jog->setEnabled(false);
        ui->groupBox_state->setEnabled(false);

        ui->widget_jogPanel->setEnabled(false);
        ui->widget_coordinate->setEnabled(false);
        ui->btn_r0->setEnabled(false);
        ui->btn_r1->setEnabled(false);
        ui->widget_suctioncup->setEnabled(false);
        ui->widget_gripper->setEnabled(false);
        ui->widget_download->setEnabled(false);
    } else if (isConnected) {
        ui->groupBox_extend->setEnabled(true);
        ui->groupBox_packet->setEnabled(true);
        ui->groupBox_jog->setEnabled(true);
        ui->groupBox_state->setEnabled(true);

        if (m_deviceType == DEVICE_TYPE_MAGICBOX) {
            ui->widget_jogPanel->setEnabled(false);
            ui->widget_coordinate->setEnabled(false);
            ui->btn_r0->setEnabled(false);
            ui->btn_r1->setEnabled(false);
            ui->groupBox_pose->setEnabled(false);
            ui->widget_suctioncup->setEnabled(false);
            ui->widget_gripper->setEnabled(false);
            ui->groupBox_extend->setEnabled(true);
            ui->widget_download->setEnabled(true);
        } else if (m_deviceType == DEVICE_TYPE_M1) {
            ui->widget_jogPanel->setEnabled(true);
            ui->widget_coordinate->setEnabled(true);
            ui->btn_r0->setEnabled(false);
            ui->btn_r1->setEnabled(false);
            ui->groupBox_pose->setEnabled(true);
            ui->widget_suctioncup->setEnabled(false);
            ui->widget_gripper->setEnabled(false);
            ui->groupBox_extend->setEnabled(false);
            ui->widget_download->setEnabled(false);
        } else {
            ui->widget_jogPanel->setEnabled(true);
            ui->widget_coordinate->setEnabled(true);
            ui->btn_r0->setEnabled(true);
            ui->btn_r1->setEnabled(true);
            ui->groupBox_pose->setEnabled(true);
            ui->widget_suctioncup->setEnabled(true);
            ui->widget_gripper->setEnabled(true);
            ui->groupBox_extend->setEnabled(true);
            ui->widget_download->setEnabled(false);
        }
    }
}

QString DMagicianTestForm::_getRPCMethod(QString method)
{
    QString res = QString("dobotlink.%1.%2").arg(ui->comboBox_devType->currentText()).arg(method);
    return res;
}

/* 中英文切换 */
void DMagicianTestForm::SwitchLanguage_slot(QString language)
{
    QTranslator translator;
    if (language == "en") {
        if (translator.load(":/dobotlink_en.qm")) {
            bool res = qApp->installTranslator(&translator);
            qDebug() << "qApp->installTranslator(en) true";
        } else {
            qDebug() << "translator.load(en) false";
        }
    } else if (language == "ch") {
        if (translator.load(":/dobotlink.qm")) {
            bool res = qApp->installTranslator(&translator);
            qDebug() << "qApp->installTranslator(ch)" << res;
        } else {
            qDebug() << "translator.load(ch) false";
        }
    }
    ui->retranslateUi(this);
}

/* cmd id 1~3, 2~4 互换 */
int DMagicianTestForm::GetfixJointCmd(int cmd) {
    if (cmd ==1 || cmd == 2){
        return (cmd + 2);
    } else {
        return (cmd - 2);
    }
}

/* 点动控制按钮按下 */
void DMagicianTestForm::panelBtnPressed_slot(int id)
{
    if (id >=1 && id <=4 && m_isJoint == true) {
        int cmd = GetfixJointCmd(id);
        setJOGCmd(cmd);
    } else {
        setJOGCmd(id);
    }
}

/* 点动控制按钮松开 */
void DMagicianTestForm::panelBtnReleased_slot(int id)
{
    Q_UNUSED(id)

    setJOGCmd(0);
}

void DMagicianTestForm::setJOGCmd(int cmd)
{
    m_functionIDMap.insert(m_id, "SetJOGCmd");

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod("SetJOGCmd"));

    QJsonObject paramsObj;
    paramsObj.insert("portName", m_portName);
    paramsObj.insert("cmd", cmd);
    if (cmd == 9 or cmd == 10) {
        //滑轨必须把isJonint设置为true
        paramsObj.insert("isJoint", true);
    } else {
        paramsObj.insert("isJoint", m_isJoint ? true : false);
    }
    obj.insert("params", paramsObj);

    _sendMessgeObj(obj);
}

/* 切换坐标系 */
void DMagicianTestForm::changeCoordinates_slot(int id)
{
    QString style;

    if (id == 1) {
        m_isJoint = false;
        style = QString("QPushButton{border-image: url(://images//panel/panel_normal.png);}" \
                        "QPushButton:hover{border-image: url(://images//panel/panel_hover.png)}" \
                        "QPushButton:pressed{border-image: url(://images//panel/panel_pressed.png)}" \
                        "QPushButton:disabled{border-image: url(://images/panel/panel_disabled.png)}");
    } else if (id == 2) {
        m_isJoint = true;
        style = QString("QPushButton{border-image: url(://images//panel/panel_normal_J.png);}" \
                        "QPushButton:hover{border-image: url(://images//panel/panel_hover_J.png)}" \
                        "QPushButton:pressed{border-image: url(://images//panel/panel_pressed_J.png)}" \
                        "QPushButton:disabled{border-image: url(://images/panel/panel_disabled_J.png)}");
    }

    if (!style.isEmpty()) {
        ui->btn_y_up->setStyleSheet(style);
        ui->btn_y_down->setStyleSheet(style);
        ui->btn_x_left->setStyleSheet(style);
        ui->btn_x_right->setStyleSheet(style);
        ui->btn_z_up->setStyleSheet(style);
        ui->btn_z_down->setStyleSheet(style);
    }
}

void DMagicianTestForm::extensionGroupBtnClicked_slot(int id)
{
    if (id == 1) {
        setSuctionCupEnable(true, true);
    } else if (id == 2) {
        setSuctionCupEnable(true, false);
    } else if (id == 3) {
        setGripperEnable(true, false);
    } else if (id == 4) {
        setGripperEnable(true, true);
    } else if (id == 10) {
        m_downloadDialog->show();
    }
}

void DMagicianTestForm::extensionGroupBtnPressed_slot(int id)
{
    int cmd = 0;

    if (id == 5) {
        cmd = 9;
    } else if (id == 6) {
        cmd = 10;
    } else {
        return;
    }

    m_functionIDMap.insert(m_id, "SetJOGCmd");

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod("SetJOGCmd"));

    QJsonObject paramsObj;
    paramsObj.insert("portName", m_portName);
    paramsObj.insert("isJoint", true);
    paramsObj.insert("cmd", cmd);
    obj.insert("params", paramsObj);

    _sendMessgeObj(obj);
}

void DMagicianTestForm::extensionGroupBtnReleased_slot(int id)
{
    if (id == 5 || id == 6) {
        setJOGCmd(0);
    }
}

void DMagicianTestForm::on_btn_wsConnect_clicked()
{
    if (ui->btn_wsConnect->text() == tr("connect")) {
        /* 已连接 */
        QString urlStr = QString("%1:%2")
                .arg(ui->lineEdit_ws_address->text())
                .arg(ui->lineEdit_ws_port->text());
        m_websocket->open(QUrl(urlStr));

        ui->groupBox_device->setEnabled(true);
    } else {
        /* 断开连接 */
        m_websocket->close();
        ui->btn_wsConnect->setText(tr("connect"));

        ui->groupBox_device->setEnabled(false);
        _setDeviceControlEnable(false);

        if (ui->btn_devConnect->text() == tr("disConnect")) {
            ui->btn_devConnect->setText(tr("connect"));
        }
    }
}

void DMagicianTestForm::on_btn_search_clicked()
{
    QJsonObject obj(m_baseObj);

    m_functionIDMap.insert(m_id, "SearchDobot");
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod("SearchDobot"));

    QJsonObject paramsObj;
    paramsObj.insert("connectCheck", ui->checkBox_connectCheck->isChecked());
    if (!ui->lineEdit_filter->text().isEmpty()) {
        paramsObj.insert("filter", ui->lineEdit_filter->text());
    }

    obj.insert("params", paramsObj);
    _sendMessgeObj(obj);
}

void DMagicianTestForm::on_btn_devConnect_clicked()
{
    QJsonObject obj(m_baseObj);
    QJsonObject paramsObj;
    obj.insert("id", static_cast<double>(m_id));
    qDebug() << __FUNCTION__ << ui->btn_devConnect->text();
    if (ui->btn_devConnect->text() == tr("connect")) {
        QStringList list = ui->comboBox_port->currentText().split(",", QString::SkipEmptyParts);
        if (list.isEmpty()) {
            qDebug() << tr("please choose a available device.");
            return;
        }
//        m_portName = ui->comboBox_port->currentData().toString();
        m_portName = list.first();
        if (m_portName.isEmpty()) {
            m_portName = ui->comboBox_port->currentData().toString();
        }

        m_functionIDMap.insert(m_id, "ConnectDobot");
        obj.insert("method", _getRPCMethod("ConnectDobot"));
        ui->lineEdit_value1->setText(m_portName);
    } else if (ui->btn_devConnect->text() == tr("disConnect")) {

        m_functionIDMap.insert(m_id, "DisconnectDobot");
        obj.insert("method", _getRPCMethod("DisconnectDobot"));
    }

    paramsObj.insert("portName", m_portName);
    obj.insert("params", paramsObj);

    _sendMessgeObj(obj);
}

void DMagicianTestForm::on_btn_home_clicked()
{
    if (m_deviceType == DEVICE_TYPE_M1) {
        m_functionIDMap.insert(m_id, "SetHOMEWithSwitch");
    } else {
        m_functionIDMap.insert(m_id, "SetHOMECmd");
    }

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod("SetHOMECmd"));

    QJsonObject paramsObj;
    paramsObj.insert("portName", m_portName);
    obj.insert("params", paramsObj);

    _sendMessgeObj(obj);
}

/* 发送自定义命令 */
void DMagicianTestForm::on_btn_sendPacket_clicked()
{
    QString functionStr = ui->comboBox_functionName->currentText();

    m_functionIDMap.insert(m_id, functionStr);

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod(functionStr));

    QJsonObject paramsObj;
    if (!ui->lineEdit_key1->text().isEmpty() && !ui->lineEdit_value1->text().isEmpty()) {
        if (ui->rBtn_string1->isChecked()) {
            paramsObj.insert(ui->lineEdit_key1->text(), ui->lineEdit_value1->text());
        } else if (ui->rBtn_number1->isChecked()) {
            paramsObj.insert(ui->lineEdit_key1->text(), ui->lineEdit_value1->text().toDouble());
        } else if (ui->rBtn_bool1->isChecked()) {
            if (ui->lineEdit_value1->text().contains("true", Qt::CaseInsensitive)) {
                paramsObj.insert(ui->lineEdit_key1->text(), true);
            } else {
                paramsObj.insert(ui->lineEdit_key1->text(), false);
            }
        }
    }
    if (!ui->lineEdit_key2->text().isEmpty() && !ui->lineEdit_value2->text().isEmpty()) {
        if (ui->rBtn_string2->isChecked()) {
            paramsObj.insert(ui->lineEdit_key2->text(), ui->lineEdit_value2->text());
        } else if (ui->rBtn_number2->isChecked()) {
            paramsObj.insert(ui->lineEdit_key2->text(), ui->lineEdit_value2->text().toDouble());
        } else if (ui->rBtn_bool2->isChecked()) {
            if (ui->lineEdit_value2->text().contains("true", Qt::CaseInsensitive)) {
                paramsObj.insert(ui->lineEdit_key2->text(), true);
            } else {
                paramsObj.insert(ui->lineEdit_key2->text(), false);
            }
        }
    }
    if (!ui->lineEdit_key3->text().isEmpty() && !ui->lineEdit_value3->text().isEmpty()) {
        if (ui->rBtn_string3->isChecked()) {
            paramsObj.insert(ui->lineEdit_key3->text(), ui->lineEdit_value3->text());
        } else if (ui->rBtn_number3->isChecked()) {
            paramsObj.insert(ui->lineEdit_key3->text(), ui->lineEdit_value3->text().toDouble());
        } else if (ui->rBtn_bool3->isChecked()) {
            if (ui->lineEdit_value3->text().contains("true", Qt::CaseInsensitive)) {
                paramsObj.insert(ui->lineEdit_key3->text(), true);
            } else {
                paramsObj.insert(ui->lineEdit_key3->text(), false);
            }
        }
    }
    if (!ui->lineEdit_key4->text().isEmpty() && !ui->lineEdit_value4->text().isEmpty()) {
        if (ui->rBtn_string4->isChecked()) {
            paramsObj.insert(ui->lineEdit_key4->text(), ui->lineEdit_value4->text());
        } else if (ui->rBtn_number4->isChecked()) {
            paramsObj.insert(ui->lineEdit_key4->text(), ui->lineEdit_value4->text().toDouble());
        } else if (ui->rBtn_bool4->isChecked()) {
            if (ui->lineEdit_value4->text().contains("true", Qt::CaseInsensitive)) {
                paramsObj.insert(ui->lineEdit_key4->text(), true);
            } else {
                paramsObj.insert(ui->lineEdit_key4->text(), false);
            }
        }
    }
    if (!ui->lineEdit_key5->text().isEmpty() && !ui->lineEdit_value5->text().isEmpty()) {
        if (ui->rBtn_string5->isChecked()) {
            paramsObj.insert(ui->lineEdit_key5->text(), ui->lineEdit_value5->text());
        } else if (ui->rBtn_number5->isChecked()) {
            paramsObj.insert(ui->lineEdit_key5->text(), ui->lineEdit_value5->text().toDouble());
        } else if (ui->rBtn_bool5->isChecked()) {
            if (ui->lineEdit_value5->text().contains("true", Qt::CaseInsensitive)) {
                paramsObj.insert(ui->lineEdit_key5->text(), true);
            } else {
                paramsObj.insert(ui->lineEdit_key5->text(), false);
            }
        }
    }
    if (!ui->lineEdit_key6->text().isEmpty() && !ui->lineEdit_value6->text().isEmpty()) {
        if (ui->rBtn_string6->isChecked()) {
            paramsObj.insert(ui->lineEdit_key6->text(), ui->lineEdit_value6->text());
        } else if (ui->rBtn_number6->isChecked()) {
            paramsObj.insert(ui->lineEdit_key6->text(), ui->lineEdit_value6->text().toDouble());
        } else if (ui->rBtn_bool6->isChecked()) {
            if (ui->lineEdit_value6->text().contains("true", Qt::CaseInsensitive)) {
                paramsObj.insert(ui->lineEdit_key6->text(), true);
            } else {
                paramsObj.insert(ui->lineEdit_key6->text(), false);
            }
        }
    }

    obj.insert("params", paramsObj);

    _sendMessgeObj(obj);
}

void DMagicianTestForm::on_btn_userSendPacket_clicked()
{
    m_sendUserPacketDialog->show();
}

void DMagicianTestForm::on_comboBox_SuctionCup_stateChanged(int arg1)
{
    if (arg1 == 2) {
        setEndEffectorParams(END_TYPE_SUCTIONCUP);
        setSuctionCupEnable(true, false);

        if (ui->comboBox_Gripper->checkState() == Qt::Checked) {
            ui->comboBox_Gripper->setChecked(false);
        }
    } else if (arg1 == 0) {
        setSuctionCupEnable(false, false);
        setEndEffectorParams(END_TYPE_CUSTOM);
    }
}

void DMagicianTestForm::on_comboBox_Gripper_stateChanged(int arg1)
{
    if (arg1 == 2) {
        setEndEffectorParams(END_TYPE_GRIPPER);
        setGripperEnable(true, false);

        if (ui->comboBox_SuctionCup->checkState() == Qt::Checked) {
            ui->comboBox_SuctionCup->setChecked(false);
        }
    } else if (arg1 == 0) {
        setGripperEnable(false, false);
        setEndEffectorParams(END_TYPE_CUSTOM);
    }
}

void DMagicianTestForm::on_comboBox_SlidingRail_stateChanged(int arg1)
{
    if (arg1 == 2) {
        setDeviceWithLCmd(true);
    } else if (arg1 == 0) {
        setDeviceWithLCmd(false);
    }
}

void DMagicianTestForm::setEndEffectorParams(EndEffectorType type)
{
    double offset_x = 0;

    switch (type) {
    case END_TYPE_CUSTOM:
        offset_x = 0;
        break;
    case END_TYPE_SUCTIONCUP:
        offset_x = 59.7;
        break;
    case END_TYPE_GRIPPER:
        offset_x = 59.7;
        break;
    case END_TYPE_LASER:
        offset_x = 70;
        break;
    case END_TYPE_PEN:
        offset_x = 61;
        break;
    }

    m_functionIDMap.insert(m_id, "SetEndEffectorParams");

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod("SetEndEffectorParams"));

    QJsonObject paramsObj;
    paramsObj.insert("portName", m_portName);
    paramsObj.insert("xOffset", offset_x);
    paramsObj.insert("yOffset", 0);
    paramsObj.insert("zOffset", 0);

    obj.insert("params", paramsObj);

    _sendMessgeObj(obj);
}

void DMagicianTestForm::setDeviceWithLCmd(bool on, int version)
{
    m_functionIDMap.insert(m_id, "SetDeviceWithL");

    if (version > 1) {
        version = 1;
    }

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod("SetDeviceWithL"));

    QJsonObject paramsObj;
    paramsObj.insert("portName", m_portName);
    paramsObj.insert("version", version);     /* 0 or 1 */
    paramsObj.insert("enable", on);

    obj.insert("params", paramsObj);

    _sendMessgeObj(obj);
}

void DMagicianTestForm::setSuctionCupEnable(bool enable, bool on)
{
    m_functionIDMap.insert(m_id, "SetEndEffectorSuctionCup");

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod("SetEndEffectorSuctionCup"));

    QJsonObject paramsObj;
    paramsObj.insert("portName", m_portName);
    paramsObj.insert("enable", enable);
    paramsObj.insert("on", on);

    obj.insert("params", paramsObj);
    _sendMessgeObj(obj);
}

void DMagicianTestForm::setGripperEnable(bool enable, bool on)
{
    m_functionIDMap.insert(m_id, "SetEndEffectorGripper");

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod("SetEndEffectorGripper"));

    QJsonObject paramsObj;
    paramsObj.insert("portName", m_portName);
    paramsObj.insert("enable", enable);
    paramsObj.insert("on", on);

    obj.insert("params", paramsObj);
    _sendMessgeObj(obj);
}

void DMagicianTestForm::on_btn_getpose_clicked()
{
    m_functionIDMap.insert(m_id, "GetPose");

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod("GetPose"));

    QJsonObject paramsObj;
    paramsObj.insert("portName", m_portName);
    obj.insert("params", paramsObj);

    _sendMessgeObj(obj);
}

void DMagicianTestForm::on_btn_statusUpdate_clicked()
{
    ui->lineEdit_status->clear();

    m_functionIDMap.insert(m_id, "GetAlarmsState");

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod("GetAlarmsState"));

    QJsonObject paramsObj;
    paramsObj.insert("portName", m_portName);
    obj.insert("params", paramsObj);

    _sendMessgeObj(obj);
}

void DMagicianTestForm::on_btn_statusReset_clicked()
{
    m_functionIDMap.insert(m_id, "ClearAllAlarmsState");

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod("ClearAllAlarmsState"));

    QJsonObject paramsObj;
    paramsObj.insert("portName", m_portName);
    obj.insert("params", paramsObj);

    _sendMessgeObj(obj);
}

/* WebSocket slot */
void DMagicianTestForm::onConnected_slot()
{
    ui->btn_wsConnect->setText(tr("close"));
    ui->groupBox_device->setEnabled(true);
}

void DMagicianTestForm::onMessageReceive_slot(QString message)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "DMagicianTestForm: Json Parse Error" << jsonError.errorString();
        return;
    }

    if (doc.isObject()) {
        QJsonObject obj = doc.object();

        /* GET MESSAGE */
        _handleWSMessageReceived(obj);
    }
}

/* 处理消息 */
void DMagicianTestForm::_handleWSMessageReceived(QJsonObject obj)
{
    /* Notification 断线通知 */
    if (obj.value("method").toString().endsWith("lostConnection")) {
        _handleDobotDisConnectCmd();
        return;
    }

    double resID = obj.value("id").toDouble(-1);
    QString functionName = m_functionIDMap.take(resID);

    if (functionName.isEmpty()) {
        qDebug() << "DMagicianTestForm: can not find function" << obj;
        return;
    }

    if (obj.contains("result")) {
        if (functionName == "SearchDobot") {
            _handleSearchCmd(obj.value("result").toArray());
        } else if (functionName == "GetPose") {
            _handleGetPoseCmd(obj.value("result").toObject());
        } else if (functionName == "ConnectDobot") {
            _handleDobotConnectCmd();
        } else if (functionName == "DisconnectDobot") {
            _handleDobotDisConnectCmd();
        } else if (functionName == "GetAlarmsState") {
            _handleGetAlarmCmd(obj.value("result").toObject());
        } else if (functionName == "DownloadProgram") {
            _handleDownloadCmd();
        }
    } else if (obj.contains("error")) {
        if (functionName == "SearchDobot") {
            ui->comboBox_port->clear();
        }
    }
}

void DMagicianTestForm::_handleSearchCmd(QJsonArray resultArr)
{
    ui->comboBox_port->clear();

    for (int i = 0; i < resultArr.count(); i++) {
        QJsonObject deviceObj = resultArr.at(i).toObject();
        QString portName = deviceObj.value("portName").toString();
        QString status = deviceObj.value("status").toString();
        ui->comboBox_port->addItem(QString("%1, %2").arg(portName).arg(status), portName);
    }
}

void DMagicianTestForm::_handleGetPoseCmd(QJsonObject resultObj)
{
    ui->lineEdit_X->setText(QString::number(resultObj.value("x").toDouble()));
    ui->lineEdit_Y->setText(QString::number(resultObj.value("y").toDouble()));
    ui->lineEdit_Z->setText(QString::number(resultObj.value("z").toDouble()));
    ui->lineEdit_R->setText(QString::number(resultObj.value("r").toDouble()));

    if (resultObj.contains("jointAngle")) {
        QJsonArray jointArray = resultObj.value("jointAngle").toArray();
        ui->lineEdit_J1->setText(QString::number(jointArray[0].toDouble()));
        ui->lineEdit_J2->setText(QString::number(jointArray[1].toDouble()));
        ui->lineEdit_J3->setText(QString::number(jointArray[2].toDouble()));
        ui->lineEdit_J4->setText(QString::number(jointArray[3].toDouble()));
    }
}

void DMagicianTestForm::_handleDobotConnectCmd()
{
    isConnected = true;
    ui->btn_devConnect->setText(tr("disConnect"));
    QString text = ui->comboBox_port->currentText().replace("unconnected", "connected");
    ui->comboBox_port->setItemText(ui->comboBox_port->currentIndex(), text);

    _setDeviceControlEnable(true);
}

void DMagicianTestForm::_handleDobotDisConnectCmd()
{
    isConnected = false;
    ui->btn_devConnect->setText(tr("connect"));
    if (!ui->comboBox_port->currentText().contains("unconnected")) {
        QString text = ui->comboBox_port->currentText().replace("connected", "unconnected");
        ui->comboBox_port->setItemText(ui->comboBox_port->currentIndex(), text);
    }
    _setDeviceControlEnable(false);
}

void DMagicianTestForm::_handleGetAlarmCmd(QJsonObject resultObj)
{
    if (resultObj.contains("state")) {
        QString status;
        QJsonArray statusArr = resultObj.value("state").toArray();
        for (int i = 0; i < statusArr.count(); ++i) {
            int s = statusArr.at(i).toInt();
            status.append(QString::number(s));
            status.append(",");
        }
        status.remove(status.length() - 1, 1);
        ui->lineEdit_status->setText(status);
    }
}

void DMagicianTestForm::_handleDownloadCmd()
{
    m_downloadDialog->hide();
}

/* 设备类型 */
void DMagicianTestForm::on_comboBox_devType_currentIndexChanged(const QString &arg1)
{
    ui->label_rpcMethod->setText(QString("method : dobotlink.%1.").arg(arg1));

    if (arg1 == "Magician") {
        m_deviceType = DEVICE_TYPE_MAGICIAN;
    } else if (arg1 == "MagicianLite") {
        m_deviceType = DEVICE_TYPE_MAGICIANLITE;
    } else if (arg1 == "MagicBox") {
        m_deviceType = DEVICE_TYPE_MAGICBOX;
    } else if (arg1 == "M1") {
        m_deviceType = DEVICE_TYPE_M1;
    }

    if (isConnected) {
        if (m_deviceType == DEVICE_TYPE_MAGICBOX) {
            ui->widget_jogPanel->setEnabled(false);
            ui->widget_coordinate->setEnabled(false);
            ui->btn_r0->setEnabled(false);
            ui->btn_r1->setEnabled(false);
            ui->groupBox_pose->setEnabled(false);
            ui->widget_suctioncup->setEnabled(false);
            ui->widget_gripper->setEnabled(false);
            ui->widget_download->setEnabled(true);
        } else {
            ui->widget_jogPanel->setEnabled(true);
            ui->widget_coordinate->setEnabled(true);
            ui->btn_r0->setEnabled(true);
            ui->btn_r1->setEnabled(true);
            ui->groupBox_pose->setEnabled(true);
            ui->widget_suctioncup->setEnabled(true);
            ui->widget_gripper->setEnabled(true);
            ui->widget_download->setEnabled(false);
        }
    }
}

void DMagicianTestForm::downloadCode_slot(QString code)
{
    m_functionIDMap.insert(m_id, "DownloadProgram");

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", _getRPCMethod("DownloadProgram"));

    QJsonObject paramsObj;
    paramsObj.insert("portName", m_portName);
    paramsObj.insert("code", code);

    obj.insert("params", paramsObj);
    _sendMessgeObj(obj);
}

void DMagicianTestForm::sendUserMessage_slot(QString text)
{
    if (m_websocket->isValid()) {
        m_websocket->sendTextMessage(text);
        m_id++;
        ui->label_id->setText(QString("id: %1").arg(m_id));
    }
}


void DMagicianTestForm::on_comboBox_functionName_currentTextChanged(const QString &arg1)
{
    ui->lineEdit_key2->clear();
    ui->lineEdit_key3->clear();
    ui->lineEdit_key4->clear();
    ui->lineEdit_key5->clear();
    ui->lineEdit_key6->clear();

    if (arg1.endsWith(QStringLiteral("UpgradeStatus"))) {
        ui->lineEdit_key2->setText("length");
        ui->lineEdit_key3->setText("md5");
    } else if (arg1 == QLatin1String("SetPTPCmd")) {
        ui->lineEdit_key2->setText("x");
        ui->lineEdit_key3->setText("y");
        ui->lineEdit_key4->setText("z");
        ui->lineEdit_key5->setText("r");
        ui->lineEdit_key6->setText("isQueued");
    }  else if (arg1 == QLatin1String("SetHOMEParams")
                or arg1 == QLatin1String("SetUserCoordinate")
                or arg1 == QLatin1String("SetToolCoordinate")) {
        ui->lineEdit_key2->setText("x");
        ui->lineEdit_key3->setText("y");
        ui->lineEdit_key4->setText("z");
        ui->lineEdit_key5->setText("r");
    } else if (arg1 == QLatin1String("SetDeviceSN")) {
        ui->lineEdit_key2->setText("deviceSN");
    } else if (arg1 == QLatin1String("SetDeviceName")) {
        ui->lineEdit_key2->setText("deviceName");
    } else if (arg1 == QLatin1String("SetDeviceWithL")) {
        ui->lineEdit_key2->setText("enable");
        ui->lineEdit_key3->setText("version");
    } else if (arg1 == QLatin1String("SetArmSpeedRatio")
               or arg1 == QLatin1String("SetLSpeedRatio")) {
        ui->lineEdit_key2->setText("type");
        ui->lineEdit_key3->setText("value");
    } else if (arg1 == QLatin1String("GetArmSpeedRatio")
               or arg1 == QLatin1String("GetLSpeedRatio")) {
        ui->lineEdit_key2->setText("type");
    } else if (arg1 == QLatin1String("SetServoAngle")) {
        ui->lineEdit_key2->setText("index");
        ui->lineEdit_key3->setText("value");
    } else if (arg1 == QLatin1String("ResetPose")) {
        ui->lineEdit_key2->setText("manualEnable");
        ui->lineEdit_key3->setText("frontArmAngle");
        ui->lineEdit_key4->setText("rearArmAngle");
    } else if (arg1 == QLatin1String("SetAutoLeveling")) {
        ui->lineEdit_key2->setText("enable");
        ui->lineEdit_key3->setText("precision");
    } else if (arg1 == QLatin1String("SetEndEffectorParams")) {
        ui->lineEdit_key2->setText("xOffset");
        ui->lineEdit_key3->setText("yOffset");
        ui->lineEdit_key4->setText("zOffset");
    } else if (arg1 == QLatin1String("SetEndEffectorType")) {
        ui->lineEdit_key2->setText("type");
    } else if (arg1 == QLatin1String("SetEndEffectorLaser")
               or arg1 == QLatin1String("SetEndEffectorSuctionCup")
               or arg1 == QLatin1String("SetEndEffectorGripper")) {
        ui->lineEdit_key2->setText("enable");
        ui->lineEdit_key3->setText("on");
    } else if (arg1 == QLatin1String("SetJOGCmd")) {
        ui->lineEdit_key2->setText("isJoint");
        ui->lineEdit_key3->setText("cmd");
    } else if (arg1 == QLatin1String("SetJOGCommonParams")) {
        ui->lineEdit_key2->setText("velocityRatio");
        ui->lineEdit_key3->setText("accelerationRatio");
    } else if (arg1 == QLatin1String("SetPTPJumpParams")) {
        ui->lineEdit_key2->setText("zLimit");
        ui->lineEdit_key3->setText("jumpHeight");
    } else if (arg1 == QLatin1String("SetIOMultiplexing")) {
        ui->lineEdit_key2->setText("port");
        ui->lineEdit_key3->setText("multiplex");
    } else if (arg1 == QLatin1String("SetIODO")) {
        ui->lineEdit_key2->setText("port");
        ui->lineEdit_key3->setText("level");
    } else if (arg1 == QLatin1String("SetIOPWM")) {
        ui->lineEdit_key2->setText("port");
        ui->lineEdit_key3->setText("frequency");
        ui->lineEdit_key4->setText("dutyCycle");
    } else if (arg1 == QLatin1String("SetEMotor")) {
        ui->lineEdit_key2->setText("index");
        ui->lineEdit_key3->setText("enable");
        ui->lineEdit_key4->setText("speed");
    } else if (arg1 == QLatin1String("SetEMotorS")) {
        ui->lineEdit_key2->setText("index");
        ui->lineEdit_key3->setText("enable");
        ui->lineEdit_key4->setText("speed");
        ui->lineEdit_key5->setText("distance");
    } else if (arg1 == QLatin1String("SetColorSensor")
               or arg1 == QLatin1String("SetInfraredSensor")) {
        ui->lineEdit_key2->setText("port");
        ui->lineEdit_key3->setText("enable");
        ui->lineEdit_key4->setText("version");
    } else if (arg1 == QLatin1String("GetInfraredSensor")) {
       ui->lineEdit_key2->setText("port");
    } else if (arg1 == QLatin1String("SetLostStepValue")) {
        ui->lineEdit_key2->setText("value");
    } else if (arg1 == QLatin1String("SetCPCmd")) {
        ui->lineEdit_key2->setText("cpMode");
        ui->lineEdit_key3->setText("x");
        ui->lineEdit_key4->setText("y");
        ui->lineEdit_key5->setText("z");
        ui->lineEdit_key6->setText("power");
    } else if (arg1 == QLatin1String("SetCPParams")) {
        ui->lineEdit_key2->setText("targetAcc");
        ui->lineEdit_key3->setText("junctionVel");
        ui->lineEdit_key4->setText("isRealTimeTrack");
        ui->lineEdit_key5->setText("acc");
    } else if (arg1 == QLatin1String("SetLanPortConfig")) {
        ui->lineEdit_key2->setText("addr");
        ui->lineEdit_key3->setText("mask");
        ui->lineEdit_key4->setText("gateway");
        ui->lineEdit_key5->setText("dns");
        ui->lineEdit_key6->setText("isdhcp");
    } else if (arg1 == QLatin1String("SendGetCmd")) {
        ui->lineEdit_key2->setText("url");
    } else if (arg1 == QLatin1String("ReadFile")) {
        ui->lineEdit_key2->setText("fileName");
    } else if (arg1 == QLatin1String("WriteFile")) {
        ui->lineEdit_key2->setText("url");
        ui->lineEdit_key3->setText("fileName");
        ui->lineEdit_key4->setText("content");
    }
}
