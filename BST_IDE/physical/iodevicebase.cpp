#include "iodevicebase.h"
#ifdef IDE
    #include "devicescene.h"
    #include "projectmanager.h"
    #include "toolbar.h"
#elif DISPLAY
    #include "devicemanager.h"
#endif

IODeviceBase::IODeviceBase(QObject *parent) :
    QIODevice(parent)
{
    m_SettingGroup = 0;
#ifdef IDE
    m_DeviceScene = VAR_CAST<DeviceScene *>(parent);
#elif DISPLAY
    m_DevManager = VAR_CAST<DeviceManager *>(parent);
#endif
}

IODeviceBase::~IODeviceBase()
{
    if(m_SettingGroup)
    {
        m_SettingGroup->deleteLater();
        m_SettingGroup = 0;
    }

#ifdef IDE
    DeleteDevItem();
#endif
}

#ifdef IDE
void IODeviceBase::DeleteDevItem()
{
    if(m_DeviceItem)
    {
        m_DeviceItem = 0;
    }
}
#endif


QFrame* IODeviceBase::CreateLine()
{
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}

QHBoxLayout* IODeviceBase::CreateButton(QString pName, QStringList pBtnListTag)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    ButtonList *tmpButtonList = new ButtonList(pBtnListTag);
    tmpButtonList->setObjectName(pName);
    connect(tmpButtonList, SIGNAL(sChanged(QObject*)), this, SLOT(UpdatePara(QObject*)));
    mParaItems.insert(pName, tmpButtonList);
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addLayout(tmpButtonList);
    return tmpHBoxLayout;
}

bool IODeviceBase::ModifyButton(ButtonList *pComponent, QDomElement &pElement, QStringList &pDstValue, QStringList pNewValue)
{
    if(pComponent && !pElement.isNull())
    {
        return true;
    }
    return false;
}

QHBoxLayout* IODeviceBase::CreateTextEdit(QString pName, QString pValue)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    TextEdit *tmpTextEdit = new TextEdit(pValue);
    tmpTextEdit->setObjectName(pName);
    connect(tmpTextEdit, SIGNAL(sChanged(QObject*)), this, SLOT(UpdatePara(QObject*)));
    mParaItems.insert(pName, tmpTextEdit);
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addWidget(tmpTextEdit);
    return tmpHBoxLayout;
}

bool IODeviceBase::ModifyTextEdit(TextEdit *pComponent, QDomElement &pElement, QString &pDstValue, QString pNewValue)
{
    if(pComponent && !pElement.isNull())
    {
        QString tmpOldValue = pElement.text();
        if(tmpOldValue.compare(pNewValue))
        {
            if(!SetElementText(pElement, pNewValue))
            {
                pComponent->setText(tmpOldValue);
                return false;
            }
            pDstValue = pNewValue;
        }
        return true;
    }
    return false;
}

QHBoxLayout* IODeviceBase::CreateEnum(QString pName, QStringList pParaList, QString pSelectPara)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    ComboBox *tmpComboBox = new ComboBox(pParaList, pSelectPara);
    tmpComboBox->setObjectName(pName);
    connect(tmpComboBox, SIGNAL(sChanged(QObject*)), this, SLOT(UpdatePara(QObject*)));
    mParaItems.insert(pName, tmpComboBox);
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addWidget(tmpComboBox);
    return tmpHBoxLayout;
}

bool IODeviceBase::ModifyEnum(ComboBox *pComponent, QDomElement &pElement, QString &pDstValue, QString pNewValue)
{
    if(pComponent && !pElement.isNull())
    {
        QString tmpOldValue = pElement.text();
        if(tmpOldValue.compare(pNewValue))
        {
            if(!SetElementText(pElement, pNewValue))
            {
                pComponent->setCurrentText(tmpOldValue);
                return false;
            }
            pDstValue = pNewValue;
        }
        return true;
    }
    return false;
}

QHBoxLayout* IODeviceBase::CreateBool(QString pName, QString pPara)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    Switch *tmpSwitch = new Switch(pPara, "TRUE", "FALSE");
    tmpSwitch->setObjectName(pName);
    connect(tmpSwitch, SIGNAL(sChanged(QObject*)), this, SLOT(UpdatePara(QObject*)));
    mParaItems.insert(pName, tmpSwitch);
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addWidget(tmpSwitch);
    return tmpHBoxLayout;
}

