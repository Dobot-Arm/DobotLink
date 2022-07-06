#include "DMicrobitTestForm.h"
#include "ui_DMicrobitTestForm.h"

#include <QJsonDocument>
#include <QDebug>

DMicrobitTestForm::DMicrobitTestForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DMicrobitTestForm)
{
    ui->setupUi(this);
    ui->groupBox_workspace->setEnabled(false);

    m_id = 0;
    m_baseObj.insert("jsonrpc", "2.0");

    m_websocket = new QWebSocket();
    m_websocket->setParent(this);
    connect(m_websocket, &QWebSocket::connected,
            this, &DMicrobitTestForm::onConnected_slot);
    connect(m_websocket, &QWebSocket::textMessageReceived,
            this, &DMicrobitTestForm::onMessageReceive_slot);
}

DMicrobitTestForm::~DMicrobitTestForm()
{
    delete ui;
}

void DMicrobitTestForm::_sendMessgeObj(QJsonObject &obj)
{
    QJsonDocument document;
    document.setObject(obj);

    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    if (m_websocket->isValid()) {
        m_websocket->sendTextMessage(QString(byteArray));
        m_id++;
    }
}

void DMicrobitTestForm::on_btn_send_clicked()
{
    ui->btn_send->setEnabled(false);

    m_functionIDMap.insert(m_id, "MicrobitProgram");

    QJsonObject obj(m_baseObj);
    obj.insert("id", m_id);
    obj.insert("method", "dobotlink.Microbit.MicrobitProgram");

    QJsonObject paramsObj;
    paramsObj.insert("type", "Python");
    paramsObj.insert("data", ui->plainTextEdit_code->document()->toPlainText());

    obj.insert("params", paramsObj);

    _sendMessgeObj(obj);
}

void DMicrobitTestForm::on_btn_wsConnect_clicked()
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

void DMicrobitTestForm::onConnected_slot()
{
    ui->btn_wsConnect->setText(tr("close"));
    ui->groupBox_workspace->setEnabled(true);
}

void DMicrobitTestForm::onMessageReceive_slot(QString message)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "DMicrobitTestForm: Json Parse Error" << jsonError.errorString();
        return;
    }

    if (doc.isObject()) {
        QJsonObject obj = doc.object();

        /* GET MESSAGE */
        _handleWSMessageReceived(obj);
    }
}

void DMicrobitTestForm::_handleWSMessageReceived(QJsonObject obj)
{
    double resID = obj.value("id").toDouble(-1);
    QString functionName = m_functionIDMap.take(resID);
    if (functionName.isEmpty()) {
        qDebug() << "DMicrobitTestForm: can not find function" << obj;
        return;
    }

    if (functionName == "MicrobitProgram") {
        bool ok = obj.value("result").toBool(false);
        if (ok == true) {
            ui->btn_send->setEnabled(true);
        }
    }
}

void DMicrobitTestForm::on_plainTextEdit_code_textChanged()
{
    ui->btn_send->setEnabled(true);
}
