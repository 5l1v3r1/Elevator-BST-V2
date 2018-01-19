#include "effectprogress.h"

EffectProgress::EffectProgress(QWidget *parent, QRect pRect) :
    EffectBase(parent, pRect)
{
    m_FontSizeValue = 40;
}

bool EffectProgress::InitEffectRc(STATE_INFO* pEffect)
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
            if(LoadPixmap(index, tmpRc.RcFile) == false)
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

bool EffectProgress::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return EffectBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool EffectProgress::Start()
{
    EffectBase::Start();
    m_FontSize = CalcSizeFromFontSize("Arial", m_FontSizeValue);
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    m_PixmapPointer.mCurPointer = m_EffectImage.value(0);
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = OPERATE_RUN;
    //>@默认隐藏Message窗口
    hide();
    return true;
}

bool EffectProgress::PaintEffect(QPainter &p)
{
    DrawPixmap(p, m_EffectImage.value(BGIMG), rect());
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
    p.setFont(font);
    //>@绘制文字进度
    QTransform transform;
    transform.translate(rect().width()/2, rect().height()/2);
    p.setTransform(transform);
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
    p.drawText(tmpRect, QString("%1").arg(m_ProgressValue));
    return true;
}

void EffectProgress::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
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
            //>@分解坐标，如果点击上半区域，则向上滚动
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < height()/2)
            {
                UpdateValue(1);
            }
            else if(tmpPoint.y() > height()/2)
            {
                UpdateValue(99);
            }
            break;
        }
        default:
            break;
    }
}

void EffectProgress::updateEffect(int pValue, AREA_OPERATE pOperate)
{
    if(m_EffectValid == false)
    {
        IDE_TRACE();
        return;
    }
    if(pValue <= 0 || pValue >= 100)
    {
        Stop();
        hide();
        return;
    }
    m_ProgressValue = pValue;
    //>@执行STEP0中的内容
    if(isVisible()==false)
        show();
    OperateStep0();
}

void EffectProgress::UpdateValue(quint32 pValue)
{
    updateEffect(pValue, OPERATE_RUN);
}
