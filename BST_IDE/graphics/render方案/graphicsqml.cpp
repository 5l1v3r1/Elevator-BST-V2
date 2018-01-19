#include "graphicsqml.h"

GraphicsQML::GraphicsQML(QGraphicsItem *parent, QRectF pRect) :
    GraphicsBase(parent, pRect)
{
#ifdef SURRPORT_QML
    m_QMLEngine = 0;
    m_QMLItem = 0;
#endif
}

bool GraphicsQML::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    else if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        RESOURCE_INFO tmpRc;
        int index;
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
    else
    {
        return false;
    }
    return true;
}

bool GraphicsQML::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return GraphicsBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool GraphicsQML::Start()
{
    if(GraphicsBase::Start() == false)
        return false;
    m_CurRcLabel = -1;
    updateEffect(0); //>@
    return true;
}

bool GraphicsQML::PaintEffect(QPainter *p)
{
    if(GraphicsBase::PaintEffect(p) == false)
    {
        return false;
    }
    return true;
}

void GraphicsQML::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    GraphicsBase::updateEffect(pOperate, pPara);
    switch(pOperate)
    {
        case OPERATE_CHANGE:
        {
            int index = pPara.toInt();
            //>@如果遇到0xffff，则默认播放下一张
            if(index == 0x0ffff)
                index = getNextRc(m_CurRcLabel);
            updateEffect(index);
            break;
        }
        default:
            break;
    }
}

void GraphicsQML::UnloadQML()
{
#ifdef SURRPORT_QML
    if(m_QMLItem)
    {
        m_QMLItem->deleteLater();
        m_QMLItem = 0;
    }
    if(m_QMLEngine)
    {
        m_QMLEngine->deleteLater();
        m_QMLEngine = 0;
    }
#endif
}

bool GraphicsQML::LoadQML(QString pFileName)
{
    UnloadQML();
#ifdef SURRPORT_QML
    if(m_QMLEngine == 0)
        m_QMLEngine = new QDeclarativeEngine;
    QDeclarativeComponent component(m_QMLEngine, pFileName);
    QGraphicsObject *tmpQMLObject = VAR_CAST<QGraphicsObject *>(component.create());
    if(tmpQMLObject == 0)
        return false;
    tmpQMLObject->setParentItem(this);
    tmpQMLObject->setZValue(zValue()-1);
    m_QMLItem = VAR_CAST<QDeclarativeItem*>(tmpQMLObject);
    if(m_QMLItem == 0)
        return false;
    m_QMLItem->setSize(size());
#endif
    return true;
}

void GraphicsQML::updateEffect(int pState)
{
    bool tmpStateChange = false;
    if(pState != m_CurRcLabel)
        tmpStateChange = true;

    if(tmpStateChange)
    {
        QString tmpFileName = m_EffectPath.value(pState);
        LoadQML(tmpFileName);
        m_CurRcLabel = pState;
        IDE_TRACE_STR(tmpFileName);
    }
}

DesignQML::DesignQML(QGraphicsItem *parent, QRectF pRect) :
    GraphicsQML(parent, pRect)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
    for(int i=1;i<10;i++)
    {
        m_ActionGroup->addAction(QString("%1").arg(i));
    }
    tmpDefault->setChecked(true);
}

QList<QAction*> DesignQML::GetActionList()
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

void DesignQML::ExecAction(QAction *pAction)
{
    updateEffect(pAction->text().toInt());
}
