#include "tcpserver.h"
#include "ui_tcpserver.h"
#include <QTcpSocket>
#include <QFileDialog>
#include <QMessageBox>

TcpServer::TcpServer(QWidget *parent):QDialog(parent),
 ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    //this->setFixedSize(350,180);
    InitVariable();   
    InitServer();
    InitUi();   
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void TcpServer::InitVariable()
{
    loadSize = 4*1024;
    TotalBytes = 0;
    bytesWritten = 0;
    bytesToWrite = 0;
    bytesReceived = 0;
    fileNameSize = 0;
}

void TcpServer::InitServer()//初始化
{  
    tcpPort = 6666;
    tcpServer = new QTcpServer(this);
    connect(this,SIGNAL(sendFileName()),this,SLOT(slot_sendMessage()));
    tcpServer->close();
    tcpServer->listen(QHostAddress::Any,tcpPort);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slot_GetNextPendingConnect()));
}

void TcpServer::InitUi()
{
    ui->serverStatusLabel->setText(tr("Please select the file you want to send"));
    ui->progressBar->reset();
    ui->serverOpenBtn->setEnabled(true);
    ui->serverSendBtn->setEnabled(false);
}

void TcpServer::slot_GetNextPendingConnect()
{
    clientConnection = tcpServer->nextPendingConnection();//连接失败返回0
    connect(clientConnection,SIGNAL(bytesWritten(qint64)),SLOT(slot_updateClientProgress(qint64)));
    connect(clientConnection, SIGNAL(readyRead()),this, SLOT(slot_ReceiveProgress()));
    connect(clientConnection, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(slot_displayError(QAbstractSocket::SocketError)));
}

void TcpServer::slot_sendMessage()  //开始发送数据
{
    if(clientConnection==0)
    {
        QMessageBox::warning(this,tr("Connect Failed"),tr("Please select a client to connect"));
        return;
    }
    ui->serverSendBtn->setEnabled(false);
    ui->serverStatusLabel->setText(tr("Start transferring files %1 ！").arg(fileName));
    localFile = new QFile(fileName);
    if(!localFile->open((QFile::ReadOnly))){//以只读方式打开
        QMessageBox::warning(this,tr("Application"),tr("Unable to read file %1:\n%2").arg(fileName).arg(localFile->errorString()));
        return;
    }

    QByteArray Block;
    QDataStream PackageStream(&Block,QIODevice::WriteOnly);
    PackageStream.setVersion(QDataStream::Qt_4_6);
    PackageStream<<int(0);
    PackageStream.device()->seek(0);
    PackageStream<<0x80<<0x40<<0x03<<0x7f<<0x81;

    qDebug()<<"lenth"<<Block.size();

    TotalBytes = localFile->size();
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);
    time.start();  //开始计时
    QString currentFile = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
    sendOut<<qint64(0)<<qint64(0)<<currentFile;

    qDebug()<<"---"<<outBlock.right(sizeof(qint64)*2);

    TotalBytes += outBlock.size();
    sendOut.device()->seek(0);
    sendOut<<TotalBytes<<qint64((outBlock.size()-sizeof(qint64)*2));
    bytesToWrite = TotalBytes - clientConnection->write(outBlock);
    qDebug()<<currentFile<<TotalBytes;
    outBlock.resize(0);
}

void TcpServer::slot_updateClientProgress(qint64 numBytes)//更新进度条
{
    bytesWritten += (int)numBytes;
    if(bytesToWrite > 0)
    {
        outBlock = localFile->read(qMin(bytesToWrite,loadSize));
        bytesToWrite -= (int)clientConnection->write(outBlock);
        outBlock.resize(0);
    }
    else
    {
        localFile->close();
    }
    ui->progressBar->setMaximum(TotalBytes);
    ui->progressBar->setValue(bytesWritten);

    float useTime = time.elapsed();
    double speed = bytesWritten / useTime;
    ui->serverStatusLabel->setText(tr("Received %1MB (%2MB/s) \nAltogether%3MB Elapsed time:%4Second\nTime remaining：%5Second")
                                  .arg(bytesWritten / (1024*1024))//已发送
                                  .arg(speed*1000/(1024*1024),0,'f',2)//速度
                                  .arg(TotalBytes / (1024 * 1024))//总大小
                                  .arg(useTime/1000,0,'f',0)//用时
                                  .arg(TotalBytes/speed/1000 - useTime/1000,0,'f',0));//剩余时间

   //num.sprintf("%.1f KB/s", (bytesWritten*1000) / (1024.0*time.elapsed()));
    if(bytesWritten == TotalBytes)
        ui->serverStatusLabel->setText(tr("Transfer files %1 Successfull").arg(theFileName));
}


void TcpServer::slot_ReceiveProgress()//更新进度条
{
    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_4_6);
    if(bytesReceived <= sizeof(qint64)*2)
    {
        if((clientConnection->bytesAvailable() >= sizeof(qint64)*2) && (fileNameSize == 0))
        {
            in>>TotalBytes>>fileNameSize;
            bytesReceived += sizeof(qint64)*2;
        }
        if((clientConnection->bytesAvailable() >= fileNameSize) && (fileNameSize != 0))
        {
            in>>fileName;
            bytesReceived +=fileNameSize;
            //>@若需另存为则去掉下面一句
            localFile = new QFile(QDir::currentPath()+QString("/")+fileName);
            qDebug()<<"---+++"<<QDir::currentPath()+fileName;
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
        bytesReceived += clientConnection->bytesAvailable();
        inBlock = clientConnection->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }
    qDebug()<<bytesReceived<<"received"<<TotalBytes;
    if(bytesReceived == TotalBytes)
    {
        localFile->close();   //接收完文件后，一定要关闭，不然可能出问题
    }
}

void TcpServer::slot_displayError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QTcpSocket::RemoteHostClosedError)
        return;
    QString tmpString = QString("Socket server generate errors as follows: %1.").arg(clientConnection->errorString());
    qDebug()<<tmpString;
    clientConnection->close();
}

void TcpServer::on_serverOpenBtn_clicked()  //打开
{
    fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        theFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
        ui->serverStatusLabel->setText(tr("The files to be transferred：%1 ").arg(theFileName));
        ui->serverSendBtn->setEnabled(true);
        ui->serverOpenBtn->setEnabled(false);
    }
}

void TcpServer::refused()   //被对方拒绝
{
    tcpServer->close();
    ui->serverStatusLabel->setText(tr("Target rejected！！！"));
}

void TcpServer::on_serverSendBtn_clicked()  //发送
{
    ui->serverStatusLabel->setText(tr("Waiting for Target to receive... ..."));
    emit sendFileName();
}

void TcpServer::on_serverCloseBtn_clicked()//退出
{   
    if(tcpServer->isListening())
    {
        tcpServer->close();
        clientConnection->abort();
    }
    this->close();
}



