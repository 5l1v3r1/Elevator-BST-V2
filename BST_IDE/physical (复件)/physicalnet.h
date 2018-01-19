#ifndef PHYSICALNETWORK_H
#define PHYSICALNETWORK_H

#include "physicalmovable.h"

#include "tooltcp.h"
#include "tooludp.h"
#include <QHostAddress>

//>@网络应用接口解析类
class PhysicalSocket : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalSocket(QObject *pParent = 0);
    PhysicalSocket(QTcpSocket *pSocket, QObject *pParent = 0);
    ~PhysicalSocket();

    bool Open(QHostAddress pIP, quint16 pPort, quint32 pTimeout);
    void Open(QHostAddress pIP, quint16 pPort);
    void Close();

    QHostAddress GetPeerIP();
    quint16 GetPeerPort();

    QHostAddress GetLocalIP();
    quint16 GetLocalPort();

    bool isConnected()  { return (mTcpSocket->state() == QAbstractSocket::ConnectedState); }

signals:
    void sConnState(bool pState);
    void sError(QString pError);

public slots:
    void slot_Conn();
    void slot_Disconn();
    virtual void slot_ReadData() {}

public:
    uint             mAppType;  //>@此应用的类型
    QTcpSocket      *mTcpSocket;
};

class NetManagerSocket : public PhysicalSocket
{
    Q_OBJECT
public:
    NetManagerSocket(QObject *pParent = 0);
    NetManagerSocket(QTcpSocket *pSocket, QObject *pParent = 0);
    ~NetManagerSocket();

    qint64 write(QByteArray data);
    QByteArray readAll();

public slots:
    void slot_ReadData();

public:
    FRAME_STATE                 m_FrameState;
    QByteArray                  m_DataBuffer, m_FrameBuffer;
};

/*
    默认开启UDP连接，TCP连接
    仅仅用于创建新的PhysicalDevice用于解析网络接口数据
*/
class PhysicalLAN : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalLAN(QObject *parent = 0);
    ~PhysicalLAN();

    void OpenNetDiscovery();
    void CloseNetDiscovery();

    void OpenNetManager();
    void CloseNetManager();

public slots:
    void slot_NewSocket(QTcpSocket *pSocket, uint pType);

public:
    ToolUdp                    *m_SharedUdp;
    ToolTcpServer              *m_SharedTcp;            //>@用于建立服务器，允许注册应用

    QList<PhysicalSocket*>      m_SocketParsers;
};


class PhysicalWAN : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalWAN(QObject *parent = 0);
    ~PhysicalWAN();
};


class PhysicalRMII : public PhysicalMovable
{
    Q_OBJECT
public:
    PhysicalRMII(QObject *parent = 0);
    ~PhysicalRMII();

    virtual bool InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout);
    virtual void UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement);
    virtual void PreInitProcess();
    virtual void PreUpdateProcess();
    virtual void PostInitProcess();
    virtual void PostUpdateProcess();

    void OpenNetCard();
    void CloseNetCard();

    QString GetInformation() { return QString("%1:%2 %3 DHCP(%4)").arg(mDevName).arg(mDevPath).arg(m_IPAddr.toQString()).arg(m_DHCP?(QString("true")):(QString("false"))); }

public:
    QString                     mModel;
    ENET_ADDR                   m_IPAddr;
    ENET_ADDR                   m_SubmaskAddr;
    ENET_ADDR                   m_GatewayAddr;
    ENET_ADDR                   m_DNSAddr;
    bool                        m_DHCP;
    QString                     m_DHCPConfFile;
};

class PhysicalWIFI : public PhysicalRMII
{
    Q_OBJECT
public:
    typedef enum {AP=0, Client} MODE;
    PhysicalWIFI(QObject *parent = 0);
    ~PhysicalWIFI();

    bool SetPlugged();
    bool SetUnplugged();

    bool InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout);
    void UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement);

    void PreInitProcess();
    void PreUpdateProcess();
    void PostInitProcess();
    void PostUpdateProcess();

    MODE GetMode(QString pMode)
    {
        if(pMode.compare("AP", Qt::CaseInsensitive) == 0)
            return AP;
        else if(pMode.compare("Client", Qt::CaseInsensitive) == 0)
            return Client;
    }
    QString GetModeString(MODE pMode)
    {
        if(pMode == AP)
            return "AP";
        else if(pMode == Client)
            return "Client";
    }

public:
    MODE            mMode;
    QString         mEssid;
    QString         mPasswd;
};

class PhysicalGPRS : public PhysicalMovable
{
    Q_OBJECT
public:
    PhysicalGPRS(QObject *parent = 0);
    ~PhysicalGPRS();

    bool InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout);
    void UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement);

    void PostInitProcess() { InitGPRS(); }

    void InitGPRS();

    bool SetPlugged();
    bool SetUnplugged();

    void OpenNetCard();
    void CloseNetCard();

    QString GetInformation() { return QString("%1:%2  %3").arg(mDevName).arg(mModel).arg(mBuad); }

public:
    QString         mModel;
    QString         mBuad;
};

#endif // PHYSICALNETWORK_H
