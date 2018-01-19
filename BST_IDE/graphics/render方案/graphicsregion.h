#ifndef GRAPHICSREGION_H
#define GRAPHICSREGION_H

#include "graphicsbase.h"

class GraphicsRegion : public GraphicsBase
{
    Q_OBJECT
public:
    GraphicsRegion(QGraphicsItem *parent, QRectF pRect);

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
    void UpdateCom(int pFrame);

public:
    int            m_RegionState;
};

//>@################################################################################
//>@################################################################################

class GraphicsDesktop : public GraphicsRegion
{
    Q_OBJECT
public:
    GraphicsDesktop(QGraphicsItem *parent, QRectF pRect);
    virtual bool PaintEffect(QPainter *p) { return GraphicsRegion::PaintEffect(p); }
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
};

class DesignDesktop : public GraphicsDesktop
{
    Q_OBJECT
public:
    DesignDesktop(QGraphicsItem *parent, QRectF pRect) :
        GraphicsDesktop(parent, pRect)
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
        GraphicsDesktop::PaintEffect(p);
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

class GraphicsButtonOperate : public GraphicsRegion
{
    Q_OBJECT
public:
    GraphicsButtonOperate(QGraphicsItem *parent, QRectF pRect);
    virtual bool PaintEffect(QPainter *p) { return GraphicsRegion::PaintEffect(p); }
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
};

class DesignButtonOperate : public GraphicsButtonOperate
{
    Q_OBJECT
public:
    DesignButtonOperate(QGraphicsItem *parent, QRectF pRect) :
        GraphicsButtonOperate(parent, pRect)
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
        GraphicsButtonOperate::PaintEffect(p);
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

class GraphicsFuncInfo : public GraphicsRegion
{
    Q_OBJECT
public:
    GraphicsFuncInfo(QGraphicsItem *parent, QRectF pRect);
    virtual bool PaintEffect(QPainter *p) { return GraphicsRegion::PaintEffect(p); }
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
};

class DesignFuncInfo : public GraphicsFuncInfo
{
    Q_OBJECT
public:
    DesignFuncInfo(QGraphicsItem *parent, QRectF pRect) :
        GraphicsFuncInfo(parent, pRect)
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
        GraphicsFuncInfo::PaintEffect(p);
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

class GraphicsIntegratedInfo : public GraphicsRegion
{
    Q_OBJECT
public:
    GraphicsIntegratedInfo(QGraphicsItem *parent, QRectF pRect);
    virtual bool PaintEffect(QPainter *p) { return GraphicsRegion::PaintEffect(p); }
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
};

class DesignIntegratedInfo : public GraphicsIntegratedInfo
{
    Q_OBJECT
public:
    DesignIntegratedInfo(QGraphicsItem *parent, QRectF pRect) :
        GraphicsIntegratedInfo(parent, pRect)
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
        GraphicsIntegratedInfo::PaintEffect(p);
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

class GraphicsMultimedia : public GraphicsRegion
{
    Q_OBJECT
public:
    GraphicsMultimedia(QGraphicsItem *parent, QRectF pRect);
    virtual bool PaintEffect(QPainter *p) { return GraphicsRegion::PaintEffect(p); }
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
};

class DesignMultimedia : public GraphicsMultimedia
{
    Q_OBJECT
public:
    DesignMultimedia(QGraphicsItem *parent, QRectF pRect) :
        GraphicsMultimedia(parent, pRect)
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
        GraphicsMultimedia::PaintEffect(p);
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

class GraphicsTextBulletin : public GraphicsRegion
{
    Q_OBJECT
public:
    GraphicsTextBulletin(QGraphicsItem *parent, QRectF pRect);
    virtual bool PaintEffect(QPainter *p) { return GraphicsRegion::PaintEffect(p); }
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
};

class DesignTextBulletin : public GraphicsTextBulletin
{
    Q_OBJECT
public:
    DesignTextBulletin(QGraphicsItem *parent, QRectF pRect) :
        GraphicsTextBulletin(parent, pRect)
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
        GraphicsTextBulletin::PaintEffect(p);
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

class GraphicsImage : public GraphicsRegion
{
    Q_OBJECT
public:
    GraphicsImage(QGraphicsItem *parent, QRectF pRect);
    virtual bool PaintEffect(QPainter *p) { return GraphicsRegion::PaintEffect(p); }
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
};

class DesignImage : public GraphicsImage
{
    Q_OBJECT
public:
    DesignImage(QGraphicsItem *parent, QRectF pRect) :
        GraphicsImage(parent, pRect)
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
        GraphicsImage::PaintEffect(p);
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

class GraphicsLogo : public GraphicsRegion
{
    Q_OBJECT
public:
    GraphicsLogo(QGraphicsItem *parent, QRectF pRect);
    virtual bool PaintEffect(QPainter *p) { return GraphicsRegion::PaintEffect(p); }
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
};

class DesignLogo : public GraphicsLogo
{
    Q_OBJECT
public:
    DesignLogo(QGraphicsItem *parent, QRectF pRect) :
        GraphicsLogo(parent, pRect)
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
        GraphicsLogo::PaintEffect(p);
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

class GraphicsWeather : public GraphicsRegion
{
    Q_OBJECT
public:
    GraphicsWeather(QGraphicsItem *parent, QRectF pRect);
    virtual bool PaintEffect(QPainter *p) { return GraphicsRegion::PaintEffect(p); }
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
};

class DesignWeather : public GraphicsWeather
{
    Q_OBJECT
public:
    DesignWeather(QGraphicsItem *parent, QRectF pRect) :
        GraphicsWeather(parent, pRect)
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
        GraphicsWeather::PaintEffect(p);
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

class GraphicsNamePlate : public GraphicsRegion
{
    Q_OBJECT
public:
    GraphicsNamePlate(QGraphicsItem *parent, QRectF pRect);
    virtual bool PaintEffect(QPainter *p) { return GraphicsRegion::PaintEffect(p); }
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
};

class DesignNamePlate : public GraphicsNamePlate
{
    Q_OBJECT
public:
    DesignNamePlate(QGraphicsItem *parent, QRectF pRect) :
        GraphicsNamePlate(parent, pRect)
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
        GraphicsNamePlate::PaintEffect(p);
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


#endif // GraphicsREGION_H
