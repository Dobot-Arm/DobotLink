#ifndef DREMOTEMONITORFORM_H
#define DREMOTEMONITORFORM_H

#include <QWidget>

namespace Ui {
class DRemoteMonitorForm;
}

class QTcpSocket;
class DRemoteMonitorForm : public QWidget
{
    Q_OBJECT

public:
    explicit DRemoteMonitorForm(QWidget *parent = nullptr);
    ~DRemoteMonitorForm();

private:
    Ui::DRemoteMonitorForm *ui;

    QTcpSocket *m_tcpSocket;

    void showImage(QByteArray ba);

private slots:
    void on_pushButton_connect_clicked();

    void onConnected_slot();
    void onReadyRead_slot();
};

#endif // DREMOTEMONITORFORM_H
