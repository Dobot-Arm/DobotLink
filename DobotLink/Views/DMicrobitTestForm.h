#ifndef DMICROBITTESTFORM_H
#define DMICROBITTESTFORM_H

#include <QWidget>
#include <QWebSocket>
#include <QJsonObject>

namespace Ui {
class DMicrobitTestForm;
}

class DMicrobitTestForm : public QWidget
{
    Q_OBJECT

public:
    explicit DMicrobitTestForm(QWidget *parent = nullptr);
    ~DMicrobitTestForm();

private:
    Ui::DMicrobitTestForm *ui;

    QWebSocket *m_websocket;
    double m_id;
    QJsonObject m_baseObj;
    QMap<double, QString> m_functionIDMap;

    void _sendMessgeObj(QJsonObject &obj);
    void _handleWSMessageReceived(QJsonObject obj);

private slots:
    /* button slot */
    void on_btn_wsConnect_clicked();
    void on_btn_send_clicked();

    /* websocket slot */
    void onConnected_slot();
    void onMessageReceive_slot(QString message);

    void on_plainTextEdit_code_textChanged();
};

#endif // DMICROBITTESTFORM_H
