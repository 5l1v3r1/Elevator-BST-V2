#ifndef PHYSICALAUDIO_H
#define PHYSICALAUDIO_H

#include "physicalbase.h"

class PhysicalAudio : public PhysicalDevice
{
    Q_OBJECT
public:
    typedef enum{ALSA=0, OSS} MODE;

    PhysicalAudio(QObject *parent = 0);
    ~PhysicalAudio();

public:
    quint32 InitPara();
    MODE GetMode(QString pMode)
    {
        if(pMode.compare("ALSA", Qt::CaseInsensitive) == 0)
            return ALSA;
        else if(pMode.compare("OSS", Qt::CaseInsensitive) == 0)
            return OSS;
    }
    QString GetModeString(MODE pMode)
    {
        if(pMode == ALSA)
            return "ALSA";
        else if(pMode == OSS)
            return "OSS";
    }
    void ScanAudioDevice();
    QString GetInformation() {return QString("%1:%2  %3").arg(mDevName).arg(GetModeString(mMode)).arg(mVolume);}

public slots:
    void UpdatePara(QObject* pObject);

public:
    MODE         mMode;
    QStringList  mOutputDevList;  //>@可用的输出设备列表
    QStringList  mInputDevList;  //>@可用的输入设备列表
    int          mVolume;
    int          mMinVolume, mMaxVolume;
};

#endif // NODECPU_H
