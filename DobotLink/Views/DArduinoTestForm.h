#ifndef DARDUINOTESTFORM_H
#define DARDUINOTESTFORM_H

#include <QWidget>
#include <QWebSocket>
#include <QJsonObject>

namespace Ui {
class DArduinoTestForm;
}

class DArduinoTestForm : public QWidget
{
    Q_OBJECT

public:
    explicit DArduinoTestForm(QWidget *parent = nullptr);
    ~DArduinoTestForm();

private:
    Ui::DArduinoTestForm *ui;

    QWebSocket *m_websocket;
    double m_id;
    QJsonObject m_baseObj;
    QMap<double, QString> m_functionIDMap;
    QString m_portName;

    void _sendMessgeObj(QJsonObject &obj);
    void _handleWSMessageReceived(QJsonObject obj);

private slots:
    /* button slot */
    void on_btn_searchPort_clicked();
    void on_btn_upload_clicked();
    void on_btn_wsConnect_clicked();

    /* websocket slot */
    void onConnected_slot();
    void onMessageReceive_slot(QString message);
    void on_plainTextEdit_code_textChanged();
};

#endif // DARDUINOTESTFORM_H
