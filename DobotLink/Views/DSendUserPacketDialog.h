#ifndef DSENDUSERPACKETDIALOG_H
#define DSENDUSERPACKETDIALOG_H

#include <QDialog>

namespace Ui {
class DSendUserPacketDialog;
}

class DSendUserPacketDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DSendUserPacketDialog(QWidget *parent = nullptr);
    ~DSendUserPacketDialog();

signals:
    void sendUserMessage_signal(QString text);

private slots:
    void on_pushButton_send_clicked();

private:
    Ui::DSendUserPacketDialog *ui;
};

#endif // DSENDUSERPACKETDIALOG_H
