#include "interfacebase.h"

InterfaceBase::InterfaceBase(QWidget *parent) :
    Groupbox(parent)
{
}

bool InterfaceBase::InitAnalysis(QDomElement pElement)
{
    if(pElement.isNull())
        return false;
    if(!pElement.hasChildNodes())
        return false;
    mUIType = ANALYSIS;
    //>@初始化参数项
    quint32 count = InitSubAnalysis(pElement.childNodes());
    setFixedHeight(80+count);
    return true;
}

bool InterfaceBase::InitGenerator(QDomElement pElement)
{
    if(pElement.isNull())
        return false;
    if(!pElement.hasChildNodes())
        return false;
    mUIType = GENERATOR;
    //>@初始化参数项
    quint32 count = InitSubGenerator(pElement.childNodes());
    setFixedHeight(80+count);
    return true;
}

quint32 InterfaceBase::InitSubAnalysis(QDomNodeList pParaList)
{
    quint32 validcount = 0;
    QHBoxLayout* tmpHBoxLayout;
    for(int i=0;i<pParaList.count();i++)
    {
        QDomElement tmpElement = pParaList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpType = tmpElement.attribute("Para");
        BASICUI_TYPE tmpUIType = getBasicUiType(tmpType);
        switch(tmpUIType)
        {
            case BASICUI_ENUM:
            {
                tmpHBoxLayout = CreateEnum(tmpElement.tagName(), tmpElement.attribute("List").split(';'), tmpElement.text());
                break;
            }
            case BASICUI_CHECK:
            {
                tmpHBoxLayout = CreateCheck(tmpElement.tagName(), tmpElement.attribute("List").split(';'), tmpElement.text().split(';'));
                break;
            }
            case BASICUI_BUTTON:
            {
                tmpHBoxLayout = CreateButton(tmpElement.tagName(), tmpElement.attribute("List").split(';'));
                break;
            }
            case BASICUI_SWITCH:
            {
                tmpHBoxLayout = CreateSwitch(tmpElement.tagName(), tmpElement.text());
                break;
            }
            case BASICUI_NUM:
            {
                QStringList tmpValueList = tmpElement.attribute("List").split(';');
                if(tmpValueList.count()!=2)
                    continue;
                tmpHBoxLayout = CreateNum(tmpElement.tagName(), tmpElement.text().toInt(), tmpValueList.at(0).toInt(), tmpValueList.at(1).toInt());
                break;
            }
            default:
                continue;
        }
        validcount++;
        m_VLayout->addWidget(CreateLine());
        m_VLayout->addLayout(tmpHBoxLayout);
    }
    return (validcount*35);
}

quint32 InterfaceBase::InitSubGenerator(QDomNodeList pParaList)
{
    quint32 validcount = 0;
    QHBoxLayout* tmpHBoxLayout;
    for(int i=0;i<pParaList.count();i++)
    {
        QDomElement tmpElement = pParaList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpType = tmpElement.attribute("Para");
        BASICUI_TYPE tmpUIType = getBasicUiType(tmpType);
        switch(tmpUIType)
        {
            case BASICUI_ENUM:
            {
                tmpHBoxLayout = CreateEnum(tmpElement.tagName(), tmpElement.attribute("List").split(';'), tmpElement.text());
                break;
            }
            case BASICUI_CHECK:
            {
                tmpHBoxLayout = CreateCheck(tmpElement.tagName(), tmpElement.attribute("List").split(';'), tmpElement.text().split(';'));
                break;
            }
            case BASICUI_BUTTON:
            {
                tmpHBoxLayout = CreateButton(tmpElement.tagName(), tmpElement.attribute("List").split(';'));
                break;
            }
            case BASICUI_SWITCH:
            {
                tmpHBoxLayout = CreateSwitch(tmpElement.tagName(), tmpElement.text());
                break;
            }
            case BASICUI_NUM:
            {
                QStringList tmpValueList = tmpElement.attribute("List").split(';');
                if(tmpValueList.count()!=2)
                    continue;
                tmpHBoxLayout = CreateNum(tmpElement.tagName(), tmpElement.text().toInt(), tmpValueList.at(0).toInt(), tmpValueList.at(1).toInt());
                break;
            }
            default:
                continue;
        }
        validcount++;
        m_VLayout->addWidget(CreateLine());
        m_VLayout->addLayout(tmpHBoxLayout);
    }
    return (validcount*35);
}

