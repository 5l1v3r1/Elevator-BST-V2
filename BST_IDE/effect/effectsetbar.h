#ifndef EFFECTSETBAR_H
#define EFFECTSETBAR_H

#include "global.h"
#include "tool.h"
#include "effectbase.h"
#include <QTimeEdit>
#include <QSpacerItem>
#include "logquerypage.h"
#include <QSpacerItem>

class SetBarItem;
class EffectSetBar;

class EffectSetBar : public EffectBaseItem
{
    Q_OBJECT
public:
    EffectSetBar(QGraphicsItem *parent, QRectF pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool Start();
    void PaintEffect(QPainter *p);
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    //>@设置当前位置显示序号为pIndex的ITEM
    void updateEffect(int pIndex);
    void updateEffect(bool pFlag);
    void updateEffect(MOVE_DIRECTION pDirection);

    QWidget* GetFocusSubctrl();
public:
    bool NeedReboot();
    bool GetValidRect(QRect &pRect);
    void AddItem(int pIndex, ITEM_TYPE pType, QPixmap pPixmap);
    bool InitItems();
    int GetCurItemIndex();
    SETITEM_INFO* GetCurSetItem();
    bool UpdateItems(int pIndex);
public slots:
    void Operate0();    //>@翻转为滑动模式
    void Operate1();    //>@进行滑动
    void Operate2();    //>@翻转为设置模式
    void OperateItem(int pFrameNum);

    void Initialize();
    void Release();
    void GotoPage();
    void LeavePage();
    void GotoModify();
    void LeaveModify();
    void MoveLeft();
    void MoveRight();
    void MoveFocusIn();
    void MoveFocusOut();
    void MoveFocusUp();
    void MoveFocusDown();
    void MoveFocusLoop();
    void ModifyValueUp();
    void ModifyValueDown();
public:
    QRect               m_ValidRect;
    SETITEM_INFO*       m_CurItemInfo;
    QList<SETITEM_INFO*>m_ItemList;
    OPERATE_INFO        m_SlipOperateInfo;
    int                 m_ItemCount;   //>@有效ITEM的个数
    int                 m_ItemSlipCount;  //>@当前需要移动多少个Item位置

    qreal               m_Diff;             //>@ITEM当执行特效的值差
    qreal               m_CurValue;         //>@ITEM当前所处状态值，如果为Rotate特效，则为当前旋转的角度
};

class ITEM_RC : public QObject
{
    Q_OBJECT
public:
    ITEM_RC(QObject* parent = 0)
        :QObject(parent)
    {
        SetType(ITEM_UNKNOW);
    }
    ITEM_RC(ITEM_TYPE pType)
    {
        SetType(pType);
    }
    ITEM_RC(ITEM_TYPE pType, QPixmap pBackPix)
    {
        SetType(pType);
        mBackPix = pBackPix;
    }
    ~ITEM_RC()
    {
    }
    ITEM_RC & operator=(const ITEM_RC &s)
    {
        mItemType = s.mItemType;
        mBackPix = s.mBackPix;
        mFrontPix = s.mFrontPix;
        mText = s.mText;
        return *this;
    }
    bool isValid() { return (mItemType != ITEM_UNKNOW); }

    void SetType(ITEM_TYPE pType)
    {
        mItemType = pType;
        if(isValid() == false)
            return;
        switch(mItemType)
        {
            case ITEM_MAINTENANCE:
                mFrontPix = QPixmap(":/SetBar/rc/SetBar/MAINTENANCE.png");
                mText = "MAINTENANCE";
                break;
            case ITEM_DEBUGGING:
                mFrontPix = QPixmap(":/SetBar/rc/SetBar/DEBUGGING.png");
                mText = "DEBUGGING";
                break;
            case ITEM_CONFIGURATION:
                mFrontPix = QPixmap(":/SetBar/rc/SetBar/CONFIGURATION.png");
                mText = "CONFIGURATION";
                break;
            case ITEM_THEMESETTING:
                mFrontPix = QPixmap(":/SetBar/rc/SetBar/THEMESETTING.png");
                mText = "THEMESETTING";
                break;
            case ITEM_DATAMONITOR:
                mFrontPix = QPixmap(":/SetBar/rc/SetBar/DATAMONITOR.png");
                mText = "DATAMONITOR";
                break;
            default:
                mText.clear();
                break;
        }
    }

