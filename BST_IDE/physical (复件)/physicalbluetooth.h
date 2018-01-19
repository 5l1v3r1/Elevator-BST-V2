#ifndef PHYSICALBLUETOOTH_H
#define PHYSICALBLUETOOTH_H

#include "physicalmovable.h"

class PhysicalBluetooth : public PhysicalMovable
{
    Q_OBJECT
public:
    PhysicalBluetooth(QObject *parent = 0);
    ~PhysicalBluetooth();

    bool InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout);
    void UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement);

    bool SetPlugged();
    bool SetUnplugged();

    QString GetInformation() {return QString("%1:%2 %3").arg(mDevName).arg(mDevPath).arg(mModel);}

public:
    QString             mModel;
};

#endif // PHYSICALBLUETOOTH_H
