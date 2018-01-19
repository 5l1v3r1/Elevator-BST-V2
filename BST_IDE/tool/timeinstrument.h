/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include "global.h"

class TimeInstrument : public QWidget
{
    Q_OBJECT

public:
    TimeInstrument(QWidget *parent = 0);
    ~TimeInstrument()
    {
        if(m_Timer)
        {
            m_Timer->stop();
            m_Timer = 0;
        }
    }

    void Initial(CLOCK_TYPE pType);

    void updateEffect(CLOCK_INFO pTime);

    bool LoadPixmap(int pKey, QString pPixmapName);
    bool LoadPixmap(int pKey, QString pPixmapName, QSize pSize);
    //>@任何动作最多只需要两步，如果有两步，则第一步肯定是状态CHANGE，第二步一定是状态的操作变化
    void OperateStep0();
public slots:
    void OperateStep1();
    void UpdateTime();

protected:
    void StartTimeline(int pStart, int pEnd, int pInterval, QTimeLine::CurveShape pShape);
    bool DrawPixmap(QPainter &p, QPixmap pPixmap, QRect pTargetRect);
    bool DrawPixmap(QPainter &p, QPixmap pPixmap, QRect pTargetRect, QRect pSourceRect);
    void CalcElementInfo(CLOCK_TYPE pType);
    void CalcAnalogClockPointer();
    void paintEvent(QPaintEvent *event);
    void Paint2D_None(QPainter &p, QRect targetRect);
    void Paint2D_Flip(QPainter &p,QRect targetRect);
    void Paint2D_Roll(QPainter &p,QRect targetRect);

public:
    QTimer                             *m_Timer;
    QHash<int, QPixmap>                 m_EffectImage;  //>@加载到内存的资源
    QTimeLine                           m_Animator;
    QRect                               m_Rect;         //>@区域大小
    bool                                m_EffectValid;

    OPERATE_INFO                        m_OperateInfo[2];
    PIXMAP_POINTER                      m_PixmapPointer;

    CLOCK_PATTERN                       m_Parttern;
    CLOCK_TYPE                          m_Type;
    AREA_ANIMATE                        m_AreaAnimate;

    //>@Digital : 图元(指显示数字的一个个区域)个数为19个：RectYear0~RectSecond1 ---- 0~9 SUN~WES : -
    //>@          图片资源个数为10+7+1+1 = 19
    //>@          0~9 Sun~Sat : -
    //>@Analog : 图元个数为11+3个：RectYear0~RectWeek  时指针 分指针 秒指针
    //>@         图片资源个数为10+7+1+4+1 = 23(1个表针最上面的点)个
    //>@         0~9 Sun~Sat - 时指针 分指针 秒指针 时钟底图
    QHash<CLOCKE_ELEMENT, CLOCK_ELEMENTINFO*>    m_ClockElement;
    qreal                    m_Diff;                 //>@图片一次移动(旋转)距离
    CLOCK_INFO               m_SavedTime;
    qreal                    m_ItemWidth, m_ItemHeight;
    int                      m_FontSize;

    //>@针对Analog时钟
    qreal       m_PointerLength[3];
    qreal       m_PointerAngle[3][2];   //>@对于指针，区域指长度和角度(待旋转的角度，也即角度差)
};

inline void TimeInstrument::StartTimeline(int pStart, int pEnd, int pInterval, QTimeLine::CurveShape pShape)
{
    m_Animator.stop();
    m_Animator.setFrameRange(pStart, pEnd);
    m_Animator.setDuration(pInterval);
    m_Animator.setCurveShape(pShape);
    m_Animator.start();
}

inline bool TimeInstrument::LoadPixmap(int pKey, QString pPixmapName)
{
    if(pPixmapName.isEmpty())
        return false;
    FILEPATH_TYPE tmpType = getRcFileType(pPixmapName);
    if(tmpType == PATH_TYPENONE || tmpType == PATH_TYPENETADDR)
        return false;
    QImage tmpImage;
    if(tmpImage.load(pPixmapName) == false)
        return false;
    m_EffectImage.insert(pKey, QPixmap::fromImage(tmpImage));
    return true;
}