    bool CreateUi(QVBoxLayout* pLayout, quint32 &pMinHeight)
    {
        if(isValid() == false)
            return false;
        if(pLayout == 0)
            return false;
        switch(mItemType)
        {
            case ITEM_MAINTENANCE:   //>@电梯检修，触点量 开关量
            {
                CreateUi(pLayout, QString("VSpacer"));
                QGroupBox *tmpGroupbox1 = new QGroupBox(QString("Contactor"));
                //tmpGroupbox1->setFlat(true);
                tmpGroupbox1->setFocusPolicy(Qt::NoFocus);
                QHBoxLayout *tmpHLayout1 = new QHBoxLayout(0);
                tmpGroupbox1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
                tmpHLayout1->addWidget(new QPushButton("1"));
                tmpHLayout1->addWidget(new QPushButton("2"));
                tmpHLayout1->addWidget(new QPushButton("3"));
                tmpHLayout1->addWidget(new QPushButton("4"));
                tmpGroupbox1->setLayout(tmpHLayout1);
                pLayout->insertWidget(0, tmpGroupbox1);
                pMinHeight += tmpGroupbox1->height();
                QGroupBox *tmpGroupbox2 = new QGroupBox(QString("Switch"));
                tmpGroupbox2->setFocusPolicy(Qt::NoFocus);
                tmpGroupbox1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
                //tmpGroupbox2->setFlat(true);
                QStringList tmpParaList;
                tmpParaList << "1" << "2" << "3" << "4";
                CheckList *tmpCheckList = new CheckList(tmpParaList);
                tmpGroupbox2->setLayout(tmpCheckList);
                pLayout->insertWidget(0,tmpGroupbox2);
                pMinHeight += tmpGroupbox2->height();
                break;
            }
            case ITEM_DEBUGGING:     //>@功能测试
            {
                pMinHeight = 100;
                break;
            }
            case ITEM_CONFIGURATION: //>@菜单配置（包括日期、语言、屏幕、接口等）
            {
                mdpManager *tmpMdpManager = new mdpManager(D_FSRCPATH + D_BOOTDIR + D_BOOTFILE);
                if(!tmpMdpManager->OpenMDP())
                {
                    tmpMdpManager->m_IsChanged = false;
                    delete tmpMdpManager;
                    return false;
                }
                QStringList tmpList = tmpMdpManager->m_CfgparaElementGroup.keys();
                for(int i=0;i<tmpList.count();i++)
                {
                    QGroupBox *tmpGroupbox = 0;
                    QString tmpString = tmpList.at(i);
                    if(tmpString.count('/') != 2)
                    {
                        IDE_TRACE_INT(tmpString.count('/'));
                        continue;
                    }
                    QDomElement tmpElement = tmpMdpManager->m_CfgparaElementGroup.value(tmpString);
                    QSignalMapper *sMap = new QSignalMapper(this);
                    InitUIFromDom(tmpGroupbox, tmpElement, sMap);
                    if(tmpGroupbox == 0)
                        continue;
                    tmpGroupbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
                    //tmpGroupbox->setFlat(true);
                    pLayout->addWidget(tmpGroupbox);
                    pMinHeight += tmpGroupbox->height();
                }
                CreateUi(pLayout, QString("VSpacer"));
                break;
            }
            case ITEM_THEMESETTING:  //>@主题设置（包括主题中所有元素，包括按钮）
            {
                pMinHeight = 100;
                break;
            }
            case ITEM_DATAMONITOR:   //>@数据监控
            {
                pMinHeight = 100;
//                if(m_ParaUI)
//                    m_ParaUI->deleteLater();
//                m_ParaUI = new logQueryPage("D:/Log");
//                m_ParaUI->resize(pSize);
//                setWidget(m_ParaUI);
                break;
            }
            default:
                return false;
        }
        return true;
    }

    bool InitUIFromDom(QGroupBox *&pGroupBox, QDomElement pElement, QSignalMapper *sMap)
    {
        if(pElement.isNull())
        {
            IDE_TRACE();
            return false;
        }
        if(pGroupBox)  //>@如果UI不存在则创建，否则只要根据UI初始化即可
        {
            pGroupBox->deleteLater();
            pGroupBox=0;
        }
        pGroupBox = new QGroupBox;
        pGroupBox->setTitle(pElement.tagName());
        QVBoxLayout *tmpVBoxLayout = new QVBoxLayout;
        QDomNodeList tmpList = pElement.childNodes();
        for(int i=0;i<tmpList.count();i++)
        {
            QDomElement tmpElement = tmpList.at(i).toElement();
            if(tmpElement.isNull())
                continue;
            CreateUi(tmpVBoxLayout, tmpElement, sMap);
        }
        pGroupBox->setFocusPolicy(Qt::NoFocus);
        pGroupBox->setLayout(tmpVBoxLayout);
        pGroupBox->setMinimumHeight(tmpList.count()*25);
        //pGroupBox->setMaximumHeight(tmpList.count()*25);
        return true;
    }

