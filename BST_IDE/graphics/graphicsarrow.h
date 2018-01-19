#ifndef GraphicsARROW_H
#define GraphicsARROW_H

#include "graphicsoperate.h"

#ifdef IDE
    #define ComArrow  DesignArrow
#elif DISPLAY
    #define ComArrow  GraphicsArrow
#endif

class GraphicsArrow : public GraphicsOperate
{
    Q_OBJECT
public:
    GraphicsArrow(QGraphicsItem *parent);
    bool Start();
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
    void UpdateCom(int pFrame) { GraphicsComponent::UpdateCom(pFrame); }
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
    DesignArrow(QGraphicsItem *parent);

    bool Start()
    {
        if(GraphicsComponent::Start() == false)
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
