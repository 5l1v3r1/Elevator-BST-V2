#ifndef DESIGNSETBAR_H
#define DESIGNSETBAR_H

#include "designbase.h"

class DesignSetBar : public DesignBase
{
    Q_OBJECT
public:
    DesignSetBar(QGraphicsItem *parent, QRectF pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool Start();
    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();
    void ExecAction(QAction *pAction);
    bool PaintEffect(QPainter *p);
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    //>@设置当前位置显示序号为pIndex的ITEM
    void updateEffect(int pIndex);
    void updateEffect(MOVE_DIRECTION pDirection);
public:
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
public:
    QRect               m_ValidRect;
    QList<SETITEM_INFO*>m_ItemList;
    OPERATE_INFO        m_SlipOperateInfo;
    int                 m_ItemCount;        //>@有效ITEM的个数
    int                 m_ItemSlipCount;    //>@当前需要移动多少个Item位置
    qreal               m_Diff;             //>@ITEM当执行特效的值差
    qreal               m_CurValue;         //>@ITEM当前所处状态值，如果为Rotate特效，则为当前旋转的角度
};

class DesignSetBarItem : public QWidget
{
    Q_OBJECT
public:
    DesignSetBarItem(QPixmap pPixmap, QWidget *parent = 0);
    void paintEvent(QPaintEvent *e);
    QPixmap m_Pixmap;
};

#endif // EFFECTSETBAR_H
