#include "graphicsaudio.h"

GraphicsAudio::GraphicsAudio(QGraphicsItem *parent, QRectF pRect):
    GraphicsBase(parent, pRect)
{
    m_CurRcLabel = -1;
    m_BaseAudio = 0;
    m_BaseAudio = new baseaudio(this);
    connect(m_BaseAudio,SIGNAL(sFinished()),this,SLOT(slot_Finished()));
}

GraphicsAudio::~GraphicsAudio()
{
    m_CurRcLabel = -1;
    if(m_BaseAudio != 0)
    {
        disconnect(m_BaseAudio,SIGNAL(sFinished()),0,0);
        delete m_BaseAudio;
        m_BaseAudio = 0;
    }
}

bool GraphicsAudio::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        RESOURCE_INFO tmpRc;
        int index;
        int count = pEffect->Effect.RcFiles.count();
        for(int i=0;i<count;i++)
        {
            tmpRc = pEffect->Effect.RcFiles.at(i);
            index = GetKey(tmpRc.RcName);
            IDE_TRACE_INT(index);
            LoadPath(index, tmpRc.RcFile);
        }
    }
    else if(tmpString.compare(QString("Change"), Qt::CaseInsensitive) == 0)
    {
        //>@当有音频播放时，才显示播放的音频信息
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Zoom"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeZoom;
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
    else
    {
        return false;
    }
    return true;
}

bool GraphicsAudio::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return GraphicsBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool GraphicsAudio::Start()
{
    if(GraphicsBase::Start() == false)
        return false;
    m_CurRcLabel = INVALIDRC;
    m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer = GetPixmap(BGRC);
    UpdateCom(0);
    return true;
}

bool GraphicsAudio::PaintEffect(QPainter *p)
{
    if(GraphicsBase::PaintEffect(p) == false)
    {
        return false;
    }
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeZoom:
            Paint2D_Zoom(p, rect());
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

void GraphicsAudio::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    if(pOperate == OPERATE_VOCAL)
    {
        if(m_BaseAudio !=0)
            m_BaseAudio->SetVoiceEnable(true);
    }
    else if(pOperate == OPERATE_MUTE)
    {
        if(m_BaseAudio !=0)
            m_BaseAudio->SetVoiceEnable(false);
    }
    else if(pOperate == OPERATE_PAUSE)
    {
        if(m_BaseAudio !=0)
            m_BaseAudio->Pause();
    }
    else if(pOperate == OPERATE_RESUME)
    {
        if(m_BaseAudio !=0)
            m_BaseAudio->Resume();
    }
    else if(pOperate == OPERATE_REPEAT)
    {
        m_RepeatMode = GetRepeatMode(pPara.toString());
    }
    else
    {
        GraphicsBase::updateEffect(pOperate, pPara);
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

void GraphicsAudio::updateEffect(int pIndex)
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

    //>@play
    if(m_BaseAudio == 0)
        return;
    if(m_RepeatMode == MODE_REPEATONE)
        m_BaseAudio->Play(m_EffectPath.value(pIndex), QString(), true, false);
    else
        m_BaseAudio->Play(m_EffectPath.value(pIndex), QString(), false, false);

    //>@执行STEP0中的内容
    OperateStep0();
}

void GraphicsAudio::slot_Finished()
{
    if(m_RepeatMode == MODE_REPEATALL)
    {
        updateEffect(getNextRc(m_CurRcLabel));
    }
}

DesignAudio::DesignAudio(QGraphicsItem *parent, QRectF pRect):
    GraphicsAudio(parent, pRect)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
    for(int i=1;i<20;i++)
    {
        m_ActionGroup->addAction(QString("%1").arg(i));
    }
    tmpDefault->setChecked(true);
}

void DesignAudio::updateEffect(int pIndex)
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

QList<QAction*> DesignAudio::GetActionList()
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

void DesignAudio::ExecAction(QAction *pAction)
{
    updateEffect(pAction->text().toInt());
}

