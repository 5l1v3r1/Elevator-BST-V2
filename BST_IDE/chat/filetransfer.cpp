#include "filetransfer.h"
#include "ui_filetransfer.h"

FileTransfer::FileTransfer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileTransfer)
{
    ui->setupUi(this);
    VariableInit();
    UiInit();
}

FileTransfer::~FileTransfer()
{
    delete ui;
}

void FileTransfer::VariableInit()
{
    Ip = "0.0.0.0";
}

void FileTransfer::UiInit()
{
    Server = new TcpServer(0);
    Client = new TcpClient(0);

    QGridLayout *Layout = new QGridLayout(0);
    Layout->addWidget(Server,0,0);
    Layout->addWidget(Client,0,1);
    ui->widget->setLayout(Layout);

    ui->ip0->setText("10");
    ui->ip1->setText("200");
    ui->ip2->setText("19");
    ui->ip3->setText("5");
    ui->adr->setText("tmp");
    //m_nTimerId = startTimer(100);
}

QString FileTransfer::GetServerIp()
{
    QString Ip0 = ui->ip0->text();
    QString Ip1 = ui->ip1->text();
    QString Ip2 = ui->ip2->text();
    QString Ip3 = ui->ip3->text();
    QString Ip = Ip0+QString(".")+Ip1+QString(".")+Ip2+QString(".")+Ip3;
    return Ip;
}

void FileTransfer::on_btn_connect_clicked()
{
    QString tmp = GetServerIp();
    if(tmp=="...")
        return;
    if(tmp!=Ip)
    {
        Ip = tmp;
        Client->setHostAddress((QHostAddress)(tmp));
    }
}

void FileTransfer::on_pushButton_2_clicked()
{
    Client->setFileName(ui->adr->text());
}

void FileTransfer::timerEvent( QTimerEvent *event )
{}

