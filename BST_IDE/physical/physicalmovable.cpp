#include "physicalmovable.h"

PhysicalMovable::PhysicalMovable(QObject *parent):
    PhysicalDevice(parent)
{
    mIsMovable = false;
}

PhysicalMovable::~PhysicalMovable()
{
}

quint32 PhysicalMovable::InitPara()
{
    QDomNodeList tmpParaNodeList = m_ParaElement.childNodes();
    if(tmpParaNodeList.count() < 1)
        return 0;
    PreInitProcess();
    quint32 validcount = 0;
    for(int i=0;i<tmpParaNodeList.count();i++)
    {
        QDomElement tmpElement = tmpParaNodeList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QHBoxLayout* tmpHBoxLayout = 0;
        QString tmpName = tmpElement.tagName();
        if(!tmpName.compare("Movable"))
        {
            QString tmpValue = tmpElement.text();
            mIsMovable = (tmpValue.compare("true",Qt::CaseInsensitive)==0)?true:false;
            tmpHBoxLayout = CreateBool(tmpName, tmpValue);
        }
        else if(!InitSubPHPara(tmpName, tmpElement, tmpHBoxLayout))
        {
            continue;
        }
        validcount++;
        m_SettingGroup->m_VLayout->addWidget(CreateLine());
        m_SettingGroup->m_VLayout->addLayout(tmpHBoxLayout);
    }
    if(!mIsMovable)
    {
        SetPlugged();
        mDevState = S_PLUGGED;
    }
    //>@后处理
    PostInitProcess();
    return validcount;
}

void PhysicalMovable::UpdatePara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    PreUpdateProcess();
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("Movable"))
    {
        Switch *tmpComponent = VAR_CAST<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, tmpElement, mIsMovable, tmpComponent->isChecked()))
            {
                if(!mIsMovable)
                {
                    SetPlugged();
                    mDevState = S_PLUGGED;
                }
            }
        }
    }
    else
    {
        UpdateSubPHPara(tmpName, pObject, tmpElement);
    }
    //>@后处理
    PostUpdateProcess();
}

//>@对于任何可移动设备，只允许有一个活动设备，如果再插入相同的设备，系统不进行处理。
//>@当拔出设备时，必须判断此设备的路径与mDevPath是否相同，如果不相同，则认为不是当前有效设备被拔出
void PhysicalMovable::slot_DevStateChanged(DEVICESTATE pState, QString pDevPath)
{
    if(!mIsMovable)  //>@如果当前设备不是可插拔设备，则执行此函数无效
        return;
    if(mDevState != pState)  //>@查看是否已经存在可插拔设备，如果是，则新插入的设备不处理
    {
        bool ok = false;
        switch(pState)
        {
            case S_UNPLUGGED:
            {
                if(mDevPath.compare(pDevPath))
                    return;
                ok = SetUnplugged();
                break;
            }
            case S_PLUGGED:
            {
                QString tmpBkPath = mDevPath;
                mDevPath = pDevPath;
                ok = SetPlugged();
                if(!ok)
                    mDevPath = tmpBkPath;
                break;
            }
            default:
                break;
        }
        if(ok)
            mDevState = pState;
    }
}