    QString GetUiValue(QWidget *pParentUi, QString pUiName, QString pUiType)
    {
        if(pParentUi == 0 || pUiName.isEmpty())
            return QString();
        if(pUiType.compare(QString("enum")) == 0)
        {
            ComboBox *tmpComboBox = pParentUi->findChild<ComboBox *>(pUiName);
            if(tmpComboBox == 0)
                return QString();
            return tmpComboBox->currentText();
        }
        else if(pUiType.compare(QString("check")) == 0)
        {
            CheckList *tmpChecklist = pParentUi->findChild<CheckList *>(pUiName);
            if(tmpChecklist == 0)
                return QString();
            return tmpChecklist->GetSelectText();
        }
        else if(pUiType.compare(QString("string")) == 0)
        {
            ENetAddrEdit *tmpLineEdit = pParentUi->findChild<ENetAddrEdit *>(pUiName);
            if(tmpLineEdit == 0)
                return QString();
            return tmpLineEdit->text();
        }
        else if(pUiType.compare(QString("num")) == 0)
        {
            QSpinBox *tmpSpinBox = pParentUi->findChild<QSpinBox *>(pUiName);
            if(tmpSpinBox == 0)
                return QString();
            return QString("%1").arg(tmpSpinBox->value());
        }
        else if(pUiType.compare(QString("bool")) == 0)
        {
            Checkbox *tmpCheckBox = pParentUi->findChild<Checkbox *>(pUiName);
            if(tmpCheckBox == 0)
                return QString();
            QString tmpString;
            if(tmpCheckBox->isChecked())
                tmpString = "YES";
            else
                tmpString = "NO";
            return tmpString;
        }
        return QString();
    }

    bool ModifyDomFromUI(QGroupBox *&pGroupBox, QDomElement &pElement)
    {
        if(pElement.isNull() || pGroupBox == 0)
            return false;
        QDomNodeList tmpList = pElement.childNodes();
        for(int i=0;i<tmpList.count();i++)
        {
            QDomElement tmpElement = tmpList.at(i).toElement();
            if(tmpElement.isNull())
                continue;
            QString tmpString = GetUiValue(pGroupBox, tmpElement.tagName(), tmpElement.attribute("Para"));
            if(tmpString.isNull())
                continue;
            //ModifyItemElement(tmpElement, tmpString);
        }
        return true;
    }

