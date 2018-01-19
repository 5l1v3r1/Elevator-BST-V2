#ifndef GRAPHICSFUNCTION_H
#define GRAPHICSFUNCTION_H

#include "graphicsoperate.h"

#ifdef IDE
    #define ComFunction  DesignFunction
#elif DISPLAY
    #define ComFunction  GraphicsFunction
#endif

class GraphicsFunction : public GraphicsOperate
{
    Q_OBJECT
public:
    GraphicsFunction(QGraphicsItem *parent);
    bool Start();
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
    DesignFunction(QGraphicsItem *parent) :
        GraphicsFunction(parent)
    {
        QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
        for(int i=1;i<10;i++)
        {
            m_ActionGroup->addAction(QString("%1").arg(i));
        }
        tmpDefault->setChecked(true);
    }
    bool Start()
    {
        if(GraphicsComponent::Start() == false)
            return false;
        updateEffect(OPERATE_CHANGE, QVariant(m_RcList.first()));
        return true;
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
