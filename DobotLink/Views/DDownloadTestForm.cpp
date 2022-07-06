#include "DDownloadTestForm.h"
#include "ui_DDownloadTestForm.h"

#include <QJsonDocument>
#include <QFileDialog>
#include <QDebug>

#include "Module/DNetworkManager.h"

DDownloadTestForm::DDownloadTestForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DDownloadTestForm)
{
    ui->setupUi(this);
    ui->groupBox_download->setEnabled(false);
    ui->progressBar->setValue(0);

    m_id = 0;
    m_baseObj.insert("jsonrpc", "2.0");

    s_websocket = new QWebSocket();
    s_websocket->setParent(this);
    connect(s_websocket, &QWebSocket::connected,
            this, &DDownloadTestForm::onConnected_slot);
    connect(s_websocket, &QWebSocket::textMessageReceived,
            this, &DDownloadTestForm::onMessageReceive_slot);

    s_manager = DNetworkManager::getInstance();
    connect(s_manager, &DNetworkManager::downloadProgress_signal,
            this, &DDownloadTestForm::onProgress_slot);

    QString path = s_manager->getDownloadPath();
    ui->lineEdit_targetPath->setText(path);
    ui->progressBar->setMaximum(1000);
}

DDownloadTestForm::~DDownloadTestForm()
{
    delete ui;
}

void DDownloadTestForm::_sendMessgeObj(QJsonObject &obj)
{
    QJsonDocument document;
    document.setObject(obj);

    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    if (s_websocket->isValid()) {
        s_websocket->sendTextMessage(QString(byteArray));
    }
}

void DDownloadTestForm::_handleWSMessageReceived(QJsonObject obj)
{
    quint64 resID = static_cast<quint64>(obj.value("id").toDouble());
    QString functionName = m_functionIDMap.value(resID);
    if (functionName.isEmpty()) {
        qDebug() << "DDownloadTestForm: can not find function" << obj;
        return;
    }
}

void DDownloadTestForm::on_btn_wsConnect_clicked()
{
    if (ui->btn_wsConnect->text() == tr("connect")) {
        QString urlStr = QString("%1:%2").arg(ui->lineEdit_ws_address->text())
                .arg(ui->lineEdit_ws_port->text());
        s_websocket->open(QUrl(urlStr));
    } else {
        s_websocket->close();
        ui->btn_wsConnect->setText(tr("connect"));
        ui->groupBox_download->setEnabled(false);
    }
}

void DDownloadTestForm::onConnected_slot()
{
    ui->btn_wsConnect->setText(tr("close"));
    ui->groupBox_download->setEnabled(true);
}

void DDownloadTestForm::onMessageReceive_slot(QString message)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "DDownloadTestForm: Json Parse Error" << jsonError.errorString();
        return;
    }

    if (doc.isObject()) {
        QJsonObject obj = doc.object();

        /* GET MESSAGE */
        _handleWSMessageReceived(obj);
    }
}

void DDownloadTestForm::on_pushButton_savepath_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
                this, tr("choose target file Directory"),
                ui->lineEdit_targetPath->text());
    if (!dir.isEmpty()) {
        ui->lineEdit_targetPath->setText(dir);
        s_manager->setDownloadFilePath(dir);
    }
}

void DDownloadTestForm::on_pushButton_download_clicked()
{
    QFileInfo info(ui->lineEdit_source->text());
    ui->progressBar->setValue(0);

#if 0
    s_manager->downloadFile(info.filePath(), info.fileName());
#else
    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", "dobotlink.api.DownloadFile");

    QJsonObject paramsObj;
    paramsObj.insert("url", info.filePath());
    paramsObj.insert("fileName", info.fileName());

    obj.insert("params", paramsObj);
    _sendMessgeObj(obj);
#endif

    m_functionIDMap.insert(m_id++, "DownloadFile");
}

void DDownloadTestForm::onProgress_slot(qint64 bytesReceived, qint64 bytesTotal)
{
    double percent = bytesReceived * 1000 / bytesTotal;
    ui->progressBar->setValue(static_cast<int>(percent));
//    qDebug() << bytesReceived << "/" << bytesTotal;
}
