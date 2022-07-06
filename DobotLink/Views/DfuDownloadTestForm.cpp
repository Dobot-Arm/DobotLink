#include "DfuDownloadTestForm.h"
#include "ui_DfuDownloadTestForm.h"

#include <QJsonDocument>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QRegularExpression>
#include <QDebug>

DfuDownloadTestForm::DfuDownloadTestForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DfuDownloadTestForm)
{
    ui->setupUi(this);
    ui->groupBox_dfu->setEnabled(false);

    m_id = 0;
    m_baseObj.insert("jsonrpc", "2.0");

    s_websocket = new QWebSocket();
    s_websocket->setParent(this);
    connect(s_websocket, &QWebSocket::connected,
            this, &DfuDownloadTestForm::onConnected_slot);
    connect(s_websocket, &QWebSocket::textMessageReceived,
            this, &DfuDownloadTestForm::onMessageReceive_slot);

    QString appPath = QCoreApplication::applicationDirPath().remove(QRegExp("_d$"));
    QDir dir(appPath);

#ifdef Q_OS_MAC
    dir.cdUp();
    dir.cd("Resources");
#endif

    if (!dir.exists("firmware")) {
        dir.mkdir("firmware");
    }
    dir.cd("firmware");

    ui->lineEdit_filePath->setText(dir.absolutePath());
}

DfuDownloadTestForm::~DfuDownloadTestForm()
{
    delete ui;
}

void DfuDownloadTestForm::_sendMessgeObj(QJsonObject &obj)
{
    QJsonDocument document;
    document.setObject(obj);

    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    if (s_websocket->isValid()) {
        s_websocket->sendTextMessage(QString(byteArray));
    }
}

void DfuDownloadTestForm::_handleWSMessageReceived(QJsonObject obj)
{
    qDebug() << "get obj" << obj;

    if (obj.contains("id")) {
        quint64 resID = static_cast<quint64>(obj.value("id").toDouble());
        QString functionName = m_functionIDMap.value(resID);
        if (functionName.isEmpty()) {
            qDebug() << "DfuDownloadTestForm: can not find function" << obj;
            return;
        }
    } else {
        QString method = obj.value("method").toString();
        QJsonObject paramsObj = obj.value("params").toObject();

        if (method.endsWith("dfudownload.process")) {
            QString message = paramsObj.value("message").toString();

            /* set progress value */
            QRegularExpression re("\\((?<progress>\\d+)\\)");
            QRegularExpressionMatch match = re.match(message);
            if (match.hasMatch()) {
                QString progress = match.captured("progress");
                int value = progress.toInt();
                if (value <= 100 and value >= 0) {
                    ui->progressBar->setValue(value);
                }
            }

#ifdef Q_OS_WIN
            if (message.contains("Download") and !message.contains("Erase")) {
                m_downloadMsg = message;
            } else if (message.contains("Erase")) {
                m_eraseMsg = message;
            } else if (m_eraseMsg.isEmpty()) {
                m_headMsg.append(message);
            } else if (!m_downloadMsg.isEmpty()) {
                m_endMsg.append(message);
            }

            ui->textEdit->setText(m_headMsg + m_eraseMsg + m_downloadMsg + m_endMsg);
#elif defined (Q_OS_MAC)
            if (message.contains("[") and message.contains("]")) {
                m_downloadMsg = message;
            } else {
                m_headMsg.append(message);
            }
            ui->textEdit->setText(m_headMsg + m_downloadMsg);
#endif

            auto bar = ui->textEdit->verticalScrollBar();
            bar->setValue(bar->maximum());
        }
    }
}

void DfuDownloadTestForm::on_btn_openfile_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("choose target file"),
                ui->lineEdit_filePath->text(), "dfu File(*.dfu)");
    if (!dir.isEmpty()) {
        ui->lineEdit_filePath->setText(dir);
    }
}

void DfuDownloadTestForm::on_btn_download_clicked()
{
    if (!ui->radioButton_magicBox->isChecked() and !ui->radioButton_magicianLite->isChecked()) {
        QMessageBox::information(this, tr("error"), tr("please choose a device type."));
    } else if (!ui->lineEdit_filePath->text().isEmpty()
               and !ui->lineEdit_filePath->text().endsWith(".dfu")) {
        QMessageBox::information(this, tr("error"), tr("please choose a efu file."));
        return;
    }

    QJsonObject obj(m_baseObj);
    obj.insert("id", static_cast<double>(m_id));
    obj.insert("method", "dobotlink.Download.firmware");

    QJsonObject paramsObj;

    if (ui->radioButton_magicBox->isChecked()) {
        paramsObj.insert("device", "MagicBox");
    } else if (ui->radioButton_magicianLite->isChecked()) {
        paramsObj.insert("device", "MagicianLite");
    }

    paramsObj.insert("fileName", ui->lineEdit_filePath->text());

    obj.insert("params", paramsObj);
    _sendMessgeObj(obj);

    m_headMsg.clear();
    m_eraseMsg.clear();
    m_downloadMsg.clear();
    m_endMsg.clear();
    ui->progressBar->reset();

    m_functionIDMap.insert(m_id++, "firmware");
}

void DfuDownloadTestForm::on_btn_wsConnect_clicked()
{
    if (ui->btn_wsConnect->text() == tr("connect")) {
        QString urlStr = QString("%1:%2")
                .arg(ui->lineEdit_ws_address->text())
                .arg(ui->lineEdit_ws_port->text());
        s_websocket->open(QUrl(urlStr));
    } else {
        s_websocket->close();
        ui->btn_wsConnect->setText(tr("connect"));
        ui->groupBox_dfu->setEnabled(false);
    }
}

void DfuDownloadTestForm::onConnected_slot()
{
    ui->btn_wsConnect->setText(tr("close"));
    ui->groupBox_dfu->setEnabled(true);
}

void DfuDownloadTestForm::onMessageReceive_slot(QString message)
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


