#ifndef DABOUTUSDIALOG_H
#define DABOUTUSDIALOG_H

#include <QDialog>

namespace Ui {
class DAboutUsDialog;
}

class DAboutUsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DAboutUsDialog(QWidget *parent = nullptr);
    ~DAboutUsDialog();

    void setVersion(QString version);
    void setReleaseTime(QString dateStr);

private:
    Ui::DAboutUsDialog *ui;
};

#endif // DABOUTUSDIALOG_H