    bool CreateUi(QLayout *pLayout, QDomElement pElement, QSignalMapper *sMap)
    {
        if(pLayout == 0 || pElement.isNull())
        {
            IDE_TRACE();
            return false;
        }
        QString tmpTagName = pElement.tagName();
        QString tmpUitype = pElement.attribute("Para");
        if(tmpUitype.compare(QString("enum")) == 0)
        {
            //>@创建combobox
            QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
            QLabel *tmpLabel = new QLabel(tmpTagName);
            tmpLabel->setFocusPolicy(Qt::NoFocus);
            tmpLabel->setMinimumWidth(45);
            tmpLabel->setMaximumWidth(50);
            QStringList tmpTextLsit = pElement.attribute("List").split(';');
            QString tmpItem = pElement.text();
            ComboBox *tmpComboBox = new ComboBox(tmpTextLsit, tmpItem);
            tmpComboBox->setObjectName(tmpTagName);

            connect(tmpComboBox, SIGNAL(currentIndexChanged(QString)), sMap, SLOT(map()));
            sMap->setMapping(tmpComboBox, tmpComboBox);
            connect(sMap, SIGNAL(mapped(QWidget*)), this, SLOT(slot_SyncControl(QWidget*)));

            tmpHBoxLayout->addWidget(tmpLabel);
            tmpHBoxLayout->addWidget(tmpComboBox);
            pLayout->addItem(tmpHBoxLayout);
        }
        else if(tmpUitype.compare(QString("check")) == 0)
        {
            //>@创建checklist
            QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
            QLabel *tmpLabel = new QLabel(tmpTagName);
            tmpLabel->setFocusPolicy(Qt::NoFocus);
            tmpLabel->setMinimumWidth(45);
            tmpLabel->setMaximumWidth(50);
            QStringList tmpTextLsit = pElement.attribute("List").split(';');
            QStringList tmpSelectList = pElement.text().split(';');
            CheckList *tmpChecklist = new CheckList(tmpTextLsit, tmpSelectList);
            tmpChecklist->setObjectName(tmpTagName);

            connect(tmpChecklist, SIGNAL(stateChanged(int)), sMap, SLOT(map()));
            sMap->setMapping(tmpChecklist, tmpChecklist);
            connect(sMap, SIGNAL(mapped(QWidget*)), this, SLOT(slot_SyncControl(QWidget*)));

            tmpHBoxLayout->addWidget(tmpLabel);
            tmpHBoxLayout->addLayout(tmpChecklist);
            pLayout->addItem(tmpHBoxLayout);
        }
        else if(tmpUitype.compare(QString("string")) == 0)
        {
            //>@创建LineEdit
            QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
            QLabel *tmpLabel = new QLabel(tmpTagName);
            tmpLabel->setFocusPolicy(Qt::NoFocus);
            tmpLabel->setMinimumWidth(45);
            tmpLabel->setMaximumWidth(50);
            ENET_ADDR tmpAddr(pElement.text());
            if(tmpAddr.isValid())
            {
                SpinList *tmpSpinList = new SpinList(tmpAddr.toQList());
                tmpSpinList->setObjectName(tmpTagName);

                connect(tmpSpinList, SIGNAL(valueChanged(int)), sMap, SLOT(map()));
                sMap->setMapping(tmpSpinList, tmpSpinList);
                connect(sMap, SIGNAL(mapped(QWidget*)), this, SLOT(slot_SyncControl(QWidget*)));

                tmpHBoxLayout->addWidget(tmpLabel);
                tmpHBoxLayout->addLayout(tmpSpinList);
            }
            else
            {
                TextEdit *tmpTextEdit = new TextEdit(pElement.text());
                tmpTextEdit->setObjectName(tmpTagName);

                connect(tmpTextEdit, SIGNAL(textChanged(QString)), sMap, SLOT(map()));
                sMap->setMapping(tmpTextEdit, tmpTextEdit);
                connect(sMap, SIGNAL(mapped(QWidget*)), this, SLOT(slot_SyncControl(QWidget*)));

                tmpHBoxLayout->addWidget(tmpLabel);
                tmpHBoxLayout->addWidget(tmpTextEdit);
            }
            pLayout->addItem(tmpHBoxLayout);
        }
        else if(tmpUitype.compare(QString("num")) == 0)
        {
            //>@创建SpinBox
            QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
            QLabel *tmpLabel = new QLabel(tmpTagName);
            tmpLabel->setFocusPolicy(Qt::NoFocus);
            tmpLabel->setMinimumWidth(45);
            tmpLabel->setMaximumWidth(50);
            SpinEdit *tmpSpinBox = new SpinEdit;
            tmpSpinBox->setObjectName(tmpTagName);

            connect(tmpSpinBox, SIGNAL(valueChanged(int)), sMap, SLOT(map()));
            sMap->setMapping(tmpSpinBox, tmpSpinBox);
            connect(sMap, SIGNAL(mapped(QWidget*)), this, SLOT(slot_SyncControl(QWidget*)));

            tmpSpinBox->setValue(pElement.text().toInt());
            tmpHBoxLayout->addWidget(tmpLabel);
            tmpHBoxLayout->addWidget(tmpSpinBox);
            pLayout->addItem(tmpHBoxLayout);
        }
        else if(tmpUitype.compare(QString("bool")) == 0)
        {
            //>@创建checkbox
            QHBoxLayout *tmpHBoxLayout = new QHBoxLayout;
            Checkbox *tmpCheckBox = new Checkbox;
            tmpCheckBox->setObjectName(tmpTagName);

            connect(tmpCheckBox, SIGNAL(stateChanged(QString)), sMap, SLOT(map()));
            sMap->setMapping(tmpCheckBox, tmpCheckBox);
            connect(sMap, SIGNAL(mapped(QWidget*)), this, SLOT(slot_SyncControl(QWidget*)));

            QString pBool = pElement.text();
            if(pBool.compare("YES", Qt::CaseInsensitive) == 0)
            {
                tmpCheckBox->setChecked(true);
            }
            if(pBool.compare("NO", Qt::CaseInsensitive) == 0)
            {
                tmpCheckBox->setChecked(false);
            }
            tmpHBoxLayout->addWidget(tmpCheckBox);
            pLayout->addItem(tmpHBoxLayout);
        }
        return true;
    }

