#ifndef EFFECTTEXT_H
#define EFFECTTEXT_H

#include "effectbase.h"

class EffectText : public EffectBase
{
    Q_OBJECT
public:
    EffectText(QWidget *parent, QRect pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(int pState, AREA_OPERATE pOperate);
    void Paint2D_TextRoll(QPainter &p, QRect targetRect);
private:
    int            m_TextWidth, m_TextHeight;
    int            m_TextState;
};

inline void EffectText::Paint2D_TextRoll(QPainter &p, QRect targetRect)
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
    p.save();
    QTransform transform1;
    if(m_AreaAnimate->mDirection == Vertical)
    {
        tmpDiff = pixhh / FrameCount;
        y = (qreal)(tmpDiff*index);
        x = 0;
        transform1.translate(targetRect.left()+x, targetRect.top()-y);
        p.setTransform(transform1);
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(x,y,pixhw-x,pixhh-y), QRect(x,y,pixhw-x,pixhh-y));
        int tmp, tmp1, i = 1;
        while((tmp = i*pixhh-y) < hh)
        {
            p.resetTransform();
            x = 0;
            QTransform transform2;
            transform2.translate(targetRect.left()+x, targetRect.top()+tmp);
            tmp1 = (hh-tmp > pixhh)?(pixhh):(hh-tmp);
            p.setTransform(transform2);
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
        transform1.translate(targetRect.left()-x, targetRect.top()+y);
        p.setTransform(transform1);
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(x,y,pixhw-x,pixhh-y), QRect(x,y,pixhw-x,pixhh-y));
        int tmp, tmp1, i = 1;
        while((tmp = i*pixhw-x) < hw)
        {
            p.resetTransform();
            y = 0;
            QTransform transform2;
            transform2.translate(targetRect.left()+tmp, targetRect.top()+y);
            tmp1 = (hw-tmp > pixhw)?(pixhw):(hw-tmp);
            p.setTransform(transform2);
            DrawPixmap(p, m_PixmapPointer.mCurPointer,
                       QRect(0,0,tmp1,pixhh-y),
                       QRect(0,0,tmp1,pixhh-y));
            i++;
        }
    }
    p.restore();
}

#endif // EFFECTTEXT_H
