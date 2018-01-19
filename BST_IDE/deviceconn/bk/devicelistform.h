#ifndef DEVICELISTFORM_H
#define DEVICELISTFORM_H

#include "nodeudp.h"
#include "parserbroadcast.h"
#include "nodetcp.h"
#include "parsernfd.h"
#include "devicemessagedlg.h"

#include <QWidget>
#include <QMultiHash>
#include <QThread>
#include <QReadWriteLock>

typedef enum{
    Broadcast = 0,
    Unicast
}CastOB;

typedef enum{
    Offline = 0,
    Online,
    Connected
}BusState;

class NetInfo
{
public:
    NetInfo(){InitPara();}
    ~NetInfo(){InitPara();}
    void InitPara(){mChecked = false;mConn = Offline;mName.clear();mIP.clear();}

    bool operator>=(const NetInfo &s) const
    {
        if((mIP!= s.mIP))
            return false;
        return true;
    }
    bool operator==(const NetInfo &s) const
    {
        if((mConn != s.mConn) || (mName!= s.mName) || (mIP!= s.mIP))
            return false;
        return true;
    }
    bool operator!=(const NetInfo &s) const{return !operator==(s);}
    NetInfo & operator=(const NetInfo &s)
    {
        mConn = s.mConn;
        mName = s.mName;
        mIP = s.mIP;
        return *this;
    }
public:
    bool                    mChecked;
    BusState                mConn;
    QString                 mName;
    QString                 mIP;
};

class UdiskInfo
{
public:
    UdiskInfo(){InitPara();}
    ~UdiskInfo(){InitPara();}
    void InitPara(){mConn = Offline;mPath.clear();mName.clear();mPath.clear();}

    bool operator>=(const UdiskInfo &s) const
    {
        if(mPath!= s.mPath)
            return false;
        return true;
    }
    bool operator==(const UdiskInfo &s) const
    {
        if((mConn != s.mConn) || (mName != s.mName) || (mPath == s.mPath))
            return false;
        return true;
    }
    bool operator!=(const UdiskInfo &s) const{return !operator==(s);}
    UdiskInfo & operator=(const UdiskInfo &s)
    {
        mConn = s.mConn;
        mName = s.mName;
        mPath = s.mPath;
        return *this;
    }
public:
    BusState                mConn;
    QString                 mName;
    QString                 mPath;
};

class ComInfo
{
public:
    ComInfo(){InitPara();}
    ~ComInfo(){InitPara();}
    void InitPara(){mConn = Online;mName.clear();}

    bool operator>=(const ComInfo &s) const
    {
        if(mName!= s.mName)
            return false;
        return true;
    }
    bool operator==(const ComInfo &s) const
    {
        if((mConn != s.mConn) || (mName != s.mName))
            return false;
        return true;
    }
    bool operator!=(const ComInfo &s) const{return !operator==(s);}
    ComInfo & operator=(const ComInfo &s)
    {
        mConn = s.mConn;
        mName = s.mName;
        return *this;
    }
public:
    BusState                mConn;
    QString                 mName;
};

class DevList
{
public:
    DevList(){InitPara();}
    ~DevList(){InitPara();}

    void InitPara()
    {
        mNets.clear();
        mUdisks.clear();
        mComs.clear();
    }

    bool IsNetExist(QString pKey)
    {
        return mNets.contains(pKey);
    }
    NetInfo GetNet(QString pKey)
    {
        NetInfo netInfo = mNets.value(pKey);
        return netInfo;
    }
    void AppendNet(NetInfo pNetInfo)
    {
        if(pNetInfo == NetInfo())
            return;
        if(IsNetExist(pNetInfo.mIP))
        {
            NetInfo netInfo = GetNet(pNetInfo.mIP);
            if(netInfo.mConn == Connected)
                pNetInfo.mConn = netInfo.mConn;
        }
        mNets.insert(pNetInfo.mIP, pNetInfo);
    }
    BusState GetNetBusState(QString pKey)
    {
        NetInfo netInfo = GetNet(pKey);
        return netInfo.mConn;
    }
    void ModifyNetsBusState(QString pKey, BusState pState)
    {
        if(!IsNetExist(pKey))
            return;
        NetInfo netInfo = GetNet(pKey);
        netInfo.mConn = pState;
        mNets.insert(pKey, netInfo);
    }

    bool operator==(const DevList &s) const
    {
        if((mNets != s.mNets)||
           (mUdisks != s.mUdisks)||
           (mComs != s.mComs))
            return false;
        return true;
    }
    bool operator!=(const DevList &s) const{return !operator==(s);}
    DevList & operator=(const DevList &s)
    {
        mNets = s.mNets;
        mUdisks = s.mUdisks;
        mComs = s.mComs;
        return *this;
    }
public:
    QHash<QString, NetInfo>    mNets;
    QHash<QString, UdiskInfo>  mUdisks;
    QHash<QString, ComInfo>    mComs;
};

class ScanDevThread;

namespace Ui {
class devicelistform;
}

class devicelistform : public QWidget
{
    Q_OBJECT
    friend class devicelistform;
public:
    explicit devicelistform(QWidget *parent = 0);
    ~devicelistform();

    void ReleaseAct();
    void InitAct();

    void ReleaseTreeWidget();
    void InitTreeWidget();

    QStringList GetAllSelectIP()
    {
        QStringList IPList;
        for(int i=0;i<mNetitem->childCount();i++)
        {
            if(mNetitem->child(i)->checkState(0) == Qt::Checked)
                IPList<<mNetitem->child(i)->text(2);
        }
        return IPList;
    }

