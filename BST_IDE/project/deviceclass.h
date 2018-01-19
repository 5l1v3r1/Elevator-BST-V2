#ifndef DEVICECLASS_H
#define DEVICECLASS_H

#include "global.h"
#include "physicalnet.h"
#include "logicnetdiscovery.h"
#include "logicnetmanager.h"
#include "devicewathcer.h"

#include <QMultiHash>
#include <QThread>
#include <QReadWriteLock>

class DeviceTreeItem;

class BaseDevice : public QObject
{
    Q_OBJECT
public:
    typedef enum{
        TYPE_LOCAL = 0,     //>@本地缓冲，也即离线制作主题
        TYPE_NET,           //>@网络设备
        TYPE_STORAGE,       //>@存储设备
        TYPE_COM            //>@串口设备
    }DEVICETYPE;

    BaseDevice(QString pAddr=QString(), QString pBufferPath=QString(), QObject *parent=0);
    ~BaseDevice();

    void SetDeviceTreeItem(DeviceTreeItem *pDevItem, DEVICESTATE pState=S_OFFLINE);
    virtual bool Scan();        //>@扫描缓冲路径下的工程
    bool ScanDirectiory(QString pPath);

    virtual bool isSync() {return false;}
    virtual void Sync() {}
    bool Sync(quint32 pTimeout);
    virtual void Async() {}
    bool Async(quint32 pTimeout);

    virtual bool Abort();

    virtual void ChangeDevAddr(QString pDevAddr);  //>@不允许修改设备类型，但可以修改设备地址，比如修改IP
    virtual void ChangeBuffer(QString pBufferPath);

    //>@下载主题、映射、配置文件
    virtual bool Download(QString pThemeName) {return false;}
    virtual bool Upload(QString pThemeName) {return false;}
    //>@仅下载配置文件，一般用于快速定制设备之类的操作
    virtual bool DownloadConfigure(QString pThemeName, QString pMdpName) {return false;}
    virtual bool UploadConfigure(QString pThemeName, QString pMdpName) {return false;}

    bool LocalCompress(QString pSrc, QString pDst, bool pCover);
    bool LocalDecompress(QString pSrc, QString pDir, bool pCover);

    inline DEVICETYPE GetDeviceType(QString pDevType)
    {
        if(pDevType.compare("NET", Qt::CaseInsensitive) == 0)
            return TYPE_NET;
        else if(pDevType.compare("STORAGE", Qt::CaseInsensitive) == 0)
            return TYPE_STORAGE;
        else if(pDevType.compare("COM", Qt::CaseInsensitive) == 0)
            return TYPE_COM;
        return TYPE_LOCAL;
    }

signals:
    void sConnState(bool pState);
    void sZip(QString pSrc, QString pDst, bool pCover);
    void sUnzip(QString pSrc, QString pDst, bool pCover);

public slots:
    void slot_ZipTaskEnd();
    void slot_ZipTaskErr();
    void slot_ZipTaskProgress(int ratio);

public:
    DeviceTreeItem  *mDevTreeItem;

    DEVICESTATE      mDevState;
    DEVICETYPE       mDevType;

    TaskState        mZipTaskState;

    QString          mDevNickname;
    QString          mDevAddr;
    QString          mBufferPath;

    bool             mIsBusy;
};

class NetDevice : public BaseDevice
{
    Q_OBJECT
public:
    NetDevice(QString pAddr, QString pBufferPath=QString(), QObject *parent=0);
    ~NetDevice();

    bool Scan();        //>@扫描缓冲路径下的工程

    bool isSync();
    void Sync();
    void Async();

    bool Abort();

    void UpdateDevAddr();    //>@支持10.200.19.1:8080或者10.200.19.1两种格式

    //>@先将boot以及map文件拷贝至当前主题中，再下载
    bool Download(QString pThemeName);
    bool Upload(QString pThemeName);

    bool DownloadConfigure(QString pThemeName, QString pMdpName);
    bool UploadConfigure(QString pThemeName, QString pMdpName);

    static bool ParseIP(QString pIP, QHostAddress &pIPAddr, quint16 &pPort);
    static bool CheckIP(QString pIP);

public slots:
    void slot_PHConnState(bool pState);
    void slot_LGConnState(bool pState);

public:
    NetManagerSocket*       mTcpClient;
    HostNetManager*         mNetManager;
};

class StorageDevice : public BaseDevice
{
    Q_OBJECT
public:
    StorageDevice(QString pAddr, QString pBufferPath=QString(), QObject *parent=0);
    ~StorageDevice();

    bool Scan();        //>@扫描缓冲路径下的工程

    bool isSync();
    void Sync();
    void Async();

    bool Download(QString pThemeName);
    bool Upload(QString pThemeName);

    bool DownloadConfigure(QString pThemeName, QString pMdpName);
    bool UploadConfigure(QString pThemeName, QString pMdpName);

public:

};

class ComDevice : public BaseDevice
{
    Q_OBJECT
public:
    ComDevice(QString pAddr, QString pBufferPath=QString(), QObject *parent=0);
    ~ComDevice();

    bool Scan();        //>@扫描缓冲路径下的工程

    bool isSync();
    void Sync();
    void Async();

    bool Download(QString pThemeName);
    bool Upload(QString pThemeName);

public:

};

#endif // DEVICECLASS_H