bool IODeviceBase::ModifyBool(Switch *pComponent, QDomElement &pElement, bool &pDstValue, bool pNewValue)
{
    if(pComponent && !pElement.isNull())
    {
        bool tmpOldValue = (pElement.text().compare("TRUE", Qt::CaseInsensitive))?false:true;
        if(tmpOldValue != pNewValue)
        {
            if(!SetElementText(pElement, pNewValue?("TRUE"):("FALSE")))
            {
                pComponent->setChecked(tmpOldValue);
                return false;
            }
            pDstValue = pNewValue;
        }
        return true;
    }
    return false;
}

QHBoxLayout* IODeviceBase::CreateSwitch(QString pName, QString pPara)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    Switch *tmpSwitch = new Switch(pPara, "ON", "OFF");
    tmpSwitch->setObjectName(pName);
    connect(tmpSwitch, SIGNAL(sChanged(QObject*)), this, SLOT(UpdatePara(QObject*)));
    mParaItems.insert(pName, tmpSwitch);
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addWidget(tmpSwitch);
    return tmpHBoxLayout;
}

bool IODeviceBase::ModifySwitch(Switch *pComponent, QDomElement &pElement, bool &pDstValue, bool pNewValue)
{
    if(pComponent && !pElement.isNull())
    {
        bool tmpOldValue = (pElement.text().compare("ON", Qt::CaseInsensitive))?false:true;
        if(tmpOldValue != pNewValue)
        {
            if(!SetElementText(pElement, pNewValue?("ON"):("OFF")))
            {
                pComponent->setChecked(tmpOldValue);
                return false;
            }
            pDstValue = pNewValue;
        }
        return true;
    }
    return false;
}

QHBoxLayout* IODeviceBase::CreateENetAddrEdit(QString pName, QString pIP)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    ENetAddrEdit *tmpNetAddrEdit = new ENetAddrEdit(ENET_ADDR(pIP));
    tmpNetAddrEdit->setObjectName(pName);
    connect(tmpNetAddrEdit, SIGNAL(sChanged(QObject*)), this, SLOT(UpdatePara(QObject*)));
    mParaItems.insert(pName, tmpNetAddrEdit);
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addLayout(tmpNetAddrEdit);
    return tmpHBoxLayout;
}

bool IODeviceBase::ModifyENetAddrEdit(ENetAddrEdit *pComponent, QDomElement &pElement, ENET_ADDR &pDstValue, ENET_ADDR pNewValue)
{
    if(pComponent && !pElement.isNull())
    {
        QString tmpOldValue = pElement.text();
        QString tmpNewValue = pNewValue.toQString();
        if(tmpOldValue.compare(tmpNewValue))
        {
            if(!SetElementText(pElement, tmpNewValue))
            {
                pComponent->SetIP(ENET_ADDR(tmpOldValue));
                return false;
            }
            pDstValue = pNewValue;
        }
        return true;
    }
    return false;
}

QHBoxLayout* IODeviceBase::CreateDateTimeEdit(QString pName, QDateTime pDateTime)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    DateTimeEdit *tmpDateTimeEdit = new DateTimeEdit(pDateTime);
    tmpDateTimeEdit->setObjectName(pName);
    connect(tmpDateTimeEdit, SIGNAL(sChanged(QObject*)), this, SLOT(UpdatePara(QObject*)));
    mParaItems.insert(pName, tmpDateTimeEdit);
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addLayout(tmpDateTimeEdit);
    return tmpHBoxLayout;
}

bool IODeviceBase::ModifyDateTimeEdit(DateTimeEdit *pComponent, QDomElement &pElement, QDateTime &pDstValue, QDateTime pNewValue)
{
    if(pComponent && !pElement.isNull())
    {
        QString tmpNewValue = pElement.text();
        QString tmpOldValue = pNewValue.toString(Qt::LocalDate);
        if(tmpNewValue.compare(tmpOldValue))
        {
            if(!SetElementText(pElement, pNewValue.toString(Qt::LocalDate)))
            {
                pComponent->SetDateTime(QDateTime::fromString(tmpOldValue, Qt::LocalDate));
                return false;
            }
            pDstValue = pNewValue;
        }
        return true;
    }
    return false;
}

