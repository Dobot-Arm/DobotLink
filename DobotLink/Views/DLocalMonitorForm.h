#ifndef DLOCALMONITORFORM_H
#define DLOCALMONITORFORM_H

#include <QWidget>

namespace Ui {
class DMonitorForm;
}

class QTimer;
class QCamera;
class QCameraViewfinder;
class DLocalMonitorForm : public QWidget
{
    Q_OBJECT

public:
    explicit DLocalMonitorForm(QWidget *parent = nullptr);
    ~DLocalMonitorForm() override;

    void start();
    void stop();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::DMonitorForm *ui;

    QTimer *m_timer;

    QCamera *m_camera;
    QCameraViewfinder *m_viewfinder;

private slots:
    void onTimeout_slot();
};

#endif // DLOCALMONITORFORM_H