    bool CreateUi(QLayout *pLayout, QString pUiName)
    {
        if(pLayout == 0)
            return false;
        if(pUiName.compare(QString("HSpacer")) == 0)
        {
            QSpacerItem *tmpSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
            pLayout->addItem(tmpSpacer);
        }
        else if(pUiName.compare(QString("VSpacer")) == 0)
        {
            QSpacerItem *tmpSpacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
            pLayout->addItem(tmpSpacer);
        }
        else
            return false;
        return true;
    }

    QPixmap GetBackgroundPixmap(QRect pRect)
    {
        //>@设置窗体背景图片
        QImage tmpImage(pRect.size(), QImage::Format_ARGB32_Premultiplied);
        if(tmpImage.isNull())
            return false;
        tmpImage.fill(Qt::transparent); //>@创建透明图层
        QPainter p(&tmpImage);
        p.setRenderHint(QPainter::Antialiasing);
        //>@绘制第一页面
        if(isValid() == false)
            return QPixmap();
        if(mBackPix.isNull() == false)
            p.drawPixmap(QRect(QPoint(0,0),pRect.size()), mBackPix);
        //>@计算图标与文本的绘制位置
        qreal tmpX, tmpY, tmpMinLen;
        if(pRect.width() >= pRect.height())      //>@当前设备是横屏
        {
            tmpMinLen = pRect.height();
            tmpX = (pRect.width()-tmpMinLen)/2.0;
            tmpY = 0;
        }
        else                                        //>@当前设备是竖屏
        {
            tmpMinLen = pRect.width();
            tmpX = 0;
            tmpY = (pRect.height()-tmpMinLen)/2.0;
        }
        if(mFrontPix.isNull() == false)
        {
            p.drawPixmap(QRect(tmpX + tmpMinLen/4.0,
                               tmpY + tmpMinLen/4.0,
                               tmpMinLen/2.0,
                               tmpMinLen/2.0),
                         mFrontPix);
        }
        if(mText.isEmpty() == false)
        {
            WriteText(&p, mText,
                      QRect(tmpX + tmpMinLen/4.0,
                            tmpY + tmpMinLen*3.0/4.0,
                            tmpMinLen/2.0,
                            tmpMinLen/4.0),
                      Qt::blue,
                      PRIOR_WIDTH);
        }
        p.end();
        return QPixmap::fromImage(tmpImage);
    }

    void SetLuminance(int pVaule)
    {
        QString tmpCmdString = QString("echo %1 > /sys/devices/platform/pwm-backlight/backlight/pwm-backligh/brightness").arg(pVaule);
        system(tmpCmdString.toAscii().data());
    }
    //>@音量范围,0~100
    void SetVolume(int pVaule)
    {
        QString tmpCmdString = QString("amixer set PCM %1 &").arg(pVaule+50);
        IDE_TRACE_STR(tmpCmdString);
        system(tmpCmdString.toAscii().data());
    }
public:
    ITEM_TYPE                  mItemType;
    QPixmap                    mBackPix;
    QPixmap                    mFrontPix;
    QString                    mText;
public slots:
    void slot_SyncControl(QWidget* pWidget)
    {
        QString OBName = pWidget->objectName();
        if(OBName.compare("Luminance", Qt::CaseInsensitive) == 0)
        {
            SpinEdit* LuminanceEdit = qobject_cast<SpinEdit*>(pWidget);
            if(LuminanceEdit != 0)
            {
                SetLuminance(LuminanceEdit->value());
            }
        }
        else if(OBName.compare("Sound", Qt::CaseInsensitive) == 0)
        {
            SpinEdit* SoundEdit = qobject_cast<SpinEdit*>(pWidget);
            if(SoundEdit != 0)
            {
                SetVolume(SoundEdit->value());
            }
        }
    }
};

class SetBarItem : public QScrollArea
{
    Q_OBJECT
public:
    enum ITEM_STAGE{ BEGIN=0, END=1 };
    explicit SetBarItem(ITEM_TYPE pType, QPixmap pPixmap, QRect rect, QWidget *parent=0);
    ~SetBarItem();
    bool InitUI(QSize pSize);
    void ShowUI();
    void HideUI();
protected:
    void resizeEvent(QResizeEvent *e);
public:
    ITEM_RC*                m_ItemRc;
    QPixmap                 m_BgPixmap;
    QWidget*                m_ParaUI;
    quint32                 m_ParaUIHeight;
    ITEM_STAGE              m_ItemStage;
};


#endif // EFFECTSETBAR_H

