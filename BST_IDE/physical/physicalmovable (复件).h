#ifndef PHYSICALMOVABLE_H
#define PHYSICALMOVABLE_H

#include "physicalbase.h"

class PhysicalMovable : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalMovable(QObject *parent = 0);
    ~PhysicalMovable();

    bool isMovable() { return mIsMovable; }

    quint32 InitPara();
    virtual bool InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout) { return false; }
    virtual void UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement) {}
    virtual void PreInitProcess() {}
    virtual void PreUpdateProcess() {}
    virtual void PostInitProcess() {}
    virtual void PostUpdateProcess() {}

    virtual bool SetPlugged() {return false;}
    virtual bool SetUnplugged() {return false;}

public slots:
    void UpdatePara(QObject* pObject);
    void slot_DevStateChanged(DEVICESTATE pState, QString pDevPath);

public:
    bool        mIsMovable;
};

#endif // PHYSICALMOBILE_H
