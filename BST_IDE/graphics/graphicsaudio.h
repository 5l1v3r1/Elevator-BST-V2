#ifndef GraphicsAUDIO_H
#define GraphicsAUDIO_H

#include "graphicsoperate.h"
#include "baseaudio.h"

#ifdef IDE
    #define ComAudio  DesignAudio
#elif DISPLAY
    #define ComAudio  GraphicsAudio
#endif

class GraphicsAudio : public GraphicsOperate
{
    Q_OBJECT
public:
    GraphicsAudio(QGraphicsItem *parent);
    ~GraphicsAudio();
    bool Start();
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
    void UpdateCom(int pFrame) { GraphicsComponent::UpdateCom(pFrame); }
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    void slot_Finished();
public:
    virtual void updateEffect(int pIndex);
public:
    int             m_CurRcLabel;
    QList<int>      m_RcList;
    baseaudio      *m_BaseAudio;
    REPEAT_MODE     m_RepeatMode;
};

class DesignAudio : public GraphicsAudio
{
    Q_OBJECT
public:
    DesignAudio(QGraphicsItem *parent);

    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();

    bool PaintEffect(QPainter *p)
    {
        GraphicsAudio::PaintEffect(p);
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

#endif // GraphicsAUDIO_H
