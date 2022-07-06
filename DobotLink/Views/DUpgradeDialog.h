#ifndef DUPGRADEDIALOG_H
#define DUPGRADEDIALOG_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class DUpgradeDialog;
}

class DNetworkManager;
class QNetworkReply;
class DSettings;
class QProcess;
class DUpgradeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DUpgradeDialog(QWidget *parent = nullptr);
    ~DUpgradeDialog();

    void checkUpgrade();

private:
    Ui::DUpgradeDialog *ui;

    DNetworkManager *m_network;
    QNetworkReply *m_handleReply;
    DSettings *settings;
    QString m_url;
    QProcess *m_process;

    int compareVersion(QString v1, QString v2);

signals:
    void closeApp_signal();

private slots:
    void on_pushButton_download_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_install_clicked();

    void handleNetworkReplyMessage_slot(QString url, QJsonObject resObj, QString user);
    void handleNetworkReplyData_slot(QString url, QByteArray resBa);
    void onProgress_slot(qint64 bytesReceived, qint64 bytesTotal);
    void handledownloadFinished_slot(QString url, quint64 id);
};

#endif // DUPGRADEDIALOG_H
