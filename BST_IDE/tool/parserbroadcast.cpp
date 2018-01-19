#include "parserbroadcast.h"
#include "nodeudp.h"

parserBroadcast::parserBroadcast(QObject *parent):
    LogicDevice(parent)
{}

parserBroadcast::~parserBroadcast()
{
    IDE_TRACE();
}

QString parserBroadcast::getIP()  //获取ip地址
{
    NodeUdp *tmpNodeUdp = VAR_CAST<NodeUdp *>(m_PHDev);
    if(tmpNodeUdp == 0)
        return QString();
    QString tmpSenderIP = tmpNodeUdp->m_SenderHost.toString();
    QStringList tmpIPList = GetLocalIPList();
    for(int i=0;i<tmpIPList.count();i++)
    {
        ENET_ADDR tmpAddr = tmpIPList.at(i);
        if(tmpAddr.isValid() && tmpAddr.isOnSameNet(tmpSenderIP))
        {
            return tmpIPList.at(i);
        }
    }
    return QString();
}

qint64 parserBroadcast::SendData(QStringList pIPList, QString pData)
{
    if(!ISDevActive())
    {
        IDE_TRACE_STR("SendData#DevUnactive#Failed");
        return -1;
    }
    NodeUdp *tmpNodeUdp = (NodeUdp *)(m_PHDev);
    if(tmpNodeUdp == 0)
    {
        IDE_TRACE_STR("NodeUdp#DevUnactive#Failed");
        return -1;
    }
    qint64 tmpWriteBytes = 0;
    for(int i=0;i<pIPList.count();i++)
        tmpWriteBytes + tmpNodeUdp->write(pData.toAscii(), QHostAddress(pIPList.at(i)));
    return tmpWriteBytes;
}

//>@应答发送者
qint64 parserBroadcast::ReplySender(QString pData)
{
    if(!ISDevActive())
    {
        IDE_TRACE_STR("SendData#DevUnactive#Failed");
        return -1;
    }
    NodeUdp *tmpNodeUdp = (NodeUdp *)(m_PHDev);
    if(tmpNodeUdp == 0)
    {
        IDE_TRACE_STR("NodeUdp#DevUnactive#Failed");
        return -1;
    }
    return tmpNodeUdp->reply(pData.toAscii());
}

//>@广播发送
qint64 parserBroadcast::SendData(QString pData)
{
    if(!ISDevActive())
    {
        IDE_TRACE_STR("SendData#DevUnactive#Failed");
        return -1;
    }
    return m_PHDev->write(pData.toAscii());
}

void parserBroadcast::ParseDataStream(QObject* pSender)
{
    PhysicalDevice* PHDev = VAR_CAST<PhysicalDevice*>(pSender);
    if(PHDev == 0)
    {
        IDE_TRACE_STR("System#Err");
        return;
    }
    SyncDev(PHDev);
    QByteArray byteArray = m_PHDev->readAll();
    QString info = QString(byteArray);
    byteArray.clear();
    ReturnInfo(info);
}

/*******************************************************
//>@
*******************************************************/
BCHostParser::BCHostParser(QObject *parent):
    parserBroadcast(parent)
{}

BCHostParser::~BCHostParser()
{}

//>@广播
bool BCHostParser::Broadcast(QString pMessage)
{
    if(SendData(pMessage))
        return true;
    return false;
}
//>@组播
bool BCHostParser::Groupcast(QStringList pIPList, QString pMessage)
{
    if(SendData(pIPList, pMessage))
        return true;
    return false;
}
//>@单播，只用于回复消息发送者
bool BCHostParser::Singlecast(QString pMessage)
{
    if(ReplySender(pMessage))
        return true;
    return false;
}

bool BCHostParser::ReturnInfo(QString &pInfo)
{
    if(parserBroadcast::ReturnInfo(pInfo))
        return true;
    if(pInfo.isEmpty())
        return true;
    QStringList paraList = pInfo.split(";");
    quint8 paraCnt = paraList.count();
    if(paraCnt < 2)
        return true;
    QString type = paraList.at(0);
    QString content = pInfo.remove(0, type.length()+1);
    if(type.compare("Message", Qt::CaseInsensitive) == 0)
    {
        emit sMessage(content);
        return false;
    }
    else if(type.compare("Command", Qt::CaseInsensitive) == 0)
    {
        QString subType = paraList.at(1);
        if(subType.compare("ScanIP", Qt::CaseInsensitive) == 0)
        {
            if(paraCnt == 2)
            {
#ifdef UDPLOCALFLITER
                SendPcIP();
                return true;
#endif
            }
            else if(paraCnt == 5)
            {
                mIPList.append(content);
                return true;
            }
        }
        return true;
    }
    return true;
}
/*******************************************************
//>@
*******************************************************/
BCDevParser::BCDevParser(QString pName, QObject *parent):
    parserBroadcast(parent)
{
    SetDevname(pName);
}

BCDevParser::~BCDevParser()
{
    disconnect(this,SIGNAL(sCommand(QString)),0,0);
    disconnect(this,SIGNAL(sInform(QString)),0,0);
}

bool BCDevParser::ReturnInfo(QString &pInfo)
{
    if(parserBroadcast::ReturnInfo(pInfo))
        return true;
    if(pInfo.isEmpty())
        return true;
    QStringList paraList = pInfo.split(";");
    quint8 paraCnt = paraList.count();
    if(paraCnt < 2)
        return true;
    QString type = paraList.at(0);
    QString content = pInfo.remove(0, type.length()+1);
    if(type.compare("Message", Qt::CaseInsensitive) == 0)
    {
        emit sMessage(content);
        return false;
    }
    else if(type.compare("Command", Qt::CaseInsensitive) == 0)
    {
        QString subType = paraList.at(1);
        if(subType.compare("ScanIP", Qt::CaseInsensitive) == 0)
        {
            if(paraCnt == 2)
            {
                SendDevIP();
            }
            return true;
        }
        emit sCommand(content);
        return false;
    }
    else if(type.compare("Inform", Qt::CaseInsensitive) == 0)
    {
        emit sInform(content);
        return false;
    }
    return false;
}
