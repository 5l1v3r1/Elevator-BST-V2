#include "graphicsarrow.h"
#include "logicbase.h"

GraphicsArrow::GraphicsArrow(QGraphicsItem *parent, QRectF pRect) :
    GraphicsBase(parent, pRect)
{
}

bool GraphicsArrow::InitEffectRc(STATE_INFO* pEffect)
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
            if(LoadPath(index, tmpRc.RcFile) == false)
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
        //>@根据延时时间修改特效参数
        tmpAreaAnimate->mLoopCount = 0;
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
        //>@根据延时时间修改特效参数
        tmpAreaAnimate->mLoopCount = 0;
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_HALT, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool GraphicsArrow::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return GraphicsBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool GraphicsArrow::Start()
{
    if(GraphicsBase::Start() == false)
        return false;
    updateEffect(OPERATE_CHANGE, QVariant(ArrowUnknow));
    return true;
}

bool GraphicsArrow::PaintEffect(QPainter *p)
{
    if(GraphicsBase::PaintEffect(p) == false)
    {
        return false;
    }
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeRoll:
            Paint2D_Roll(p, rect());
            break;
        case EffectTypeRotate:
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

void GraphicsArrow::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    GraphicsBase::updateEffect(pOperate, pPara);
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            //>@分解坐标，如果点击上半区域，则向上滚动
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < size().height()/3)
            {
                updateEffect(OPERATE_CHANGE, ArrowUproll);
            }
            else if(tmpPoint.y() > size().height()/3 && tmpPoint.y() < size().height()*2/3)
            {
                updateEffect(OPERATE_CHANGE, ArrowUpDown);
            }
            else if(tmpPoint.y() > size().height()*2/3)
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
                    if(tmpAreaAnimate && tmpAreaAnimate->mEffectType == EffectTypeRoll)
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
                    if(tmpAreaAnimate && tmpAreaAnimate->mEffectType == EffectTypeRoll)
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
                    if(tmpAreaAnimate && tmpAreaAnimate->mEffectType == EffectTypeRoll)
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
                    updateEffect(BGRC, OPERATE_NONE);
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

void GraphicsArrow::updateEffect(int pState, AREA_OPERATE pOperate)
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
        m_PixmapPointer.mCurPointer = GetPixmap(pState);
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = pOperate;
    m_CurRcLabel = pState;

    OperateStep0();
}

DesignArrow::DesignArrow(QGraphicsItem *parent, QRectF pRect) :
    GraphicsArrow(parent, pRect)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("UP"));
    m_ActionGroup->addAction(tr("UPROLL"));
    m_ActionGroup->addAction(tr("UPHALT"));
    m_ActionGroup->addAction(tr("DOWN"));
    m_ActionGroup->addAction(tr("DOWNROLL"));
    m_ActionGroup->addAction(tr("DOWNHALT"));
    m_ActionGroup->addAction(tr("UPDOWN"));
    m_ActionGroup->addAction(tr("UPDOWNROLL"));
    m_ActionGroup->addAction(tr("UPDOWNHALT"));
    tmpDefault->setChecked(true);
}

QList<QAction*> DesignArrow::GetActionList()
{
    QList<QAction*> tmpList;
    if(m_ActionGroup)
    {
        bool hasUP = m_EffectPath.contains(ArrowUp);
        bool hasDOWN = m_EffectPath.contains(ArrowDown);
        bool hasUPDOWN = m_EffectPath.contains(ArrowUpDown);
        tmpList = m_ActionGroup->actions();
        //>@根据当前已有的资源使能响应功能Action
        for(int i=0;i<tmpList.count();i++)
        {
            QAction *tmpAction = tmpList.at(i);
            if(tmpAction == 0)
                continue;
            ARROWSTATE tmpState = GetArrowState(tmpAction->text());
            if(tmpState == ArrowUnknow)
                continue;
            if((tmpState == ArrowUp || tmpState == ArrowUproll || tmpState == ArrowUphalt) && hasUP)
                tmpAction->setEnabled(true);
            else if((tmpState == ArrowDown || tmpState == ArrowDownroll || tmpState == ArrowDownhalt) && hasDOWN)
                tmpAction->setEnabled(true);
            else if((tmpState == ArrowUpDown || tmpState == ArrowUpDownroll || tmpState == ArrowUpDownhalt) && hasUPDOWN)
                tmpAction->setEnabled(true);
            else
                tmpAction->setDisabled(true);
        }
    }
    return tmpList;
}

void DesignArrow::ExecAction(QAction *pAction)
{
    updateEffect(OPERATE_CHANGE, QVariant(GetArrowState(pAction->text())));
}
