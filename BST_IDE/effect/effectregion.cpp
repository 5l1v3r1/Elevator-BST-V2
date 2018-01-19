#include "effectregion.h"

EffectRegion::EffectRegion(QWidget *parent, QRect pRect) :
    EffectBase(parent, pRect)
{
}

bool EffectRegion::InitEffectRc(STATE_INFO* pEffect)
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
//            IDE_TRACE_INT(index);
            if(LoadPath(index, tmpRc.RcFile) == false)
            {
                IDE_TRACE_STR(tmpRc.RcFile);
            }
        }
    }
    else if(tmpString.compare(QString("Change"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("SlipCycle"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSlipCycle;
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

bool EffectRegion::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return EffectBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool EffectRegion::Start()
{
    if(EffectBase::Start() == false)
    {
        IDE_TRACE();
        return false;
    }
    m_RegionState = -1;
    m_PixmapPointer.mLastPointer.load(m_EffectPath.value(m_EffectPath.keys().first()));
    m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer;
    updateEffect(m_EffectPath.keys().first(), OPERATE_NONE);
    return true;
}

bool EffectRegion::PaintEffect(QPainter &p)
{
    if(EffectBase::PaintEffect(p) == false)
    {
//        IDE_TRACE();
        return false;
    }
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeRoll:
            Paint2D_Roll(p, rect());
            break;
        case EffectTypeSlipCycle:
            Paint2D_Slipcycle(p, rect());
            break;
        case EffectTypeTransparent:
            Paint2D_Transparent(p, rect());
            break;
        default:
            Paint2D_None(p, rect());
            break;
    }
    return true;
}

void EffectRegion::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
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
                updateEffect(0, OPERATE_NONE);
            }
            else if(tmpPoint.y() > height()/2)
            {
                updateEffect(1, OPERATE_NONE);
            }
            break;
        }
        case OPERATE_CHANGE:
        {
            updateEffect(pPara.toInt(), OPERATE_STEADY);
            break;
        }
        default:
            break;
    }
}

void EffectRegion::updateEffect(int pState, AREA_OPERATE pOperate)
{
    bool tmpStateChange = false;
    bool tmpOperateChange = false;
    if(pState != m_RegionState)
        tmpStateChange = true;
    if(pOperate != m_OperateInfo[STEP1].mOperate)
        tmpOperateChange = true;
    if(tmpStateChange == false && tmpOperateChange == false)
    {
        return;
    }
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
    m_RegionState = pState;
    //>@执行STEP0中的内容
    OperateStep0();
}

EffectDesktop::EffectDesktop(QWidget *parent, QRect pRect) :
    EffectRegion(parent, pRect)
{
}

EffectButtonOperate::EffectButtonOperate(QWidget *parent, QRect pRect) :
    EffectRegion(parent, pRect)
{
}

EffectFuncInfo::EffectFuncInfo(QWidget *parent, QRect pRect) :
    EffectRegion(parent, pRect)
{
}

EffectIntegratedInfo::EffectIntegratedInfo(QWidget *parent, QRect pRect) :
    EffectRegion(parent, pRect)
{
}

EffectMultimedia::EffectMultimedia(QWidget *parent, QRect pRect) :
    EffectRegion(parent, pRect)
{
}

EffectTextBulletin::EffectTextBulletin(QWidget *parent, QRect pRect) :
    EffectRegion(parent, pRect)
{
}
