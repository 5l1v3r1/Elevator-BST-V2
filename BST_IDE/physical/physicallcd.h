#ifndef PHYSICALLCD_H
#define PHYSICALLCD_H

#include "physicalbase.h"
////backlight
//#include<stdio.h>
//#include<unistd.h>
//#include<sys/types.h>
//#include<sys/ipc.h>
//#include<sys/ioctl.h>
//#include<pthread.h>
//#include<fcntl.h>
//#include<linux/input.h>

//#define KPD_DEV "/dev/input/event0"
//#define FB_DEV "/dev/fb0"

//#define IOCTL_LCD_BRIGHTNESS	_IOW('v', 27, unsigned int)	//brightness control
////backlight

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
    bool        backlight_initialized;

//    //brightness
//    struct input_event data;
//    int kpd_fd, fb_fd;
//    //brightness
};

#endif // NODELCD_H
