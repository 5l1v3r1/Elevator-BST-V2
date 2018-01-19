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

#include "timeinstrument.h"

TimeInstrument::TimeInstrument(QWidget *parent)
    : QWidget(parent)
{
    m_EffectValid = false;
    m_Timer = 0;
    connect(&m_Animator, SIGNAL(finished()), SLOT(OperateStep1()));
    connect(&m_Animator, SIGNAL(frameChanged(int)), SLOT(update()));
}

void TimeInstrument::Initial(CLOCK_TYPE pType)
{
    if(pType == DIGITAL)
    {
        m_Type = DIGITAL;
        m_AreaAnimate.mEffectType = EffectTypeFlip;
        m_AreaAnimate.mDirection = Vertical;    //>@图片特效方向
        m_AreaAnimate.mOrientation = Positive;  //>@特效正逆序
        m_AreaAnimate.mFrameStart = 0;
        m_AreaAnimate.mFrameEnd = 20;
        m_AreaAnimate.mAnimateTime = 600;
        m_AreaAnimate.mAnimateType = QTimeLine::EaseInOutCurve;

        CalcElementInfo(m_Type);

        LoadPixmap(BGRC, ":/Clock/rc/Clock/clock.png");
        LoadPixmap(Text0, ":/Clock/rc/Clock/num-0.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text1, ":/Clock/rc/Clock/num-1.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text2, ":/Clock/rc/Clock/num-2.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text3, ":/Clock/rc/Clock/num-3.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text4, ":/Clock/rc/Clock/num-4.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text5, ":/Clock/rc/Clock/num-5.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text6, ":/Clock/rc/Clock/num-6.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text7, ":/Clock/rc/Clock/num-7.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text8, ":/Clock/rc/Clock/num-8.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text9, ":/Clock/rc/Clock/num-9.png", QSize(m_ItemWidth, m_ItemHeight));

        LoadPixmap(ClockColon, ":/Clock/rc/Clock/num-maohao.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockMinus, ":/Clock/rc/Clock/num--.png", QSize(m_ItemWidth, m_ItemHeight));

        LoadPixmap(ClockMon, ":/Clock/rc/Clock/1.png", QSize(4*m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockTue, ":/Clock/rc/Clock/2.png", QSize(4*m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockWed, ":/Clock/rc/Clock/3.png", QSize(4*m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockThu, ":/Clock/rc/Clock/4.png", QSize(4*m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockFri, ":/Clock/rc/Clock/5.png", QSize(4*m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockSat, ":/Clock/rc/Clock/6.png", QSize(4*m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockSun, ":/Clock/rc/Clock/7.png", QSize(4*m_ItemWidth, m_ItemHeight));
    }
    else if(pType == ANALOG)
    {
        m_Type = ANALOG;
        m_AreaAnimate.mEffectType = EffectTypeRoll;
        m_AreaAnimate.mDirection = Vertical;    //>@图片特效方向
        m_AreaAnimate.mOrientation = Positive;  //>@特效正逆序
        m_AreaAnimate.mFrameStart = 0;
        m_AreaAnimate.mFrameEnd = 20;
        m_AreaAnimate.mAnimateTime = 600;
        m_AreaAnimate.mAnimateType = QTimeLine::EaseInOutCurve;

        CalcElementInfo(m_Type);
        LoadPixmap(BGRC, ":/Clock/rc/Clock/clock.png");

        LoadPixmap(Text0, ":/Clock/rc/Clock/num-0.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text1, ":/Clock/rc/Clock/num-1.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text2, ":/Clock/rc/Clock/num-2.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text3, ":/Clock/rc/Clock/num-3.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text4, ":/Clock/rc/Clock/num-4.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text5, ":/Clock/rc/Clock/num-5.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text6, ":/Clock/rc/Clock/num-6.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text7, ":/Clock/rc/Clock/num-7.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text8, ":/Clock/rc/Clock/num-8.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(Text9, ":/Clock/rc/Clock/num-9.png", QSize(m_ItemWidth, m_ItemHeight));

        LoadPixmap(ClockColon, ":/Clock/rc/Clock/num-maohao.png", QSize(m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockMinus, ":/Clock/rc/Clock/num--.png", QSize(m_ItemWidth, m_ItemHeight));

        LoadPixmap(ClockMon, ":/Clock/rc/Clock/1.png", QSize(4*m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockTue, ":/Clock/rc/Clock/2.png", QSize(4*m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockWed, ":/Clock/rc/Clock/3.png", QSize(4*m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockThu, ":/Clock/rc/Clock/4.png", QSize(4*m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockFri, ":/Clock/rc/Clock/5.png", QSize(4*m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockSat, ":/Clock/rc/Clock/6.png", QSize(4*m_ItemWidth, m_ItemHeight));
        LoadPixmap(ClockSun, ":/Clock/rc/Clock/7.png", QSize(4*m_ItemWidth, m_ItemHeight));

        LoadPixmap(ClockHourPointer, ":/Clock/rc/Clock/hour.png");
        LoadPixmap(ClockMinutePointer, ":/Clock/rc/Clock/minute.png");
        LoadPixmap(ClockSecondPointer, ":/Clock/rc/Clock/second.png");
        LoadPixmap(ClockDot, ":/Clock/rc/Clock/dot.png");

        CalcAnalogClockPointer();
    }
    m_EffectValid = true;

    m_SavedTime.fromChar(3,5,2,1,1,1);
    CLOCK_INFO tmpTime(0,1,1,2,0,0);
    updateEffect(tmpTime);

    if(m_Timer == 0)
    {
        m_Timer = new QTimer(this);
        connect(m_Timer, SIGNAL(timeout()), this, SLOT(UpdateTime()));
    }
    m_Timer->start(1000);
}

void TimeInstrument::UpdateTime()
{
    QDateTime tmpDateTime = QDateTime::currentDateTime();
    CLOCK_INFO tmpClock(tmpDateTime.date().year()-2000,
                        tmpDateTime.date().month(),
                        tmpDateTime.date().day(),
                        tmpDateTime.time().hour(),
                        tmpDateTime.time().minute(),
                        tmpDateTime.time().second());
    updateEffect(tmpClock);
}

void TimeInstrument::OperateStep0()
{
    if(m_EffectValid == false)
        return;
    update();
    //>@执行STEP0中的内容，如果执行不成功，尝试执行STEP1中的内容
    if(m_OperateInfo[STEP0].mValid)
    {
        StartTimeline(m_AreaAnimate.mFrameStart,
                      m_AreaAnimate.mFrameEnd,
                      m_AreaAnimate.mAnimateTime,
                      m_AreaAnimate.mAnimateType);
        return;
    }
    OperateStep1();
}

void TimeInstrument::OperateStep1()
{
    if(m_EffectValid == false)
        return;
    //>@准备资源，两个必定为同一张图片
    m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer;
    update();
    if(m_OperateInfo[STEP1].mValid)
    {
        StartTimeline(m_AreaAnimate.mFrameStart,
                      m_AreaAnimate.mFrameEnd,
                      m_AreaAnimate.mAnimateTime,
                      m_AreaAnimate.mAnimateType);
    }
}

void TimeInstrument::CalcElementInfo(CLOCK_TYPE pType)
{
    if(pType == ANALOG)
    {
        //>@根据区域大小计算时间布局
        int hw = ::qMin(width(), height());
        int hh = hw;
        m_ItemWidth = (qreal)hw / 20.0;
        m_ItemHeight = ::qMin((int)m_ItemWidth, hh);
        //>@下面几行代码要放在for(int i 循环之前，因为在其中使用m_PointerLength来计算指针的RECT。
        m_PointerLength[Element_HourPointer] = (qreal)(hw / 2.0) * 6.0 / 10.0;
        m_PointerLength[Element_MinutePointer] = (qreal)(hw / 2.0) * 6.5 / 10.0;
        m_PointerLength[Element_SecondPointer] = (qreal)(hw / 2.0) * 8.0 / 10.0;
        m_PointerAngle[Element_HourPointer][Last] = 0;
        m_PointerAngle[Element_MinutePointer][Last] = 0;
        m_PointerAngle[Element_SecondPointer][Last] = 0;
        for(int i=Element_HourPointer; i<=Element_Week; i++)
        {
            CLOCK_ELEMENTINFO *tmpEle = new CLOCK_ELEMENTINFO;
            tmpEle->mAnimateFlag = false;
            switch(i)
            {
                case Element_Dot:
                {
                    int w = (qreal)hw / 10.0;
                    tmpEle->mRect = QRectF(hw/2.0-w/2.0,hw/2.0-w/2.0, w, w);
                    tmpEle->mRcPointer.mLastPointer = ClockDot;
                    tmpEle->mRcPointer.mCurPointer = ClockDot;
                    break;
                }
                case Element_Year0:
                case Element_Year1:
                case Element_Year2:
                case Element_Year3:
                case Element_Month0:
                case Element_Month1:
                case Element_Day0:
                case Element_Day1:
                    tmpEle->mRect = QRectF(m_ItemWidth*(i-Element_Year0+5),
                                          (qreal)hh / 3.0,
                                          m_ItemWidth,
                                          m_ItemHeight);
                    tmpEle->mRcPointer.mLastPointer = Text0;
                    tmpEle->mRcPointer.mCurPointer = Text0;
                    break;
                case Element_Minus0:
                case Element_Minus1:
                    tmpEle->mRect = QRectF(m_ItemWidth*(i-Element_Year0+5),
                                          (qreal)hh / 3.0,
                                          m_ItemWidth,
                                          m_ItemHeight);
                    tmpEle->mRcPointer.mLastPointer = ClockMinus;  //>@ClockMinus指绘制-符号
                    tmpEle->mRcPointer.mCurPointer = ClockMinus;
                    break;
                case Element_Week:
                    tmpEle->mRect = QRectF(m_ItemWidth*12,
                                          m_ItemHeight*10 - m_ItemHeight/2.0,
                                          m_ItemWidth*4,
                                          m_ItemHeight);
                    tmpEle->mRcPointer.mLastPointer = ClockSun;  //>@ClockSun表示星期日
                    tmpEle->mRcPointer.mCurPointer = ClockSun;
                    break;
                default:
                    delete tmpEle;
                    continue;
            }
            m_ClockElement.insert((CLOCKE_ELEMENT)i, tmpEle);
        }
    }
    else if(pType == DIGITAL)
    {
        int hw = width();
        int hh = height();
        switch (m_Parttern)
        {
            case PTN_V_YMDWHMS:
            {
                m_ItemWidth = (qreal)hw/10.0;
                m_ItemHeight = (qreal)hh/4.0;
                for(int i=Element_Year0; i<=Element_Second1; i++)
                {
                    CLOCK_ELEMENTINFO *tmpEle = new CLOCK_ELEMENTINFO;
                    tmpEle->mAnimateFlag = false;
                    if(i < Element_Week)        //>@年月日其区域
                    {
                        int j = i-Element_Year0;
                        tmpEle->mRect = QRectF(m_ItemWidth*j, 0, m_ItemWidth, m_ItemHeight);
                        if(i == Element_Minus0 || i == Element_Minus1)
                        {
                            tmpEle->mRcPointer.mLastPointer = ClockMinus;
                            tmpEle->mRcPointer.mCurPointer = ClockMinus;
                        }
                        else
                        {
                            tmpEle->mRcPointer.mLastPointer = Text0;
                            tmpEle->mRcPointer.mCurPointer = Text0;
                        }
                    }
                    else if(i == Element_Week)  //>@星期区域
                    {
                        tmpEle->mRect = QRectF(m_ItemWidth*3, m_ItemHeight*3/2, m_ItemWidth*4, m_ItemHeight);
                        tmpEle->mRcPointer.mLastPointer = ClockSun;
                        tmpEle->mRcPointer.mCurPointer = ClockSun;
                    }
                    else                    //>@11~19 时分秒区域
                    {
                        int j = i-Element_Hour0;
                        tmpEle->mRect = QRectF(m_ItemWidth*(j+1), m_ItemHeight*3, m_ItemWidth, m_ItemHeight);
                        if(i == Element_Colon0 || i == Element_Colon1)
                        {
                            tmpEle->mRcPointer.mLastPointer = ClockColon;
                            tmpEle->mRcPointer.mCurPointer = ClockColon;
                        }
                        else
                        {
                            tmpEle->mRcPointer.mLastPointer = Text0;
                            tmpEle->mRcPointer.mCurPointer = Text0;
                        }
                    }
                    m_ClockElement.insert((CLOCKE_ELEMENT)i, tmpEle);
                }
                break;
            }
            case PTN_H_YMDWHMS:
            {
                m_ItemWidth = (qreal)hw/24.0;
                m_ItemHeight = (qreal)hh;
                for(int i=Element_Year0; i<=Element_Second1; i++)
                {
                    CLOCK_ELEMENTINFO *tmpEle = new CLOCK_ELEMENTINFO;
                    tmpEle->mAnimateFlag = false;
                    if(i < Element_Week)        //>@年月日其区域
                    {
                        int j = i-Element_Year0;
                        tmpEle->mRect = QRectF(m_ItemWidth*j, 0, m_ItemWidth, m_ItemHeight);
                        if(i == Element_Minus0 || i == Element_Minus1)
                        {
                            tmpEle->mRcPointer.mLastPointer = ClockMinus;
                            tmpEle->mRcPointer.mCurPointer = ClockMinus;
                        }
                        else
                        {
                            tmpEle->mRcPointer.mLastPointer = Text0;
                            tmpEle->mRcPointer.mCurPointer = Text0;
                        }
                    }
                    else if(i == Element_Week)  //>@星期区域
                    {
                        int j = i-Element_Year0;
                        tmpEle->mRect = QRectF(m_ItemWidth*(j+1), 0, m_ItemWidth*4, m_ItemHeight);
                        tmpEle->mRcPointer.mLastPointer = ClockSun;
                        tmpEle->mRcPointer.mCurPointer = ClockSun;
                    }
                    else                    //>@11~19 时分秒区域
                    {
                        int j = i-Element_Year0;
                        tmpEle->mRect = QRectF(m_ItemWidth*(j+5), 0, m_ItemWidth, m_ItemHeight);
                        if(i == Element_Colon0 || i == Element_Colon1)
                        {
                            tmpEle->mRcPointer.mLastPointer = ClockColon;
                            tmpEle->mRcPointer.mCurPointer = ClockColon;
                        }
                        else
                        {
                            tmpEle->mRcPointer.mLastPointer = Text0;
                            tmpEle->mRcPointer.mCurPointer = Text0;
                        }
                    }
                    m_ClockElement.insert((CLOCKE_ELEMENT)i, tmpEle);
                }
                break;
            }
            case PTN_MOJI_MDWHM:
            {
                m_ItemWidth = (qreal)hw/10.0;
                m_ItemHeight = (qreal)hh/2.5;
                break;
            }
            default:
                return;
        }
    }
}

void TimeInstrument::CalcAnalogClockPointer()
{
    for(int i=Element_HourPointer; i<=Element_SecondPointer; i++)
    {
        CLOCK_ELEMENTINFO *tmpEle = new CLOCK_ELEMENTINFO;
        tmpEle->mAnimateFlag = false;
        switch(i)
        {
            case Element_HourPointer:
            case Element_MinutePointer:
            case Element_SecondPointer:
            {
                QPixmap tmpPixmap = m_EffectImage.value(i+ClockHourPointer);
                if(tmpPixmap.isNull())
                {
                    IDE_TRACE();
                    break;
                }
                IDE_TRACE_INT(i);
                qreal ww,hh;
                int w = tmpPixmap.width();
                int h = tmpPixmap.height();
                if(w >= h)
                {
                    ww = m_PointerLength[i];
                    qreal scale = (qreal)w / ww;
                    hh = (qreal)h / scale;
                    tmpEle->mRect = QRectF(0,-hh/2.0, ww, hh);
                }
                else
                {
                    hh = m_PointerLength[i];
                    qreal scale = (qreal)h / hh;
                    ww = (qreal)w / scale;
                    tmpEle->mRect = QRectF(-ww/2.0, 0, ww, hh);
                }
                tmpEle->mRcPointer.mLastPointer = i+ClockHourPointer;
                tmpEle->mRcPointer.mCurPointer = i+ClockHourPointer;
                break;
            }
        }
        m_ClockElement.insert((CLOCKE_ELEMENT)i, tmpEle);
    }
}

void TimeInstrument::paintEvent(QPaintEvent *event)
{
    if(m_EffectValid == false)
        return;
    QPainter p(this);
    if(m_Type == ANALOG)
    {
        int bgw = ::qMin(width(), height());
        //>@绘制背景
        DrawPixmap(p, m_EffectImage.value(BGRC), QRect(0,0,bgw,bgw));
        //>@绘制日期、星期
        for(int i=Element_Year0;i<=Element_Week;i++)
        {
            CLOCK_ELEMENTINFO* tmpEle = m_ClockElement.value((CLOCKE_ELEMENT)i);
            if(tmpEle == 0)
                continue;
            DrawPixmap(p, m_EffectImage.value(tmpEle->mRcPointer.mCurPointer), tmpEle->mRect.toRect());
        }
        //>@绘制指针
        int index = m_Animator.currentFrame();
        int tmpFrameCount;
        for(int i=Element_HourPointer;i<=Element_SecondPointer;i++)
        {
            CLOCK_ELEMENTINFO* tmpEle = m_ClockElement.value((CLOCKE_ELEMENT)i);
            if(tmpEle == 0)
                continue;
            p.save();
            QTransform transform;
            transform.translate(bgw/2, bgw/2);
            if(tmpEle->mAnimateFlag)
            {
                tmpFrameCount = m_AreaAnimate.mFrameEnd - m_AreaAnimate.mFrameStart;
                m_Diff = (qreal)(m_PointerAngle[i][Current] - m_PointerAngle[i][Last]) / tmpFrameCount;
                transform.rotate(m_PointerAngle[i][Last] + m_Diff*index);
            }
            else
            {
                transform.rotate(m_PointerAngle[i][Current]);
            }
            p.setTransform(transform);
            DrawPixmap(p, m_EffectImage.value(tmpEle->mRcPointer.mCurPointer), tmpEle->mRect.toRect());
            p.restore();
        }
        //>@绘制Dot
        CLOCK_ELEMENTINFO* tmpEle = m_ClockElement.value(Element_Dot);
        if(tmpEle)
        {
            DrawPixmap(p, m_EffectImage.value(tmpEle->mRcPointer.mCurPointer), tmpEle->mRect.toRect());
        }
    }
    else if(m_Type == DIGITAL)
    {
        //>@绘制背景
        DrawPixmap(p, m_EffectImage.value(BGRC), rect());
        //>@绘制时间
        for(int i=Element_Year0;i<=Element_Second1;i++)
        {
            CLOCK_ELEMENTINFO* tmpEle = m_ClockElement.value((CLOCKE_ELEMENT)i);
            if(tmpEle == 0)
                continue;
            m_PixmapPointer.mLastPointer = m_EffectImage.value(tmpEle->mRcPointer.mLastPointer);
            m_PixmapPointer.mCurPointer = m_EffectImage.value(tmpEle->mRcPointer.mCurPointer);
            if(tmpEle->mAnimateFlag == false)
            {
                Paint2D_None(p, tmpEle->mRect.toRect());
            }
            else
            {
                switch(m_AreaAnimate.mEffectType)
                {
                case EffectTypeRoll:
                    Paint2D_Roll(p, tmpEle->mRect.toRect());
                    break;
                case EffectTypeFlip:
                    Paint2D_Flip(p, tmpEle->mRect.toRect());
                    break;
                default:
                    Paint2D_None(p, tmpEle->mRect.toRect());
                    break;
                }
            }
        }
    }
    return;
}

void TimeInstrument::updateEffect(CLOCK_INFO pTime)
{
    if(m_EffectValid == false)
        return;
    if(pTime.isValid() == false)
    {
        IDE_DEBUG("DateTime is invalid,please check it!");
        return;
    }
    bool flag = false;
    //>@确定数字时钟各部位的图片
    quint32 tmpNewYear = pTime.mYear + 2000;
    quint32 tmpOldYear = m_SavedTime.mYear + 2000;
    CLOCK_ELEMENTINFO* tmpEle = m_ClockElement.value(Element_Year0);
    if(tmpEle != 0)
    {
        tmpEle->mAnimateFlag = false;
        quint32 tmpNewYear0 = tmpNewYear / 1000;
//        quint32 tmpOldYear0 = tmpOldYear / 1000;
//        if(tmpNewYear0 != tmpOldYear0)
//        {
//            tmpEle->mAnimateFlag = true;
//            tmpEle->mRcPointer.mLastPointer = tmpOldYear0;
//            tmpEle->mRcPointer.mCurPointer = tmpNewYear0;
//            IDE_TRACE_INT(tmpEle->mRcPointer.mLastPointer);
//            IDE_TRACE_INT(tmpEle->mRcPointer.mCurPointer);
//            flag = true;
//        }
        tmpEle->mAnimateFlag = true;
        tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
        tmpEle->mRcPointer.mCurPointer = tmpNewYear0;
    }
    tmpEle = m_ClockElement.value(Element_Year1);
    if(tmpEle != 0)
    {
        tmpEle->mAnimateFlag = false;
        quint32 tmpNewYear1 = tmpNewYear%1000/100;
//        quint32 tmpOldYear1 = tmpOldYear%1000/100;
//        if(tmpNewYear1 != tmpOldYear1)
//        {
//            tmpEle->mAnimateFlag = true;
//            tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
//            tmpEle->mRcPointer.mCurPointer = tmpNewYear1;
//            flag = true;
//        }
        tmpEle->mAnimateFlag = true;
        tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
        tmpEle->mRcPointer.mCurPointer = tmpNewYear1;
    }
    tmpEle = m_ClockElement.value(Element_Year2);
    if(tmpEle != 0)
    {
        tmpEle->mAnimateFlag = false;
        quint32 tmpNewYear2 = tmpNewYear%100/10;
        quint32 tmpOldYear2 = tmpOldYear%100/10;
        if(tmpOldYear2 != tmpNewYear2)
        {
            tmpEle->mAnimateFlag = true;
            tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
            tmpEle->mRcPointer.mCurPointer = tmpNewYear2;
            //IDE_TRACE_INT(tmpEle->mRcPointer.mLastPointer);
            //IDE_TRACE_INT(tmpEle->mRcPointer.mCurPointer);
            flag = true;
        }
    }
    tmpEle = m_ClockElement.value(Element_Year3);
    if(tmpEle != 0)
    {
        tmpEle->mAnimateFlag = false;
        quint32 tmpNewYear3 = tmpNewYear%10;
        quint32 tmpOldYear3 = tmpOldYear%10;
        if(tmpNewYear3 != tmpOldYear3)
        {
            tmpEle->mAnimateFlag = true;
            tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
            tmpEle->mRcPointer.mCurPointer = tmpNewYear3;
            flag = true;
        }
    }

    tmpEle = m_ClockElement.value(Element_Month0);
    if(tmpEle != 0)
    {
        tmpEle->mAnimateFlag = false;
        quint32 tmpNewMonth0 = pTime.mMonth/10;
        quint32 tmpOldMonth0 = m_SavedTime.mMonth/10;
        if(tmpNewMonth0 != tmpOldMonth0)
        {
            tmpEle->mAnimateFlag = true;
            tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
            tmpEle->mRcPointer.mCurPointer = tmpNewMonth0;
            flag = true;
        }
    }
    tmpEle = m_ClockElement.value(Element_Month1);
    if(tmpEle != 0)
    {
        tmpEle->mAnimateFlag = false;
        quint32 tmpNewMonth1 = pTime.mMonth%10;
        quint32 tmpOldMonth1 = m_SavedTime.mMonth%10;
        if(tmpNewMonth1 != tmpOldMonth1)
        {
            tmpEle->mAnimateFlag = true;
            tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
            tmpEle->mRcPointer.mCurPointer = tmpNewMonth1;
            flag = true;
        }
    }

    tmpEle = m_ClockElement.value(Element_Day0);
    if(tmpEle != 0)
    {
        tmpEle->mAnimateFlag = false;
        quint32 tmpNewDay0 = pTime.mDay/10;
        quint32 tmpOldDay0 = m_SavedTime.mDay/10;
        if(tmpNewDay0 != tmpOldDay0)
        {
            tmpEle->mAnimateFlag = true;
            tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
            tmpEle->mRcPointer.mCurPointer = tmpNewDay0;
            flag = true;
        }
    }
    tmpEle = m_ClockElement.value(Element_Day1);
    if(tmpEle != 0)
    {
        tmpEle->mAnimateFlag = false;
        quint32 tmpNewDay0 = pTime.mDay%10;
        quint32 tmpOldDay0 = m_SavedTime.mDay%10;
        if(tmpNewDay0 != tmpOldDay0)
        {
            tmpEle->mAnimateFlag = true;
            tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
            tmpEle->mRcPointer.mCurPointer = tmpNewDay0;
            flag = true;
        }
    }
    //>@week
    tmpEle = m_ClockElement.value(Element_Week);
    if(tmpEle != 0)
    {
        tmpEle->mAnimateFlag = false;
        if(m_SavedTime.mWeekDay != pTime.mWeekDay)
        {
            tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
            tmpEle->mRcPointer.mCurPointer = 10 + pTime.mWeekDay/*1~7*/ - 1;
            tmpEle->mAnimateFlag = true;
            flag = true;
        }
    }

    //>@查看时钟部分
    if(m_Type == DIGITAL)
    {
        tmpEle = m_ClockElement.value(Element_Hour0);
        if(tmpEle != 0)
        {
            tmpEle->mAnimateFlag = false;
            quint32 tmpNewHour0 = pTime.mHour/10;
            quint32 tmpOldHour0 = m_SavedTime.mHour/10;
            if(tmpNewHour0 != tmpOldHour0)
            {
                tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
                tmpEle->mRcPointer.mCurPointer = tmpNewHour0;
                tmpEle->mAnimateFlag = true;
                flag = true;
            }
        }
        tmpEle = m_ClockElement.value(Element_Hour1);
        if(tmpEle != 0)
        {
            tmpEle->mAnimateFlag = false;
            quint32 tmpNewHour1 = pTime.mHour%10;
            quint32 tmpOldHour1 = m_SavedTime.mHour%10;
            if(tmpNewHour1 != tmpOldHour1)
            {
                tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
                tmpEle->mRcPointer.mCurPointer = tmpNewHour1;
                tmpEle->mAnimateFlag = true;
                flag = true;
            }
        }

        tmpEle = m_ClockElement.value(Element_Minute0);
        if(tmpEle != 0)
        {
            tmpEle->mAnimateFlag = false;
            quint32 tmpNewMinute0 = pTime.mMinute/10;
            quint32 tmpOldMinute0 = m_SavedTime.mMinute/10;
            if(tmpNewMinute0 != tmpOldMinute0)
            {
                tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
                tmpEle->mRcPointer.mCurPointer = tmpNewMinute0;
                tmpEle->mAnimateFlag = true;
                flag = true;
            }
        }
        tmpEle = m_ClockElement.value(Element_Minute1);
        if(tmpEle != 0)
        {
            tmpEle->mAnimateFlag = false;
            quint32 tmpNewMinute1 = pTime.mMinute%10;
            quint32 tmpOldMinute1 = m_SavedTime.mMinute%10;
            if(tmpNewMinute1 != tmpOldMinute1)
            {
                tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
                tmpEle->mRcPointer.mCurPointer = tmpNewMinute1;
                tmpEle->mAnimateFlag = true;
                flag = true;
            }
        }


        tmpEle = m_ClockElement.value(Element_Second0);
        if(tmpEle != 0)
        {
            tmpEle->mAnimateFlag = false;
            quint32 tmpNewSecond0 = pTime.mSecond/10;
            quint32 tmpOldSecond0 = m_SavedTime.mSecond/10;
            if(tmpNewSecond0 != tmpOldSecond0)
            {
                tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
                tmpEle->mRcPointer.mCurPointer = tmpNewSecond0;
                tmpEle->mAnimateFlag = true;
                flag = true;
            }
        }
        tmpEle = m_ClockElement.value(Element_Second1);
        if(tmpEle != 0)
        {
            tmpEle->mAnimateFlag = false;
            quint32 tmpNewSecond1 = pTime.mSecond%10;
            quint32 tmpOldSecond1 = m_SavedTime.mSecond%10;
            if(tmpNewSecond1 != tmpOldSecond1)
            {
                tmpEle->mRcPointer.mLastPointer = tmpEle->mRcPointer.mCurPointer;
                tmpEle->mRcPointer.mCurPointer = tmpNewSecond1;
                tmpEle->mAnimateFlag = true;
                flag = true;
            }
        }
    }
    else if(m_Type == ANALOG)
    {
        tmpEle = m_ClockElement.value(Element_HourPointer);
        if(tmpEle != 0)
        {
            tmpEle->mAnimateFlag = false;
            if(m_SavedTime.mHour != pTime.mHour)
            {
                //>@用来解决当从60变为1时出现的指针跳变
                if(pTime.mHour == 0)
                {
                    m_PointerAngle[0][Last] = 6.0 * (-1 + pTime.mMinute / 60.0) + 180;
                    m_PointerAngle[0][Current] = 6.0 * pTime.mMinute / 60.0 + 180;
                }
                else
                {
                    m_PointerAngle[0][Last] = m_PointerAngle[0][Current];
                    m_PointerAngle[0][Current] = 30.0 * ((pTime.mHour + pTime.mMinute / 60.0)) + 180;
                }
                tmpEle->mAnimateFlag = true;
                flag = true;
            }
        }
        //>@Minute
        tmpEle = m_ClockElement.value(Element_MinutePointer);
        if(tmpEle != 0)
        {
            tmpEle->mAnimateFlag = false;
            if(m_SavedTime.mMinute != pTime.mMinute)
            {
                //>@用来解决当从60变为1时出现的指针跳变
                if(pTime.mMinute == 0)
                {
                    m_PointerAngle[1][Last] = 6.0 * (-1 + pTime.mSecond / 60.0) + 180;
                    m_PointerAngle[1][Current] = 6.0 * pTime.mSecond / 60.0 + 180;
                }
                else
                {
                    m_PointerAngle[1][Last] = m_PointerAngle[1][Current];
                    m_PointerAngle[1][Current] = 6.0 * (pTime.mMinute + pTime.mSecond / 60.0) + 180;
                }
                tmpEle->mAnimateFlag = true;
                flag = true;
            }
        }
        //>@Second
        tmpEle = m_ClockElement.value(Element_SecondPointer);
        if(tmpEle != 0)
        {
            tmpEle->mAnimateFlag = false;
            if(m_SavedTime.mSecond != pTime.mSecond)
            {
                //>@用来解决当从60变为1时出现的指针跳变
                if(pTime.mSecond == 0)
                {
                    m_PointerAngle[2][Last] = -6.0 + 180;
                    m_PointerAngle[2][Current] = 0 + 180;
                }
                else
                {
                    m_PointerAngle[2][Last] = m_PointerAngle[2][Current];
                    m_PointerAngle[2][Current] = 6.0 * (pTime.mSecond) + 180;
                }
                tmpEle->mAnimateFlag = true;
                flag = true;
            }
        }
    }

    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    if(flag)
    {
        if(m_SavedTime.fromClock(pTime) == false)
        {
            IDE_DEBUG("DateTime is invalid,please check it!");
        }
        m_OperateInfo[STEP0].mValid = true;
        m_OperateInfo[STEP0].mOperate = OPERATE_CHANGE;
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = OPERATE_NONE;
    //>@执行STEP0中的内容
    OperateStep0();
}
