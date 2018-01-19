#include "logicnetdiscovery.h"
#ifdef IDE
    #include "devicescene.h"
#elif DISPLAY
    #include "devicemanager.h"
#endif

LogicNetDiscovery::LogicNetDiscovery(QObject *parent):
    LogicDevice(parent)
{
    m_PHDev = 0;
}

LogicNetDiscovery::~LogicNetDiscovery()
{
}

void LogicNetDiscovery::SyncDev(QIODevice *pIODevice, PH_Type pType)
{
    m_PHType = pType;
    m_PHDev = pIODevice;
}

QString LogicNetDiscovery::GetLocalAddress()
{
    QString tmpAddr;
    if(!m_PHDev)
        return tmpAddr;
    //>@要么为网络设备中的UDP，要么为其他设备
    ToolUdp *tmpUdp = VAR_CAST<ToolUdp*>(m_PHDev);
    if(tmpUdp)
    {
        tmpAddr = QString("%1:%2").arg(tmpUdp->mUdpSocket->localAddress().toString())
                                .arg(tmpUdp->mPort);
    }
    else
    {
        PhysicalDevice* tmpPHDev = VAR_CAST<PhysicalDevice*>(m_PHDev);
        if(!tmpPHDev)
            return tmpAddr;
    }
    return tmpAddr;
}

QString LogicNetDiscovery::GetPeerAddress()
{
    QString tmpAddr;
    if(!m_PHDev)
        return tmpAddr;
    //>@要么为网络设备中的UDP，要么为其他设备
    ToolUdp *tmpUdp = VAR_CAST<ToolUdp*>(m_PHDev);
    if(tmpUdp)
    {
        tmpAddr = QString("%1:%2").arg(tmpUdp->m_SenderHost.toString())
                                .arg(tmpUdp->m_SenderPort);
    }
    else
    {
        PhysicalDevice* tmpPHDev = VAR_CAST<PhysicalDevice*>(m_PHDev);
        if(!tmpPHDev)
            return tmpAddr;
    }
    return tmpAddr;
}

QStringList LogicNetDiscovery::AppendPHDevices(QStringList pPHDevs)
{
    QStringList tmpList;
    DeletePHDevices();
    for(int i=0;i<pPHDevs.count();i++)
    {
        PH_Type tmpType = GetPHType(pPHDevs.at(i));
        if(tmpType == P_INVALID)
            continue;
#ifdef IDE
        PhysicalDevice *tmpPHDev = m_DeviceScene->m_PHDevList.value(tmpType);
#elif DISPLAY
        PhysicalDevice *tmpPHDev = m_DevManager->m_PHDevList.value(tmpType);
#endif
        if(!tmpPHDev)
            continue;
        if(!tmpPHDev->mLGDevices.contains(mDevType))
            tmpPHDev->mLGDevices.insert(mDevType, this);
        //>@
        switch(tmpType)
        {
            case P_LAN:
            {
                PhysicalLAN *tmpNetwork = VAR_CAST<PhysicalLAN*>(tmpPHDev);
                if(!tmpNetwork)
                    continue;
                connect(tmpNetwork->m_SharedUdp, SIGNAL(readyRead()), this, SLOT(ParseDataStream()));
                break;
            }
            case P_SodimasCAN:
            {
                connect(tmpPHDev, SIGNAL(readyRead()), this, SLOT(ParseDataStream()));
                break;
            }
            default:continue;
        }
        mPHDevices.insert(tmpType, tmpPHDev);
        tmpList.append(pPHDevs.at(i));
    }
    return tmpList;
}

void LogicNetDiscovery::DeletePHDevices()
{
    QList<PH_Type> tmpList = mPHDevices.keys();
    for(int i=0;i<tmpList.count();i++)
    {
        PhysicalDevice* tmpPHDev = mPHDevices.take(tmpList.at(i));
        if(!tmpPHDev)
            continue;
        switch(tmpPHDev->mDevType)
        {
            case P_LAN:
            {
                PhysicalLAN *tmpNetwork = VAR_CAST<PhysicalLAN*>(tmpPHDev);
                if(!tmpNetwork)
                    continue;
                disconnect(tmpNetwork->m_SharedUdp, SIGNAL(readyRead()), this, SLOT(ParseDataStream()));
                break;
            }
            case P_SodimasCAN:
            {
                disconnect(tmpPHDev, SIGNAL(readyRead()), this, SLOT(ParseDataStream()));
                break;
            }
            default:continue;
        }
    }
    mPHDevices.clear();
}

