#ifndef GraphicsTEXT_H
#define GraphicsTEXT_H

#include "graphicsoperate.h"

class GraphicsText : public GraphicsOperate
{
    Q_OBJECT
public:
    GraphicsText(QGraphicsItem *parent);
    bool Start();
    void updateEffect(int pState, AREA_OPERATE pOperate);
    void Paint2D_TextRoll(QPainter *p, QRectF targetRect);
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
    void UpdateCom(int pFrame) { GraphicsComponent::UpdateCom(pFrame); }
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    int            m_TextWidth, m_TextHeight;
    int            m_TextState;
};

inline void GraphicsText::Paint2D_TextRoll(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    qreal pixhh = m_PixmapPointer.mCurPointer.height();
    qreal pixhw = m_PixmapPointer.mCurPointer.width();
    qreal tmpDiff;
    if(m_Animator.state() != QTimeLine::Running)
        return;
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
    {
        IDE_TRACE();
        return;
    }
    qreal index = m_Animator.currentFrame();
    qreal x, y;
    //>@首先将坐标原点移到当前需要绘制的区域。
    p->save();
    if(m_AreaAnimate->mDirection == Vertical)
    {
        tmpDiff = pixhh / FrameCount;
        y = (qreal)(tmpDiff*index);
        x = 0;
        p->translate(targetRect.left()+x, targetRect.top()-y);
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(x,y,pixhw-x,pixhh-y), QRect(x,y,pixhw-x,pixhh-y));
        int tmp, tmp1, i = 1;
        while((tmp = i*pixhh-y) < hh)
        {
            p->restore();
            p->save();
            x = 0;
            p->translate(targetRect.left()+x, targetRect.top()+tmp);
            tmp1 = (hh-tmp > pixhh)?(pixhh):(hh-tmp);
            DrawPixmap(p, m_PixmapPointer.mCurPointer,
                       QRect(0,0,pixhw-x,tmp1),
                       QRect(0,0,pixhw-x,tmp1));
            i++;
        }
    }
    else
    {
        tmpDiff = (qreal)(pixhw) / FrameCount;
        x = (qreal)(tmpDiff*index);
        y = 0;
        p->translate(targetRect.left()-x, targetRect.top()+y);
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(x,y,pixhw-x,pixhh-y), QRect(x,y,pixhw-x,pixhh-y));
        int tmp, tmp1, i = 1;
        while((tmp = i*pixhw-x) < hw)
        {
            p->restore();
            p->save();
            y = 0;
            p->translate(targetRect.left()+tmp, targetRect.top()+y);
            tmp1 = (hw-tmp > pixhw)?(pixhw):(hw-tmp);
            DrawPixmap(p, m_PixmapPointer.mCurPointer,
                       QRect(0,0,tmp1,pixhh-y),
                       QRect(0,0,tmp1,pixhh-y));
            i++;
        }
    }
    p->restore();
}

class DesignText : public GraphicsText
{
    Q_OBJECT
public:
    DesignText(QGraphicsItem *parent);
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
        GraphicsText::PaintEffect(p);
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

#endif // EFFECTTEXT_H
