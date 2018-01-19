#include "physicallcd.h"
#ifdef DISPLAY
#include "devicemanager.h"
#endif

PhysicalLCD::PhysicalLCD(QObject *parent) :
    PhysicalDevice(parent)
{
//    backlight_initialized = true;
//    if ((kpd_fd = open(KPD_DEV, O_RDONLY)) < 0)
//    {
//        printf("open device %s failed", KPD_DEV);
//       backlight_initialized = false;
//    }

//    if ((fb_fd = open(FB_DEV, O_RDONLY)) < 0)
//    {
//        printf("open device %s failed", FB_DEV);
//        backlight_initialized = false;
//    }
}

PhysicalLCD::~PhysicalLCD()
{
    mMonitorTimer.stop();
}

quint32 PhysicalLCD::InitPara()
{
    QDomNodeList tmpParaNodeList = m_ParaElement.childNodes();
    if(tmpParaNodeList.count() < 1)
        return 0;
    quint32 validcount = 0;
    QHBoxLayout* tmpHBoxLayout;
    for(int i=0;i<tmpParaNodeList.count();i++)
    {
        QDomElement tmpElement = tmpParaNodeList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(!tmpName.compare("OverLayer"))
        {
            mOverlayers = tmpElement.text().split(';');
            tmpHBoxLayout = CreateCheck(tmpName, tmpElement.attribute("List").split(';'), mOverlayers);
        }
        else if(!tmpName.compare("Rotation"))
        {
            QString tmpValue = tmpElement.text();
            mRotation = tmpValue.toInt();
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), tmpValue);
        }
        else if(!tmpName.compare("Luminance"))
        {
            mLuminance = tmpElement.text().toInt();
            QStringList tmpValueList = tmpElement.attribute("List").split(';');
            if(tmpValueList.count()!=2)
            {
                mLuminance = 0;
                mMinLuminance = 0;
                mMaxLuminance = 0;
            }
            else
            {
                mMinLuminance = tmpValueList.at(0).toInt();
                mMaxLuminance = tmpValueList.at(1).toInt();
                if(mLuminance < mMinLuminance || mLuminance > mMaxLuminance)  //>@判断值是否有效
                    mLuminance = 0;
            }
            SYSTEM_CMD::SetLuminance(mLuminance);
            tmpHBoxLayout = CreateNum(tmpName, mLuminance, mMinLuminance, mMaxLuminance);
        }
        else if(!tmpName.compare("DarkTimeout"))
        {
            mDarkTimeout = tmpElement.text().toInt();
            QStringList tmpValueList = tmpElement.attribute("List").split(';');
            if(tmpValueList.count()!=2)
            {
                mDarkTimeout = 0;
                mMinTimeout = 0;
                mMaxTimeout = 0;
            }
            else
            {
                mMinTimeout = tmpValueList.at(0).toInt();
                mMaxTimeout = tmpValueList.at(1).toInt();
                if(mDarkTimeout < mMinTimeout || mDarkTimeout > mMaxTimeout)  //>@判断值是否有效
                    mDarkTimeout = 0;
            }
            tmpHBoxLayout = CreateNum(tmpName, mDarkTimeout, mMinTimeout, mMaxTimeout);
        }
        else if(!tmpName.compare("DarkLevel"))
        {
            QString tmpValue = tmpElement.text();
            mDarkLevel = tmpValue.toInt();
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), tmpValue);
        }
        else
        {
            continue;
        }
        validcount++;
        m_SettingGroup->m_VLayout->addWidget(CreateLine());
        m_SettingGroup->m_VLayout->addLayout(tmpHBoxLayout);
    }
    if(mDarkTimeout)
        mMonitorTimer.singleShot(mDarkTimeout, this, SIGNAL(slot_DarkTimeout()));
    return validcount;
}

void PhysicalLCD::UpdatePara(QObject* pObject)
{
    IDE_TRACE();
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("OverLayer"))
    {
        CheckList *tmpComponent = VAR_CAST<CheckList *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyCheck(tmpComponent, tmpElement, mOverlayers, tmpComponent->GetSelectList()))
            {
            }
        }
    }
    else if(!tmpName.compare("Rotation"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpRotation;
            if(!ModifyEnum(tmpComponent, tmpElement, tmpRotation, tmpComponent->currentText()))
            {
            }
            else
            {
                mRotation = tmpRotation.toInt();
            }
        }
    }
    else if(!tmpName.compare("Luminance"))
    {
        IDE_TRACE();
        SpinEdit *tmpComponent = VAR_CAST<SpinEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyNum(tmpComponent, tmpElement, mLuminance, tmpComponent->value(), mMinLuminance, mMaxLuminance))
            {
                IDE_TRACE();
            }
            else
            {
                IDE_TRACE_INT(mLuminance);
                //ioctl(fb_fd, IOCTL_LCD_BRIGHTNESS, &mLuminance);
                IDE_TRACE_INT(mLuminance);
                QString tmpcmd = QString("/usr/backlight_demo %1").arg(mLuminance);
                system(tmpcmd.toLatin1().data());
                //SYSTEM_CMD::SetLuminance(mLuminance);
            }
        }
    }
    else if(!tmpName.compare("DarkTimeout"))
    {
        SpinEdit *tmpComponent = VAR_CAST<SpinEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyNum(tmpComponent, tmpElement, mDarkTimeout, tmpComponent->value(), mMinTimeout, mMaxTimeout))
            {
            }
            else
            {
                if(mDarkTimeout)
                    mMonitorTimer.singleShot(mDarkTimeout, this, SIGNAL(slot_DarkTimeout()));
            }
        }
    }
    else if(!tmpName.compare("DarkLevel"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpDarkLevel;
            if(!ModifyEnum(tmpComponent, tmpElement, tmpDarkLevel, tmpComponent->currentText()))
            {
            }
            else
            {
                mDarkLevel = tmpDarkLevel.toInt();
            }
        }
    }
}

void PhysicalLCD::slot_DarkTimeout()
{
     SYSTEM_CMD::SetLuminance(mDarkLevel);
#ifdef DISPLAY
    //>@关闭视频
    LogicThemeManager *tmpThemeManager = VAR_CAST<LogicThemeManager *>(m_DevManager->GetLGDev(L_ThemeManager));
    if(!tmpThemeManager)
        return;
    tmpThemeManager->CloseVideo();
#endif
}

void PhysicalLCD::slot_StartDarkMonitor()
{
    SYSTEM_CMD::SetLuminance(mLuminance);
    if(mDarkTimeout)
        mMonitorTimer.singleShot(mDarkTimeout, this, SIGNAL(slot_DarkTimeout()));
}
