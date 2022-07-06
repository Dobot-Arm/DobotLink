#include "DAboutUsDialog.h"
#include "ui_DAboutUsDialog.h"

#include <QDebug>

DAboutUsDialog::DAboutUsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::DAboutUsDialog)
{
    ui->setupUi(this);
}

DAboutUsDialog::~DAboutUsDialog()
{
    delete ui;
}

void DAboutUsDialog::setVersion(QString version)
{
    QString text = ui->label_version->text().replace("[VERSION]", version);
    ui->label_version->setText(text);
}

void DAboutUsDialog::setReleaseTime(QString dateStr)
{
    ui->label_releasetime->setText(dateStr);
}
