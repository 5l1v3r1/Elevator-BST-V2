#ifndef PARSERBROADCAST_H
#define PARSERBROADCAST_H

#include "parserbase.h"
#include <QHostInfo>

//>@---------------------------------------------------------
//>@--              指令列表
//>@--  1、Message#xxx
//>@--  2、Command#xxx
//>@--  3、Inform#xxx#1000  数字表示消息显示1000ms后消失，只在Display中有效
//>@---------------------------------------------------------

class NodeUdp;
//>@此协议类中支持单播、组播、广播操作
class parserBroadcast : public LogicDevice
{
    Q_OBJECT
public:
    parserBroadcast(QObject *parent = 0);
    ~parserBroadcast();
    qint64 SendData(QStringList pIPList, QString pData);
    qint64 SendData(QString pData);
    qint64 ReplySender(QString pData);
    QString getIP();  //获取ip地址
    virtual bool ReturnInfo(QString &pInfo){return false;}
signals:
    void sMessage(QString pMessage);
    void sCommand(QString pMessage);
    void sInform(QString pMessage);
public slots:
    void ParseDataStream(QObject* pSender);
};

class BCHostParser : public parserBroadcast
{
    Q_OBJECT
public:
    BCHostParser(QObject *parent = 0);
    ~BCHostParser();

    bool Broadcast(QString pMessage);
    bool Groupcast(QStringList pIPList, QString pMessage);
    bool Singlecast(QString pMessage);
    QString GetPcName()
    {
        QStringList envVariables;
        envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                     << "HOSTNAME.*" << "DOMAINNAME.*";
        QStringList environment = QProcess::systemEnvironment();
        foreach (QString string, envVariables)
        {
            int index = environment.indexOf(QRegExp(string));
            if (index != -1)
            {
                QStringList stringList = environment.at(index).split('=');
                if (stringList.size() == 2)
                    return stringList.at(1);
            }
        }
        return QString();
    }
    QStringList ScanDevIP(quint16 pTimeout)
    {
        mIPList.clear();
        if(Broadcast("Command;ScanIP") == false)
            return mIPList;
        while(pTimeout--)
            QtSleep(1);
        return mIPList;
    }
    void SendPcIP()
    {
        QString localIP = getIP();
        if(localIP.isEmpty())
            return;
        QString name = GetPcName();
        QString info = QString("Command;ScanIP;%1;%2;Success").
                            arg(name.insert(0, "Pc:")).
                            arg(localIP);
        Broadcast(info);
    }

    virtual bool ReturnInfo(QString &pInfo);

    void SyncEventMap(EventMap *pEventMap)
    {
        LogicDevice::SyncEventMap(pEventMap);
    }
    bool InitPHPara(QDomElement pElement, EventMap *pEventMap=0)
    {
        return LogicDevice::InitPHPara(pElement, pEventMap);
    }
public:
    QStringList         mIPList;
};

class BCDevParser : public parserBroadcast
{
    Q_OBJECT
public:
    BCDevParser(QString pName, QObject *parent = 0);
    ~BCDevParser();

    void SetDevname(QString pName)
    {
        mName = pName;
    }
    void SendDevIP()
    {
        QString localIP = getIP();
        if(localIP.isEmpty())
            return;
        QString tmpName = mName;
        QString info = QString("Command;ScanIP;%1;%2;Success").
                            arg(tmpName.insert(0, "Device:")).
                            arg(localIP);
        qint64 num = ReplySender(info);
    }
    virtual bool ReturnInfo(QString &pInfo);

    void SyncEventMap(EventMap *pEventMap)
    {
        LogicDevice::SyncEventMap(pEventMap);
        connect(this,SIGNAL(sCommand(QString)),m_EventMap,SLOT(slot_UDPCommand(QString)));
        connect(this,SIGNAL(sInform(QString)),m_EventMap,SLOT(slot_UDPInform(QString)));
    }
    bool InitPHPara(QDomElement pElement, EventMap *pEventMap=0)
    {
        return LogicDevice::InitPHPara(pElement, pEventMap);
    }
public:
    QString             mName;   
};
#endif // PARSERBROADCAST_H
