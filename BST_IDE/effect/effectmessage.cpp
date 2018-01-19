#include "effectmessage.h"

EffectMessage::EffectMessage(QWidget *parent, QRect pRect) :
    EffectBase(parent, pRect)
{
}

bool EffectMessage::InitEffectRc(STATE_INFO* pEffect)
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
        if(pEffect->Effect.EffectType.compare(QString("Transparent"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeTransparent;
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

bool EffectMessage::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return EffectBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool EffectMessage::Start()
{
    EffectBase::Start();
    disconnect(&m_Animator, SIGNAL(finished()), 0, 0);
    connect(&m_Animator, SIGNAL(finished()), this, SLOT(hide()));
    //>@默认隐藏Message窗口
    hide();
    return true;
}

bool EffectMessage::PaintEffect(QPainter &p)
{
    if(m_AreaAnimate == 0)
    {
        Paint2D_None(p, rect());
        return false;
    }
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeTransparent:
            Paint2D_Transparent(p, rect());
            break;
        default:
            break;
    }
    return true;
}

void EffectMessage::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
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
            if(tmpPoint.y() < height()/2)
            {
                info(QString("Test Info"), 1000);
            }
            else if(tmpPoint.y() > height()/2)
            {
                warn(QString("Test Warn"), 1000);
            }
            break;
        }
        default:
            break;
    }
}

void EffectMessage::updateEffect(MSG_INFO pMsgInfo)
{
    if(pMsgInfo.mMessage.isEmpty())
    {
        m_Animator.stop();
        hide();
        return;
    }
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    m_OperateInfo[STEP0].mValid = true;
    m_OperateInfo[STEP0].mOperate = OPERATE_CHANGE;
    //>@根据延时时间修改特效参数
    AREA_ANIMATE* tmpAreaAnimate =  m_EffectGroup.value(OPERATE_CHANGE);
    if(tmpAreaAnimate)
    {
        tmpAreaAnimate->mAnimateTime = pMsgInfo.mDelay;
        tmpAreaAnimate->mFrameStart = 0;
        tmpAreaAnimate->mFrameEnd = pMsgInfo.mDelay/50;
    }
    //>@生成显示资源
    m_PixmapPointer.mLastPointer = LoadMessage(pMsgInfo.mMsgLevel, pMsgInfo.mMessage, size());
    m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer;
    //>@显示消息窗口，并执行STEP0中的内容
    if(isVisible() == false)
        show();
    OperateStep0();
}

QPixmap EffectMessage::LoadMessage(MSG_LEVEL pMsgLevel, QString pContent, QSize pSize)
{
    if(pContent.isEmpty())
        return QPixmap();
    QImage tmpImage(pSize.width(), pSize.height(), QImage::Format_ARGB32_Premultiplied);
    if(tmpImage.isNull())
        return false;
    tmpImage.fill(Qt::transparent); //>@创建透明图层
    QPainter p(&tmpImage);
    QPixmap tmpPixmap = m_EffectImage.value(pMsgLevel);
    if(tmpPixmap.isNull() == false)
    {
        p.drawPixmap(0, 0, pSize.width(), pSize.height(), tmpPixmap);
    }
    //>@计算最优的文本绘制位置
    QRect tmpRect = rect();
    QColor tmpColor;
    switch(pMsgLevel)
    {
        case LEVEL_INFO:
            tmpColor = Qt::black;
            break;
        case LEVEL_WARN:
            tmpColor = Qt::darkMagenta;
            break;
        case LEVEL_ERROR:
            tmpColor = Qt::red;
            break;
        default:
            return QPixmap();
    }
    WriteText(&p, pContent, QRect(tmpRect.x()+20,tmpRect.y()+40,tmpRect.width()-40,tmpRect.height()-80), tmpColor, 15);
    p.end();
    return QPixmap::fromImage(tmpImage);
}

void EffectMessage::info(QString pContent, int pDelay)
{
    MSG_INFO tmpMsgInfo;
    tmpMsgInfo.mMsgLevel = LEVEL_INFO;
    tmpMsgInfo.mMessage = pContent;
    tmpMsgInfo.mDelay = pDelay;
    updateEffect(tmpMsgInfo);
}

void EffectMessage::warn(QString pContent, int pDelay)
{
    MSG_INFO tmpMsgInfo;
    tmpMsgInfo.mMsgLevel = LEVEL_WARN;
    tmpMsgInfo.mMessage = pContent;
    tmpMsgInfo.mDelay = pDelay;
    updateEffect(tmpMsgInfo);
}

void EffectMessage::error(QString pContent, int pDelay)
{
    MSG_INFO tmpMsgInfo;
    tmpMsgInfo.mMsgLevel = LEVEL_ERROR;
    tmpMsgInfo.mMessage = pContent;
    tmpMsgInfo.mDelay = pDelay;
    updateEffect(tmpMsgInfo);
}
