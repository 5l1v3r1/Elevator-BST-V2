#include "effectclock.h"

EffectClock::EffectClock(QWidget *parent, QRect pRect):
    EffectBase(parent, pRect)
{
    m_Layout = DWT_YMDHMS;
    m_ClockAlign = ClockVertical;
    m_Type = DIGITAL;

    m_Diff = 0;
    m_ItemWidth = m_ItemHeight = 0;
}

bool EffectClock::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        IDE_TRACE();
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        RESOURCE_INFO tmpRc;
        int index;
        int count = pEffect->Effect.RcFiles.count();
        if(pEffect->Effect.EffectType == QString("Digital"))
        {
            m_Type = DIGITAL;
            EffectBase::analysisEffectPara((AREA_ANIMATE*)0, pEffect->Effect.EffectPara);
            CalcElementInfo(m_Type);
            for(int i=0;i<count;i++)
            {
                tmpRc = pEffect->Effect.RcFiles.at(i);
                index = GetKey(tmpRc.RcName);
                IDE_TRACE_INT(index);
                if(index == BGIMG)
                {
                    LoadPixmap(index, tmpRc.RcFile);
                }
                else if((index >= Text0 && index <= Text9) || (index >= ClockColon && index <= ClockMinus))
                {
                    LoadPixmap(index, tmpRc.RcFile, QSize(m_ItemWidth, m_ItemHeight));
                }
                else if(index >= ClockMon && index <= ClockSun)
                {
                    LoadPixmap(index, tmpRc.RcFile, QSize(4*m_ItemWidth, m_ItemHeight));
                }
            }
        }
        else if(pEffect->Effect.EffectType == QString("Analog"))
        {
            m_Type = ANALOG;
            CalcElementInfo(m_Type);
            for(int i=0;i<count;i++)
            {
                tmpRc = pEffect->Effect.RcFiles.at(i);
                index = GetKey(tmpRc.RcName);
                IDE_TRACE_INT(index);
                if(index == BGIMG)
                {
                    LoadPixmap(index, tmpRc.RcFile);
                }
                else if((index >= Text0 && index <= Text9) || index == ClockMinus)
                {
                    LoadPixmap(index, tmpRc.RcFile, QSize(m_ItemWidth, m_ItemHeight));
                }
                else if(index >= ClockMon && index <= ClockSun)
                {
                    LoadPixmap(index, tmpRc.RcFile, QSize(4*m_ItemWidth, m_ItemHeight));
                }
                else if(index >= ClockHourPointer && index <= ClockDot)
                {
                    LoadPixmap(index, tmpRc.RcFile);
                }
            }
            CalcAnalogClockPointer();
        }
    }
    else if(tmpString.compare(QString("Change"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Roll"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRoll;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Flip"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeFlip;
        }
        else
        {
            IDE_TRACE_STR(pEffect->Effect.EffectType);
            delete tmpAreaAnimate;
            return false;
        }
        EffectBase::analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_CHANGE, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool EffectClock::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    EffectBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
    if(pLabel.compare(PARA_PATTERN, Qt::CaseInsensitive) == 0)
    {
        if(pContent == QString("0"))
            m_Layout = DWT_YMDHMS;
        else if(pContent == QString("1"))
            m_Layout = DWT_DMYHMS;
        else if(pContent == QString("2"))  //>@指去掉星期的时间排布
            m_Layout = DWT_MDYHMS;
        else if(pContent == QString("3"))
            m_Layout = DT_YMDHMS;
        else if(pContent == QString("4"))
            m_Layout = DT_DMYHMS;
        else if(pContent == QString("5"))
            m_Layout = DT_MDYHMS;
    }
    else if(pLabel.compare(PARA_ALIGN, Qt::CaseInsensitive) == 0)
    {
        if(pContent.compare(QString("V"), Qt::CaseInsensitive) == 0)
            m_ClockAlign = ClockVertical;
        else if(pContent.compare(QString("H"), Qt::CaseInsensitive) == 0)
            m_ClockAlign = ClockHorizontal;
    }
    return true;
}

void EffectClock::CalcElementInfo(CLOCK_TYPE pType)
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
                    tmpEle->mRect = QRect(hw/2.0-w/2.0,hw/2.0-w/2.0, w, w);
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
                    tmpEle->mRect = QRect(m_ItemWidth*(i-Element_Year0+5),
                                          (qreal)hh / 3.0,
                                          m_ItemWidth,
                                          m_ItemHeight);
                    tmpEle->mRcPointer.mLastPointer = Text0;
                    tmpEle->mRcPointer.mCurPointer = Text0;
                    break;
                case Element_Minus0:
                case Element_Minus1:
                    tmpEle->mRect = QRect(m_ItemWidth*(i-Element_Year0+5),
                                          (qreal)hh / 3.0,
                                          m_ItemWidth,
                                          m_ItemHeight);
                    tmpEle->mRcPointer.mLastPointer = ClockMinus;  //>@ClockMinus指绘制-符号
                    tmpEle->mRcPointer.mCurPointer = ClockMinus;
                    break;
                case Element_Week:
                    tmpEle->mRect = QRect(m_ItemWidth*12,
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
        if(m_ClockAlign == ClockVertical)
        {
            switch (m_Layout)
            {
                case DWT_YMDHMS:
                case DWT_DMYHMS:
                case DWT_MDYHMS:
                    m_ItemWidth = (qreal)hw/10.0;
                    m_ItemHeight = (qreal)hh/4.0;
                    break;
                case DT_YMDHMS:
                case DT_DMYHMS:
                case DT_MDYHMS:
                    m_ItemWidth = (qreal)hw/10.0;
                    m_ItemHeight = (qreal)hh/2.5;
                    break;
            }
        }
        else if(m_ClockAlign == ClockHorizontal)
        {
            switch (m_Layout)
            {
                case DWT_YMDHMS:
                case DWT_DMYHMS:
                case DWT_MDYHMS:
                    m_ItemWidth = (qreal)hw/24.0; //>@YYYY_MM_DD_WEEK_HH_MM_SS
                    m_ItemHeight = (qreal)hh;
                    break;
                case DT_YMDHMS:
                case DT_DMYHMS:
                case DT_MDYHMS:
                    m_ItemWidth = (qreal)hw/19.0;//>@YYYY_MM_DD_HH_MM_SS
                    m_ItemHeight = (qreal)hh;
                    break;
            }
        }
        else
            return;

        for(int i=Element_Year0; i<=Element_Second1; i++)
        {
            CLOCK_ELEMENTINFO *tmpEle = new CLOCK_ELEMENTINFO;
            tmpEle->mAnimateFlag = false;
            if(i < Element_Week)        //>@年月日其区域
            {
                int j = i-Element_Year0;
                switch (m_Layout)
                {
                    case DWT_YMDHMS:
                    case DT_YMDHMS:
                        tmpEle->mRect = QRect(m_ItemWidth*j, 0, m_ItemWidth, m_ItemHeight);
                        break;
                    case DWT_DMYHMS:
                    case DT_DMYHMS:
                    {
                        int tmpi;
                        if(j >= Element_Year0 && j <= Element_Year3)
                            tmpi = j + 6;
                        else if(j == Element_Minus0)
                            tmpi = j - 2;
                        else if(j >= Element_Month0 && j <= Element_Month1)
                            tmpi = j - 2;
                        else if(i == Element_Minus1)
                            tmpi = j- 2;
                        else if(j >= Element_Day0 && j <= Element_Day1)
                            tmpi = j - 8;
                        tmpEle->mRect = QRect(m_ItemWidth*tmpi, 0, m_ItemWidth, m_ItemHeight);
                        break;
                    }
                    case DWT_MDYHMS:
                    case DT_MDYHMS:
                    {
                        int tmpi;
                        if(j >= Element_Year0 && j<= Element_Year3)
                            tmpi = j + 6;
                        else if(j == Element_Minus0)
                            tmpi = j - 2;
                        else if(j >= Element_Month0 && j <= Element_Month1)
                            tmpi = j - 5;
                        else if(i == Element_Minus1)
                            tmpi = j - 2;
                        else if(j >= Element_Day0 && j <= Element_Day1)
                            tmpi = j - 5;
                        tmpEle->mRect = QRect(m_ItemWidth*tmpi, 0, m_ItemWidth, m_ItemHeight);
                        break;
                    }
                }
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
                switch (m_Layout)
                {
                    case DWT_YMDHMS:
                    case DWT_DMYHMS:
                    case DWT_MDYHMS:
                        if(m_ClockAlign == ClockHorizontal)
                            tmpEle->mRect = QRect(m_ItemWidth*11, 0, m_ItemWidth*4, m_ItemHeight);
                        else
                            tmpEle->mRect = QRect(m_ItemWidth*3, m_ItemHeight*3/2, m_ItemWidth*4, m_ItemHeight);
                        break;
                    case DT_YMDHMS:
                    case DT_DMYHMS:
                    case DT_MDYHMS:
                        tmpEle->mRect = QRect(0,0,0,0);
                        break;
                }
                tmpEle->mRcPointer.mLastPointer = ClockSun;
                tmpEle->mRcPointer.mCurPointer = ClockSun;
            }
            else                    //>@11~19 时分秒区域
            {
                int j = i-Element_Hour0;
                switch (m_Layout)
                {
                    case DWT_YMDHMS:
                    case DWT_DMYHMS:
                    case DWT_MDYHMS:
                        if(m_ClockAlign == ClockHorizontal)
                        {
                            tmpEle->mRect = QRect(m_ItemWidth*(i+5), 0, m_ItemWidth, m_ItemHeight);
                        }
                        else
                        {
                            tmpEle->mRect = QRect(m_ItemWidth*(j+1), m_ItemHeight*3, m_ItemWidth, m_ItemHeight);
                        }
                        break;
                    case DT_YMDHMS:
                    case DT_DMYHMS:
                    case DT_MDYHMS:
                        if(m_ClockAlign == ClockHorizontal)
                            tmpEle->mRect = QRect(m_ItemWidth*(i+5), 0, m_ItemWidth, m_ItemHeight);
                        else
                            tmpEle->mRect = QRect(m_ItemWidth*(j+1), m_ItemHeight*3/2, m_ItemWidth, m_ItemHeight);
                        break;
                }
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
    }
}

void EffectClock::CalcAnalogClockPointer()
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
                    tmpEle->mRect = QRect(0,-hh/2.0, ww, hh);
                }
                else
                {
                    hh = m_PointerLength[i];
                    qreal scale = (qreal)h / hh;
                    ww = (qreal)w / scale;
                    tmpEle->mRect = QRect(-ww/2.0, 0, ww, hh);
                }
                tmpEle->mRcPointer.mLastPointer = i+ClockHourPointer;
                tmpEle->mRcPointer.mCurPointer = i+ClockHourPointer;
                break;
            }
        }
        m_ClockElement.insert((CLOCKE_ELEMENT)i, tmpEle);
    }
}

