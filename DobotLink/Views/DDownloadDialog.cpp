#include "DDownloadDialog.h"
#include "ui_DDownloadDialog.h"

DDownloadDialog::DDownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DDownloadDialog)
{
    ui->setupUi(this);
}

DDownloadDialog::~DDownloadDialog()
{
    delete ui;
}

void DDownloadDialog::on_pushButton_send_clicked()
{
    QString code = ui->plainTextEdit->toPlainText();
    emit downloadCode_signal(code);
}
