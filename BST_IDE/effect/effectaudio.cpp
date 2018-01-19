#include "effectaudio.h"

EffectAudio::EffectAudio(QWidget *parent, QRect pRect):
    EffectBase(parent, pRect)
{
    m_CurRcLabel = -1;
    m_Repeat = false;
    m_BaseAudio = 0;
    m_BaseAudio = new baseaudio(this);
    connect(m_BaseAudio,SIGNAL(sFinished()),this,SLOT(slot_finished()));
}

EffectAudio::~EffectAudio()
{
    m_CurRcLabel = -1;
    m_Repeat = false;
    if(m_BaseAudio != 0)
    {
        disconnect(m_BaseAudio,SIGNAL(sFinished()),0,0);
        delete m_BaseAudio;
        m_BaseAudio = 0;
    }
}

bool EffectAudio::InitEffectRc(STATE_INFO* pEffect)
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
            tmpString = tmpRc.RcName;
            if(tmpString.startsWith("Audio",Qt::CaseInsensitive) == false)
                continue;
            tmpString.remove(0, 5);
            IDE_TRACE_STR(tmpString);
            index = tmpString.toInt();
            if(LoadPath(index, tmpRc.RcFile) == false)
            {
                IDE_TRACE_STR(tmpRc.RcFile);
            }
        }
    }
    else if(tmpString.compare(QString("Run"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Spectrum"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSpectrum;
            RESOURCE_INFO tmpRc;
            int index;
            int count = pEffect->Effect.RcFiles.count();
            for(int i=0;i<count;i++)
            {
                tmpRc = pEffect->Effect.RcFiles.at(i);
                tmpString = tmpRc.RcName;
                index = GetKey(tmpString);
                IDE_TRACE_INT(index);
                if(LoadPath(index, tmpRc.RcFile) == false)
                {
                    IDE_TRACE_STR(tmpRc.RcFile);
                }
            }
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

bool EffectAudio::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return EffectBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool EffectAudio::Start()
{
    if(EffectBase::Start() == false)
        return false;
    m_CurRcLabel = -1;
    m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer = QPixmap(":/Background/rc/Background/Audio.png").scaled(size());
    if(m_EffectPath.keys().count()>0)
        updateEffect(OPERATE_INIT, QVariant(m_EffectPath.keys().first()));
    return true;
}

bool EffectAudio::PaintEffect(QPainter &p)
{
    if(m_AreaAnimate == 0)
    {
        Paint2D_None(p, rect());
        return false;
    }
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeSpectrum:
            Paint2D_Transparent(p, rect());
            break;
        default:
            Paint2D_None(p, rect());
            break;
    }
    return true;
}

void EffectAudio::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
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
            if(m_BaseAudio !=0)
                m_BaseAudio->Stop();
            break;
        }
        case OPERATE_KEYPRESS:
        {
            //>@分解坐标，如果点击上半区域，则向上滚动
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < height()/3)
            {
                updateEffect(0);
            }
            else if(tmpPoint.y() > height()/3 && tmpPoint.y() < height()*2/3)
            {
                updateEffect(1);
            }
            else if(tmpPoint.y() > height()*2/3)
            {
                updateEffect(2);
            }
            break;
        }
        case OPERATE_INIT:
        case OPERATE_CHANGE:
        {
            updateEffect(pPara.toInt());
            break;
        }
        case OPERATE_VOICEENABLE:
            if(m_BaseAudio !=0)
                m_BaseAudio->SetVoiceEnable(true);
            break;
        case OPERATE_MUTE:
            if(m_BaseAudio !=0)
                m_BaseAudio->SetVoiceEnable(false);
            break;
        case OPERATE_PAUSE:
            if(m_BaseAudio !=0)
                m_BaseAudio->Pause();
            break;
        case OPERATE_RESUME:
            if(m_BaseAudio !=0)
                m_BaseAudio->Resume();
            break;
        case OPERATE_REPEAT:
            m_Repeat = pPara.toBool();
            break;
        default:
            break;
    }
}

void EffectAudio::updateEffect(int pIndex)
{
    bool tmpIndexChange = false;
    if(pIndex != m_CurRcLabel)
        tmpIndexChange = true;
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    QString tmpAudioPath = m_EffectPath.value(pIndex);
    if(tmpIndexChange)
    {
        m_OperateInfo[STEP0].mValid = true;
        m_OperateInfo[STEP0].mOperate = OPERATE_RUN;
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = OPERATE_NONE;
    m_CurRcLabel = pIndex;
    //>@执行STEP0中的内容
    OperateStep0();
    m_BaseAudio->Play(tmpAudioPath);
}

void EffectAudio::slot_finished()
{
    if(m_Repeat)
    {
        updateEffect(m_CurRcLabel);
    }
}


