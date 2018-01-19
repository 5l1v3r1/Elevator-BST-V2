#include "physicalaudio.h"

PhysicalAudio::PhysicalAudio(QObject *parent) :
    PhysicalDevice(parent)
{
}

PhysicalAudio::~PhysicalAudio()
{
}

void PhysicalAudio::ScanAudioDevice()
{
    mOutputDevList.clear();
    mInputDevList.clear();
    //>@寻找音频输入输出设备
}

quint32 PhysicalAudio::InitPara()
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
        if(!tmpName.compare("Mode"))
        {
            QString tmpMode = tmpElement.text();
            mMode = GetMode(tmpMode);
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), tmpMode);
        }
        else if(!tmpName.compare("Volume"))
        {
            mVolume = tmpElement.text().toInt();
            QStringList tmpValueList = tmpElement.attribute("List").split(';');
            if(tmpValueList.count()!=2)
            {
                mMinVolume = 0;
                mMaxVolume = 0;
                mVolume = mMinVolume;
            }
            else
            {
                mMinVolume = tmpValueList.at(0).toInt();
                mMaxVolume = tmpValueList.at(1).toInt();
                if(mVolume < mMinVolume || mVolume > mMaxVolume)  //>@判断值是否有效
                    mVolume = mMinVolume;
            }
            SYSTEM_CMD::SetVolume(mVolume);
            tmpHBoxLayout = CreateNum(tmpName, mVolume, mMinVolume, mMaxVolume);
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

void PhysicalAudio::UpdatePara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("Mode"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpMode;
            if(!ModifyEnum(tmpComponent, tmpElement, tmpMode, tmpComponent->currentText()))
            {
                return;
            }
            mMode = GetMode(tmpMode);
        }
    }
    else if(!tmpName.compare("Volume"))
    {
        SpinEdit *tmpComponent = VAR_CAST<SpinEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyNum(tmpComponent, tmpElement, mVolume, tmpComponent->value(), mMinVolume, mMaxVolume))
            {
                return;
            }
            SYSTEM_CMD::SetVolume(mVolume);
        }
    }
}
