#ifndef PHYSICALDISK_H
#define PHYSICALDISK_H

#include "physicalmovable.h"

class PhysicalDisk : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalDisk(QObject *parent = 0);
    ~PhysicalDisk();

    quint32 InitPara();
    QString GetInformation() { return QString("%1:%2 %3 %4%").arg(mDevName)
                                                                .arg(mModel)
                                                                .arg(mDevPath)
                                                                .arg(mUsageRate); }

public slots:
    void UpdatePara(QObject* pObject);
    void slot_MonitorStorage();

public:
    QString         mModel;
    int             mThreshold, mMinThreshold, mMaxThreshold;
    QTimer          mMonitorTimer;

    double          mUsageRate;
    double          mDiskUsed;
    double          mDiskFree;
    double          mDiskTotal;
};

class PhysicalMobileDisk : public PhysicalMovable
{
    Q_OBJECT
public:
    PhysicalMobileDisk(QObject *parent = 0);
    ~PhysicalMobileDisk();

    bool SetPlugged();
    bool SetUnplugged();

    void ScanDisks();

    bool copy(QString pSrc, QString pDst)  {return CopyFile(pSrc, pDst);}
    bool cut(QString pSrc, QString pDst)  {return CutFile(pSrc, pDst);}
    bool del(QString pPath)  {return DelFile(pPath);}

    bool mkdir(QString pPath)  {return CreatPath(pPath);}
    bool copydir(QString pSrc, QString pDst, bool subfolder)  {return CopyFolder(pSrc, pDst, subfolder);}
    bool movedir(QString pSrc, QString pDst)  {return MoveFolder(pSrc, pDst);}
    bool deldir(QString pDir)  {return DelFolder(pDir);}
    bool cleardir(QString pDir)  {return ClearFolder(pDir);}

    bool InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout);
    void UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement);

public:
    QStringList              mMobileDisks;
    bool                     mExecutable;  //>@是否作为执行盘，否则作为升级盘
};

#endif // NODEMEM_H
