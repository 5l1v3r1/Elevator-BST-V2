#ifndef PHYSICALRTC_H
#define PHYSICALRTC_H

#include "physicalbase.h"

class RTCThread;

class PhysicalRTC : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalRTC(QObject *parent = 0);
    ~PhysicalRTC();

public:
    void InitRTC();
    void ReleaseRTC();
    void ProcessRTC();

    quint32 InitPara();
    QString GetInformation() { return QString("%1:DotBlink(%1) SecondEnable(%2)")
                                      .arg(mDevName)
                                      .arg(mDotBlink?(QString("true")):(QString("false"))
                                      .arg(mSecondEnable?(QString("true")):(QString("false")))); }

public slots:
    void UpdatePara(QObject* pObject);

public:
    RTCThread   *mRTCThread;

    bool         mDotBlink;
    bool         mSecondEnable;
};

class RTCThread: public QThread
{
    Q_OBJECT
public:
    RTCThread(QObject *parent = 0);
    ~RTCThread();

    virtual void run();
    void Poll();
    void stop();

public:
    bool            terminate;
    PhysicalRTC    *mPhysicalRTC;
    quint32         mDelay;
};

#endif // PHYSICALRTC_H