inline bool TimeInstrument::LoadPixmap(int pKey, QString pPixmapName, QSize pSize)
{
    if(pPixmapName.isEmpty())
        return false;
    FILEPATH_TYPE tmpType = getRcFileType(pPixmapName);
    if(tmpType == PATH_TYPENONE || tmpType == PATH_TYPENETADDR)
        return false;
    if(!pSize.isValid())
        return false;
    QImage tmpImage;
    if(tmpImage.load(pPixmapName) == false)
        return false;
    m_EffectImage.insert(pKey, QPixmap::fromImage(tmpImage.scaled(pSize, Qt::IgnoreAspectRatio)));
    return true;
}

inline bool TimeInstrument::DrawPixmap(QPainter &p, QPixmap pPixmap, QRect pTargetRect)
{
    if(pTargetRect.isValid() == false)
    {
        //IDE_TRACE();
        return false;
    }
    if(pPixmap.isNull() == false)
    {
        p.drawPixmap(pTargetRect, pPixmap);
        return true;
    }
    //IDE_TRACE();
    return false;
}

inline bool TimeInstrument::DrawPixmap(QPainter &p, QPixmap pPixmap, QRect pTargetRect, QRect pSourceRect)
{
    if(pTargetRect.isValid() == false || pSourceRect.isValid() == false)
    {
        //IDE_TRACE();
        return false;
    }
    if(pPixmap.isNull() == false)
    {
        p.drawPixmap(pTargetRect.left(), pTargetRect.top(), pTargetRect.width(), pTargetRect.height(),
                     pPixmap,
                     pSourceRect.left(), pSourceRect.top(), pSourceRect.width(), pSourceRect.height());
        return true;
    }
    return false;
}


inline void TimeInstrument::Paint2D_None(QPainter &p, QRect targetRect)
{
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect);
}

//>@需要绘制底图和特效图片
inline void TimeInstrument::Paint2D_Flip(QPainter &p, QRect targetRect)
{
    qreal w = targetRect.width();
    qreal h = targetRect.height();
    qreal hw = w / 2.0;
    qreal hh = h / 2.0;
    // behind is the new pixmap
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect);
    if(m_Animator.state() != QTimeLine::Running)
        return;
    qreal FrameCount = m_AreaAnimate.mFrameEnd - m_AreaAnimate.mFrameStart;
    if(FrameCount <= 0)
        return;
    qreal halfcount =  FrameCount / 2.0;
    QTransform transform;
    qreal index = m_Animator.currentFrame();

    p.save();
    if (index <= halfcount)
    {
        // the top part of the old pixmap is flipping
        qreal angle;
        if(m_AreaAnimate.mOrientation == Positive)
        {
            angle = -180.0 * index / FrameCount;
        }
        else
        {
            angle = 180.0 * index / FrameCount;
        }
        transform.translate(targetRect.left()+hw, targetRect.top()+hh);
        if(m_AreaAnimate.mDirection == Horizontal)
            transform.rotate(angle, Qt::YAxis);
        else
            transform.rotate(angle, Qt::XAxis);
        p.setTransform(transform);
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(-hw, -hh, w, h), QRect(0,0,w,h));

        // the bottom part is still the old pixmap
        p.resetTransform();
        if(m_AreaAnimate.mDirection == Horizontal)
            if(m_AreaAnimate.mOrientation == Positive)
                p.setClipRect(hw, 0, hw, targetRect.height());
            else
                p.setClipRect(0, 0, hw, targetRect.height());
        else
            if(m_AreaAnimate.mOrientation == Positive)
                p.setClipRect(0, hh, targetRect.width(), hh);
            else
                p.setClipRect(0, 0, targetRect.width(), hh);
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(0,0,w,h), QRect(0,0,w,h));
    }
    else
    {
        transform.translate(targetRect.left(), targetRect.top());
        p.setTransform(transform);
        if(m_AreaAnimate.mDirection == Horizontal)
            if(m_AreaAnimate.mOrientation == Positive)
                p.setClipRect(hw, 0, hw, targetRect.height());
            else
                p.setClipRect(0, 0, hw, targetRect.height());
        else
            if(m_AreaAnimate.mOrientation == Positive)
                p.setClipRect(0, hh, targetRect.width(), hh);
            else
                p.setClipRect(0, 0, targetRect.width(), hh);

        // the bottom part is still the old pixmap
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(0,0,w,h), QRect(0,0,w,h));
        p.resetTransform();
        // the bottom part of the new pixmap is flipping
        qreal angle;
        if(m_AreaAnimate.mOrientation == Positive)
        {
            angle = 180.0 - 180.0 * index / FrameCount;
        }
        else
        {
            angle = - 180.0 * (FrameCount-index) / FrameCount;
        }
        transform.translate(targetRect.left()+hw, targetRect.top()+hh);
        if(m_AreaAnimate.mDirection == Horizontal)
            transform.rotate(angle, Qt::YAxis);
        else
            transform.rotate(angle, Qt::XAxis);
        p.setTransform(transform);
        DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(-hw, -hh, w, h), QRect(0,0,w,h));
    }
    p.restore();
}

