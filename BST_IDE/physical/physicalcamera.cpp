#include "physicalcamera.h"

PhysicalCamera::PhysicalCamera(QObject *parent):
    PhysicalMovable(parent)
{
}

PhysicalCamera::~PhysicalCamera()
{
}

bool PhysicalCamera::InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout)
{
    if(!pName.compare("Mode"))
    {
        QString tmpMode = pElement.text();
        mMode = GetMode(tmpMode);
        pHBoxLayout = CreateEnum(pName, pElement.attribute("List").split(';'), tmpMode);
    }
    else if(!pName.compare("Channel"))
    {
        mChannels = pElement.text().split(';');
        pHBoxLayout = CreateCheck(pName, pElement.attribute("List").split(';'), mChannels);
    }
    else if(!pName.compare("MaxSize"))
    {
        QString tmpMaxSizeString = pElement.text();
        mMaxSize = RESOLUTION(tmpMaxSizeString);
        pHBoxLayout = CreateEnum(pName, pElement.attribute("List").split(';'), tmpMaxSizeString);
    }
    else
    {
        return false;
    }
    return true;
}

void PhysicalCamera::UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement)
{
    if(!pName.compare("Mode"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpMode;
            if(!ModifyEnum(tmpComponent, pElement, tmpMode, tmpComponent->currentText()))
            {
            }
            else
            {
                mMode = GetMode(tmpMode);
            }
        }
    }
    else if(!pName.compare("Channel"))
    {
        CheckList *tmpComponent = VAR_CAST<CheckList *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyCheck(tmpComponent, pElement, mChannels, tmpComponent->GetSelectList()))
            {
            }
        }
    }
    else if(!pName.compare("MaxSize"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpSize;
            if(!ModifyEnum(tmpComponent, pElement, tmpSize, tmpComponent->currentText()))
            {
            }
            else
            {
                mMaxSize = RESOLUTION(tmpSize);
            }
        }
    }
}