QFrame* InterfaceBase::CreateLine()
{
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}

QHBoxLayout* InterfaceBase::CreateButton(QString pName, QStringList pBtnListTag)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    ButtonList *tmpButtonList = new ButtonList(pBtnListTag);
    tmpButtonList->setObjectName(pName);
    tmpButtonList->SetButtonCheckable(true);
    connect(tmpButtonList, SIGNAL(sChanged(QObject*)), this, (mUIType == ANALYSIS)?(SLOT(UpdateAnalysis(QObject*))):(SLOT(UpdateGenerator(QObject*))));
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addLayout(tmpButtonList);
    return tmpHBoxLayout;
}

QHBoxLayout* InterfaceBase::CreateTextEdit(QString pName, QString pValue)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    TextEdit *tmpTextEdit = new TextEdit(pValue);
    tmpTextEdit->setObjectName(pName);
    connect(tmpTextEdit, SIGNAL(sChanged(QObject*)), this, (mUIType == ANALYSIS)?(SLOT(UpdateAnalysis(QObject*))):(SLOT(UpdateGenerator(QObject*))));
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addWidget(tmpTextEdit);
    return tmpHBoxLayout;
}

QHBoxLayout* InterfaceBase::CreateEnum(QString pName, QStringList pParaList, QString pSelectPara)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    ComboBox *tmpComboBox = new ComboBox(pParaList, pSelectPara);
    tmpComboBox->setObjectName(pName);
    connect(tmpComboBox, SIGNAL(sChanged(QObject*)), this, (mUIType == ANALYSIS)?(SLOT(UpdateAnalysis(QObject*))):(SLOT(UpdateGenerator(QObject*))));
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addWidget(tmpComboBox);
    return tmpHBoxLayout;
}

QHBoxLayout* InterfaceBase::CreateBool(QString pName, QString pPara)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    Switch *tmpSwitch = new Switch(pPara, "TRUE", "FALSE");
    tmpSwitch->setObjectName(pName);
    connect(tmpSwitch, SIGNAL(sChanged(QObject*)), this, (mUIType == ANALYSIS)?(SLOT(UpdateAnalysis(QObject*))):(SLOT(UpdateGenerator(QObject*))));
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addWidget(tmpSwitch);
    return tmpHBoxLayout;
}

QHBoxLayout* InterfaceBase::CreateSwitch(QString pName, QString pPara)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    Switch *tmpSwitch = new Switch(pPara, "ON", "OFF");
    tmpSwitch->setObjectName(pName);
    connect(tmpSwitch, SIGNAL(sChanged(QObject*)), this, (mUIType == ANALYSIS)?(SLOT(UpdateAnalysis(QObject*))):(SLOT(UpdateGenerator(QObject*))));
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addWidget(tmpSwitch);
    return tmpHBoxLayout;
}

QHBoxLayout* InterfaceBase::CreateCheck(QString pName, QStringList pParaList, QStringList pSelectParaList)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    CheckList *tmpCheckList = new CheckList(pParaList, pSelectParaList);
    tmpCheckList->setObjectName(pName);
    connect(tmpCheckList, SIGNAL(sChanged(QObject*)), this, (mUIType == ANALYSIS)?(SLOT(UpdateAnalysis(QObject*))):(SLOT(UpdateGenerator(QObject*))));
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addLayout(tmpCheckList);
    return tmpHBoxLayout;
}

QHBoxLayout* InterfaceBase::CreateNum(QString pName, int pValue, int pMinValue, int pMaxValue)
{
    QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
    Label *tmpLabel = new Label(pName);
    SpinEdit *tmpSpinEdit = new SpinEdit(pValue, pMinValue, pMaxValue);
    tmpSpinEdit->setObjectName(pName);
    connect(tmpSpinEdit, SIGNAL(sChanged(QObject*)), this, (mUIType == ANALYSIS)?(SLOT(UpdateAnalysis(QObject*))):(SLOT(UpdateGenerator(QObject*))));
    //>@
    tmpHBoxLayout->addWidget(tmpLabel);
    tmpHBoxLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    tmpHBoxLayout->addWidget(tmpSpinEdit);
    return tmpHBoxLayout;
}

