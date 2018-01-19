#include "graphicsvideo.h"

GraphicsVideo::GraphicsVideo(QGraphicsItem *parent, qreal pAngle):
    GraphicsOperate(parent)
{
    m_ScreenRotate = pAngle;
    m_CurRcLabel = -1;
    m_Video = 0;
    m_Video = new basevideo(this);
    connect(m_Video,SIGNAL(sFinished()),this,SLOT(slot_Finished()));
}

GraphicsVideo::~GraphicsVideo()
{
    m_CurRcLabel = -1;
    if(m_Video != 0)
    {
        disconnect(m_Video,SIGNAL(sFinished()),this,SLOT(slot_Finished()));
        delete m_Video;
        m_Video = 0;
    }
}

bool GraphicsVideo::Start()
{
    if(GraphicsOperate::Start() == false)
        return false;
    m_CurRcLabel = -1;
    m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer = GetPixmap(BGRC);

    QPointF tmpPoint = this->mapToScene(QPoint(0,0));
    m_Video->SetVideoRect(QRect(tmpPoint.toPoint(), size().toSize()), m_ScreenRotate);

    updateEffect(OPERATE_INIT, QVariant(0));
    return true;
}

void GraphicsVideo::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    if(pOperate == OPERATE_VOCAL)
    {
        if(m_Video !=0)
            m_Video->SetVolume(pPara.toInt());
    }
    else if(pOperate == OPERATE_MUTE)
    {
        if(m_Video !=0)
            m_Video->SetVolume(0);
    }
    else if(pOperate == OPERATE_PAUSE)
    {
        if(m_Video !=0)
            m_Video->Pause();
    }
    else if(pOperate == OPERATE_RESUME)
    {
        if(m_Video !=0)
            m_Video->Resume();
    }
    else if(pOperate == OPERATE_REPEAT)
    {
        m_RepeatMode = GetRepeatMode(pPara.toString());
    }
    else
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
                    updateEffect(getPrevRc(m_CurRcLabel));
                }
                else if(tmpPoint.y() > size().height()/2)
                {
                    updateEffect(getNextRc(m_CurRcLabel));
                }
                break;
            }
            case OPERATE_CHANGE:
            {
                int index = pPara.toInt();
                updateEffect(index);
                break;
            }
            default:
                break;
        }
    }
}

void GraphicsVideo::updateEffect(int pIndex)
{
    if(PREVRC == pIndex)
        pIndex = getPrevRc(m_CurRcLabel);
    else if(NEXTRC == pIndex)
        pIndex = getNextRc(m_CurRcLabel);

    if(BGRC == pIndex)
    {
        slot_Finished();
        return;
    }

    bool tmpIndexChange = false;
    if(pIndex != m_CurRcLabel)
        tmpIndexChange = true;
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    if(tmpIndexChange)
    {
        m_OperateInfo[STEP0].mValid = true;
        m_OperateInfo[STEP0].mOperate = OPERATE_CHANGE;
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = OPERATE_NONE;
    m_CurRcLabel = pIndex;

    QString tmpPath = m_EffectPath.value(pIndex);
    PlayVideo(tmpPath);

    //>@执行STEP0中的内容
    OperateStep0();
}

void GraphicsVideo::PlayVideo(QString pPath)
{
    if(m_Video == 0)
        return;
    m_Video->Stop();
    if(m_RepeatMode == MODE_REPEATONE)
        m_Video->Play(pPath, QString(), true, false);
    else
        m_Video->Play(pPath, QString(), false, false);
}

void GraphicsVideo::slot_Finished()
{
    if(m_RepeatMode == MODE_REPEATALL)
    {
        updateEffect(getNextRc(m_CurRcLabel));
    }
}

DesignVideo::DesignVideo(QGraphicsItem *parent):
    GraphicsVideo(parent)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
    for(int i=1;i<10;i++)
    {
        m_ActionGroup->addAction(QString("%1").arg(i));
    }
    tmpDefault->setChecked(true);
}

void DesignVideo::updateEffect(int pIndex)
{
    bool tmpIndexChange = false;
    if(pIndex != m_CurRcLabel)
        tmpIndexChange = true;
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    if(tmpIndexChange)
    {
        m_OperateInfo[STEP0].mValid = true;
        m_OperateInfo[STEP0].mOperate = OPERATE_CHANGE;
        m_PixmapPointer.mCurPointer = GenerateSpectrum(m_EffectPath.value(pIndex), rect(), m_PixmapPointer.mLastPointer);
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = OPERATE_NONE;
    m_CurRcLabel = pIndex;
    //>@执行STEP0中的内容
    OperateStep0();
}


QList<QAction*> DesignVideo::GetActionList()
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

void DesignVideo::ExecAction(QAction *pAction)
{
    updateEffect(pAction->text().toInt());
}
