#include "nodecpu.h"

bool NodeCpu::InitPHPara(QDomElement pElement, EventMap *pEventMap)
{
    PhysicalDevice::InitPHPara(pElement, pEventMap);
    int count = m_ParaElementList.count();
    if(count < 1)
        return false;

    for(int i=0;i<count-1;i++)
    {
        QDomElement tmpElement = m_ParaElementList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(!tmpName.compare("Model"))
        {
            mCpuModel = m_ParaElementList.at(0).toElement().text();
        }
        else if(!tmpName.compare("UserName"))
        {
            mUsername = m_ParaElementList.at(1).toElement().text();
        }
        else if(!tmpName.compare("AutoRelease"))
        {
            mAutoRelease = (tmpElement.text().compare("False")==0)?(false):(true);
        }
        else if(!tmpName.compare("Threshold"))
        {
            mThreshold = tmpElement.text().toInt();
        }
    }
    return true;
}

void NodeCpu::UpdatePHPara(QWidget* pWidget)
{
    QString tmpObjectName = pWidget->objectName();
    for(int i=0;i<m_ParaElementList.count();i++)
    {
        QDomElement tmpElement = m_ParaElementList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(tmpObjectName.compare(tmpName))
            continue;
        if(!tmpName.compare("Model"))
        {
            TextEdit *tmpComponent = VAR_CAST<TextEdit *>(pWidget);
            if(tmpComponent)
            {
                mCpuModel = tmpComponent->text();
            }
        }
        else if(!tmpName.compare("UserName"))
        {
            TextEdit *tmpComponent = VAR_CAST<TextEdit *>(pWidget);
            if(tmpComponent)
            {
                mUsername = tmpComponent->text();
            }
        }
        else if(!tmpName.compare("AutoRelease"))
        {
            Switch *tmpComponent = VAR_CAST<Switch *>(pWidget);
            if(tmpComponent)
            {
                mAutoRelease = tmpComponent->isChecked();
            }
        }
        else if(!tmpName.compare("Threshold"))
        {
            SpinEdit *tmpComponent = VAR_CAST<SpinEdit *>(pWidget);
            if(tmpComponent)
            {
                mThreshold = tmpComponent->value();
            }
        }
        break;
    }
}

