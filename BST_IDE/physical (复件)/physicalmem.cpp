#include "physicalmem.h"
#ifdef DISPLAY
#include "devicemanager.h"
#endif

PhysicalMEM::PhysicalMEM(QObject *parent):
    PhysicalDevice(parent)
{
    connect(&mMonitorTimer, SIGNAL(timeout()), this, SLOT(slot_MonitorMem()));
}

PhysicalMEM::~PhysicalMEM()
{
    mMonitorTimer.stop();
}

void PhysicalMEM::slot_MonitorMem()
{
    double tmpMemoryUsed, tmpMemoryFree, tmpMemoryTotal;
    if(!SYSTEM_CMD::GetMemSpace(tmpMemoryTotal, tmpMemoryUsed, tmpMemoryFree))
        return;
    mUsageRate = tmpMemoryUsed * 100 / tmpMemoryTotal;
    if(mUsageRate > mThreshold)
    {
#ifdef DISPLAY
        //>@通过释放THEME中缓存的图片释放MEM
        LogicThemeManager *tmpThemeManager = VAR_CAST<LogicThemeManager *>(m_DevManager->GetLGDev(L_ThemeManager));
        if(!tmpThemeManager)
            return;
        tmpThemeManager->ReleaseImageBuffer();
#endif
    }
}

quint32 PhysicalMEM::InitPara()
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
            mMemModel = tmpElement.text();
            tmpHBoxLayout = CreateTextEdit(tmpName, mMemModel);
        }
        else if(!tmpName.compare("AutoRelease"))
        {
            mAutoRelease = (tmpElement.text().compare("false")==0)?(false):(true);
            tmpHBoxLayout = CreateBool(tmpName, tmpElement.text());
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
    //>@后处理
    if(mAutoRelease && mThreshold)
        mMonitorTimer.start(60000); //>@1分钟监测一次
    return validcount;
}

void PhysicalMEM::UpdatePara(QObject* pObject)
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
            if(!ModifyTextEdit(tmpComponent, tmpElement, mMemModel, tmpComponent->text()))
            {
            }
        }
    }
    else if(!tmpName.compare("AutoRelease"))
    {
        Switch *tmpComponent = VAR_CAST<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, tmpElement, mAutoRelease, tmpComponent->isChecked()))
            {
            }
            else
            {
                if(mAutoRelease && mThreshold)
                    mMonitorTimer.start(60000); //>@1分钟监测一次
                else
                    mMonitorTimer.stop();
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
                if(mAutoRelease && mThreshold)
                    mMonitorTimer.start(60000); //>@1分钟监测一次
                else
                    mMonitorTimer.stop();
            }
        }
    }
}

