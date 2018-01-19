#ifndef GraphicsSETBAR_H
#define GraphicsSETBAR_H

#include "graphicsbase.h"
#include "tool.h"

class SetBarItem : public QScrollArea
{
    Q_OBJECT
public:
    enum ITEM_STAGE{ BEGIN=0, END=1 };
    explicit SetBarItem(QPixmap pPixmap, QRect rect, QWidget *parent=0);
    ~SetBarItem();

    void ShowUI();
    void HideUI();

protected:
    void resizeEvent(QResizeEvent *e);

public:
    QPixmap                 m_BgPixmap;
    QWidget*                m_ParaUI;
    ITEM_STAGE              m_ItemStage;
};

class SETITEM_INFO
{
public:
    SETITEM_INFO(ITEM_TYPE pType)
    {
        mItemProxy = 0;
        mItemType = pType;
    }
    ~SETITEM_INFO()
    {
        //>@注意，此处不能添加释放的代码，否则会在某个SETITEM_INFO注销时，同时注销其他与此相同的窗口
    }
    SetBarItem* GetWidget()
    {
        return mItemWidget;
    }
public:
    ITEM_TYPE             mItemType;
    QGraphicsProxyWidget *mItemProxy;
    SetBarItem           *mItemWidget;
    int                   mCurParaIndex;     //>@所代理的ITEM标号
    int                   mNextParaIndex;    //>@所代理的ITEM标号
};

class DeviceManager;

class GraphicsSetBar : public GraphicsBase
{
    Q_OBJECT
public:
    GraphicsSetBar(QGraphicsItem *parent, QRectF pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool Start();
    virtual bool PaintEffect(QPainter *p);
#ifdef DISPLAY
    void SyncDevManager(DeviceManager *pDevManager) { m_DevManager = pDevManager; }
#endif
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    bool updateEffect(int pIndex);    //>@设置当前位置显示序号为pIndex的ITEM
    bool updateEffect(bool pFlag);
    bool updateEffect(MOVE_DIRECTION pDirection);

    QWidget* GetFocusSubctrl();
    bool NeedReboot();
    bool GetValidRect(QRect &pRect);

    void AddItem(int pIndex, ITEM_TYPE pType, QPixmap pPixmap);
    void ReleaseItems();
    bool InitItems();
    bool UpdateItems(int pIndex);

    int GetCurItemIndex();
    SETITEM_INFO* GetCurSetItem();

    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }

public slots:
    virtual void ExecAction(QAction *pAction) {}
    void UpdateCom(int pFrame) { GraphicsBase::UpdateCom(pFrame); }
    void Operate0();    //>@翻转为滑动模式
    void Operate1();    //>@进行滑动
    void Operate2();    //>@翻转为设置模式
    void OperateItem(int pFrameNum);

    void Initialize();
    void Release();
    bool GotoPage();
    bool LeavePage();
    bool GotoModify();
    bool LeaveModify();
    bool MoveLeft();
    bool MoveRight();
    bool MoveFocusIn();
    bool MoveFocusOut();
    bool MoveFocusUp();
    bool MoveFocusDown();
    bool MoveFocusLoop();
    bool ModifyValueUp();
    bool ModifyValueDown();

public:
#ifdef DISPLAY
    DeviceManager          *m_DevManager;
#endif
    QRect                   m_ValidRect;
    SETITEM_INFO*           m_CurItemInfo;
    QList<SETITEM_INFO*>    m_ItemList;
    OPERATE_INFO            m_SlipOperateInfo;
    int                     m_ItemCount;   //>@有效ITEM的个数
    int                     m_ItemSlipCount;  //>@当前需要移动多少个Item位置

    SB_PATTERN              m_Pattern;  //>@单个页面的样式

    qreal                   m_Diff;             //>@ITEM当执行特效的值差
    qreal                   m_CurValue;         //>@ITEM当前所处状态值，如果为Rotate特效，则为当前旋转的角度
};

class DesignSetBar : public GraphicsSetBar
{
    Q_OBJECT
public:
    DesignSetBar(QGraphicsItem *parent, QRectF pRect);

    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();

    bool PaintEffect(QPainter *p)
    {
        GraphicsSetBar::PaintEffect(p);
        //>@绘制边框
        QRectF tmpRect = geometry();
        qreal Width = tmpRect.width();
        qreal Height = tmpRect.height();
        p->translate(Width / 2, Height / 2);
        p->setPen(Qt::DashDotDotLine);
        p->setPen(QColor(10,10,255));
        p->drawRect(QRect(-Width/2, -Height/2, Width, Height));
        p->drawRect(QRect(-Width/2+1, -Height/2+1, Width-2, Height-2));
        return true;
    }

public slots:
    void ExecAction(QAction *pAction);
};

#endif // EFFECTSETBAR_H
