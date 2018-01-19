#include "graphicstext.h"

GraphicsText::GraphicsText(QGraphicsItem *parent):
    GraphicsOperate(parent)
{
}

bool GraphicsText::Start()
{
    if(GraphicsOperate::Start() == false)
        return false;
    m_TextState = -1;
    updateEffect(OPERATE_CHANGE, QVariant(m_EffectPath.keys().first()));
    return true;
}

void GraphicsText::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
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
                updateEffect(getPrevRc(m_TextState), OPERATE_STEADY);
            }
            else if(tmpPoint.y() > size().height()/2)
            {
                updateEffect(getNextRc(m_TextState), OPERATE_STEADY);
            }
            break;
        }
        case OPERATE_CHANGE:
        {
            int index = pPara.toInt();
            //>@如果遇到0xffff，则默认播放下一张
            if(index == 0x0ffff)
                index = getNextRc(m_TextState);
            updateEffect(index, OPERATE_STEADY);
            break;
        }
        default:
            break;
    }
}

void GraphicsText::updateEffect(int pState, AREA_OPERATE pOperate)
{
    bool tmpStateChange = false;
    bool tmpOperateChange = false;
    if(pState != m_TextState)
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
        m_PixmapPointer.mCurPointer = GetPixmap(pState);
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = pOperate;
    m_TextState = pState;
    //>@执行STEP0中的内容
    OperateStep0();
}

DesignText::DesignText(QGraphicsItem *parent):
    GraphicsText(parent)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
    for(int i=1;i<10;i++)
    {
        m_ActionGroup->addAction(QString("%1").arg(i));
    }
    tmpDefault->setChecked(true);
}

QList<QAction*> DesignText::GetActionList()
{
    QList<QAction*> tmpList;
    if(m_ActionGroup)
    {
        tmpList = m_ActionGroup->actions();
        //>@根据当前已有的资源使能响应功能Action
        for(int i=0;i<tmpList.count();i++)
        {
            QAction *tmpAction = tmpList.at(i);
            if(tmpAction == 0)
                continue;
            int tmpState = tmpAction->text().toInt();
            if(m_EffectPath.contains(tmpState))
                tmpAction->setEnabled(true);
            else
                tmpAction->setDisabled(true);
        }
    }
    return tmpList;
}

void DesignText::ExecAction(QAction *pAction)
{
    updateEffect(pAction->text().toInt(), OPERATE_STEADY);
}
