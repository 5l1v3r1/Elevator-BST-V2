#include "nodeudp.h"

NodeUdp::NodeUdp(QObject *parent) :
    PhysicalDevice(parent)
{
    mUdpSocket = 0;
    mPort = 0;
    mCon = false;
}

NodeUdp::~NodeUdp()
{
    mPort = 0;
    mCon = false;
    Close();
}

bool NodeUdp::OpenUdp(quint16 pPort)
{
    Close();
    mUdpSocket = new QUdpSocket(this);
    if(!mUdpSocket->bind(QHostAddress("0.0.0.0"), pPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
        return false;
    mPort = pPort;
    mCon = true;
    connect(mUdpSocket, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
    return true;
}

void NodeUdp::Close()
{
    if(mUdpSocket != 0)
    {
        disconnect(mUdpSocket, SIGNAL(readyRead()), 0, 0);
        mUdpSocket->abort();
        mUdpSocket->close();
        mUdpSocket->deleteLater();
        mUdpSocket = 0;
    }
}

qint64 NodeUdp::write(const QByteArray &data)
{
    return (mUdpSocket->writeDatagram(data, data.length(), QHostAddress::Broadcast, mPort));
}

qint64 NodeUdp::write(QByteArray data, QHostAddress host)
{
    return (mUdpSocket->writeDatagram(data, data.length(), host, mPort));
}

qint64 NodeUdp::echo()
{
    return (mUdpSocket->writeDatagram(m_RevByteArray, m_RevByteArray.length(), m_SenderHost, mPort));
}

//>@回复发送者信息
qint64 NodeUdp::reply(const QByteArray &data)
{
    return (mUdpSocket->writeDatagram(data, data.length(), m_SenderHost, mPort));
}

QByteArray NodeUdp::readAll()
{
    QHostAddress senderIP;
    quint16 sendPort;
    m_RevByteArray.clear();
    m_RevByteArray.resize(mUdpSocket->pendingDatagramSize());
    mUdpSocket->readDatagram(m_RevByteArray.data(), m_RevByteArray.size(), &senderIP, &sendPort);
#ifdef UDPLOCALFLITER
    if(IsLocalIP(senderIP, sendPort))
        return QByteArray();
#endif
    m_SenderHost = senderIP;
    m_SenderPort = sendPort;
    return m_RevByteArray;
}
