#include "DUpgradeDialog.h"
#include "ui_DUpgradeDialog.h"

#include <QMessageBox>
#include <QProcess>
#include <QDebug>

#include "Module/DNetworkManager.h"
#include "Module/DSettings.h"

DUpgradeDialog::DUpgradeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DUpgradeDialog)
{
    ui->setupUi(this);
    ui->pushButton_download->hide();
    ui->stackedWidget->setCurrentIndex(0);
    ui->textEdit_content->hide();
    ui->progressBar->setMaximum(1000);

    m_network = DNetworkManager::getInstance();
    connect(m_network, &DNetworkManager::onReplyMassage_signal,
            this, &DUpgradeDialog::handleNetworkReplyMessage_slot);
    connect(m_network, &DNetworkManager::onReplyData_signal,
            this, &DUpgradeDialog::handleNetworkReplyData_slot);
    connect(m_network, &DNetworkManager::downloadFinished_signal,
            this, &DUpgradeDialog::handledownloadFinished_slot);

    settings = DSettings::getInstance();
    m_process = new QProcess(this);
}

DUpgradeDialog::~DUpgradeDialog()
{
    delete ui;
}

void DUpgradeDialog::checkUpgrade()
{
    qDebug() << "检查更新...";
    m_network->getDobotRequest("DobotLink/version.json", "DUpgradeDialog");
    ui->stackedWidget->setCurrentWidget(ui->page_home);
}

int DUpgradeDialog::compareVersion(QString v1, QString v2)
{
    QStringList v1list = v1.split(".", QString::SkipEmptyParts);
    QStringList v2list = v2.split(".", QString::SkipEmptyParts);

    int len1 = v1list.count();
    int len2 = v2list.count();
    int i;
    for (i = 0; i < qMin(len1, len2); ++i) {
        if (v1list.at(i).toUInt() > v2list.at(i).toUInt()) {
            return 1;
        } else if (v1list.at(i).toUInt() < v2list.at(i).toUInt()) {
            return -1;
        }
    }
    if (v1list.count() > v2list.count()) {
        for (int j = i; j < len1; ++j) {
            if (v1list.at(j).toUInt() != 0) {
                return 1;
            }
        }
    } else if (v1list.count() < v2list.count()) {
        for (int j = i; j < len2; ++j) {
            if (v2list.at(j).toUInt() != 0) {
                return 1;
            }
        }
    }
    return 0;
}

void DUpgradeDialog::on_pushButton_download_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_download);

    QFileInfo info(m_url);

    QNetworkReply *reply = m_network->downloadFile(m_url, info.fileName());
    reply->setProperty("user", "DUpgradeDialog");
    connect(reply, &QNetworkReply::downloadProgress,
            this, &DUpgradeDialog::onProgress_slot);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            [=](QNetworkReply::NetworkError code){
        qDebug() << "error code:" << code;
    });

    m_handleReply = reply;
}

void DUpgradeDialog::on_pushButton_cancel_clicked()
{
    QMessageBox msgBox;
    msgBox.setText(tr("Cancel download the new version?"));
    msgBox.setInformativeText(tr("choose ok to stop. choose cancel to return."));
    msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Ok) {
        this->close();
        m_handleReply->abort();
    }
}

void DUpgradeDialog::handleNetworkReplyMessage_slot(QString url, QJsonObject resObj, QString user)
{
    if (user != "DUpgradeDialog") {
        return;
    }

    if (url == "DobotLink/version.json") {
        QJsonObject section_main = resObj.value("main").toObject();
        QString latestV = section_main.value("version").toString();
        QJsonObject urlObj = section_main.value("url").toObject();

#ifdef Q_OS_WIN
        m_url = urlObj.value("win").toString();
#elif defined(Q_OS_MAC)
        m_url = urlObj.value("mac").toString();
#endif

        QString localV = settings->getVersion("main");
        qDebug().noquote() << QString("check new version. local:V%1 latest:V%2 link:%3")
                              .arg(localV).arg(latestV).arg(m_url);
        int res = compareVersion(localV, latestV);

        if (res < 0) {
            ui->label_home->setText(tr("get a new version. v%1").arg(latestV));
            QString description = section_main.value("description").toString();
            ui->textEdit_content->setPlainText(description);
            ui->textEdit_content->show();
            ui->pushButton_download->show();
        } else {
            ui->pushButton_download->hide();
            ui->label_home->setText(tr("this is the latest version."));
        }
    }
}

void DUpgradeDialog::handleNetworkReplyData_slot(QString url, QByteArray resBa)
{
    qDebug() << "999";
    if (url == "x") {
        qDebug() << "res" << resBa;
    }
}

void DUpgradeDialog::onProgress_slot(qint64 bytesReceived, qint64 bytesTotal)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    QString user = reply->property("user").toString();
    if (user != "DUpgradeDialog") {
        return;
    }

    double rec = bytesReceived / 1024.00;
    double total = bytesTotal / 1024.00;
    QString lable = QString("%1/%2 KB")
            .arg(static_cast<quint64>(rec))
            .arg(static_cast<quint64>(total));
    ui->label_percent->setText(lable);

    double percent = bytesReceived * 1000 / bytesTotal;
    ui->progressBar->setValue(static_cast<int>(percent));
}

void DUpgradeDialog::handledownloadFinished_slot(QString url, quint64 id)
{
    Q_UNUSED(id)

    if (url == m_url) {
        if (ui->progressBar->value() >= ui->progressBar->maximum()) {
            ui->stackedWidget->setCurrentWidget(ui->page_finish);
        } else {
            ui->stackedWidget->setCurrentWidget(ui->page_home);
        }
    }
}

void DUpgradeDialog::on_pushButton_install_clicked()
{
    bool ok = false;
    QString fileName = m_network->getDownloadPath();

    //启动安装程序
#ifdef Q_OS_WIN
    fileName.append("\\DobotLinkSetup.exe");

    ok = m_process->startDetached(fileName, QStringList(fileName));
    m_process->waitForStarted(1000);
#elif defined (Q_OS_MAC)
    fileName.append("/DobotLinkSetup.dmg");

    m_process->start("hdiutil", QStringList() << "attach" << fileName);
    if (m_process->waitForFinished(10000)) {
        m_process->start("open", QStringList("/Volumes/DobotLinkSetup"));
        ok = m_process->waitForFinished(2000);
    }
#endif

    //关闭软件
    if (ok) {
        this->close();
        emit closeApp_signal();
    }
}

#if 0
    QString program1 = "hdiutil attach " + fileName;
    ok2 = system(program1.toLatin1().data());
    if (ok2 == 0) {
        ok2 = system("open /Volumes/DobotLink");
    }
#endif