    void ReleaseNFD()
    {
        if(mTcpclient !=0 )
        {
            disconnect(mTcpclient, SIGNAL(readyRead()), 0, 0);
            connect(mTcpclient, SIGNAL(sConnState(bool)), 0, 0);
            mTcpclient->mCon = false;
            mTcpclient->deleteLater();
            mTcpclient = 0;
        }
        if(mNFDHostApp !=0 )
        {
            disconnect(mNFDHostApp, SIGNAL(sProgress(quint8)), 0, 0);
            mNFDHostApp->deleteLater();
            mNFDHostApp = 0;
        }
    }
    void InitNFD()
    {
        ReleaseNFD();
        mTcpclient = new NodeTcpclient(this);
        mTcpclient->setObjectName("SocketClient");
        mNFDHostApp = new NFDHostApp(this);
        mNFDHostApp->SyncDev(mTcpclient);
        QSignalMapper* connectMap = new QSignalMapper(this);
        connect(mTcpclient, SIGNAL(readyRead()), connectMap, SLOT(map()));
        connectMap->setMapping(mTcpclient, mTcpclient);
        connect(connectMap, SIGNAL(mapped(QObject*)), mNFDHostApp, SLOT(ParseDataStream(QObject*)));
//        connect(mTcpclient, SIGNAL(readyRead()), mNFDHostApp, SLOT(ParseDataStream()));
        connect(mTcpclient, SIGNAL(sConnState(bool)), this, SLOT(slot_PHConnState(bool)));
        connect(mNFDHostApp, SIGNAL(sProgress(quint8)), this, SLOT(slot_NFDProgress(quint8)));
    }
    bool NFDConnect(QHostAddress pIP, quint16 pPort, quint32 pTimeout)
    {
        NFDDisconnect(pTimeout/4);
        if(mTcpclient != 0)
        {
            if(mTcpclient->OpenClient(pIP, pPort, pTimeout/4))
            {
                if(mNFDHostApp != 0)
                {
                    if(mNFDHostApp->SFConn(pTimeout/4))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    bool NFDDisconnect(quint32 pTimeout)
    {
        if(mTcpclient != 0)
        {
            if(mNFDHostApp != 0)
            {
                if(mNFDHostApp->SFDisconn(pTimeout))
                {
                    return true;
                }
            }
        }
        return false;
    }

    void ReleaseUdp()
    {
        if(mHostUdp !=0 )
        {
            disconnect(mHostUdp, SIGNAL(readyRead()), 0, 0);
            mHostUdp->mCon = false;
            mHostUdp->deleteLater();
            mHostUdp = 0;
        }
        if(mHostBroad !=0 )
        {
            mHostBroad->deleteLater();
            mHostBroad = 0;
        }
    }
    void InitUdp()
    {
        mHostUdp = new NodeUdp(this);
        mHostUdp->setObjectName("UdpServer");
        if(!mHostUdp->OpenUdp(D_UDPPORT))
        {
            IDE_TRACE_STR("InitHostUdp#OpenUdp#Failed");
        }
        mHostBroad = new BCHostParser(this);
        mHostBroad->SyncDev(mHostUdp);
        QSignalMapper* connectMap = new QSignalMapper(this);
        connect(mHostUdp, SIGNAL(readyRead()), connectMap, SLOT(map()));
        connectMap->setMapping(mHostUdp, mHostUdp);
        connect(connectMap, SIGNAL(mapped(QObject*)), mHostBroad, SLOT(ParseDataStream(QObject*)));
//        connect(mHostUdp, SIGNAL(readyRead()), mHostBroad, SLOT(ParseDataStream()));
    }

    void StopScanThread();
    void KillScanThread();
    void StartScanThread();

    void ScanIP();
    void EnumerateUdisk();
    void EnumerateCom();
    //>@用于轮询网络、U盘以及串口设备
    bool Scan();

signals:
    void sConnstate(bool pState, QString pIP);
public slots:
    void slot_ModifyTempPath();
    void slot_Broadcast();
    void slot_SyncTime();
    void slot_PHConnState(bool pState);
    void slot_NFDProgress(quint8 pProgress);
    void slot_NetConn();
    void slot_NetDisconn();
    void slot_Message();

    void Slot_ItemClicked(QTreeWidgetItem* pItem, int pColumn);
    void slot_RefreshTreelist();
public:
    //>@消息框相关控件
    //>@缓存路径
    QString             mTempPath;
    //>@设备表
    DevList             mDevList;
    //>@近场调试工具
    NodeTcpclient*      mTcpclient;
    NFDHostApp*         mNFDHostApp;
    //>@Arp扫描
    NodeUdp*            mHostUdp;
    BCHostParser*       mHostBroad;
    ScanDevThread*      mScanDevThread;
    //>@列表项
    QTreeWidgetItem     *mNetitem,
                        *mUseritem,
                        *mUdiskitem,
                        *mComitem;
    //>@设置
    QAction             *TempDirAct,
                        *BroadcastAct,
                        *SyncTimeAct;
    //>@动作
    QAction             *NetconnAct,
                        *NetDisconnAct,
                        *MessageAct;
private:
    Ui::devicelistform *ui;
};

class ScanDevThread : public QThread
{
    Q_OBJECT
public:
    ScanDevThread(devicelistform *qesp)
        :QThread()
    {
        mQesp = qesp;
        mRun = true;
    }
    ~ScanDevThread()
    {
        Stop();
    }
    void Stop()
    {
        mRun = false;
    }
    devicelistform*         mQesp;
    bool                    mRun;
protected:
    virtual void run()
    {
        while(mRun)
        {
            if(mQesp&&mQesp->Scan())
                emit sRefreshTreelist();
            msleep(500);
        }
        mRun = true;
    }
signals:
    void sRefreshTreelist();
};
#endif // DEVICELISTFORM_H
