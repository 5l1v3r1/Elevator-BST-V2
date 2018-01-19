#include "effectvideo.h"

EffectVideo::EffectVideo(QWidget *parent, QRect pRect, qreal pAngle):
    EffectBase(parent, pRect)
{
    m_ScreenRotate = pAngle;
    m_FilmIndex = -1;
    m_Repeat = false;
    m_Video = 0;
    updateEffect(OPERATE_INIT, QVariant());
}

EffectVideo::~EffectVideo()
{
    m_FilmIndex = -1;
    m_Repeat = false;
    updateEffect(OPERATE_RELEASE, QVariant());
}

bool EffectVideo::InitEffectRc(STATE_INFO* pEffect)
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
            tmpString = tmpRc.RcName;
            if(tmpString.startsWith("Film",Qt::CaseInsensitive) == false)
                continue;
            tmpString.remove(0, 4);
            index = tmpString.toInt();
            IDE_TRACE_INT(index);
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

bool EffectVideo::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return EffectBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool EffectVideo::Start()
{
    if(EffectBase::Start() == false)
        return false;
    m_FilmIndex = -1;
    m_Repeat = true;
    updateEffect(OPERATE_INIT, QVariant(m_FilmIndex = 0));
    return true;
}

bool EffectVideo::PaintEffect(QPainter &p)
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

void EffectVideo::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
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
            if(m_Video !=0)
                m_Video->Stop();
            break;
        }
        case OPERATE_INIT:
        {
            if(m_Video)
            {
                disconnect(m_Video,SIGNAL(sFinished()),this,SLOT(slot_finished()));
                m_Video->deleteLater();
                m_Video = 0;
            }
            m_Video = new basevideo(this);
            connect(m_Video,SIGNAL(sFinished()),this,SLOT(slot_finished()));
            IDE_TRACE_INT(m_ScreenRotate);
            m_Video->SetVideoRect(this, m_ScreenRotate);
            updateEffect(pPara.toInt());
            break;
        }
        case OPERATE_RELEASE:
        {
            if(m_Video != 0)
            {
                disconnect(m_Video,SIGNAL(sFinished()),this,SLOT(slot_finished()));
                delete m_Video;
                m_Video = 0;
            }
            break;
        }
        case OPERATE_CHANGE:
        {
            if(m_Video == 0)
                updateEffect(OPERATE_INIT, QVariant());
            updateEffect(pPara.toInt());
            break;
        }
        case OPERATE_VOICEENABLE:
            if(m_Video !=0)
                m_Video->SetVolume(pPara.toInt());
            break;
        case OPERATE_MUTE:
            if(m_Video !=0)
                m_Video->SetVolume(0);
            break;
        case OPERATE_PAUSE:
            if(m_Video !=0)
                m_Video->Pause();
            break;
        case OPERATE_RESUME:
            if(m_Video !=0)
                m_Video->Resume();
            break;
        case OPERATE_REPEAT:
            m_Repeat = pPara.toBool();
            break;
        default:
            break;
    }
}

void EffectVideo::updateEffect(int pIndex)
{
    bool tmpIndexChange = false;
    if(pIndex != m_FilmIndex)
        tmpIndexChange = true;
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    QString tmpVideoPath = m_EffectPath.value(pIndex);
    if(tmpIndexChange)
    {
        m_OperateInfo[STEP0].mValid = true;
        m_OperateInfo[STEP0].mOperate = OPERATE_RUN;
        m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer;
        QPixmap tmpPixmap = GenerateSpectrum(tmpVideoPath, QRect(0,0,width()*2/3,height()*2/3));
        QImage tmpImage(m_EffectPath.value(BGIMG));
        if(tmpImage.isNull() == false)
        {
            QPainter painter(&tmpImage);
            painter.drawPixmap(tmpImage.rect(), tmpPixmap, tmpPixmap.rect());
            painter.end();
            m_PixmapPointer.mCurPointer = QPixmap::fromImage(tmpImage);
        }
        else
        {
            m_PixmapPointer.mCurPointer = tmpPixmap;
        }
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = OPERATE_NONE;
    m_FilmIndex = pIndex;
    //>@执行STEP0中的内容
    OperateStep0();
    if(m_Video)
        m_Video->Play(tmpVideoPath);
}

void EffectVideo::slot_finished()
{
    if(m_Repeat)
    {
        updateEffect(m_FilmIndex);
    }
}


