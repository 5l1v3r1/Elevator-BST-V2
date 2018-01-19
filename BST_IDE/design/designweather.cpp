#include "effectweather.h"

EffectWeather::EffectWeather(QWidget *parent, QRect pRect) :
    EffectBase(parent, pRect)
{
}

bool EffectWeather::InitEffectRc(STATE_INFO* pEffect)
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
        if(pEffect->Effect.EffectType.compare(QString("Zoom"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeZoom ;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Roll"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRoll;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Transparent"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeTransparent;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_CHANGE, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool EffectWeather::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return EffectBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool EffectWeather::Start()
{
    if(EffectBase::Start() == false)
        return false;
    m_WeatherState = 0;
    m_PixmapPointer.mLastPointer.load(m_EffectPath.value(0));
    m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer;
    updateEffect(0, OPERATE_NONE);
    return true;
}

bool EffectWeather::PaintEffect(QPainter &p)
{
    if(EffectBase::PaintEffect(p) == false)
        return false;
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeRoll:
            Paint2D_Roll(p, rect());
            break;
        case EffectTypeRotate:
            Paint2D_Rotate(p, rect());
            break;
        case EffectTypeTransparent:
            Paint2D_Transparent(p, rect());
            break;
        case EffectTypeZoom:
            Paint2D_Zoom(p, rect());
            break;
        default:
            Paint2D_None(p, rect());
            break;
    }
    return true;
}

void EffectWeather::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            //>@分解坐标，如果点击上半区域，则向上滚动
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < height()/2)
            {
                updateEffect(0, OPERATE_NONE);
            }
            else if(tmpPoint.y() > height()/2)
            {
                updateEffect(1, OPERATE_NONE);
            }
            break;
        }
        default:
            break;
    }
}

void EffectWeather::updateEffect(int pState, AREA_OPERATE pOperate)
{
    bool tmpStateChange = false;
    bool tmpOperateChange = false;
    if(pState != m_WeatherState)
        tmpStateChange = true;
    if(pOperate != m_OperateInfo[STEP1].mOperate)
        tmpOperateChange = true;
    if(tmpStateChange == false && tmpOperateChange == false)
        return;
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    if(tmpStateChange)
    {
        m_OperateInfo[STEP0].mValid = true;
        m_OperateInfo[STEP0].mOperate = OPERATE_CHANGE;
        m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer;
        m_PixmapPointer.mCurPointer.load(m_EffectPath.value(pState));
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = pOperate;
    m_WeatherState = pState;
    //>@执行STEP0中的内容
    OperateStep0();
}