inline void TimeInstrument::Paint2D_Roll(QPainter &p, QRect targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    qreal tmpDiff;
    if(m_Animator.state() != QTimeLine::Running)
        return;
    qreal FrameCount = m_AreaAnimate.mFrameEnd - m_AreaAnimate.mFrameStart - 1;
    if(FrameCount <= 0)
    {
        //IDE_TRACE();
        return;
    }
    qreal index = m_Animator.currentFrame();
    qreal x, y;
    //>@首先将坐标原点移到当前需要绘制的区域。
    p.save();
    QTransform transform1, transform2;
    if(m_AreaAnimate.mDirection == Vertical)
    {
        tmpDiff = hh / FrameCount;
        if(m_AreaAnimate.mOrientation == Positive)
        {
            y = (qreal)(tmpDiff*index);
            transform1.translate(targetRect.left(), targetRect.top()+y);
            p.setTransform(transform1);
            DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(0,0,hw,hh-y), QRect(0,0,hw,hh-y));
        }
        else
        {
            y = (qreal)(tmpDiff*index);
            transform1.translate(targetRect.left(), targetRect.top()-y);
            p.setTransform(transform1);
            DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(0,y,hw,hh-y), QRect(0,y,hw,hh-y));
        }
        p.resetTransform();
        if(m_AreaAnimate.mOrientation == Positive)
        {
            y = hh - y;
            transform2.translate(targetRect.left(), targetRect.top()-y);
            p.setTransform(transform2);
            DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(0,y,hw,hh-y), QRect(0,y,hw,hh-y));
        }
        else
        {
            y = hh - y;
            transform2.translate(targetRect.left(), targetRect.top()+y);
            p.setTransform(transform2);
            DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(0,0,hw,hh-y), QRect(0,0,hw,hh-y));
        }
    }
    else
    {
        tmpDiff = (qreal)(hw) / FrameCount;
        if(m_AreaAnimate.mOrientation == Positive)
        {
            x = (qreal)(tmpDiff*index);
            transform1.translate(targetRect.left()+x, targetRect.top());
            p.setTransform(transform1);
            DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(0,0,hw-x,hh), QRect(0,0,hw-x,hh));
        }
        else
        {
            x = (qreal)(tmpDiff*index);
            transform1.translate(targetRect.left()-x, targetRect.top());
            p.setTransform(transform1);
            DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(x,0,hw-x,hh), QRect(x,0,hw-x,hh));
        }
        p.resetTransform();
        if(m_AreaAnimate.mOrientation == Positive)
        {
            x = hw - x;
            transform2.translate(targetRect.left()-x, targetRect.top());
            p.setTransform(transform2);
            DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(x,0,hw-x,hh), QRect(x,0,hw-x,hh));
        }
        else
        {
            x = hw - x;
            transform2.translate(targetRect.left()+x, targetRect.top());
            p.setTransform(transform2);
            DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(0,0,hw-x,hh), QRect(0,0,hw-x,hh));
        }
    }
    p.restore();
}

#endif
