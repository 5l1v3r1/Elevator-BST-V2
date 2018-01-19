#include "global.h"
#include "connectdevice.h"
#include "ui_connectdevice.h"

ConnectDevice::ConnectDevice(NFDHostApp *pNFD, NodeTcpclient *pTcpclient, QWidget *parent) :
    QDialog(parent/*, Qt::FramelessWindowHint*/),
    ui(new Ui::Connect)
{
    ui->setupUi(this);
    if(pNFD == 0)
    {
        setWindowTitle(tr("Invalid NFC ..."));
        ui->ConnectTypeTabWidget->setDisabled(true);
        return;
    }
    setWindowTitle(tr("Connecting ..."));
    m_NFDHostApp = pNFD;
    mTcpclient = pTcpclient;
    QString tmpString = GetLocalIP();
    ui->LocalIP->setText(tmpString);
    ui->IPAddr->setText(tmpString);
    ui->IPPort->setText(QString("%1").arg(D_NFCPORT));
//    setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
//    QPalette tmpPalette = this->palette();
//    tmpPalette.setBrush(QPalette::Window,
//                        QBrush(QImage(":/Form/rc/Form/GlassFrame.png").scaled(size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
//    setPalette(tmpPalette); // widget加上了背景图.

//    QTcpSocket* tcpSocket = new QTcpSocket(0);
//    QString info = QString("%1--%2--%3").
//            arg(tcpSocket->peerName()).
//            arg(tcpSocket->peerAddress().toString()).
//            arg(tcpSocket->peerPort());
//    IDE_TRACE_STR(info);
}

ConnectDevice::~ConnectDevice()
{
    delete ui;
}

void ConnectDevice::accept()
{
    if(ui->ConnectTypeTabWidget->isEnabled() == false)
    {
        QDialog::reject();
        return;
    }
    QString tmpIP = ui->IPAddr->text();
    ENET_ADDR tmpAddr(tmpIP);
    if(tmpAddr.isValid() == false)
    {
        QMessageBox::warning(this, tr("Error"),
                                   tr("IP is invalid, please correct!"),
                             QMessageBox::Yes,
                             QMessageBox::Yes);
        return;
    }

    QString tmpPortString = ui->IPPort->text();
    if(tmpPortString.isEmpty())
    {
        QMessageBox::warning(this, tr("Error"),
                                   tr("IPPort is invalid, please correct!"),
                             QMessageBox::Yes,
                             QMessageBox::Yes);
        return;
    }
    int tmpPort = tmpPortString.toInt();
    //>@建立物理连接
    if(!mTcpclient->OpenClient(QHostAddress::LocalHost, tmpPort, 100))
    {
        IDE_TRACE_STR("OpenClient#Failed");
        return;
    }
    connect(mTcpclient, SIGNAL(readyRead()), m_NFDHostApp, SLOT(ParseDataStream()));
    QDialog::accept();
}

void ConnectDevice::reject()
{
    QDialog::reject();
}
