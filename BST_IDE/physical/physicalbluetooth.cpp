#include "physicalbluetooth.h"

PhysicalBluetooth::PhysicalBluetooth(QObject *parent):
    PhysicalMovable(parent)
{
}

PhysicalBluetooth::~PhysicalBluetooth()
{
    SetUnplugged();
}

bool PhysicalBluetooth::SetPlugged()
{
    return true;
}

bool PhysicalBluetooth::SetUnplugged()
{
    return true;
}

bool PhysicalBluetooth::InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout)
{
    if(!pName.compare("Model"))
    {
        mModel = pElement.text();
        pHBoxLayout = CreateEnum(pName, pElement.attribute("List").split(';'), mModel);
    }
    else
    {
        return false;
    }
    return true;
}

void PhysicalBluetooth::UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement)
{
    if(!pName.compare("Model"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyEnum(tmpComponent, pElement, mModel, tmpComponent->currentText()))
            {
                return;
            }
        }
    }
}
