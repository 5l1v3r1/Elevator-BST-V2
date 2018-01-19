#include "physicalnet.h"
#include "logicbase.h"

PhysicalSocket::PhysicalSocket(QObject *pParent) :
    PhysicalDevice(pParent)
{
    mTcpSocket = new QTcpSocket;
    connect(mTcpSocket, SIGNAL(readyRead()), this, SLOT(slot_ReadData()));
    //>@对于此种类型的socket，需要向外发出建立连接/释放连接的信号，以供其它类使用
    connect(mTcpSocket, SIGNAL(connected()), this, SLOT(slot_Conn()));
    connect(mTcpSocket, SIGNAL(disconnected()), this, SLOT(slot_Disconn()));
}

PhysicalSocket::PhysicalSocket(QTcpSocket *pSocket, QObject *pParent) :
    PhysicalDevice(pParent)
{
    mTcpSocket = pSocket;
    if(mTcpSocket)
    {
        connect(mTcpSocket, SIGNAL(readyRead()), this, SLOT(slot_ReadData()));
        //>@对于此种初始化的Socket，因为是在其他处初始化，所以此处不需要处理连接和断开连接的信号
    }
}

PhysicalSocket::~PhysicalSocket()
{
    if(mTcpSocket)
    {
        mTcpSocket->deleteLater();
    }
}

void PhysicalSocket::Close()
{
    if(isConnected())
        mTcpSocket->disconnectFromHost();
}

bool PhysicalSocket::Open(QHostAddress pIP, quint16 pPort, quint32 pTimeout)
{
    if(isConnected())
        return false;  //>@表示已经建立连接不需要再次建立
    quint32 tmpCloseDelayTime = pTimeout/4;
    Close();
    while((tmpCloseDelayTime --) && (mTcpSocket->state() == QAbstractSocket::ConnectedState))
        QtSleep(1);
    if(isConnected())
        return false;  //>@表示没有能够释放上次连接

    quint32 tmpConnDelayTime = pTimeout*3/4;
    mTcpSocket->connectToHost(pIP, pPort);
    while((tmpConnDelayTime --) && (mTcpSocket->state() != QAbstractSocket::ConnectedState))
        QtSleep(1);
    return isConnected();
}

void PhysicalSocket::Open(QHostAddress pIP, quint16 pPort)
{
    if(isConnected())
        return;
    mTcpSocket->connectToHost(pIP, pPort);
}

void PhysicalSocket::slot_Conn()
{
     //>@发送自己的类型帧
     char tmpData[6] = {0x5a, 0xa5, mAppType>>24, mAppType>>16, mAppType>>8, mAppType};
     mTcpSocket->write(QByteArray(tmpData));
     //>@发送信号
     emit sConnState(true);
}

void PhysicalSocket::slot_Disconn()
{
    emit sConnState(false);
}

QHostAddress PhysicalSocket::GetPeerIP() { return mTcpSocket->peerAddress(); }

quint16 PhysicalSocket::GetPeerPort() { return mTcpSocket->peerPort(); }

QHostAddress PhysicalSocket::GetLocalIP() { return mTcpSocket->localAddress(); }

quint16 PhysicalSocket::GetLocalPort() { return mTcpSocket->localPort(); }

NetManagerSocket::NetManagerSocket(QObject *pParent) :
    PhysicalSocket(pParent)
{
    mAppType = (uint)L_NetManager;
}

NetManagerSocket::NetManagerSocket(QTcpSocket *pSocket, QObject *pParent) :
    PhysicalSocket(pSocket, pParent)
{
    mAppType = (uint)L_NetManager;
}

NetManagerSocket::~NetManagerSocket()
{

}

QByteArray NetManagerSocket::readAll()
{
    return m_FrameBuffer;
}

