#ifndef LOGICAUTOCONNECT_H
#define LOGICAUTOCONNECT_H

#include "logicbase.h"

class LogicAutoConnect : public LogicDevice
{
    Q_OBJECT
public:
    LogicAutoConnect(QObject *parent = 0);
    ~LogicAutoConnect();

public:
    quint32 InitPara();

public slots:
    void UpdatePara(QObject* pObject);
    void ParseDataStream(QObject* pSender){}
    void DeviceStateChanged() {}

public:
};

#endif // LOGICAUTOCONNECT_H
