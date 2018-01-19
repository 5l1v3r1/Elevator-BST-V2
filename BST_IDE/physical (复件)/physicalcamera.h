#ifndef PHYSICALCAMERA_H
#define PHYSICALCAMERA_H

#include "physicalmovable.h"

class PhysicalCamera : public PhysicalMovable
{
    Q_OBJECT
public:
    typedef enum{MJPEG=0, YUV} MODE;
    PhysicalCamera(QObject *parent = 0);
    ~PhysicalCamera();

    bool InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout);
    void UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement);

    QString GetInformation() {return QString("%1:%2 %3").arg(mDevName)
                                                        .arg(mDevPath)
                                                        .arg(GetModeString(mMode));}
    MODE GetMode(QString pMode)
    {
        if(pMode.compare("MJPEG", Qt::CaseInsensitive) == 0)
            return MJPEG;
        else if(pMode.compare("YUV", Qt::CaseInsensitive) == 0)
            return YUV;
    }
    QString GetModeString(MODE pMode)
    {
        if(pMode == MJPEG)
            return "MJPEG";
        else if(pMode == YUV)
            return "YUV";
    }

public:
    MODE            mMode;
    QStringList     mChannels;
    RESOLUTION      mMaxSize;
};

#endif // NODECAMERA_H
