#include "DSerialTestForm.h"
#include "ui_DSerialTestForm.h"

#include <QScrollBar>
#include <QMessageBox>
#include <QDebug>

#include "Module/DSerialPort.h"

DSerialTestForm::DSerialTestForm(QWidget *parent) : QWidget(parent), ui(new Ui::DSerialTestForm)
{
    ui->setupUi(this);

    m_serialPort = new QSerialPort(this);
    m_serialPort->setBaudRate(QSerialPort::Baud115200);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    connect(m_serialPort, &QSerialPort::readyRead, this, &DSerialTestForm::onReadyRead_slot);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &DSerialTestForm::handleError_slot);

    ui->comboBox_baud->addItem("9600 " + tr("baud"), QSerialPort::Baud9600);
    ui->comboBox_baud->addItem("19200 " + tr("baud"), QSerialPort::Baud19200);
    ui->comboBox_baud->addItem("38400 " + tr("baud"), QSerialPort::Baud38400);
    ui->comboBox_baud->addItem("57600 " + tr("baud"), QSerialPort::Baud57600);
    ui->comboBox_baud->addItem("115200 " + tr("baud"), QSerialPort::Baud115200);
    ui->comboBox_baud->setCurrentIndex(4);
}

DSerialTestForm::~DSerialTestForm()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
    delete ui;
}

bool DSerialTestForm::isSerialPortRunning()
{
    return m_serialPort->isOpen();
}

/* SLOT */
/* 打开串口 */
void DSerialTestForm::on_btn_open_clicked()
{
    if (ui->btn_open->text() == tr("open")) {
        QString portName = ui->combo_port->currentText().split("(").first();
        portName = portName.remove(" ");
        m_serialPort->setPortName(portName);

        int baudType = ui->comboBox_baud->currentData().toInt();
        m_serialPort->setBaudRate(baudType);

        bool ok = m_serialPort->open(QIODevice::ReadWrite);
        if (ok) {
            qDebug() << "open serial port.  portName:" << portName << "baud:" << baudType;
            ui->btn_open->setText(tr("close"));
            ui->combo_port->setEnabled(false);
            ui->comboBox_baud->setEnabled(false);
            ui->btn_refresh->setEnabled(false);
        }
    } else {
        _closeSerialport();
    }
}

void DSerialTestForm::on_btn_refresh_clicked()
{
    ui->combo_port->clear();
    ui->combo_port->addItems(DSerialPort::getAvailablePortList());
}

void DSerialTestForm::on_btn_clear_clicked()
{
    receiveData.clear();
    ui->plainTextEdit->clear();
}

void DSerialTestForm::on_btn_send_clicked()
{
    QByteArray sdata;
    QByteArray content = ui->lineEdit_send->text().toLatin1();
    ui->lineEdit_send->clear();

    if (ui->rBtn_send_hex->isChecked()) {
        sdata = content.toHex();
    } else {
        sdata = content;
    }

    ui->plainTextEdit->insertPlainText(sdata);

    QScrollBar *scrollBar = ui->plainTextEdit->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());

    m_serialPort->write(sdata);
}

/* SLOT */
/* 收到数据 */
void DSerialTestForm::onReadyRead_slot()
{
    auto port = qobject_cast<QSerialPort*>(sender());
    const QByteArray data = port->readAll();

    QString message;
    if (ui->rBtn_receive_ascii->isChecked()) {
        message = QString(data);
    } else {
        message = QString(data.toHex());
    }
    ui->plainTextEdit->insertPlainText(message);

    QScrollBar *scrollBar = ui->plainTextEdit->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

/* 错误处理 */
void DSerialTestForm::handleError_slot(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        auto port = qobject_cast<QSerialPort*>(sender());
        QString errStr = QString(" error:%1").arg(error);
        qDebug() << "serial port:" << port->portName() << errStr;
    }
}

void DSerialTestForm::onWidgetClose_slot()
{
    if (m_serialPort->isOpen()) {
        int ok = QMessageBox::question(this,
                                       tr("serial port is still running"),
                                       tr("Do you want to close it?"),
                                       QMessageBox::Cancel,
                                       QMessageBox::Ok);
        if (ok == QMessageBox::Ok) {
            _closeSerialport();
        }
    }
}

void DSerialTestForm::_closeSerialport()
{
    m_serialPort->close();
    ui->btn_open->setText(tr("open"));
    ui->combo_port->setEnabled(true);
    ui->comboBox_baud->setEnabled(true);
    ui->btn_refresh->setEnabled(true);
}

