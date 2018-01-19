#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QFileDialog>
#include <QMessageBox>
TcpClient::TcpClient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    //this->setFixedSize(350,180);
    InitVariable();
    InitClient();
    InitUi();
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TcpClient::InitVariable()
{
    loadSize = 4*1024;
    ConnectStaus = 0;
    TotalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;
    bytesToWrite = 0;
}

void TcpClient::InitClient()
{
    tcpPort = 6666;
    tcpClient = new QTcpSocket(this);
    connect(this,SIGNAL(sendFileName()),this,SLOT(slot_sendMessage()));
    connect(tcpClient,SIGNAL(hostFound()),this,SLOT(slot_connect_Success()));
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(slot_readMessage()));
    connect(tcpClient,SIGNAL(bytesWritten(qint64)),SLOT(slot_updateClientProgress(qint64)));
    connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(displayError(QAbstractSocket::SocketError)));
}

void TcpClient::InitUi()
{
    ui->tcpClientStatusLabel->setText(tr("Waiting to receive documents"));
}

void TcpClient::setHostAddress(QHostAddress address)  //设置服务器地址并连接服务器
{
    hostAddress = address;
    qDebug()<<hostAddress;
    newConnect();
}

void TcpClient::newConnect()
{    
    blockSize = 0;
    tcpClient->abort();
    tcpClient->connectToHost(hostAddress,tcpPort);
    time.start();
}

void TcpClient::slot_readMessage()
{
    QDataStream in(tcpClient);
    in.setVersion(QDataStream::Qt_4_6);

    float useTime = time.elapsed();
    if(bytesReceived <= sizeof(qint64)*2)
    {
        if((tcpClient->bytesAvailable() >= sizeof(qint64)*2) && (fileNameSize == 0))
        {
            in>>TotalBytes>>fileNameSize;
            qDebug()<<"+++"<<TotalBytes<<fileNameSize;
            bytesReceived += sizeof(qint64)*2;
        }
        if((tcpClient->bytesAvailable() >= fileNameSize) && (fileNameSize != 0))
        {
            in>>fileName;
             qDebug()<<"+++"<<fileName;
            bytesReceived +=fileNameSize;
            //>@若需另存为则去掉下面一句
            localFile = new QFile(QDir::currentPath()+QString("/")+fileName);
            if(!localFile->open(QFile::WriteOnly))
            {
                QMessageBox::warning(this,tr("Application"),tr("Unable to read file %1:\n%2.").arg(fileName).arg(localFile->errorString()));
                return;
            }
        }
        else
        {
            return;
        }
    }
    if(bytesReceived < TotalBytes)
    {
        bytesReceived += tcpClient->bytesAvailable();
        inBlock = tcpClient->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }
    ui->progressBar->setMaximum(TotalBytes);
    ui->progressBar->setValue(bytesReceived);
    //qDebug()<<bytesReceived<<"received"<<TotalBytes;

    double speed = bytesReceived / useTime;
    ui->tcpClientStatusLabel->setText(tr("Received %1MB (%2MB/s) \nAltogether%3MB Elapsed time:%4Second\nTime remaining：%5Second")
                                   .arg(bytesReceived / (1024*1024))//已接收
                                   .arg(speed*1000/(1024*1024),0,'f',2)//速度
                                   .arg(TotalBytes / (1024 * 1024))//总大小
                                   .arg(useTime/1000,0,'f',0)//用时
                                   .arg(TotalBytes/speed/1000 - useTime/1000,0,'f',0));//剩余时间

    if(bytesReceived == TotalBytes)
    {
        tcpClient->close();
        ui->tcpClientStatusLabel->setText(tr("Receive files %1 Complete").arg(fileName));
        localFile->close();   //接收完文件后，一定要关闭，不然可能出问题
    }
}

void TcpClient::slot_sendMessage()  //开始发送数据
{
    if(ConnectStaus==0)
    {
        QMessageBox::warning(this,tr("Connect Failed"),tr("Please check connect"));
        return;
    }
    localFile = new QFile(fileName);
    if(!localFile->open((QFile::ReadOnly))){//以只读方式打开
        QMessageBox::warning(this,tr("Application"),tr("Unable to read file %1:\n%2").arg(fileName).arg(localFile->errorString()));
        return;
    }
    TotalBytes = localFile->size();
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);
    time.start();  //开始计时
    QString currentFile = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
    sendOut<<qint64(0)<<qint64(0)<<currentFile;
    TotalBytes += outBlock.size();
    sendOut.device()->seek(0);
    sendOut<<TotalBytes<<qint64((outBlock.size()-sizeof(qint64)*2));
    bytesToWrite = TotalBytes - tcpClient->write(outBlock);
    qDebug()<<currentFile<<TotalBytes;
    outBlock.resize(0);
}

void TcpClient::slot_updateClientProgress(qint64 numBytes)//更新进度条
{
    bytesWritten += (int)numBytes;
    if(bytesToWrite > 0)
    {
        outBlock = localFile->read(qMin(bytesToWrite,loadSize));
        bytesToWrite -= (int)tcpClient->write(outBlock);
        outBlock.resize(0);
    }
    else
    {
        localFile->close();
    }
}

void TcpClient::displayError(QAbstractSocket::SocketError socketError) //错误处理
{
    switch(socketError)
    {
        case QAbstractSocket::RemoteHostClosedError : break;
        default : qDebug() << tcpClient->errorString();
    }
}

void TcpClient::slot_connect_Success()
{
    ConnectStaus=1;
}

void TcpClient::on_tcpClientCloseBtn_clicked()//关闭
{
    tcpClient->abort();
    this->close();
}

void TcpClient::on_tcpClientCancleBtn_clicked()//取消
{
   tcpClient->abort();
}

void TcpClient::on_tcpClientSendBtn_clicked()
{
    emit sendFileName();
}

void TcpClient::on_tcpClientOpenBtn_clicked()
{
    fileName = QFileDialog::getOpenFileName(this);
}
