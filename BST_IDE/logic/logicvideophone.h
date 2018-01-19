#ifndef LOGICVIDEOPHONE_H
#define LOGICVIDEOPHONE_H

#include "logicbase.h"

class LogicVideoPhone : public LogicDevice
{
    Q_OBJECT
public:
    LogicVideoPhone(QObject *parent = 0);
    ~LogicVideoPhone();

public:
    quint32 InitPara();
    bool SetValue(QString pValue);

public slots:
    void UpdatePara(QObject* pObject);
    void ParseDataStream(){}
    void DeviceStateChanged() {}

public:
    RESOLUTION          mSize;
    bool                mVideo;
    bool                mAudio;

};

#endif // LOGICVIDEOPHONE_H
