#include "physicalrtc.h"
#ifdef DISPLAY
#include "devicemanager.h"
#endif

PhysicalRTC::PhysicalRTC(QObject *parent):
    PhysicalDevice(parent)
{
    mRTCThread = 0;
}

PhysicalRTC::~PhysicalRTC()
{
    ReleaseRTC();
}

void PhysicalRTC::InitRTC()
{
    ReleaseRTC();
    mRTCThread = new RTCThread(this);
    mRTCThread->start(QThread::LowPriority);
}

void PhysicalRTC::ReleaseRTC()
{
    if(mRTCThread != 0)
    {
        if(mRTCThread != 0)
        {
            if (mRTCThread->isRunning())
            {
                mRTCThread->stop();
                if (QThread::currentThread() != mRTCThread)
                    mRTCThread->wait();
            }
        }
        mRTCThread->quit();
        mRTCThread->deleteLater();
        mRTCThread = 0;
    }
}

quint32 PhysicalRTC::InitPara()
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
        if(!tmpName.compare("DotBlink"))
        {
            QString tmpValue = tmpElement.text();
            mDotBlink = (tmpValue.compare("false")==0)?(false):(true);
            tmpHBoxLayout = CreateBool(tmpName, tmpValue);
        }
        else if(!tmpName.compare("SecondEnable"))
        {
            QString tmpValue = tmpElement.text();
            mSecondEnable = (tmpValue.compare("false")==0)?(false):(true);
            tmpHBoxLayout = CreateBool(tmpName, tmpValue);
            if(mSecondEnable)
                mRTCThread->mDelay = 900;
            else
                mRTCThread->mDelay = 60*1000 - 100;
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
    InitRTC();
    return validcount;
}

void PhysicalRTC::UpdatePara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("DotBlink"))
    {
        Switch *tmpComponent = VAR_CAST<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, tmpElement, mDotBlink, tmpComponent->isChecked()))
            {
            }
        }
    }
    else if(!tmpName.compare("SecondEnable"))
    {
        Switch *tmpComponent = VAR_CAST<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, tmpElement, mSecondEnable, tmpComponent->isChecked()))
            {
            }
            else
            {
                if(mSecondEnable)
                    mRTCThread->mDelay = 900;
                else
                    mRTCThread->mDelay = 60*1000 - 100;
            }
        }
    }
}

void PhysicalRTC::ProcessRTC()
{
#ifdef DISPLAY
    LogicThemeManager *tmpThemeManager = VAR_CAST<LogicThemeManager *>(m_DevManager->GetLGDev(L_ThemeManager));
    if(!(tmpThemeManager && tmpThemeManager->m_UiView && tmpThemeManager->m_UiView->mScene->m_EventMap))
        return;
    QDateTime curDateTime = QDateTime::currentDateTime();
    //>@更新时间
    if(!mSecondEnable)      //>@如果在秒钟不使能的情况下，将秒钟清零
        curDateTime.time().setHMS(curDateTime.time().hour(), curDateTime.time().minute(), 0);
    tmpThemeManager->m_UiView->mScene->m_EventMap->slot_AlarmEvent(curDateTime);
#endif
}

RTCThread::RTCThread(QObject *parent):
    QThread(parent)
{
    mDelay = 900;
    terminate = false;
    mPhysicalRTC = VAR_CAST<PhysicalRTC *>(parent);
}

RTCThread::~RTCThread()
{
    terminate = true;
}

void RTCThread::stop()
{
    terminate = true;
}

void RTCThread::run()
{
    while (!terminate && mPhysicalRTC)
    {
        mPhysicalRTC->ProcessRTC();
        Poll();
    }
    terminate = false;
}

void RTCThread::Poll()
{
    quint32 i=0;
    while(i++ < mDelay)
        msleep(1);
}
