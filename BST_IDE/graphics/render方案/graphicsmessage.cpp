#include "graphicsmessage.h"

GraphicsMessage::GraphicsMessage(QGraphicsItem *parent, QRectF pRect) :
    GraphicsBase(parent, pRect)
{
}

bool GraphicsMessage::InitEffectRc(STATE_INFO* pEffect)
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
            if(LoadPath(index, tmpRc.RcFile) == false)
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

bool GraphicsMessage::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return GraphicsBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool GraphicsMessage::Start()
{
    GraphicsBase::Start();
    connect(&m_Animator, SIGNAL(finished()), this, SLOT(slot_Finished()));
    //>@默认隐藏Message窗口
#ifndef IDE
    hide();
#endif
    return true;
}

void GraphicsMessage::slot_Finished()
{
    hide();
}

bool GraphicsMessage::PaintEffect(QPainter *p)
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

void GraphicsMessage::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            //>@分解坐标，如果点击上半区域，则向上滚动
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < size().height()/2)
            {
                info(QString("Test Info"), 1000);
            }
            else if(tmpPoint.y() > size().height()/2)
            {
                warn(QString("Test Warn"), 1000);
            }
            break;
        }
        default:
            break;
    }
}

void GraphicsMessage::updateEffect(MSG_INFO pMsgInfo)
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

    //>@
    m_MsgInfo.mMsgLevel = pMsgInfo.mMsgLevel;
    m_MsgInfo.mMessage = pMsgInfo.mMessage;
    m_MsgInfo.mDelay = pMsgInfo.mDelay;

    OperateStep0();
}

QPixmap GraphicsMessage::LoadMessage(MSG_LEVEL pMsgLevel, QString pContent, QSizeF pSize)
{
    if(pContent.isEmpty())
        return QPixmap();
    QImage tmpImage(pSize.width(), pSize.height(), QImage::Format_ARGB32_Premultiplied);
    if(tmpImage.isNull())
        return false;
    tmpImage.fill(Qt::transparent); //>@创建透明图层
    QPainter p(&tmpImage);
    QPixmap tmpPixmap = GetPixmap(pMsgLevel);
    if(tmpPixmap.isNull() == false)
    {
        p.drawPixmap(0, 0, pSize.width(), pSize.height(), tmpPixmap);
    }
    //>@计算最优的文本绘制位置
    QRectF tmpRect = rect();
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
    WriteText(&p, pContent, QRect(tmpRect.x()+20,tmpRect.y()+40,tmpRect.width()-40,tmpRect.height()-80), QString("Arial"), tmpColor, 15);
    p.end();
    return QPixmap::fromImage(tmpImage);
}

void GraphicsMessage::info(QString pContent, int pDelay)
{
    MSG_INFO tmpMsgInfo;
    tmpMsgInfo.mMsgLevel = LEVEL_INFO;
    tmpMsgInfo.mMessage = pContent;
    tmpMsgInfo.mDelay = pDelay;
    updateEffect(tmpMsgInfo);
}

void GraphicsMessage::warn(QString pContent, int pDelay)
{
    MSG_INFO tmpMsgInfo;
    tmpMsgInfo.mMsgLevel = LEVEL_WARN;
    tmpMsgInfo.mMessage = pContent;
    tmpMsgInfo.mDelay = pDelay;
    updateEffect(tmpMsgInfo);
}

void GraphicsMessage::error(QString pContent, int pDelay)
{
    MSG_INFO tmpMsgInfo;
    tmpMsgInfo.mMsgLevel = LEVEL_ERROR;
    tmpMsgInfo.mMessage = pContent;
    tmpMsgInfo.mDelay = pDelay;
    updateEffect(tmpMsgInfo);
}


DesignMessage::DesignMessage(QGraphicsItem *parent, QRectF pRect) :
    GraphicsMessage(parent, pRect)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("INFO"));
    m_ActionGroup->addAction(tr("WARN"));
    m_ActionGroup->addAction(tr("ERROR"));
    tmpDefault->setChecked(true);
}

QList<QAction*> DesignMessage::GetActionList()
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
            tmpAction->setEnabled(true);
        }
    }
    return tmpList;
}

MSG_LEVEL GetMsgLevel(QString pLevel)
{
    if(pLevel.isEmpty())
        return LEVEL_INVALID;
    if(pLevel.compare("INFO") == 0)
    {
        return LEVEL_INFO;
    }
    else if(pLevel.compare("WARN") == 0)
    {
        return LEVEL_WARN;
    }
    else if(pLevel.compare("ERROR") == 0)
    {
        return LEVEL_ERROR;
    }
    return LEVEL_INVALID;
}

void DesignMessage::ExecAction(QAction *pAction)
{
    updateEffect(OPERATE_CHANGE, QVariant(GetMsgLevel(pAction->text())));
}

