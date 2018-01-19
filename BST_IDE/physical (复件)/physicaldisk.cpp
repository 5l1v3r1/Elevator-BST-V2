#include "physicaldisk.h"

PhysicalDisk::PhysicalDisk(QObject *parent):
    PhysicalDevice(parent)
{
    connect(&mMonitorTimer, SIGNAL(timeout()), this, SLOT(slot_MonitorStorage()));
}

PhysicalDisk::~PhysicalDisk()
{
    mMonitorTimer.stop();
}

void PhysicalDisk::slot_MonitorStorage()
{
    SYSTEM_CMD::GetDiskSpace(mDevPath, mDiskTotal, mDiskUsed, mDiskFree, mUsageRate);
}

quint32 PhysicalDisk::InitPara()
{
    QDomNodeList tmpParaNodeList = m_ParaElement.childNodes();
    if(tmpParaNodeList.count() < 1)
        return 0;
    quint32 validcount = 0;
    QHBoxLayout* tmpHBoxLayout;
    for(int i=0;i<tmpParaNodeList.count()-1;i++)
    {
        QDomElement tmpElement = tmpParaNodeList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(!tmpName.compare("Model"))
        {
            mModel = tmpElement.text();
            tmpHBoxLayout = CreateTextEdit(tmpName, mModel);
        }
        else if(!tmpName.compare("BootCatalog"))
        {
            mBootCatalog = tmpElement.text();
            tmpHBoxLayout = CreateTextEdit(tmpName, mBootCatalog);
        }
        else if(!tmpName.compare("Threshold"))
        {
            mThreshold = tmpElement.text().toInt();
            QStringList tmpValueList = tmpElement.attribute("List").split(';');
            if(tmpValueList.count()!=2)
            {
                mThreshold = 0;
                mMinThreshold = 0;
                mMaxThreshold = 0;
            }
            else
            {
                mMinThreshold = tmpValueList.at(0).toInt();
                mMaxThreshold = tmpValueList.at(1).toInt();
                if(mThreshold < mMinThreshold || mThreshold > mMaxThreshold)  //>@判断值是否有效
                    mThreshold = 0;
            }
            if(mThreshold)
                mMonitorTimer.start(60000);   //>@1分钟更新一次
            tmpHBoxLayout = CreateNum(tmpName, mThreshold, mMinThreshold, mMaxThreshold);
        }
        else
        {
            continue;
        }
        validcount++;
        m_SettingGroup->m_VLayout->addWidget(CreateLine());
        m_SettingGroup->m_VLayout->addLayout(tmpHBoxLayout);
    }
    return validcount;
}

void PhysicalDisk::UpdatePara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("Model"))
    {
        TextEdit *tmpComponent = VAR_CAST<TextEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyTextEdit(tmpComponent, tmpElement, mModel, tmpComponent->text()))
            {
            }
        }
    }
    else if(!tmpName.compare("BootCatalog"))
    {
        TextEdit *tmpComponent = VAR_CAST<TextEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyTextEdit(tmpComponent, tmpElement, mBootCatalog, tmpComponent->text()))
            {
            }
        }
    }
    else if(!tmpName.compare("Threshold"))
    {
        SpinEdit *tmpComponent = VAR_CAST<SpinEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyNum(tmpComponent, tmpElement, mThreshold, tmpComponent->value(), mMinThreshold, mMaxThreshold))
            {
            }
            else
            {
                if(mThreshold)
                    mMonitorTimer.start(60000);   //>@1分钟更新一次
            }
        }
    }
}

PhysicalMobileDisk::PhysicalMobileDisk(QObject *parent):
    PhysicalMovable(parent)
{
}

PhysicalMobileDisk::~PhysicalMobileDisk()
{
}

bool PhysicalMobileDisk::InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout)
{
    if(!pName.compare("Model"))
    {
        mModel = pElement.text();
        pHBoxLayout = CreateTextEdit(pName, mModel);
    }
    else
    {
        return false;
    }
    return true;
}

void PhysicalMobileDisk::UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement)
{
    if(!pName.compare("Model"))
    {
        TextEdit *tmpComponent = VAR_CAST<TextEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyTextEdit(tmpComponent, pElement, mModel, tmpComponent->text()))
            {
            }
        }
    }
}
