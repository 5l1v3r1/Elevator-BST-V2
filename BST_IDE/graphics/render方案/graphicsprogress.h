#ifndef GraphicsPROGRESS_H
#define GraphicsPROGRESS_H

#include "graphicsbase.h"

class GraphicsProgress : public GraphicsBase
{
    Q_OBJECT
public:
    GraphicsProgress(QGraphicsItem *parent, QRectF pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    virtual bool PaintEffect(QPainter *p);
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    void UpdateValue(quint32 pValue);
    void UpdateCom(int pFrame) { GraphicsBase::UpdateCom(pFrame); }
public:
    void updateEffect(int pValue, AREA_OPERATE pOperate);

public:
    int            m_FontSizeValue;
    int            m_ProgressValue;
    QSize          m_FontSize;
};

class DesignProgress : public GraphicsProgress
{
    Q_OBJECT
public:
    DesignProgress(QGraphicsItem *parent, QRectF pRect);

    QSize GetRcSize(QString pRcName);
    QList<QAction*> GetActionList();

    bool PaintEffect(QPainter *p)
    {
        GraphicsProgress::PaintEffect(p);
        //>@»æÖÆ±ß¿ò
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
    void ExecAction(QAction *pAction);
};

#endif // EFFECTPROGRESS_H
