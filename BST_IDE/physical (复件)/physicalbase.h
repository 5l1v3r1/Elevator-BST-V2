#ifndef PHYSICALBASE_H
#define PHYSICALBASE_H

#include "iodevicebase.h"

#ifdef IDE
    class DeviceScene;
    class DeviceItem;
#elif DISPLAY
    class DeviceManager;
#endif
class LogicDevice;

class PhysicalDevice : public IODeviceBase
{
    Q_OBJECT
public:
    PhysicalDevice(QObject *parent = 0);
    ~PhysicalDevice();

    virtual bool copy(QString pSrc, QString pDst)  {return true;}
    virtual bool cut(QString pSrc, QString pDst)  {return true;}
    virtual bool del(QString pPath)  {return true;}

    virtual bool mkdir(QString pPath)  {return true;}
    virtual bool copydir(QDir pSrc, QDir pDst, bool subfolder)  {return true;}
    virtual bool movedir(QString pSrc, QString pDst, bool subfolder)  {return true;}
    virtual bool deldir(QString pDir)  {return true;}
    virtual bool cleardir(QString pDir)  {return true;}

    virtual void SetDevState(DEVICESTATE pState, QString pDevPath);

    bool InitDevice(PH_Type pPHType, QDomElement pElement);
    void AppendLGDev(LG_Type pType, LogicDevice *pDevice);

    //>@获取本节点中的重要信息
    virtual QString GetInformation() { return QString("%1:%2").arg(mDevName).arg(mDevPath); }

signals:
    void sDeviceStateChanged();

public:
    QHash<LG_Type, LogicDevice*> mLGDevices;       //>@协议类型节点表

    QDomNodeList m_ParaElementList;
    DEVICESTATE  mDevState;     //>@设备状态(插入/拔下)
    QString      mDevPath;      //>@设备路径:设备驱动或者可访问地址
    PH_Type      mDevType;
    bool         mCon;          //>@是否建立连接
};

#endif // PHYSICALBASE_H
