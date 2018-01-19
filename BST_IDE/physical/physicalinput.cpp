#include "physicalinput.h"


PhysicalMouse::PhysicalMouse(QObject *parent):
    PhysicalMovable(parent)
{
}

PhysicalMouse::~PhysicalMouse()
{
}

bool PhysicalMouse::SetPlugged()
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    return true;
}

bool PhysicalMouse::SetUnplugged()
{
    QApplication::setOverrideCursor(Qt::BlankCursor);
    return true;
}

bool PhysicalMouse::InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout)
{
    if(!pName.compare("Visible"))
    {
        QString tmpValue = pElement.text();
        mVisible = (tmpValue.compare("true",Qt::CaseInsensitive)==0)?true:false;
        pHBoxLayout = CreateBool(pName, tmpValue);
    }
    else
    {
        return false;
    }
    return true;
}

void PhysicalMouse::UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement)
{
    if(!pName.compare("Visible"))
    {
        Switch *tmpComponent = VAR_CAST<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, pElement, mVisible, tmpComponent->isChecked()))
            {
            }
        }
    }
}

PhysicalKeyboard::PhysicalKeyboard(QObject *parent):
    PhysicalMovable(parent)
{
}

PhysicalKeyboard::~PhysicalKeyboard()
{
    SetUnplugged();
}

bool PhysicalKeyboard::SetPlugged()
{
    return true;
}

bool PhysicalKeyboard::SetUnplugged()
{
    return true;
}

bool PhysicalKeyboard::InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout)
{
    if(!pName.compare("Mode"))
    {
        QString tmpValue = pElement.text();
        mMode = GetMode(tmpValue);
        pHBoxLayout = CreateEnum(pName, pElement.attribute("List").split(';'), tmpValue);
    }
    else if(!pName.compare("Pinyin"))
    {
        QString tmpValue = pElement.text();
        mSupportPinyin = (tmpValue.compare("true",Qt::CaseInsensitive)==0)?true:false;
        pHBoxLayout = CreateBool(pName, tmpValue);
    }
    else
    {
        return false;
    }
    return true;
}

void PhysicalKeyboard::UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement)
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
    else if(!pName.compare("Pinyin"))
    {
        Switch *tmpComponent = VAR_CAST<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, pElement, mSupportPinyin, tmpComponent->isChecked()))
            {
            }
        }
    }
}


PhysicalTouch::PhysicalTouch(QObject *parent):
    PhysicalMovable(parent)
{
}

PhysicalTouch::~PhysicalTouch()
{
    SetUnplugged();
}

bool PhysicalTouch::SetPlugged()
{
    return false;
}

bool PhysicalTouch::SetUnplugged()
{
    return false;
}

bool PhysicalTouch::InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout)
{
    if(!pName.compare("Mode"))
    {
        QString tmpValue = pElement.text();
        mMode = GetMode(tmpValue);
        pHBoxLayout = CreateEnum(pName, pElement.attribute("List").split(';'), tmpValue);
    }
    else
    {
        return false;
    }
    return true;
}

void PhysicalTouch::UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement)
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
}