quint32 LogicNetDiscovery::InitPara()
{
    QDomNodeList tmpParaNodeList = m_ParaElement.childNodes();
    if(tmpParaNodeList.count() < 1)
        return 0;
    quint32 validcount = 0;
    QHBoxLayout* tmpHBoxLayout;
    for(int i=0;i<tmpParaNodeList.count();i++)
    {
        QDomElement tmpElement = tmpParaNodeList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(!tmpName.compare("Device"))
        {
            if(tmpElement.text().isEmpty())
                continue;
            QStringList tmpDevList = tmpElement.text().split(';');
            AppendPHDevices(tmpDevList);
            tmpHBoxLayout = CreateCheck(tmpName, tmpElement.attribute("List").split(';'), tmpDevList);
        }
        else
        {
            continue;
        }
        validcount++;
        m_SettingGroup->m_VLayout->addWidget(CreateLine());
        m_SettingGroup->m_VLayout->addLayout(tmpHBoxLayout);
    }
    validcount++;
    return validcount;
}

void LogicNetDiscovery::UpdatePara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("Device"))
    {
        CheckList *tmpComponent = VAR_CAST<CheckList *>(pObject);
        if(tmpComponent)
        {
            QStringList tmpDevList = tmpComponent->GetSelectList();
            QStringList tmpList = PHDevicesAdded(tmpDevList, tmpComponent);
            if(!ModifyCheck(tmpComponent, tmpElement, tmpComponent->m_SelectList, tmpList))
            {
            }
        }
    }
}

qint64 LogicNetDiscovery::SendData(QStringList pIPList, QString pData)
{
    if(!m_PHDev)
        return -1;
    qint64 tmpWriteBytes = 0;
    //>@要么为网络设备中的UDP，要么为其他设备
    ToolUdp *tmpUdp = VAR_CAST<ToolUdp*>(m_PHDev);
    if(tmpUdp)
    {
        for(int i=0;i<pIPList.count();i++)
            tmpWriteBytes += tmpUdp->write(pData.toAscii(), QHostAddress(pIPList.at(i)));
    }
    else
    {
        PhysicalDevice* tmpPHDev = VAR_CAST<PhysicalDevice*>(m_PHDev);
        if(!tmpPHDev)
            return -1;
        for(int i=0;i<pIPList.count();i++)
            tmpWriteBytes += tmpPHDev->write(pData.toAscii(), pIPList.at(i));
    }
    return tmpWriteBytes;
}

//>@应答发送者
qint64 LogicNetDiscovery::ReplySender(QString pData)
{
    if(!m_PHDev)
        return -1;
    //>@要么为网络设备中的UDP，要么为其他设备
    ToolUdp *tmpUdp = VAR_CAST<ToolUdp*>(m_PHDev);
    if(tmpUdp)
    {
        return tmpUdp->reply(pData.toAscii());
    }
    else
    {
        PhysicalDevice* tmpPHDev = VAR_CAST<PhysicalDevice*>(m_PHDev);
        if(!tmpPHDev)
            return -1;
        return tmpPHDev->write(pData.toAscii());
    }
    return -1;
}

//>@广播发送
qint64 LogicNetDiscovery::SendData(QString pData)
{
    if(!m_PHDev)
        return -1;
    //>@要么为网络设备中的UDP，要么为其他设备
    ToolUdp *tmpUdp = VAR_CAST<ToolUdp*>(m_PHDev);
    if(tmpUdp)
    {
        return tmpUdp->write(pData.toAscii());
    }
    else
    {
        PhysicalDevice* tmpPHDev = VAR_CAST<PhysicalDevice*>(m_PHDev);
        if(!tmpPHDev)
            return -1;
        return tmpPHDev->write(pData.toAscii());
    }
    return -1;
}

