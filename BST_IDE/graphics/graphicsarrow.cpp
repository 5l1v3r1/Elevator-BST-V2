#include "graphicsarrow.h"
#include "logicbase.h"

GraphicsArrow::GraphicsArrow(QGraphicsItem *parent) :
    GraphicsOperate(parent)
{
}

bool GraphicsArrow::Start()
{
    if(GraphicsOperate::Start() == false)
        return false;
    updateEffect(OPERATE_CHANGE, QVariant(ArrowUnknow));
    return true;
}

void GraphicsArrow::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    //IDE_TRACE_INT(pOperate);
    GraphicsOperate::updateEffect(pOperate, pPara);
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
            IDE_TRACE_INT(tmpArrowState);
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
                        tmpAreaAnimate->mLoopCount = 0;
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
                IDE_TRACE();
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
                        tmpAreaAnimate->mLoopCount = 0;
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
                        tmpAreaAnimate->mLoopCount = 0;
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
                IDE_TRACE();
                    updateEffect(BGRC, OPERATE_NONE);
                    break;
                }
                default:
                    updateEffect(BGRC, OPERATE_NONE);
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
    //IDE_TRACE_INT(pState);
    //IDE_TRACE_INT(pOperate);
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

DesignArrow::DesignArrow(QGraphicsItem *parent) :
    GraphicsArrow(parent)
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
