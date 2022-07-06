#ifndef DFUDOWNLOADTESTFORM_H
#define DFUDOWNLOADTESTFORM_H

#include <QWidget>
#include <QWebSocket>
#include <QJsonObject>

namespace Ui {
class DfuDownloadTestForm;
}

class DfuDownloadTestForm : public QWidget
{
    Q_OBJECT

public:
    explicit DfuDownloadTestForm(QWidget *parent = nullptr);
    ~DfuDownloadTestForm();

private:
    Ui::DfuDownloadTestForm *ui;

    QWebSocket *s_websocket;
    quint64 m_id;
    QJsonObject m_baseObj;
    QMap<quint64, QString> m_functionIDMap;

    QString m_headMsg;
    QString m_eraseMsg;
    QString m_downloadMsg;
    QString m_endMsg;

    void _sendMessgeObj(QJsonObject &obj);
    void _handleWSMessageReceived(QJsonObject obj);

private slots:
    /* button slot */
    void on_btn_wsConnect_clicked();
    void on_btn_openfile_clicked();
    void on_btn_download_clicked();

    /* websocket slot */
    void onConnected_slot();
    void onMessageReceive_slot(QString message);
};

#endif // DFUDOWNLOADTESTFORM_H