QHBoxLayout* IODeviceBase::CreateCheck(QString pName, QStringList pParaList, QStringList pSelectParaList)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    CheckList *tmpCheckList = new CheckList(pParaList, pSelectParaList);
    tmpCheckList->setObjectName(pName);
    connect(tmpCheckList, SIGNAL(sChanged(QObject*)), this, SLOT(UpdatePara(QObject*)));
    mParaItems.insert(pName, tmpCheckList);
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addLayout(tmpCheckList);
    return tmpHBoxLayout;
}

bool IODeviceBase::ModifyCheck(CheckList *pComponent, QDomElement &pElement, QStringList &pDstValue, QStringList pNewValue)
{
    if(pComponent && !pElement.isNull())
    {
        QString tmpOldValue = pElement.text();
        QString tmpNewValue = pComponent->GetSelectText(pNewValue);
        if(tmpOldValue.compare(tmpNewValue))
        {
            if(!SetElementText(pElement, tmpNewValue))
            {
                pComponent->InitCheckList(tmpOldValue.split(';'));
                return false;
            }
            pDstValue = pNewValue;
        }
        return true;
    }
    return false;
}

QHBoxLayout* IODeviceBase::CreateNum(QString pName, int pValue, int pMinValue, int pMaxValue)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    SpinEdit *tmpSpinEdit = new SpinEdit(pValue, pMinValue, pMaxValue);
    tmpSpinEdit->setObjectName(pName);
    connect(tmpSpinEdit, SIGNAL(sChanged(QObject*)), this, SLOT(UpdatePara(QObject*)));
    mParaItems.insert(pName, tmpSpinEdit);
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addWidget(tmpSpinEdit);
    return tmpHBoxLayout;
}

//>@–ﬁ∏ƒXML“‘º∞≈–∂œ¥À÷µ «∑Ò”––ß
bool IODeviceBase::ModifyNum(SpinEdit *pComponent, QDomElement &pElement, int &pDstValue, int pNewValue, int pMinValue, int pMaxValue)
{
    if(pComponent && !pElement.isNull())
    {
        int tmpOldValue = pElement.text().toInt();
        if(tmpOldValue != pNewValue)
        {
            if(pNewValue < pMinValue || pNewValue > pMaxValue)  //>@≈–∂œ÷µ «∑Ò”––ß
            {
                pComponent->setValue(tmpOldValue);
                return false;
            }
            if(!SetElementText(pElement,QString("%1").arg(pNewValue)))
            {
                pComponent->setValue(tmpOldValue);
                return false;
            }
            pDstValue = pNewValue;
        }
        return true;
    }
    return false;
}

bool IODeviceBase::SetElementText(QDomElement pElement, QString pValue)
{
    if(pElement.text().compare(pValue))
    {
#ifdef IDE
        if(m_DeviceScene && m_DeviceScene->mMdpManager)
        {
            return m_DeviceScene->mMdpManager->SetText(pElement, pValue);
        }
#elif DISPLAY
        if(m_DevManager && m_DevManager->mMdpManager)
        {
            return m_DevManager->mMdpManager->SetText(pElement, pValue);
        }
#endif
    }
    return true;
}

void IODeviceBase::UpdateProgress(QString pMsg, int pValue)
{
#ifdef IDE
    GlobalPara.updateProgress(pMsg, pValue);
#elif DISPLAY
    if(m_DevManager)
        m_DevManager->UpdateProgress(pMsg, pValue);
#endif
}

