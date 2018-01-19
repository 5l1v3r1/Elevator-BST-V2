#include "effectarrow.h"

EffectArrow::EffectArrow(QWidget *parent, QRect pRect) :
    EffectBase(parent, pRect)
{
}

bool EffectArrow::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    else if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        RESOURCE_INFO tmpRc;
        int index;
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
        if(pEffect->Effect.EffectType.compare(QString("Flip"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeFlip;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Roll"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRoll;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Zoom"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeZoom;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Transparent"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeTransparent;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Blink"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeBlink;
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
        if(pEffect->Effect.EffectType.compare(QString("Roll"), Qt::CaseInsensitive) == 0)
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
    else if(tmpString.compare(QString("Halt"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Blink"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeBlink;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_HALT, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool EffectArrow::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return EffectBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool EffectArrow::Start()
{
    if(EffectBase::Start() == false)
        return false;
    updateEffect(OPERATE_CHANGE, QVariant(ArrowUnknow));
    return true;
}

bool EffectArrow::PaintEffect(QPainter &p)
{
    if(EffectBase::PaintEffect(p) == false)
        return false;
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeRoll:
            Paint2D_Roll(p, rect());
            break;
        case EffectTypeRotate:
            IDE_TRACE();
            Paint2D_Rotate(p, rect());
            break;
        case EffectTypeFlip:
            Paint2D_Flip(p, rect());
            break;
        case EffectTypeZoom:
            Paint2D_Zoom(p, rect());
            break;
        case EffectTypeTransparent:
            Paint2D_Transparent(p, rect());
            break;
        case EffectTypeBlink:
            Paint2D_Blink(p, rect());
            break;
        default:
            Paint2D_None(p, rect());
            break;
    }
    return true;
}

void EffectArrow::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
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
            if(tmpPoint.y() < height()/3)
            {
                updateEffect(OPERATE_CHANGE, ArrowUproll);
            }
            else if(tmpPoint.y() > height()/3 && tmpPoint.y() < height()*2/3)
            {
                updateEffect(OPERATE_CHANGE, ArrowUpDown);
            }
            else if(tmpPoint.y() > height()*2/3)
            {
                updateEffect(OPERATE_CHANGE, ArrowDownroll);
            }
            break;
        }
        case OPERATE_CHANGE:
        {
            ARROWSTATE tmpArrowState = (ARROWSTATE)(pPara.toInt());
            switch(tmpArrowState)
            {
                case ArrowUp:
                {
                    updateEffect(ArrowUp, OPERATE_NONE);
                    break;
                }
                case ArrowUproll:
                {
                    AREA_ANIMATE *tmpAreaAnimate = m_EffectGroup.value(OPERATE_RUN);
                    if(tmpAreaAnimate->mEffectType == EffectTypeRoll)
                    {
                        tmpAreaAnimate->mDirection = Vertical;
                        tmpAreaAnimate->mOrientation = Negative;
                    }
                    updateEffect(ArrowUp, OPERATE_RUN);
                    break;
                }
                case ArrowUphalt:
                {
                    updateEffect(ArrowUp, OPERATE_HALT);
                    break;
                }
                case ArrowDown:
                {
                    updateEffect(ArrowDown, OPERATE_NONE);
                    break;
                }
                case ArrowDownroll:
                {
                    AREA_ANIMATE *tmpAreaAnimate = m_EffectGroup.value(OPERATE_RUN);
                    if(tmpAreaAnimate->mEffectType == EffectTypeRoll)
                    {
                        tmpAreaAnimate->mDirection = Vertical;
                        tmpAreaAnimate->mOrientation = Positive;
                    }
                    updateEffect(ArrowDown, OPERATE_RUN);
                    break;
                }
                case ArrowDownhalt:
                {
                    updateEffect(ArrowDown, OPERATE_HALT);
                    break;
                }
                case ArrowUpDown:
                {
                    updateEffect(ArrowUpDown, OPERATE_NONE);
                    break;
                }
                case ArrowUpDownroll:
                {
                    AREA_ANIMATE *tmpAreaAnimate = m_EffectGroup.value(OPERATE_RUN);
                    if(tmpAreaAnimate->mEffectType == EffectTypeRoll)
                    {
                        tmpAreaAnimate->mDirection = Horizontal;
                        tmpAreaAnimate->mOrientation = Negative;
                    }
                    updateEffect(ArrowUpDown, OPERATE_RUN);
                    break;
                }
                case ArrowUpDownhalt:
                {
                    updateEffect(ArrowUpDown, OPERATE_HALT);
                    break;
                }
                case ArrowUnknow:
                {
                    updateEffect(BGIMG, OPERATE_NONE);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}

void EffectArrow::updateEffect(int pState, AREA_OPERATE pOperate)
{
    bool tmpArrowStateChange = false;
    bool tmpOperateChange = false;
    if(pState != m_CurRcLabel)
        tmpArrowStateChange = true;
    if(pOperate != m_OperateInfo[STEP1].mOperate)
        tmpOperateChange = true;
    if(tmpArrowStateChange == false && tmpOperateChange == false)
        return;
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    if(tmpArrowStateChange)
    {
        m_OperateInfo[STEP0].mValid = true;
        m_OperateInfo[STEP0].mOperate = OPERATE_CHANGE;
        m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer;
        m_PixmapPointer.mCurPointer = m_EffectImage.value(pState);
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = pOperate;
    m_CurRcLabel = pState;
    //>@执行STEP0中的内容
    OperateStep0();
}