qint64 NetManagerSocket::write(QByteArray data)
{
    if(!mTcpSocket)
        return -1;
    QByteArray tmpBuffer;
    quint64 size = data.count();
    tmpBuffer.clear();
    tmpBuffer.append(0x5a);
    tmpBuffer.append(0x5a);
    tmpBuffer.append(size/256);
    tmpBuffer.append(size%256);
    tmpBuffer.append(data);
    quint64 ret = mTcpSocket->write(data);
    tmpBuffer.clear();
    return ret;
}

void NetManagerSocket::slot_ReadData()
{
    static quint16 sCurFrameDataLen = 0;
    QTcpSocket* tmpSocket = VAR_CAST<QTcpSocket*>(sender());
    if(!tmpSocket)
        return;
    QByteArray tmpBufferData = tmpSocket->readAll();
    int tmpCount = tmpBufferData.count();
    if(tmpCount < sizeof(quint32))
    {
        tmpBufferData.clear();
        return;
    }
    int i=0;
    quint8 tmpValue;
    while(i<tmpCount)
    {
        switch(m_FrameState)
        {
            case State_Wait:
            {
                m_DataBuffer.clear();
                sCurFrameDataLen = 0;
                for(;i<tmpCount;i++)
                {
                    tmpValue = (quint8)(tmpBufferData.at(i));
                    if(tmpValue == 0x5a)
                    {
                        m_FrameState = State_Header;
                        i++;
                        break;
                    }
                }
                break;
            }
            case State_Header:
            {
                tmpValue = (quint8)(tmpBufferData.at(i++));
                if(tmpValue == 0x5a)
                    m_FrameState = State_DatalenH;
                else
                    m_FrameState = State_Wait;
                break;
            }
            case State_DatalenH:
            {
                tmpValue = (quint8)(tmpBufferData.at(i++));
                sCurFrameDataLen = tmpValue*256;
                m_FrameState = State_DatalenL;
                break;
            }
            case State_DatalenL:  //>@查找数据长度
            {
                tmpValue = (quint8)(tmpBufferData.at(i++));
                sCurFrameDataLen += tmpValue;
                m_FrameState = State_Continus;
                break;
            }
            case State_Continus:
            {
                quint16 tmpLen = tmpCount-i;
                if(tmpLen  >= sCurFrameDataLen)
                {
                    m_DataBuffer.append(tmpBufferData.mid(i, sCurFrameDataLen));
                    m_FrameState = State_Wait;
                    i += sCurFrameDataLen;
                    sCurFrameDataLen = 0;
                    m_FrameBuffer.clear();
                    m_FrameBuffer.append(m_DataBuffer);
                    emit readyRead();
                }
                else if(tmpLen < sCurFrameDataLen)
                {
                    m_DataBuffer.append(tmpBufferData.mid(i));
                    i += tmpLen;
                    sCurFrameDataLen -= tmpLen;
                }
                break;
            }
            default:
                break;
        }
    }
    tmpBufferData.clear();
}

PhysicalLAN::PhysicalLAN(QObject *parent):
    PhysicalDevice(parent)
{
    m_SharedUdp = 0;
    m_SharedTcp = 0;
    OpenNetDiscovery();
    OpenNetManager();
}

PhysicalLAN::~PhysicalLAN()
{
    for(int i=m_SocketParsers.count()-1;i>=0;i--)
    {
        PhysicalSocket *tmpParser = m_SocketParsers.takeAt(i);
        if(tmpParser)
            tmpParser->deleteLater();
    }
    CloseNetDiscovery();
    CloseNetManager();
}

void PhysicalLAN::OpenNetDiscovery()
{
    if(!m_SharedUdp)
    {
        m_SharedUdp = new ToolUdp(this);
        if(!m_SharedUdp->OpenUdp(D_CMDPORT))
        {
            m_SharedUdp->deleteLater();
            m_SharedUdp = 0;
        }
    }
}

void PhysicalLAN::CloseNetDiscovery()
{
    if(m_SharedUdp)
    {
        m_SharedUdp->deleteLater();
        m_SharedUdp = 0;
    }
}

