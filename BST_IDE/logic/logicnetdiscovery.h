#ifndef LOGICNETDiscovery_H
#define LOGICNETDiscovery_H

#include "logicbase.h"
#include <QHostInfo>

#define D_CONTENTSPLIT      QString(';')

#define D_HOSTPREFIX        QString("HOST#")
#define D_GUESTPREFIX       QString("GUEST#")

#define D_CMDSCANIP         QString("ScanIP")

//>@---------------------------------------------------------
//>@--              指令列表 : 此协议类中支持单播、组播、广播操作
//>@--  1、HOST#(GUEST#)xxx
//>@---------------------------------------------------------
//>@

class LogicNetDiscovery : public LogicDevice
{
    Q_OBJECT
public:
    LogicNetDiscovery(QObject *parent = 0);
    ~LogicNetDiscovery();

    void SyncDev(QIODevice *pIODevice, PH_Type pType=P_RMII);

    qint64 SendData(QString pData);
    qint64 SendData(QStringList pIPList, QString pData);
    qint64 ReplySender(QString pData);

    QString GetLocalAddress();
    QString GetPeerAddress();

    QStringList AppendPHDevices(QStringList pPHDevs);
    void DeletePHDevices();

    virtual bool ParseFrameStream(QString pFrame) { return false; }

    quint32 InitPara();

public:
    void UpdatePara(QObject* pObject);

public slots:
    void ParseDataStream();

public:
    QIODevice         *m_PHDev;
    PH_Type            m_PHType;   //>@主要用于IDE中
};

class HostNetDiscovery : public LogicNetDiscovery
{
    Q_OBJECT
public:
    HostNetDiscovery(QObject *parent = 0);
    ~HostNetDiscovery();

    bool Broadcast(QString pMessage);
    bool Groupcast(QStringList pIPList, QString pMessage);
    bool Singlecast(QString pMessage);

    QStringList ScanDevIP(quint16 pTimeout);
    bool ParseFrameStream(QString pFrame);

public:
    QStringList         mIPList;
};

class GuestNetDiscovery : public LogicNetDiscovery
{
    Q_OBJECT
public:
    GuestNetDiscovery(QObject *parent = 0);
    ~GuestNetDiscovery();

    bool Singlecast(QString pMessage);
    bool ParseFrameStream(QString pFrame);
};


#endif // PARSERBROADCAST_H
