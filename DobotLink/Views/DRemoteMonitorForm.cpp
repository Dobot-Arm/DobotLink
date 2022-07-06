#include "DRemoteMonitorForm.h"
#include "ui_DRemoteMonitorForm.h"

#include <QTcpSocket>
#include <QDebug>

DRemoteMonitorForm::DRemoteMonitorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DRemoteMonitorForm)
{
    ui->setupUi(this);

    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, &QTcpSocket::connected,
            this, &DRemoteMonitorForm::onConnected_slot);
    connect(m_tcpSocket, &QTcpSocket::readyRead,
            this, &DRemoteMonitorForm::onReadyRead_slot);
}

DRemoteMonitorForm::~DRemoteMonitorForm()
{
    m_tcpSocket->abort();
    delete ui;
}

void DRemoteMonitorForm::showImage(QByteArray ba)
{
    QString ss = QString::fromLatin1(ba);
    QByteArray rc = QByteArray::fromBase64(ss.toLatin1());
    QByteArray rdc = qUncompress(rc);

    QPixmap pixmap;
    pixmap.loadFromData(rdc);

    ui->label_image->setPixmap(pixmap);
    update();
}

void DRemoteMonitorForm::on_pushButton_connect_clicked()
{
    if (ui->pushButton_connect->text() == tr("connect")) {
        QString ip = ui->lineEdit_ip->text();
        m_tcpSocket->connectToHost(ip, 9091);
    } else {
        m_tcpSocket->disconnectFromHost();
    }
}

void DRemoteMonitorForm::onConnected_slot()
{
    ui->pushButton_connect->setText(tr("disconnect"));
}

void DRemoteMonitorForm::onReadyRead_slot()
{
    static quint64 s_size = 0;
    quint64 byteAvailable = static_cast<quint64>(m_tcpSocket->bytesAvailable());

    if (byteAvailable > sizeof (quint64)) {
        QDataStream in(m_tcpSocket);
        in.setVersion(QDataStream::Qt_5_12);

        if (s_size == 0) {
            in >> s_size;
        } else if (byteAvailable >= s_size) {
            QByteArray imageData;
            in >> imageData;
            s_size = 0;
            showImage(imageData);
        }
    }
}