void PhysicalLAN::OpenNetManager()
{
    if(!m_SharedTcp)
    {
        m_SharedTcp = new ToolTcpServer;
        connect(m_SharedTcp, SIGNAL(sNewSocket(QTcpSocket*, uint)), this, SLOT(slot_NewSocket(QTcpSocket*, uint)));
        m_SharedTcp->OpenServer(QHostAddress::Any, D_NMPORT);
    }
}

void PhysicalLAN::CloseNetManager()
{
    if(m_SharedTcp)
    {
        m_SharedTcp->deleteLater();
        m_SharedTcp = 0;
    }
}

void PhysicalLAN::slot_NewSocket(QTcpSocket *pSocket, uint pType)
{
    if(!pSocket)
        return;
    LG_Type tmpType = (LG_Type)pType;
    if(!mLGDevices.contains(tmpType))
        return;
    if(tmpType == L_NetManager)
    {
        LogicDevice *tmpDev = mLGDevices.value(tmpType);
        NetManagerSocket *tmpParser = new NetManagerSocket(pSocket);
        connect(tmpParser, SIGNAL(readyRead()), tmpDev, SLOT(ParseDataStream()));
        m_SocketParsers.append(tmpParser);
    }
}


PhysicalWAN::PhysicalWAN(QObject *parent):
    PhysicalDevice(parent)
{
}

PhysicalWAN::~PhysicalWAN()
{
}

PhysicalRMII::PhysicalRMII(QObject *parent):
    PhysicalMovable(parent)
{
    mDevPath = QString("eth0");
}

PhysicalRMII::~PhysicalRMII()
{
}

bool PhysicalRMII::InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout)
{
    if(!pName.compare("Model"))
    {
        mModel = pElement.text();
        pHBoxLayout = CreateEnum(pName, pElement.attribute("List").split(';'), mModel);
    }
    else if(!pName.compare("Movable"))
    {
        QString tmpValue = pElement.text();
        mIsMovable = (tmpValue.compare("true",Qt::CaseInsensitive)==0)?true:false;
        pHBoxLayout = CreateBool(pName, tmpValue);
    }
    else if(!pName.compare("IP"))
    {
        QString tmpValue = pElement.text();
        m_IPAddr = tmpValue;
        pHBoxLayout = CreateENetAddrEdit(pName, tmpValue);
    }
    else if(!pName.compare("Submask"))
    {
        QString tmpValue = pElement.text();
        m_SubmaskAddr = tmpValue;
        pHBoxLayout = CreateENetAddrEdit(pName, tmpValue);
    }
    else if(!pName.compare("Gateway"))
    {
        QString tmpValue = pElement.text();
        m_GatewayAddr = tmpValue;
        pHBoxLayout = CreateENetAddrEdit(pName, tmpValue);
    }
    else if(!pName.compare("DHCP"))
    {
        QString tmpValue = pElement.text();
        m_DHCP = (tmpValue.compare("true",Qt::CaseInsensitive)==0)?true:false;
        pHBoxLayout = CreateBool(pName, tmpValue);
    }
    else
    {
        return false;
    }
    return true;
}

void PhysicalRMII::UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement)
{
    if(!pName.compare("Model"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpValue = tmpComponent->currentText();
            if(SetElementText(pElement, tmpValue))
                mModel = tmpValue;
            else
                tmpComponent->setCurrentText(mModel);
        }
    }
    else if(!pName.compare("Movable"))
    {
        Switch *tmpComponent = VAR_CAST<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, pElement, mIsMovable, tmpComponent->isChecked()))
            {
            }
        }
    }
    else if(!pName.compare("IP"))
    {
        ENetAddrEdit *tmpComponent = VAR_CAST<ENetAddrEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyENetAddrEdit(tmpComponent, pElement, m_IPAddr, tmpComponent->GetIP()))
            {
            }
        }
    }
    else if(!pName.compare("Submask"))
    {
        ENetAddrEdit *tmpComponent = VAR_CAST<ENetAddrEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyENetAddrEdit(tmpComponent, pElement, m_SubmaskAddr, tmpComponent->GetIP()))
            {
            }
        }
    }
    else if(!pName.compare("Gateway"))
    {
        ENetAddrEdit *tmpComponent = VAR_CAST<ENetAddrEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyENetAddrEdit(tmpComponent, pElement, m_GatewayAddr, tmpComponent->GetIP()))
            {
            }
        }
    }
    else if(!pName.compare("DHCP"))
    {
        Switch *tmpComponent = VAR_CAST<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, pElement, m_DHCP, tmpComponent->isChecked()))
            {
            }
        }
    }
}

