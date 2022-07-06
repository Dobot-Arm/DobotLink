#include "DSendUserPacketDialog.h"
#include "ui_DSendUserPacketDialog.h"

DSendUserPacketDialog::DSendUserPacketDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DSendUserPacketDialog)
{
    ui->setupUi(this);
}

DSendUserPacketDialog::~DSendUserPacketDialog()
{
    delete ui;
}

void DSendUserPacketDialog::on_pushButton_send_clicked()
{
    emit sendUserMessage_signal(ui->plainTextEdit->toPlainText());
}
