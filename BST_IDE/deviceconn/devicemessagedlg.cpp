#include "devicemessagedlg.h"
#include "ui_devicemessagedlg.h"

DeviceMessageDlg::DeviceMessageDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceMessageDlg)
{
    ui->setupUi(this);
    ui->TimeLEdit->setText("1000");
    ui->textEdit->setFontWeight(20);
    ui->textEdit->setTextBackgroundColor(QColor(30,100,0));
    ui->textEdit->setTextColor(QColor(10,0,200));
}

DeviceMessageDlg::~DeviceMessageDlg()
{
    delete ui;
}

void DeviceMessageDlg::SetMemberOB(QString pStr)
{
    ui->Memberlabel->setText(pStr);
}

QString DeviceMessageDlg::GetMessage()
{
    QString message = QString("%1#%2")
            .arg(ui->textEdit->toPlainText())
            .arg(ui->TimeLEdit->text());
    return message;
}

void DeviceMessageDlg::accept()
{
    mMessage =  GetMessage();
    QDialog::accept();
}
