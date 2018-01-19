#include "physicalbase.h"
#ifdef IDE
    #include "devicescene.h"
#elif DISPLAY
    #include "devicemanager.h"
#endif
#include "logicbase.h"

PhysicalDevice::PhysicalDevice(QObject *parent) :
    IODeviceBase(parent)
{
    mCon = false;
    mDevState = S_UNPLUGGED;
    mLGDevices.clear();
}

PhysicalDevice::~PhysicalDevice()
{
    mDevState = S_UNPLUGGED;
    mCon = false;

    //>@删除其逻辑节点中对自己的依赖
    QList<LG_Type> tmpList = mLGDevices.keys();
    for(int i=0;i<tmpList.count();i++)
    {
        LogicDevice *tmpLGDev = mLGDevices.take(tmpList.at(i));
        if(!tmpLGDev)
            continue;
        tmpLGDev->PHDeviceDeleted(this, false);
    }
    mLGDevices.clear();
}

void PhysicalDevice::SetDevState(DEVICESTATE pState, QString pDevPath)
{
    if(mDevState != pState)
    {
        if(pState == S_PLUGGED)
        {
            if(!pDevPath.endsWith("/"))
                pDevPath.append("/");
            mDevPath = pDevPath;
        }
        mDevState = pState;
        emit sDeviceStateChanged();
    }
}

void PhysicalDevice::AppendLGDev(LG_Type pType, LogicDevice *pDevice)
{
    if(!mLGDevices.contains(pType))
        mLGDevices.insert(pType, pDevice);
}

bool PhysicalDevice::InitDevice(PH_Type pPHType, QDomElement pElement)
{
    if(pPHType == P_INVALID || pElement.isNull())
        return false;
    m_ParaElementList = pElement.childNodes();
    mDevType = pPHType;
    mDevName = pElement.tagName();
    if(!pElement.hasChildNodes())
        return false;
    m_ParaElement = pElement;
    //>@创建配置页面
    if(m_SettingGroup)
        m_SettingGroup->deleteLater();
    m_SettingGroup = new Groupbox(mDevName);
    if(!m_SettingGroup)
        return false;
    //>@初始化参数项
    quint32 count = InitPara();
    if(!count)
    {
        m_SettingGroup->deleteLater();
        m_SettingGroup = 0;
        return false;
    }
    m_SettingGroup->setFixedHeight(80+count*35);
    return true;
}

