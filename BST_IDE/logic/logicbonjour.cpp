#include "logicbonjour.h"

LogicBonjour::LogicBonjour(QObject *parent)
    :LogicDevice(parent)
{
}

LogicBonjour::~LogicBonjour()
{
}

quint32 LogicBonjour::InitPara()
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
        if(!tmpName.compare("ScanEnable"))
        {
            tmpHBoxLayout = CreateBool(tmpName, tmpElement.text());
        }
        else if(!tmpName.compare("Port"))
        {
            int tmpValue, tmpMinValue, tmpMaxValue;
            QStringList tmpValueList = tmpElement.attribute("List").split(';');
            if(tmpValueList.count()!=2)
            {
                tmpValue = 0;
                tmpMinValue = 0;
                tmpMaxValue = 0;
            }
            else
            {
                tmpMinValue = tmpValueList.at(0).toInt();
                tmpMaxValue = tmpValueList.at(1).toInt();
                if(tmpValue < tmpMinValue || tmpValue > tmpMaxValue)  //>@�ж�ֵ�Ƿ���Ч
                    tmpValue = 0;
            }
            tmpHBoxLayout = CreateNum(tmpName, tmpValue, tmpMinValue, tmpMaxValue);
        }
        else if(!tmpName.compare("Type"))
        {
            tmpHBoxLayout = CreateTextEdit(tmpName, tmpElement.text());
        }
        else if(!tmpName.compare("Name"))
        {
            tmpHBoxLayout = CreateTextEdit(tmpName, tmpElement.text());
        }
        else if(!tmpName.compare("Device"))
        {
            QStringList tmpDevList = tmpElement.text().split(';');
            AppendPHDevices(tmpDevList);
            tmpHBoxLayout = CreateCheck(tmpName, tmpElement.attribute("List").split(';'), tmpDevList);
        }
        else
        {
            continue;
        }
        validcount++;
        m_SettingGroup->m_VLayout->addWidget(CreateLine());
        m_SettingGroup->m_VLayout->addLayout(tmpHBoxLayout);
    }
    validcount++;
    return validcount;
}

void LogicBonjour::UpdatePara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("ScanEnable"))
    {
        Switch *tmpComponent = VAR_CAST<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, tmpElement, mScanEnable, tmpComponent->isChecked()))
            {
            }
        }
    }
    else if(!tmpName.compare("Port"))
    {
        SpinEdit *tmpComponent = VAR_CAST<SpinEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyNum(tmpComponent, tmpElement, mPort, tmpComponent->value(), mMinPort, mMaxPort))
            {
            }
        }
    }
    else if(!tmpName.compare("Type"))
    {
        TextEdit *tmpComponent = VAR_CAST<TextEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyTextEdit(tmpComponent, tmpElement, mType, tmpComponent->text()))
            {
            }
        }
    }
    else if(!tmpName.compare("Name"))
    {
        TextEdit *tmpComponent = VAR_CAST<TextEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyTextEdit(tmpComponent, tmpElement, mName, tmpComponent->text()))
            {
            }
        }
    }
    else if(!tmpName.compare("Device"))
    {
        CheckList *tmpComponent = VAR_CAST<CheckList *>(pObject);
        if(tmpComponent)
        {
            QStringList tmpDevList = tmpComponent->GetSelectList();
            QStringList tmpList = PHDevicesAdded(tmpDevList, tmpComponent);
            if(!ModifyCheck(tmpComponent, tmpElement, tmpComponent->m_SelectList, tmpList))
            {
            }
        }
    }
}
