#include "graphicsprogress.h"

GraphicsProgress::GraphicsProgress(QGraphicsItem *parent, QRectF pRect) :
    GraphicsBase(parent, pRect)
{
    m_FontSizeValue = 40;
}

bool GraphicsProgress::InitEffectRc(STATE_INFO* pEffect)
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
        if(pEffect->Effect.EffectType.compare(QString("Wheel"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeWheel;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Roll"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRoll;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_CHANGE, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Run"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Wheel"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeWheel;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Roll"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRoll;
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

bool GraphicsProgress::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return GraphicsBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool GraphicsProgress::Start()
{
    GraphicsBase::Start();
    m_FontSize = CalcSizeFromFontSize("Arial", m_FontSizeValue);
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    m_PixmapPointer.mCurPointer = GetPixmap(0);
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = OPERATE_RUN;
    //>@默认隐藏Message窗口
#ifndef IDE
    hide();
#endif
    return true;
}

bool GraphicsProgress::PaintEffect(QPainter *p)
{
    DrawPixmap(p, GetPixmap(BGRC), rect());
    if(m_AreaAnimate)
    {
        switch(m_AreaAnimate->mEffectType)
        {
            case EffectTypeWheel:
                Paint2D_Wheel(p, rect());
                break;
            case EffectTypeRoll:
                Paint2D_Roll(p, rect());
                break;
            default:
                Paint2D_None(p, rect());
                break;
        }
    }
    QFont font("Arial", m_FontSizeValue, QFont::DemiBold, true);
    font.setUnderline(false);
    p->save();
    p->setFont(font);
    //>@绘制文字进度
    p->translate(rect().width()/2, rect().height()/2);
    QRect tmpRect;
    if(m_ProgressValue<10)
    {
        tmpRect = QRect(-m_FontSize.width()/2,
                        -m_FontSize.height()/4,
                        m_FontSize.width(),
                        m_FontSize.height());
    }
    else
    {
        tmpRect = QRect(-m_FontSize.width(),
                        -m_FontSize.height()/2,
                        m_FontSize.width()*2,
                        m_FontSize.height()*2);
    }
    p->drawText(tmpRect, QString("%1").arg(m_ProgressValue));
    p->restore();
    return true;
}

void GraphicsProgress::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            //>@分解坐标，如果点击上半区域，则向上滚动
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < size().height()/2)
            {
                UpdateValue(1);
            }
            else if(tmpPoint.y() > size().height()/2)
            {
                UpdateValue(99);
            }
            break;
        }
        default:
            break;
    }
}

void GraphicsProgress::updateEffect(int pValue, AREA_OPERATE pOperate)
{
    if(pValue <= 0 || pValue >= 100)
    {
        Stop();
        hide();
        return;
    }
    m_ProgressValue = pValue;
    //>@执行STEP0中的内容
    if(isVisible()==false)
        show();
    OperateStep0();
}

void GraphicsProgress::UpdateValue(quint32 pValue)
{
    updateEffect(pValue, OPERATE_RUN);
}

DesignProgress::DesignProgress(QGraphicsItem *parent, QRectF pRect) :
    GraphicsProgress(parent, pRect)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
    m_ActionGroup->addAction(tr("30"));
    m_ActionGroup->addAction(tr("50"));
    m_ActionGroup->addAction(tr("70"));
    m_ActionGroup->addAction(tr("90"));
    m_ActionGroup->addAction(tr("100"));
    tmpDefault->setChecked(true);
}

QSize DesignProgress::GetRcSize(QString pRcName)
{
    int tmpKey = GetKey(pRcName);
    if(tmpKey == BGRC)
        return size().toSize();
    else if(tmpKey == 0)
        return size().toSize();
    return QSize();
}

QList<QAction*> DesignProgress::GetActionList()
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

void DesignProgress::ExecAction(QAction *pAction)
{
    UpdateValue(pAction->text().toInt());
}
