#ifndef PHYSICALMEM_H
#define PHYSICALMEM_H

#include "physicalbase.h"

class PhysicalMEM : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalMEM(QObject *parent = 0);
    ~PhysicalMEM();

public:
    quint32 InitPara();
    QString GetInformation() { return QString("%1:%2  %3%").arg(mDevName).arg(mMemModel).arg(mUsageRate); }

public slots:
    void UpdatePara(QObject* pObject);
    void slot_MonitorMem();

public:
    QString         mMemModel;
    bool            mAutoRelease;
    int             mThreshold, mMinThreshold, mMaxThreshold;

    QTimer          mMonitorTimer;
    int             mUsageRate;
};

#endif // NODEMEM_H
