#include "DLocalMonitorForm.h"
#include "ui_DLocalMonitorForm.h"

#include <QCamera>
#include <QCameraViewfinder>
#include <QTimer>
#include <QScreen>
#include <QBuffer>
#include <QCloseEvent>
#include <QDebug>

#include "Module/DTcpSocketServer.h"

#define TIME_RESOLUTION (50)

DLocalMonitorForm::DLocalMonitorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DMonitorForm)
{
    ui->setupUi(this);

    m_timer = new QTimer(this);
    m_timer->setInterval(TIME_RESOLUTION);
    connect(m_timer, &QTimer::timeout, this, &DLocalMonitorForm::onTimeout_slot);

    m_viewfinder = new QCameraViewfinder(this);
    m_viewfinder->setGeometry(0, 0, 640, 320);
    m_viewfinder->show();

    m_camera = new QCamera(this);
    m_camera->setViewfinder(m_viewfinder);

    ui->verticalLayout->addWidget(m_viewfinder);
}

DLocalMonitorForm::~DLocalMonitorForm()
{
    DTcpSocketServer::getInstance()->closeServer();
    delete ui;
}

void DLocalMonitorForm::start()
{
    qDebug() << "start monitor.";
    m_camera->start();
    m_timer->start();
}

void DLocalMonitorForm::stop()
{
    qDebug() << "stop monitor.";
    m_camera->stop();
    m_timer->stop();
}

void DLocalMonitorForm::closeEvent(QCloseEvent *event)
{
    this->stop();
    event->accept();
}

void DLocalMonitorForm::onTimeout_slot()
{
    QScreen *screen = QApplication::primaryScreen();

    if (m_viewfinder->isHidden()) {
        return;
    }

    /* [1] captrue image */
    QPixmap image = screen->grabWindow(m_viewfinder->winId());

    /* [2] image -> byte */
    QByteArray byte;
    QBuffer buf(&byte);
    image.save(&buf, "JPEG");

    /* [3] byte -> compress -> toBase64 */
    QByteArray ss = qCompress(byte, 1);
    QByteArray ss_base64 = ss.toBase64();

    /* [4] send prepare */
    QByteArray imageBytes;
    QDataStream out(&imageBytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);

    /* [5] << blank(quint64) << data */
    out << static_cast<quint64>(0);
    out << ss_base64;

    /* [6] length */
    out.device()->seek(0);
    quint64 total = static_cast<quint64>(imageBytes.size());
    out << static_cast<quint64>(total - sizeof (quint64));

    DTcpSocketServer::getInstance()->sendMessage(imageBytes);
}


