#include "designqml.h"

DesignQML::DesignQML(QGraphicsItem *parent, QRectF pRect) :
    DesignBase(parent, pRect)
{
    m_QMLEngine = 0;
    m_QMLItem = 0;
    QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
    for(int i=1;i<10;i++)
    {
        m_ActionGroup->addAction(QString("%1").arg(i));
    }
    tmpDefault->setChecked(true);
}

bool DesignQML::InitEffectRc(STATE_INFO* pEffect)
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
    else
    {
        return false;
    }
    return true;
}

bool DesignQML::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return DesignBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool DesignQML::Start()
{
    if(DesignBase::Start() == false)
        return false;
    m_CurRcLabel = -1;
    updateEffect(0);
    return true;
}

bool DesignQML::PaintEffect(QPainter *p)
{
    if(DesignBase::PaintEffect(p) == false)
    {
        return false;
    }
    return true;
}

void DesignQML::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    updateEffect(pPara.toInt());
}

void DesignQML::UnloadQML()
{
    if(m_QMLItem)
    {
        m_QMLItem->deleteLater();
        m_QMLItem = 0;
    }
}

bool DesignQML::LoadQML(QString pFileName)
{
    UnloadQML();
    if(m_QMLEngine == 0)
        m_QMLEngine = new QDeclarativeEngine;
    QDeclarativeComponent component(m_QMLEngine, pFileName);
    QGraphicsObject *tmpQMLObject = qobject_cast<QGraphicsObject *>(component.create());
    if(tmpQMLObject == 0)
        return false;
    tmpQMLObject->setParentItem(this);
    tmpQMLObject->setZValue(zValue()-1);
    m_QMLItem = qobject_cast<QDeclarativeItem*>(tmpQMLObject);
    if(m_QMLItem == 0)
        return false;
    m_QMLItem->setSize(size());
    return true;
}

void DesignQML::updateEffect(int pState)
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