bool EffectClock::Start()
{
    if(EffectBase::Start() == false)
        return false;
    m_SavedTime.fromChar(3,5,2,1,1,1);
    CLOCK_INFO tmpTime(0,1,1,2,0,0);
    updateEffect(tmpTime);
    return true;
}

bool EffectClock::PaintEffect(QPainter &p)
{
    if(m_EffectValid == false)
        return false;
    if(m_Type == ANALOG)
    {
        int bgw = ::qMin(width(), height());
        //>@绘制背景
        DrawPixmap(p, m_EffectImage.value(BGIMG), QRect(0,0,bgw,bgw));
        //>@绘制日期、星期
        for(int i=Element_Year0;i<=Element_Week;i++)
        {
            CLOCK_ELEMENTINFO* tmpEle = m_ClockElement.value((CLOCKE_ELEMENT)i);
            if(tmpEle == 0)
                continue;
            DrawPixmap(p, m_EffectImage.value(tmpEle->mRcPointer.mCurPointer), tmpEle->mRect);
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
            if(tmpEle->mAnimateFlag && m_AreaAnimate)
            {
                tmpFrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart;
                m_Diff = (qreal)(m_PointerAngle[i][Current] - m_PointerAngle[i][Last]) / tmpFrameCount;
                transform.rotate(m_PointerAngle[i][Last] + m_Diff*index);
            }
            else
            {
                transform.rotate(m_PointerAngle[i][Current]);
            }
            p.setTransform(transform);
            DrawPixmap(p, m_EffectImage.value(tmpEle->mRcPointer.mCurPointer), tmpEle->mRect);
            p.restore();
        }
        //>@绘制Dot
        CLOCK_ELEMENTINFO* tmpEle = m_ClockElement.value(Element_Dot);
        if(tmpEle)
        {
            DrawPixmap(p, m_EffectImage.value(tmpEle->mRcPointer.mCurPointer), tmpEle->mRect);
        }
    }
    else if(m_Type == DIGITAL)
    {
        //>@绘制背景
        DrawPixmap(p, m_EffectImage.value(BGIMG), rect());
        //>@绘制时间
        for(int i=Element_Year0;i<=Element_Second1;i++)
        {
            CLOCK_ELEMENTINFO* tmpEle = m_ClockElement.value((CLOCKE_ELEMENT)i);
            if(tmpEle == 0)
                continue;
            m_PixmapPointer.mLastPointer = m_EffectImage.value(tmpEle->mRcPointer.mLastPointer);
            m_PixmapPointer.mCurPointer = m_EffectImage.value(tmpEle->mRcPointer.mCurPointer);
            if(tmpEle->mAnimateFlag == false || m_AreaAnimate == 0)
            {
                Paint2D_None(p, tmpEle->mRect);
            }
            else
            {
                switch(m_AreaAnimate->mEffectType)
                {
                case EffectTypeRoll:
                    Paint2D_Roll(p, tmpEle->mRect);
                    break;
                case EffectTypeFlip:
                    Paint2D_Flip(p, tmpEle->mRect);
                    break;
                case EffectTypeZoom:
                    Paint2D_Zoom(p, tmpEle->mRect);
                    break;
                case EffectTypeRotate:
                    Paint2D_Rotate(p, tmpEle->mRect);
                    break;
                case EffectTypeSlipCycle:
                    Paint2D_Slipcycle(p, tmpEle->mRect);
                    break;
                case EffectTypeTransparent:
                    Paint2D_Transparent(p, tmpEle->mRect);
                    break;
                default:
                    Paint2D_None(p, tmpEle->mRect);
                    break;
                }
            }
        }
    }
    return true;
}

void EffectClock::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    static AREA_OPERATE sOPerate = OPERATE_NONE;
    static QVariant sPara = QVariant();
    if(sOPerate != pOperate && pOperate != OPERATE_PLAY && pOperate != OPERATE_STOP)
    {
        sOPerate = pOperate;
        sPara = pPara;
    }
    switch(pOperate)
    {
        case OPERATE_PLAY:
        {
            updateEffect(sOPerate, sPara);
            break;
        }
        case OPERATE_STOP:
        {
            m_Animator.stop();
            break;
        }
        case OPERATE_KEYPRESS:
        {
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < height()/2)
            {
                CLOCK_INFO tmpTime(0,1,1,2,0,0);
                updateEffect(tmpTime);
            }
            else if(tmpPoint.y() > height()/2)
            {
                CLOCK_INFO tmpTime(3,5,2,1,1,1);
                updateEffect(tmpTime);
            }
            break;
        }
        case OPERATE_CHANGE:
        {
            quint32 tmpValue = pPara.toUInt();
            CLOCK_INFO tmpTime(tmpValue);
            updateEffect(tmpTime);
            break;
        }
        default:
            break;
    }
}

void EffectClock::updateEffect(CLOCK_INFO pTime)
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

