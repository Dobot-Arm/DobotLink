#ifndef DOBOTLINKMAIN_H
#define DOBOTLINKMAIN_H

#ifndef __arm__
#include <QMainWindow>
#include <QSystemTrayIcon>
#endif

#include <QJsonObject>
#include <QProcess>
#include <QMap>
#include <QTranslator>
namespace Ui {
class DobotLinkMain;
}

class DPluginInterface;
class DAboutUsDialog;
class DLocalMonitorForm;
class DRemoteMonitorForm;
class DUpgradeDialog;
class DobotLinkMain
#ifdef __arm__
        : public QObject
#else
        : public QMainWindow
#endif
{
    Q_OBJECT

public:
#ifdef __arm__
    explicit DobotLinkMain(QObject *parent = nullptr);
#else
    explicit DobotLinkMain(QWidget *parent = nullptr);
#endif
    ~DobotLinkMain();

private:
    DPluginInterface *interface = nullptr;
    QTranslator translator;

#ifndef __arm__
public slots:
    void showTrayMessageBox_slot(QString title, QString content);
    void showDobotLinkWidget_slot(QString widgetName, QJsonObject params);

signals:
    void onMainWidgetClose_signal();

protected:
    void closeEvent(QCloseEvent *event) override;
//    void changeEvent(QEvent *event) override;

private:
    Ui::DobotLinkMain *ui;
    QSystemTrayIcon *m_SystemTray;
    QMap<QString, QAction*> m_pluginActionMap;

    DAboutUsDialog *m_aboutUsDialog = nullptr;
    DLocalMonitorForm *m_monitorWidget = nullptr;
    DRemoteMonitorForm *m_remoteMonitorForm = nullptr;
    DUpgradeDialog *m_upgradeDialog = nullptr;

    QProcess *m_process;
    QMenu *menu;
    QMenu *manager;
    QAction *mainInterface;
    QAction *showOnTop;
    QMenu *logMenu;
    QAction *startLogAction;
    QAction *openLogFileAction;
    QAction *deleteLogFileAction;
    QAction *startAtPowerOnAction;
    QAction *helpAction;
    QAction *upgradeAction;
    QAction *aboutUsAction;
    QAction *exitAction;


    void _systemTrayInit();
    bool _checkStartAtPowerOn();
    void _startMaintenanceProgress();
    void _updateSystemTray();


private slots:
    /* Action slot */
    void showOnTop_slot();
    void startLogAct_slot();
    void openLogFileAct_slot();
    void deleteLogAct_slot();
    void startAtPowerOnAct_slot();
    void helpActionOnAct_slot();
    void upgradeActionAct_slot();
    void showAboutUsAct_slot();
    void exitApp_slot();
    void SwitchLanguage_slot(QString language);

    void onMenuTriggered_slot(QAction *action);
//    void systemTrayActivated_slot(QSystemTrayIcon::ActivationReason reason);
    void systemTrayActivated_slot();
    void handleShowLogMsg_slot(QString message);

    /* Button slot */
    void on_btn_clearbox_clicked();
    void on_checkBox_print_stateChanged(int arg1);
#endif
};

#endif // DOBOTLINKMAIN_H
