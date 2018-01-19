#include "designprogress.h"

DesignProgress::DesignProgress(QGraphicsItem *parent, QRectF pRect) :
    DesignBase(parent, pRect)
{
    m_FontSizeValue = 40;
}

bool DesignProgress::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        RESOURCE_INFO tmpRc;
        char index;
        int count = pEffect->Effect.RcFiles.count();
        for(int i=0;i<count;i++)
        {
            tmpRc = pEffect->Effect.RcFiles.at(i);
            index = GetKey(tmpRc.RcName);
            IDE_TRACE_INT(index);
            if(LoadPath(index, tmpRc.RcFile) == false)
            {
                IDE_TRACE_STR(tmpRc.RcFile);
            }
        }
    }
    else if(tmpString.compare(QString("Change"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Wheel"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeWheel;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Roll"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRoll;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_CHANGE, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Run"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Wheel"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeWheel;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Roll"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRoll;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_RUN, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool DesignProgress::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return DesignBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool DesignProgress::Start()
{
    if(DesignBase::Start() == false)
    {
        return false;
    }
    return true;
}

bool DesignProgress::PaintEffect(QPainter *p)
{
    QPixmap tmpPixmap(m_EffectPath.value(BGIMG));
    DrawPixmap(p, tmpPixmap, rect());
    if(m_AreaAnimate)
    {
        switch(m_AreaAnimate->mEffectType)
        {
            case EffectTypeWheel:
                Paint2D_Wheel(p, rect());
                break;
            case EffectTypeRoll:
                Paint2D_Roll(p, rect());
                break;
            default:
                Paint2D_None(p, rect());
                break;
        }
    }
    QFont font("Arial", m_FontSizeValue, QFont::DemiBold, true);
    font.setUnderline(false);
    p->setFont(font);
    //>@绘制文字进度
    QTransform transform;
    transform.translate(rect().width()/2, rect().height()/2);
    p->setTransform(transform);
    QRect tmpRect;
    if(m_ProgressValue<10)
    {
        tmpRect = QRect(-m_FontSize.width()/2,
                        -m_FontSize.height()/4,
                        m_FontSize.width(),
                        m_FontSize.height());
    }
    else
    {
        tmpRect = QRect(-m_FontSize.width(),
                        -m_FontSize.height()/2,
                        m_FontSize.width()*2,
                        m_FontSize.height()*2);
    }
    p->drawText(tmpRect, QString("%1").arg(m_ProgressValue));
    return true;
}

void DesignProgress::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            //>@分解坐标，如果点击上半区域，则向上滚动
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < size().height()/2)
            {
                UpdateValue(1);
            }
            else if(tmpPoint.y() > size().height()/2)
            {
                UpdateValue(99);
            }
            break;
        }
        default:
            break;
    }
}

void DesignProgress::updateEffect(int pValue, AREA_OPERATE pOperate)
{
    bool tmpValueChange = false;
    bool tmpOperateChange = false;
    if(m_ProgressValue != pValue)
        tmpValueChange = true;
    if(pOperate != m_OperateInfo[STEP1].mOperate)
        tmpOperateChange = true;
    if(tmpValueChange == false && tmpOperateChange == false)
    {
        IDE_TRACE();
        return;
    }
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    if(tmpValueChange)
    {
        m_OperateInfo[STEP0].mValid = true;
        m_OperateInfo[STEP0].mOperate = OPERATE_CHANGE;
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = pOperate;

    m_PixmapPointer.mCurPointer = GetPixmap(0);
    m_ProgressValue = pValue;
    m_FontSize = CalcSizeFromFontSize("Arial", m_FontSizeValue);
    //>@执行STEP0中的内容
    if(isVisible()==false)
        show();
    OperateStep0();
}

void DesignProgress::UpdateValue(int pValue)
{
    if(pValue <= 0 || pValue >= 100)
    {
        hide();
        return;
    }
    updateEffect(pValue, OPERATE_RUN);
}
