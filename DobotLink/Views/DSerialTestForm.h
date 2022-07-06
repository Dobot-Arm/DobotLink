#ifndef DSERIALTESTFORM_H
#define DSERIALTESTFORM_H

#include <QWidget>
#include <QSerialPort>

namespace Ui {
class DSerialTestForm;
}

class DSerialTestForm : public QWidget
{
    Q_OBJECT

public:
    explicit DSerialTestForm(QWidget *parent = nullptr);
    ~DSerialTestForm();

    bool isSerialPortRunning();

public slots:
    void onWidgetClose_slot();

private:
    Ui::DSerialTestForm *ui;

    QSerialPort *m_serialPort;
    QString m_currentPortName;
    QByteArray receiveData;

    void _closeSerialport();

private slots:
    void on_btn_open_clicked();
    void on_btn_refresh_clicked();
    void on_btn_clear_clicked();
    void on_btn_send_clicked();

    /* QSerialPort SLOT */
    void onReadyRead_slot();
    void handleError_slot(QSerialPort::SerialPortError error);
};

#endif // DSERIALTESTFORM_H
