#include "DArduinoTestForm.h"
#include "ui_DArduinoTestForm.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

DArduinoTestForm::DArduinoTestForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DArduinoTestForm)
{
    ui->setupUi(this);
    ui->groupBox_workspace->setEnabled(false);

    m_id = 0;
    m_baseObj.insert("jsonrpc", "2.0");

    m_websocket = new QWebSocket();
    m_websocket->setParent(this);
    connect(m_websocket, &QWebSocket::connected,
            this, &DArduinoTestForm::onConnected_slot);
    connect(m_websocket, &QWebSocket::textMessageReceived,
            this, &DArduinoTestForm::onMessageReceive_slot);
}

DArduinoTestForm::~DArduinoTestForm()
{
    delete ui;
}

void DArduinoTestForm::_sendMessgeObj(QJsonObject &obj)
{
    QJsonDocument document;
    document.setObject(obj);

    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    if (m_websocket->isValid()) {
        m_websocket->sendTextMessage(QString(byteArray));
        m_id++;
    }
}

void DArduinoTestForm::on_btn_searchPort_clicked()
{
    m_functionIDMap.insert(m_id, "ShowAvailablePorts");

    QJsonObject obj(m_baseObj);
    obj.insert("id", m_id);
    obj.insert("method", "dobotlink.api.ShowAvailablePorts");

    _sendMessgeObj(obj);
}

void DArduinoTestForm::on_btn_upload_clicked()
{
    ui->btn_upload->setEnabled(false);
    ui->progressBar->setValue(0);

    m_functionIDMap.insert(m_id, "ArduinoProgram");

    QJsonObject obj(m_baseObj);
    obj.insert("id", m_id);
    obj.insert("method", "dobotlink.Arduino.ArduinoProgram");

    QJsonObject paramsObj;
    paramsObj.insert("portName", ui->comboBox_port->currentData().toString());
    paramsObj.insert("type", ui->comboBox_type->currentText());
    paramsObj.insert("data", ui->plainTextEdit_code->document()->toPlainText());

    obj.insert("params", paramsObj);

    _sendMessgeObj(obj);
}

void DArduinoTestForm::on_btn_wsConnect_clicked()
{
    if (ui->btn_wsConnect->text() == tr("connect")) {
        QString urlStr = QString("%1:%2").arg(ui->lineEdit_ws_address->text())
                .arg(ui->lineEdit_ws_port->text());
        m_websocket->open(QUrl(urlStr));
    } else {
        m_websocket->close();
        ui->btn_wsConnect->setText(tr("connect"));
        ui->groupBox_workspace->setEnabled(false);
    }
}

void DArduinoTestForm::onConnected_slot()
{
    ui->btn_wsConnect->setText(tr("close"));
    ui->groupBox_workspace->setEnabled(true);
}

void DArduinoTestForm::onMessageReceive_slot(QString message)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "DArduinoTestForm: Json Parse Error" << jsonError.errorString();
        return;
    }

    if (doc.isObject()) {
        QJsonObject obj = doc.object();

        /* GET MESSAGE */
        _handleWSMessageReceived(obj);
    }
}

void DArduinoTestForm::_handleWSMessageReceived(QJsonObject obj)
{
    double resID = obj.value("id").toDouble(-1);
    QString functionName = m_functionIDMap.value(resID);
    if (functionName.isEmpty()) {
        qDebug() << "DArduinoTestForm: can not find function" << obj;
        return;
    }

    QJsonValue resValue = obj.value("result");
    if (functionName == "ShowAvailablePorts") {
        m_functionIDMap.remove(resID);

        if (resValue.isArray()) {
            QJsonArray arrObj = resValue.toArray();
            ui->comboBox_port->clear();
            for (int i=0; i<arrObj.count(); i++) {
                QJsonObject obj = arrObj[i].toObject();
                QString portName = obj.value("portName").toString();
                QString description = obj.value("description").toString();
                QString itemStr = QString("%1 : %2").arg(portName).arg(description);
                ui->comboBox_port->insertItem(-1, itemStr, portName);
            }
        }
    } else if (functionName == "ArduinoProgram") {
        if (resValue.isObject()) {
            QJsonObject resObj = resValue.toObject();
            if (resObj.contains("totalProgress")) {
                double value = resObj.value("totalProgress").toDouble();
                ui->progressBar->setValue(static_cast<int>(value));
            }
        } else if (resValue.isBool()) {
            bool ok = resValue.toBool(false);
            qDebug() << "DArduinoTestForm:: upload:" << ok;
            m_functionIDMap.remove(resID);
            ui->btn_upload->setEnabled(true);
        }
    }
}

void DArduinoTestForm::on_plainTextEdit_code_textChanged()
{
    ui->btn_upload->setEnabled(true);
}
