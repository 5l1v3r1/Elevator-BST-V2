#ifndef DESIGNWEBVIEW_H
#define DESIGNWEBVIEW_H

#include "designbase.h"
#include <QGraphicsWebView>

class DesignWebview : public QGraphicsWebView, public DesignRC
{
    Q_OBJECT
public:
    explicit DesignWebview(QGraphicsItem *parent = 0, QRectF pRect=QRectF());
    ~DesignWebview() {}

    QRectF boundingRect() const;
    QPainterPath shape() const;

    bool InitEffectRc(STATE_INFO* pEffect);
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool Start();

    QSize GetRcSize(QString pRcName) { return QSize(); }
    QList<QAction*> GetActionList();
    void ExecAction(QAction *pAction);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void updateEffect(int pIndex);

public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    void updateEffect(AREA_OPERATE pOperate, quint32 pPara);
    inline void ActionTrigger(QAction *pAction)
    {
        if(pAction)
        {
            ExecAction(pAction);
        }
    }
signals:
    void MouseRelease(QPointF Pos);
public:
    QActionGroup*   m_ActionGroup;
    QRectF          m_Rect;         //>@«¯”Ú¥Û–°
    int             m_CurRcLabel;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
};

#endif // GRAPHICSWEBVIEW_H
