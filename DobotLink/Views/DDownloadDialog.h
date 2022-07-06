#ifndef DDOWNLOADDIALOG_H
#define DDOWNLOADDIALOG_H

#include <QDialog>

namespace Ui {
class DDownloadDialog;
}

class DDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DDownloadDialog(QWidget *parent = nullptr);
    ~DDownloadDialog();

signals:
    void downloadCode_signal(QString code);

private slots:
    void on_pushButton_send_clicked();

private:
    Ui::DDownloadDialog *ui;
};

#endif // DDOWNLOADDIALOG_H