bool IODeviceBase::SetPara(QString pParaName, QString pParaValue)
{
    IDE_TRACE_STR(pParaName);
    IDE_TRACE_STR(pParaValue);
    //>@ªÒ»??ÿå˛¿‡–Õ
    QDomElement tmpOldElement = m_ParaElement.firstChildElement(pParaName);
    if(tmpOldElement.isNull())
        return false;
    BASICUI_TYPE tmpType = getBasicUiType(tmpOldElement.attribute("Para"));
    switch(tmpType)
    {
        case BASICUI_ENUM:
        {
            ComboBox *tmpComponent = m_SettingGroup->findChild<ComboBox *>(pParaName);
            if(tmpComponent)
            {
                tmpComponent->setCurrentText(pParaValue);
            }
            break;
        }
        case BASICUI_BOOL:
        {
            Switch *tmpComponent = m_SettingGroup->findChild<Switch *>(pParaName);
            if(tmpComponent)
            {
                tmpComponent->setChecked((pParaValue.compare("TRUE", Qt::CaseInsensitive))?false:true);
            }
            break;
        }
        case BASICUI_STRING:
        {
            TextEdit *tmpComponent = m_SettingGroup->findChild<TextEdit *>(pParaName);
            if(tmpComponent)
            {
                tmpComponent->setText(pParaValue);
            }
            break;
        }
        case BASICUI_ENETADDR:
        {
            ENetAddrEdit *tmpComponent = m_SettingGroup->findChild<ENetAddrEdit *>(pParaName);
            if(tmpComponent)
            {
                tmpComponent->SetIP(ENET_ADDR(pParaValue));
            }
            break;
        }
        case BASICUI_CHECK:
        {
            CheckList *tmpComponent = m_SettingGroup->findChild<CheckList *>(pParaName);
            if(tmpComponent)
            {
                tmpComponent->InitCheckList(pParaValue.split(';'));
            }
            break;
        }
        case BASICUI_BUTTON:
        {
            ButtonList *tmpComponent = m_SettingGroup->findChild<ButtonList *>(pParaName);
            if(tmpComponent)
            {
                //tmpComponent->setCurrentText(pParaValue.split(';'));
            }
            break;
        }
        case BASICUI_SWITCH:
        {
            Switch *tmpComponent = m_SettingGroup->findChild<Switch *>(pParaName);
            if(tmpComponent)
            {
                tmpComponent->setChecked((pParaValue.compare("ON", Qt::CaseInsensitive))?false:true);
            }
            break;
        }
        case BASICUI_NUM:
        {
        IDE_TRACE();
            SpinEdit *tmpComponent = m_SettingGroup->findChild<SpinEdit *>(pParaName);
            if(tmpComponent)
            {
                tmpComponent->setValue(pParaValue.toInt());
            }
            break;
        }
        default:return false;
    }
    return true;
}

bool IODeviceBase::SetPara(QDomElement &pElement, xmlParse *pParse)
{
    if(pElement.isNull() || pParse==0)
        return false;
    //>@…Ë÷√ Ù–‘??∑÷
    QList<QDomAttr> tmpList = pParse->GetItemElementAttrs(pElement);
    for(int i=0;i<tmpList.count();i++)
    {
        QDomAttr tmpDomAttr = tmpList.at(i);
        if(tmpDomAttr.isNull())
            continue;
        QString tmpName = tmpDomAttr.name();
        QString tmpValue = tmpDomAttr.value();
        SetPara(tmpName, tmpValue);
    }
    //>@÷æ––÷µ??∑÷
    return SetValue(pElement.text());
}

QString IODeviceBase::GetPara(QString pParaName)
{
    QDomElement tmpOldElement = m_ParaElement.firstChildElement(pParaName);
    if(tmpOldElement.isNull())
        return QString();
    return tmpOldElement.text();
}

bool IODeviceBase::GetPara(QDomElement &pElement, xmlParse *pParse)
{
    if(pElement.isNull() || pParse==0)
        return false;
    QList<QDomAttr> tmpList = pParse->GetItemElementAttrs(pElement);
    for(int i=0;i<tmpList.count();i++)
    {
        QDomAttr tmpDomAttr = tmpList.at(i);
        if(tmpDomAttr.isNull())
            continue;
        QString tmpName = tmpDomAttr.name();
        QString tmpValue = GetPara(tmpName);
        if(tmpValue.isEmpty())
            continue;
        //>@ªÒ»??ÿå˛¿‡–Õ
        tmpDomAttr.setValue(tmpValue);
    }
    return true;
}
