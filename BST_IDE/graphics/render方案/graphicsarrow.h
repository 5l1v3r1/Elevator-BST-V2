#ifndef GraphicsARROW_H
#define GraphicsARROW_H

#include "graphicsbase.h"

class GraphicsArrow : public GraphicsBase
{
    Q_OBJECT
public:
    GraphicsArrow(QGraphicsItem *parent, QRectF pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);   
    virtual bool PaintEffect(QPainter *p);
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
    void UpdateCom(int pFrame) { GraphicsBase::UpdateCom(pFrame); }
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(int pState, AREA_OPERATE pOperate);
private:
    int   m_CurRcLabel;
};

class DesignArrow : public GraphicsArrow
{
    Q_OBJECT
public:
    DesignArrow(QGraphicsItem *parent, QRectF pRect);

    bool Start()
    {
        if(GraphicsBase::Start() == false)
            return false;
        updateEffect(OPERATE_CHANGE, QVariant(m_RcList.first()));
        return true;
    }

    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();

    bool PaintEffect(QPainter *p)
    {
        GraphicsArrow::PaintEffect(p);
        //>@»æÖÆ±ß¿ò
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

#endif // GraphicsARROW_H