void LogicNetDiscovery::ParseDataStream()
{
    QIODevice* tmpIODev = VAR_CAST<QIODevice*>(sender());
    if(tmpIODev == 0)
        return;
    m_PHDev = tmpIODev;
    QString tmpBufferData;
    //>@要么为网络设备中的UDP，要么为其他设备
    ToolUdp *tmpUdp = VAR_CAST<ToolUdp*>(m_PHDev);
    if(tmpUdp)
    {
        tmpBufferData = QString(tmpUdp->readAll());
    }
    else
    {
        PhysicalDevice* tmpPHDev = VAR_CAST<PhysicalDevice*>(m_PHDev);
        if(!tmpPHDev)
            return;
        tmpBufferData = QString(tmpPHDev->readAll());
    }
    ParseFrameStream(tmpBufferData);
}

/*******************************************************
//>@
*******************************************************/
HostNetDiscovery::HostNetDiscovery(QObject *parent):
    LogicNetDiscovery(parent)
{
}

HostNetDiscovery::~HostNetDiscovery()
{
}

//>@广播
bool HostNetDiscovery::Broadcast(QString pMessage)
{
    if(SendData(D_HOSTPREFIX + pMessage))
        return true;
    return false;
}
//>@组播
bool HostNetDiscovery::Groupcast(QStringList pIPList, QString pMessage)
{
    if(SendData(pIPList, D_HOSTPREFIX + pMessage))
        return true;
    return false;
}
//>@单播，只用于回复消息发送者
bool HostNetDiscovery::Singlecast(QString pMessage)
{
    if(ReplySender(D_HOSTPREFIX + pMessage))
        return true;
    return false;
}

QStringList HostNetDiscovery::ScanDevIP(quint16 pTimeout)
{
    mIPList.clear();
    if(Broadcast(D_CMDSCANIP) == false)
        return mIPList;
    while(pTimeout--)
        QtSleep(1);
    return mIPList;
}

bool HostNetDiscovery::ParseFrameStream(QString pFrame)
{
    if(pFrame.isEmpty())
        return false;
    if(!pFrame.startsWith(D_GUESTPREFIX))
        return false;
    pFrame.remove(0, D_GUESTPREFIX.length());
    QStringList paraList = pFrame.split(D_CONTENTSPLIT);
    quint8 paraCnt = paraList.count();
    if(!paraCnt)
        return false;
    QString type = paraList.at(0);
    if(type.compare(D_CMDSCANIP, Qt::CaseInsensitive) == 0)
    {
        QString tmpAddr = GetPeerAddress();
        if(tmpAddr.isEmpty())
            return false;
        mIPList.append(tmpAddr);
    }
    return true;
}
/*******************************************************
//>@
*******************************************************/
GuestNetDiscovery::GuestNetDiscovery(QObject *parent):
    LogicNetDiscovery(parent)
{
}

GuestNetDiscovery::~GuestNetDiscovery()
{
    disconnect(this,SIGNAL(sCommand(QString)),0,0);
    disconnect(this,SIGNAL(sInform(QString)),0,0);
}

//>@单播，只用于回复消息发送者
bool GuestNetDiscovery::Singlecast(QString pMessage)
{
    if(ReplySender(D_GUESTPREFIX + pMessage))
        return true;
    return false;
}

bool GuestNetDiscovery::ParseFrameStream(QString pFrame)
{
    if(pFrame.isEmpty())
        return false;
    if(!pFrame.startsWith(D_HOSTPREFIX))
        return false;
    pFrame.remove(0, D_HOSTPREFIX.length());
    QStringList paraList = pFrame.split(D_CONTENTSPLIT);
    quint8 paraCnt = paraList.count();
    if(!paraCnt)
        return false;
    QString type = paraList.at(0);
    if(type.compare(D_CMDSCANIP, Qt::CaseInsensitive) == 0)
    {
        Singlecast(pFrame);
    }
    return true;
}
