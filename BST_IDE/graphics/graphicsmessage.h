#ifndef GraphicsMESSAGE_H
#define GraphicsMESSAGE_H

#include "graphicsoperate.h"

#ifdef IDE
    #define ComMessage  DesignMessage
#elif DISPLAY
    #define ComMessage  GraphicsMessage
#endif

class GraphicsMessage : public GraphicsOperate
{
    Q_OBJECT
public:
    GraphicsMessage(QGraphicsItem *parent);
    bool Start();
    void updateEffect(MSG_INFO pMsgInfo);
    QPixmap LoadMessage(MSG_LEVEL pMsgLevel, QString pContent, QSizeF pSize);
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    void UpdateCom(int pFrame) { GraphicsComponent::UpdateCom(pFrame); }
    void info(QString pContent, int pDelay);
    void warn(QString pContent, int pDelay);
    void error(QString pContent, int pDelay);
    void slot_Finished();

public:
    MSG_INFO        m_MsgInfo;
};

class DesignMessage : public GraphicsMessage
{
    Q_OBJECT
public:
    DesignMessage(QGraphicsItem *parent);

    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();

    bool PaintEffect(QPainter *p)
    {
        GraphicsMessage::PaintEffect(p);
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

#endif // EFFECTMESSAGE_H
