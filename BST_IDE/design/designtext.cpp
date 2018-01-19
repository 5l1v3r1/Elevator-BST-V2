#include "designtext.h"

DesignText::DesignText(QGraphicsItem *parent, QRectF pRect):
    DesignBase(parent, pRect)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
    for(int i=1;i<10;i++)
    {
        m_ActionGroup->addAction(QString("%1").arg(i));
    }
    tmpDefault->setChecked(true);
}

bool DesignText::InitEffectRc(STATE_INFO* pEffect)
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
            if(tmpString.startsWith("Pic",Qt::CaseInsensitive) == false)
                continue;
            tmpString.remove(0, 3);
            index = tmpString.toInt();
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
        if(pEffect->Effect.EffectType.compare(QString("Zoom"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeZoom;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Transparent"), Qt::CaseInsensitive) == 0)
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
    else if(tmpString.compare(QString("Steady"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Roll"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRoll;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_STEADY, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool DesignText::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return DesignBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool DesignText::Start()
{
    if(DesignBase::Start() == false)
        return false;
    m_TextState = -1;
    int tmpKey = m_EffectPath.keys().first();
    m_PixmapPointer.mLastPointer = GetPixmap(tmpKey);
    m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer;
    updateEffect(tmpKey, OPERATE_STEADY);
    return true;
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

bool DesignText::PaintEffect(QPainter *p)
{
    if(DesignBase::PaintEffect(p) == false)
        return false;
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeRoll:
            Paint2D_TextRoll(p, rect());
            break;
        case EffectTypeZoom:
            Paint2D_Zoom(p, rect());
            break;
        case EffectTypeTransparent:
            Paint2D_Transparent(p, rect());
            break;
        default:
            Paint2D_None(p, rect());
            break;
    }
}

void DesignText::Paint2D_TextRoll(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    qreal pixhh = m_PixmapPointer.mCurPointer.height();
    qreal pixhw = m_PixmapPointer.mCurPointer.width();
    qreal tmpDiff;
    if(m_Animator.state() != QTimeLine::Running)
        return;
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
    {
        IDE_TRACE();
        return;
    }
    qreal index = m_Animator.currentFrame();
    qreal x, y;
    //>@首先将坐标原点移到当前需要绘制的区域。
    p->save();
    QTransform transform1;
    if(m_AreaAnimate->mDirection == Vertical)
    {
        tmpDiff = pixhh / FrameCount;
        y = (qreal)(tmpDiff*index);
        x = 0;
        transform1.translate(targetRect.left()+x, targetRect.top()-y);
        p->setTransform(transform1);
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRectF(x,y,pixhw-x,pixhh-y), QRectF(x,y,pixhw-x,pixhh-y));
        int tmp, tmp1, i = 1;
        while((tmp = i*pixhh-y) < hh)
        {
            IDE_TRACE_INT(tmp);
            p->resetTransform();
            x = 0;
            QTransform transform2;
            transform2.translate(targetRect.left()+x, targetRect.top()+tmp);
            tmp1 = (hh-tmp > pixhh)?(pixhh):(hh-tmp);
            p->setTransform(transform2);
            DrawPixmap(p, m_PixmapPointer.mCurPointer,
                       QRectF(0,0,pixhw-x,tmp1),
                       QRectF(0,0,pixhw-x,tmp1));
            i++;
        }
    }
    else
    {
        tmpDiff = (qreal)(pixhw) / FrameCount;
        x = (qreal)(tmpDiff*index);
        y = 0;
        transform1.translate(targetRect.left()-x, targetRect.top()+y);
        p->setTransform(transform1);
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRectF(x,y,pixhw-x,pixhh-y), QRectF(x,y,pixhw-x,pixhh-y));
        int tmp, tmp1, i = 1;
        while((tmp = i*pixhw-x) < hw)
        {
            p->resetTransform();
            y = 0;
            QTransform transform2;
            transform2.translate(targetRect.left()+tmp, targetRect.top()+y);
            tmp1 = (hw-tmp > pixhw)?(pixhw):(hw-tmp);
            p->setTransform(transform2);
            DrawPixmap(p, m_PixmapPointer.mCurPointer,
                       QRectF(0,0,tmp1,pixhh-y),
                       QRectF(0,0,tmp1,pixhh-y));
            i++;
        }
    }
    p->restore();
}

void DesignText::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            //>@分解坐标，如果点击上半区域，则向上滚动
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < size().height()/2)
            {
                updateEffect(0, OPERATE_STEADY);
            }
            else if(tmpPoint.y() > size().height()/2)
            {
                updateEffect(1, OPERATE_STEADY);
            }
            break;
        }
        case OPERATE_CHANGE:
        {
            updateEffect(pPara.toInt(), OPERATE_STEADY);
            break;
        }
        default:
            break;
    }
}

void DesignText::updateEffect(int pState, AREA_OPERATE pOperate)
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
