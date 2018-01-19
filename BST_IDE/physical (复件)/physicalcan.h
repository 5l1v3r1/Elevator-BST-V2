#ifndef PHYSICALCAN_H
#define PHYSICALCAN_H

#include "physicalbase.h"

class PhysicalCAN : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalCAN(QObject *parent = 0) :
        PhysicalDevice(parent)
    {
        mDevType = P_CPU;
    }
    ~PhysicalCAN()
    {
    }
public:
    void SyncEventMap(EventMap *pEventMap)
    {
        PhysicalDevice::SyncEventMap(pEventMap);
    }
    bool InitPHPara(QDomElement pElement, EventMap *pEventMap=0);
    void UpdatePHPara(QWidget* pWidget);

public:
    QString         mCpuModel;
    QString         mUsername;
    bool            mAutoRelease;
    int             mThreshold;
};

#endif // NODECPU_H
