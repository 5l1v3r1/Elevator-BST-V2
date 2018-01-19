#include "graphicsprogress.h"

GraphicsProgress::GraphicsProgress(QGraphicsItem *parent) :
    GraphicsOperate(parent)
{
    m_FontSizeValue = 40;
}

bool GraphicsProgress::Start()
{
    GraphicsOperate::Start();
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
                break;
            case EffectTypeRoll:
                break;
            default:
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

DesignProgress::DesignProgress(QGraphicsItem *parent) :
    GraphicsProgress(parent)
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
