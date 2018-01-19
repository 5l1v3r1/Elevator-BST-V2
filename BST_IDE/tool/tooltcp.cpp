#include "tooltcp.h"

ToolTcpServer::ToolTcpServer(QObject *parent)
    :QTcpServer(parent)
{
}

ToolTcpServer::~ToolTcpServer()
{
    CloseServer();
}

bool ToolTcpServer::OpenServer(QHostAddress pIP, quint16 pPort)
{
    CloseServer();
    if(listen(pIP, pPort))
       return true;
    return false;
}

void ToolTcpServer::CloseServer()
{
    if(isListening())
        close();
}

qint64 ToolTcpServer::write(uint pType, QByteArray data)
{
    if(pType <= 0)
        return -1;
    TcpSocketPtr tmpPtr = m_ClientSockets.key(pType);
    QTcpSocket *tmpSocket = (QTcpSocket *)tmpPtr;
    if(!tmpSocket)
        return -1;
    return tmpSocket->write(data);
}

uint ToolTcpServer::GetClientType(QTcpSocket *pSocket)
{
    TcpSocketPtr tmpPtr = (TcpSocketPtr)pSocket;
    return m_ClientSockets.value(tmpPtr);
}

void ToolTcpServer::incomingConnection(int handle)
{
    QTcpSocket* tmpSocket = new QTcpSocket;
    if(!tmpSocket->setSocketDescriptor(handle))
    {
        tmpSocket->deleteLater();
        return;
    }
    connect(tmpSocket, SIGNAL(disconnected()), this, SLOT(slot_RemoveClient()));
    connect(tmpSocket, SIGNAL(readyRead()), this, SLOT(slot_GetClientType()));
    TcpSocketPtr tmpPtr = (TcpSocketPtr)tmpSocket;
    m_ClientSockets.insert(tmpPtr, 0);
}

void ToolTcpServer::slot_RemoveClient()
{
    QTcpSocket* tmpSocket = VAR_CAST<QTcpSocket*>(sender());
    if(!tmpSocket)
        return;
    TcpSocketPtr tmpPtr = (TcpSocketPtr)tmpSocket;

    //>@发送客户机断开的信号
    emit sRemoveSocket(tmpSocket);

    m_ClientSockets.remove(tmpPtr);
    tmpSocket->deleteLater();
}

void ToolTcpServer::slot_GetClientType()
{
    QTcpSocket* tmpSocket = VAR_CAST<QTcpSocket*>(sender());
    if(!tmpSocket)
        return;
    QByteArray tmpFrame = tmpSocket->readAll();   //>@
    if(tmpFrame.count() < 6)
        return;
    if(tmpFrame.at(0) != 0x5a || tmpFrame.at(1) != 0xa5)
        return;
    //>@关闭此Socket的接收类型的槽
    disconnect(tmpSocket, SIGNAL(readyRead()), this, SLOT(slot_GetClientType()));
    //>@
    uint tmpClientType = (tmpFrame.at(2)<<24) + (tmpFrame.at(3)<<16) + (tmpFrame.at(4)<<8) + tmpFrame.at(5);
    TcpSocketPtr tmpPtr = (TcpSocketPtr)tmpSocket;
    m_ClientSockets.insert(tmpPtr, tmpClientType);

    //>@发送检测到有用途的客户机信号
    sNewSocket(tmpSocket, tmpClientType);
}

/******************************************************
//>@
******************************************************/
ToolTcpClient::ToolTcpClient(uint pClientType, QObject *parent)
    :QTcpSocket(parent)
{
    mClientType = pClientType;
    connect(this, SIGNAL(connected()), this, SLOT(slot_Conn()));
    connect(this, SIGNAL(disconnected()), this, SLOT(slot_Disconn()));
}

ToolTcpClient::~ToolTcpClient()
{
    abort();
    close();
}

void ToolTcpClient::CloseClient()
{
    if(state() == ConnectedState)
        disconnectFromHost();
}

bool ToolTcpClient::OpenClient(QHostAddress pIP, quint16 pPort, quint32 pTimeout)
{
    quint32 delayTime = pTimeout / 2;
    CloseClient();
    QtSleep(delayTime);
    connectToHost(pIP, pPort);
    while((delayTime --) && (state() != ConnectedState))
        QtSleep(1);
    if(state() == ConnectedState)
        return true;
    return false;
}

void ToolTcpClient::OpenClient(QHostAddress pIP, quint16 pPort)
{
    connectToHost(pIP, pPort);
}

void ToolTcpClient::slot_Conn()
{
     //>@发送自己的类型帧
     char tmpData[6] = {0x5a, 0xa5, mClientType>>24, mClientType>>16, mClientType>>8, mClientType};
     write(QByteArray(tmpData));

     emit sConnState(true);
}

void ToolTcpClient::slot_Disconn()
{
    emit sConnState(false);
}