void PhysicalRMII::OpenNetCard()
{
    SYSTEM_CMD::SetIP(mDevPath, m_IPAddr);
    SYSTEM_CMD::SetNetmask(mDevPath, m_SubmaskAddr);
    SYSTEM_CMD::SetGateway(m_GatewayAddr);
    SYSTEM_CMD::SetDNS(m_DNSAddr);
    if(m_DHCP)
    {
        m_DHCPConfFile = QString("%1%2_%3").arg(D_TEMP_PATH).arg(mDevPath).arg(D_DHCPCONF);
        SYSTEM_CMD::CreateDHCPConf(m_DHCPConfFile, mDevPath, m_IPAddr.toQString(),
                                                                    m_SubmaskAddr.toQString(),
                                                                    m_GatewayAddr.toQString(),
                                                                    m_DNSAddr.toQString());
        SYSTEM_CMD::SetDHCP(true, m_DHCPConfFile);
    }
    else
    {
        SYSTEM_CMD::SetDHCP(false, QString());
    }
    SYSTEM_CMD::AddDefaultNet(mDevPath);
}

void PhysicalRMII::CloseNetCard()
{
    if(m_DHCP)
        SYSTEM_CMD::SetDHCP(false, m_DHCPConfFile);
    SYSTEM_CMD::SetNetCard(false, mDevPath);
}

void PhysicalRMII::PreInitProcess()
{
    CloseNetCard();
}

void PhysicalRMII::PreUpdateProcess()
{
    CloseNetCard();
}

void PhysicalRMII::PostInitProcess()
{
    OpenNetCard();
}

void PhysicalRMII::PostUpdateProcess()
{
    OpenNetCard();
}

PhysicalWIFI::PhysicalWIFI(QObject *parent):
    PhysicalRMII(parent)
{
    mDevPath = QString("ra0");
}

PhysicalWIFI::~PhysicalWIFI()
{
}

bool PhysicalWIFI::SetPlugged()
{
    if(!SYSTEM_CMD::SetUSBWIFI(true))
        return false;
    OpenNetCard();
    return true;
}

bool PhysicalWIFI::SetUnplugged()
{
    CloseNetCard();
    if(!SYSTEM_CMD::SetUSBWIFI(false))
        return false;
    return true;
}

void PhysicalWIFI::PreInitProcess()
{
    if(mDevState == S_PLUGGED)
        CloseNetCard();
}

void PhysicalWIFI::PreUpdateProcess()
{
    if(mDevState == S_PLUGGED)
        CloseNetCard();
}

void PhysicalWIFI::PostInitProcess()
{
    //>@WIFI网卡必须在插上时才使能，所以此处仅仅加载驱动
#ifdef DISPLAY
    system("modprobe rtutil3070ap");
    system("modprobe rt3070ap");
    system("modprobe rtnet3070ap");
#endif
    if(mDevState == S_PLUGGED)
        OpenNetCard();
}

void PhysicalWIFI::PostUpdateProcess()
{
    if(mDevState == S_PLUGGED)
        OpenNetCard();
}

