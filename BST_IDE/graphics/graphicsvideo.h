#ifndef GraphicsVIDEO_H
#define GraphicsVIDEO_H

#include "graphicsoperate.h"
#include "basevideo.h"

class GraphicsVideo : public GraphicsOperate
{
    Q_OBJECT
public:
    GraphicsVideo(QGraphicsItem *parent, qreal pAngle=0);
    ~GraphicsVideo();
    bool Start();
    virtual void updateEffect(int pIndex);
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }

    void PlayVideo(QString pPath);

public slots:
    virtual void ExecAction(QAction *pAction) {}
    void UpdateCom(int pFrame) { GraphicsComponent::UpdateCom(pFrame); }
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    void slot_Finished();
public:
    int             m_CurRcLabel;
    REPEAT_MODE     m_RepeatMode;
    basevideo      *m_Video;
    qreal           m_ScreenRotate;
};

class DesignVideo : public GraphicsVideo
{
    Q_OBJECT
public:
    DesignVideo(QGraphicsItem *parent);

    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();

    bool PaintEffect(QPainter *p)
    {
        GraphicsVideo::PaintEffect(p);
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

    void updateEffect(int pIndex);
public slots:
    void ExecAction(QAction *pAction);
};

#endif // EFFECTVIDEO_H
