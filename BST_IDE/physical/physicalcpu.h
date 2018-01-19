#ifndef PHYSICALCPU_H
#define PHYSICALCPU_H

#include "physicalbase.h"

class PhysicalCPU : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalCPU(QObject *parent = 0);
    ~PhysicalCPU();

public:
    quint32 InitPara();
    QString GetInformation() { return QString("%1:%2  %3%").arg(mDevName).arg(mCpuModel).arg(mUsageRate); }

public slots:
    void UpdatePara(QObject* pObject);
    void slot_MonitorCPU();

public:
    QString         mCpuModel;
    QString         mUsbScanPath;  //>@USB设备扫描目录，用于开机首次扫描已存在的USB设备
    bool            mAutoRelease;
    int             mThreshold, mMinThreshold, mMaxThreshold;

    QTimer          mMonitorTimer;
    int             mUsageRate;
};

#endif // PHYSICALCPU_H
