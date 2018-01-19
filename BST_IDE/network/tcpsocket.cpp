#include "tcpsocket.h"

TcpServer::TcpServer(QObject *parent)
    :QObject(parent)
{
    mTcpserver = new QTcpServer(this);
    connect(mTcpserver, SIGNAL(newConnection()), this, SLOT(slot_PHConn()));
    m_DisConnMapper = new QSignalMapper;
    m_ReadReadyMapper = new QSignalMapper;
}

TcpServer::~TcpServer()
{
    if(m_DisConnMapper)
        m_DisConnMapper->deleteLater();
    if(m_ReadReadyMapper)
        m_ReadReadyMapper->deleteLater();
    QStringList tmpList = m_SocketList.keys();
    for(int i=tmpList.count()-1;i>=0;i--)
    {
        QList<QTcpSocket*> tmpSocketList = m_SocketList.values(tmpList.at(i));
        for(int j=tmpSocketList.count()-1;j>=0;j--)
        {
            QTcpSocket* tmpNode = tmpSocketList.at(i);
            if(tmpNode)
            {
                disconnect(tmpNode, SIGNAL(disconnected()), 0, 0);
                disconnect(tmpNode, SIGNAL(readyRead()), 0, 0);
                tmpNode->abort();
                tmpNode->close();
                tmpNode->deleteLater();
            }
        }
    }
    if(mTcpserver != 0)
    {
        disconnect(mTcpserver, SIGNAL(newConnection()), 0, 0);
        mTcpserver->deleteLater();
    }
}

void TcpServer::CloseServer()
{
    if(mTcpserver && mTcpserver->isListening())
    {
        mTcpserver->close();
        mTcpserver->deleteLater();
        mTcpserver = 0;
    }
}

bool TcpServer::OpenServer(QHostAddress pIP, quint16 pPort)
{
    if(mTcpserver->listen(pIP, pPort))
       return true;
    return false;
}

void TcpServer::slot_PHConn()
{
    QTcpSocket* tmpNode =  mTcpserver->nextPendingConnection();
    QString tmpIP = tmpNode->peerAddress().toString();

    connect(tmpNode, SIGNAL(readyRead()), m_ReadReadyMapper, SLOT(map()));
    m_ReadReadyMapper->setMapping(tmpNode, tmpNode);
    connect(m_ReadReadyMapper, SIGNAL(mapped(QObject*)), this, SLOT(slot_ReadAll(QObject*)));

    connect(tmpNode, SIGNAL(disconnected()), m_DisConnMapper, SLOT(map()));
    m_DisConnMapper->setMapping(tmpNode, tmpNode);
    connect(m_DisConnMapper, SIGNAL(mapped(QObject*)), this, SLOT(slot_PHDisconn(QObject*)));

    m_SocketList.insert(tmpIP, tmpNode);
    IDE_TRACE_STR(tmpIP);
}

void TcpServer::slot_PHDisconn(QObject* pObject)
{
    QTcpSocket* tmpNode =  VAR_CAST<QTcpSocket*>(pObject);
    if(tmpNode == 0)
        return;
    QString tmpIP = tmpNode->peerAddress().toString();
    m_SocketList.remove(tmpIP, tmpNode);
    IDE_TRACE_STR(tmpIP);
}

void TcpServer::slot_ReadAll(QObject* pObject)
{
    QTcpSocket* tmpSrcNode =  VAR_CAST<QTcpSocket*>(pObject);
    if(tmpSrcNode == 0)
        return;
    emit sNewDataCome(tmpSrcNode);
    QString tmpIP = tmpSrcNode->peerAddress().toString();
    IDE_TRACE_STR(tmpIP);
}

