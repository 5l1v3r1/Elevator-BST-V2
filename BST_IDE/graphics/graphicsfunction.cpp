#include "graphicsfunction.h"

GraphicsFunction::GraphicsFunction(QGraphicsItem *parent) :
    GraphicsOperate(parent)
{
}

bool GraphicsFunction::Start()
{
    if(GraphicsOperate::Start() == false)
    {
        IDE_TRACE();
        return false;
    }
    m_RegionState = INVALIDRC;
    return true;
}

void GraphicsFunction::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    GraphicsOperate::updateEffect(pOperate, pPara);
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            //>@分解坐标，如果点击上半区域，则向上滚动
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < size().height()/2)
            {
                updateEffect(getPrevRc(m_RegionState), OPERATE_STEADY);
            }
            else if(tmpPoint.y() > size().height()/2)
            {
                updateEffect(getNextRc(m_RegionState), OPERATE_STEADY);
            }
            break;
        }
        case OPERATE_CHANGE:
        {
            int index = pPara.toInt();
            //>@如果遇到0xffff，则默认播放下一张
            if(index == NEXTRC)
                index = getNextRc(m_RegionState);
            else if(index == PREVRC)
                index = getPrevRc(m_RegionState);
            //IDE_TRACE_INT(index);
            updateEffect(index, OPERATE_STEADY);
            break;
        }
        default:
            break;
    }
}

void GraphicsFunction::updateEffect(int pState, AREA_OPERATE pOperate)
{
    bool tmpStateChange = false;
    bool tmpOperateChange = false;
    if(pState != m_RegionState)
        tmpStateChange = true;
    if(pOperate != m_OperateInfo[STEP1].mOperate)
        tmpOperateChange = true;
    if(tmpStateChange == false && tmpOperateChange == false)
    {
        IDE_TRACE();
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
        m_PixmapPointer.mCurPointer = GetPixmap(pState, size());
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = pOperate;
    m_RegionState = pState;
    //>@执行STEP0中的内容
    OperateStep0();
}
