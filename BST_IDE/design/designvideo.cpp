#include "designvideo.h"

DesignVideo::DesignVideo(QGraphicsItem *parent, QRectF pRect):
    DesignBase(parent, pRect)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
    for(int i=1;i<10;i++)
    {
        m_ActionGroup->addAction(QString("%1").arg(i));
    }
    tmpDefault->setChecked(true);
}

DesignVideo::~DesignVideo()
{
}

bool DesignVideo::InitEffectRc(STATE_INFO* pEffect)
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
            if(tmpString.startsWith("Rc",Qt::CaseInsensitive) == false)
                continue;
            tmpString.remove(0, 2);
            IDE_TRACE_STR(tmpString);
            index = tmpString.toInt();
            if(LoadPath(index, tmpRc.RcFile) == false)
            {
                IDE_TRACE_STR(tmpRc.RcFile);
            }
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

bool DesignVideo::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return DesignBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool DesignVideo::Start()
{
    if(DesignBase::Start() == false)
        return false;
    m_FilmIndex = -1;
    m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer = GetPixmap(BGIMG);
    updateEffect(0);
    return true;
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
    updateEffect(OPERATE_CHANGE, QVariant(pAction->text().toInt()));
}

bool DesignVideo::PaintEffect(QPainter &p)
{
    if(DesignBase::PaintEffect(&p) == false)
    {
        return false;
    }
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeZoom:
            Paint2D_Zoom(&p, rect());
            break;
        case EffectTypeBlink:
            Paint2D_Blink(&p, rect());
            break;
        default:
            Paint2D_None(&p, rect());
            break;
    }
    return true;
}

void DesignVideo::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_CHANGE:
        {
            updateEffect(pPara.toInt());
            break;
        }
        default:
            break;
    }
}

void DesignVideo::updateEffect(int pIndex)
{
    bool tmpIndexChange = false;
    if(pIndex != m_FilmIndex)
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
    m_FilmIndex = pIndex;
    //>@执行STEP0中的内容
    OperateStep0();
}



