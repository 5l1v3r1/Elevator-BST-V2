#ifndef PHYSICALLCD_H
#define PHYSICALLCD_H

#include "physicalbase.h"

class PhysicalLCD : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalLCD(QObject *parent = 0);
    ~PhysicalLCD();

public:
    quint32 InitPara();
    QString GetInformation() { return QString("%1:Overlay(%2)  %3").arg(mDevName).arg(mOverlayers.count()).arg(mLuminance); }

public slots:
    void UpdatePara(QObject* pObject);
    void slot_StartDarkMonitor();   //>@用于恢复屏幕亮度，并启动超时监控
    void slot_DarkTimeout();

public:
    QStringList     mOverlayers;
    int             mRotation;
    int             mLuminance, mMinLuminance, mMaxLuminance;
    QTimer          mMonitorTimer;
    int             mDarkTimeout, mMinTimeout, mMaxTimeout;
    int             mDarkLevel;
};

#endif // NODELCD_H
