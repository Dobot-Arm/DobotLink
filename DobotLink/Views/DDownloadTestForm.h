#ifndef DDOWNLOADTESTFORM_H
#define DDOWNLOADTESTFORM_H

#include <QWidget>
#include <QWebSocket>
#include <QJsonObject>

namespace Ui {
class DDownloadTestForm;
}

class DNetworkManager;
class DDownloadTestForm : public QWidget
{
    Q_OBJECT

public:
    explicit DDownloadTestForm(QWidget *parent = nullptr);
    ~DDownloadTestForm();

private:
    Ui::DDownloadTestForm *ui;

    QWebSocket *s_websocket;
    quint64 m_id;
    QJsonObject m_baseObj;
    QMap<quint64, QString> m_functionIDMap;
    DNetworkManager *s_manager;

    void _sendMessgeObj(QJsonObject &obj);
    void _handleWSMessageReceived(QJsonObject obj);

private slots:
    /* button slot */
    void on_btn_wsConnect_clicked();
    void on_pushButton_savepath_clicked();
    void on_pushButton_download_clicked();

    /* websocket slot */
    void onConnected_slot();
    void onMessageReceive_slot(QString message);

    void onProgress_slot(qint64 bytesReceived, qint64 bytesTotal);
};

#endif // DDOWNLOADTESTFORM_H
