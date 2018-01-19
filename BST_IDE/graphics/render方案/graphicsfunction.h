#ifndef GRAPHICSFUNCTION_H
#define GRAPHICSFUNCTION_H

#include "graphicsbase.h"

class GraphicsFunction : public GraphicsBase
{
    Q_OBJECT
public:
    GraphicsFunction(QGraphicsItem *parent, QRectF pRect);

    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    virtual bool PaintEffect(QPainter *p);
    void updateEffect(int pState, AREA_OPERATE pOperate);
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);

public:
    int            m_RegionState;
};

class DesignFunction : public GraphicsFunction
{
    Q_OBJECT
public:
    DesignFunction(QGraphicsItem *parent, QRectF pRect) :
        GraphicsFunction(parent, pRect)
    {
        QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
        for(int i=1;i<10;i++)
        {
            m_ActionGroup->addAction(QString("%1").arg(i));
        }
        tmpDefault->setChecked(true);
    }

    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList()
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

    bool PaintEffect(QPainter *p)
    {
        GraphicsFunction::PaintEffect(p);
        //>@绘制边框
        QRectF tmpRect = geometry();
        qreal Width = tmpRect.width();
        qreal Height = tmpRect.height();
        p->translate(Width / 2, Height / 2);
        p->setPen(Qt::DashDotDotLine);
        p->setPen(QColor(10,10,255));
        p->drawRect(QRect(-Width/2, -Height/2, Width, Height));
        p->drawRect(QRect(-Width/2+1, -Height/2+1, Width-2, Height-2));
        return true;
    }

public slots:
    void ExecAction(QAction *pAction)
    {
        updateEffect(pAction->text().toInt(), OPERATE_STEADY);
    }
};
#endif
