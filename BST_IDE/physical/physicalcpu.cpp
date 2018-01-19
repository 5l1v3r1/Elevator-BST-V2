#include "physicalcpu.h"
#ifdef DISPLAY
#include "devicemanager.h"
#endif
#include "logicthememanager.h"

PhysicalCPU::PhysicalCPU(QObject *parent) :
    PhysicalDevice(parent)
{
    connect(&mMonitorTimer, SIGNAL(timeout()), this, SLOT(slot_MonitorCPU()));
}

PhysicalCPU::~PhysicalCPU()
{
    mMonitorTimer.stop();
}

QString GetUsbScanPath(QString pCPUModel)
{
    QString tmpScanPath;
    if(pCPUModel.compare("N329xx", Qt::CaseInsensitive)==0)
        tmpScanPath = QString("/sys/devices/platform/omap/musb-ti81xx/musb-hdrc.1/");
    else if(pCPUModel.compare("AM335x", Qt::CaseInsensitive)==0)
        tmpScanPath = QString("/sys/devices/platform/omap/musb-ti81xx/musb-hdrc.1/");
    else if(pCPUModel.compare("DM3730", Qt::CaseInsensitive)==0)
        tmpScanPath = QString("/sys/devices/platform/omap/musb-ti81xx/musb-hdrc.1/");
    else if(pCPUModel.compare("IMX6x", Qt::CaseInsensitive)==0)
        tmpScanPath = QString("/sys/devices/platform/omap/musb-ti81xx/musb-hdrc.1/");
    return tmpScanPath;
}

void PhysicalCPU::slot_MonitorCPU()
{
    double tmpTotal, tmpUsage;
    if(!SYSTEM_CMD::GetCpuLoad(tmpTotal, tmpUsage))
        return;
    mUsageRate = tmpUsage*100/tmpTotal;
#ifdef DISPLAY
    //>@通过关闭视频播放模块以及关闭THEME特效释放CPU
    LogicThemeManager *tmpThemeManager = VAR_CAST<LogicThemeManager *>(m_DevManager->GetLGDev(L_ThemeManager));
    if(!tmpThemeManager)
        return;
    if(mUsageRate < mThreshold)
    {
        tmpThemeManager->CloseVideo();
        tmpThemeManager->DisableEffect();
    }
    else
    {
        tmpThemeManager->RestoreVideo();
        tmpThemeManager->EnableEffect();
    }
#endif
}

quint32 PhysicalCPU::InitPara()
{
    QDomNodeList tmpParaNodeList = m_ParaElement.childNodes();
    if(tmpParaNodeList.count() < 1)
        return 0;
    quint32 validcount = 0;
    QHBoxLayout* tmpHBoxLayout = 0;
    for(int i=0;i<tmpParaNodeList.count()-1;i++)
    {
        QDomElement tmpElement = tmpParaNodeList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(!tmpName.compare("Model"))
        {
            mCpuModel = tmpElement.text();
            mUsbScanPath = GetUsbScanPath(mCpuModel);
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), mCpuModel);
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

void PhysicalCPU::UpdatePara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("Model"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyEnum(tmpComponent, tmpElement, mCpuModel, tmpComponent->currentText()))
            {
            }
            else
            {
                mUsbScanPath = GetUsbScanPath(mCpuModel);
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