bool PhysicalWIFI::InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout)
{
    if(PhysicalRMII::InitSubPHPara(pName, pElement, pHBoxLayout))
        return true;
    if(!pName.compare("Model"))
    {
        mModel = pElement.text();
        pHBoxLayout = CreateEnum(pName, pElement.attribute("List").split(';'), mModel);
    }
    else if(!pName.compare("Mode"))
    {
        QString tmpValue = pElement.text();
        mMode = GetMode(tmpValue);
        pHBoxLayout = CreateEnum(pName, pElement.attribute("List").split(';'), tmpValue);
    }
    else if(!pName.compare("ESSID"))
    {
        mEssid = pElement.text();
        pHBoxLayout = CreateTextEdit(pName, mEssid);
    }
    else if(!pName.compare("PASSWD"))
    {
        mPasswd = pElement.text();
        pHBoxLayout = CreateTextEdit(pName, mPasswd);
    }
    else
    {
        return false;
    }
    return true;
}

void PhysicalWIFI::UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement)
{
    PhysicalRMII::UpdateSubPHPara(pName, pObject, pElement);
    if(!pName.compare("Model"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpValue = tmpComponent->currentText();
            if(SetElementText(pElement, tmpValue))
                mModel = tmpValue;
            else
                tmpComponent->setCurrentText(mModel);
        }
    }
    else if(!pName.compare("Mode"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpValue = tmpComponent->currentText();
            if(SetElementText(pElement, tmpValue))
                mMode = GetMode(tmpValue);
            else
                tmpComponent->setCurrentText(GetModeString(mMode));
        }
    }
    else if(!pName.compare("ESSID"))
    {
        TextEdit *tmpComponent = VAR_CAST<TextEdit *>(pObject);
        if(tmpComponent)
        {
            QString tmpValue = tmpComponent->text();
            if(SetElementText(pElement, tmpValue))
                mEssid = tmpValue;
            else
                tmpComponent->setText(mEssid);
        }
    }
    else if(!pName.compare("PASSWD"))
    {
        TextEdit *tmpComponent = VAR_CAST<TextEdit *>(pObject);
        if(tmpComponent)
        {
            QString tmpValue = tmpComponent->text();
            if(SetElementText(pElement, tmpValue))
                mPasswd = tmpValue;
            else
                tmpComponent->setText(mPasswd);
        }
    }
}


PhysicalGPRS::PhysicalGPRS(QObject *parent):
    PhysicalMovable(parent)
{
}

PhysicalGPRS::~PhysicalGPRS()
{
    SetUnplugged();
}

bool PhysicalGPRS::SetPlugged()
{
    if(!SYSTEM_CMD::SetGPRS(true))
        return false;
    return true;
}

bool PhysicalGPRS::SetUnplugged()
{
    if(!SYSTEM_CMD::SetNetCard(false, mDevPath))
        return false;
    if(!SYSTEM_CMD::SetGPRS(false))
        return false;
    return true;
}

void PhysicalGPRS::InitGPRS()
{
    if(mModel.compare("ZTE3100", Qt::CaseInsensitive) == 0)
    {
#ifndef WINDOWS
        system("modprobe usbserial vendor=0x19d2 product=0xffeb");
        system("mkdir /var/lock");
#endif
    }
}

bool PhysicalGPRS::InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout)
{
    if(!pName.compare("Model"))
    {
        mModel = pElement.text();
        pHBoxLayout = CreateEnum(pName, pElement.attribute("List").split(';'), mModel);
    }
    else if(!pName.compare("Buad"))
    {
        mBuad = pElement.text();
        //>@添加修改GPRS配置文件代码
        pHBoxLayout = CreateEnum(pName, pElement.attribute("List").split(';'), mBuad);
    }
    else
    {
        return false;
    }
    return true;
}

void PhysicalGPRS::UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement)
{
    if(!pName.compare("Model"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyEnum(tmpComponent, pElement, mModel, tmpComponent->currentText()))
            {
            }
        }
    }
    else if(!pName.compare("Movable"))
    {
        Switch *tmpComponent = VAR_CAST<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, pElement, mIsMovable, tmpComponent->isChecked()))
            {
            }
        }
    }
    else if(!pName.compare("Buad"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyEnum(tmpComponent, pElement, mBuad, tmpComponent->currentText()))
            {
            }
        }
    }
}
