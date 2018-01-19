#ifndef DIAGRAMFRAME_H
#define DIAGRAMFRAME_H

#include "global.h"

class DiagramScene;

class DiagramFrame : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit DiagramFrame(QGraphicsItem *parent = 0);

    void InitFrame(COM_INFO* pComInfo);
    void DrawFrame();

    COM_OPERTATE GetCursorShape(QPointF Point);
    void ReleaseCursorShape();

    void UpdateFrame(QGraphicsSceneMouseEvent *event);
    void UpdateCoordinate();

    void setColor(const QColor &color){ m_Color = color; }

    QRectF AddGroup(QList<COM_INFO*> Child);
    void DestoryGroup(QList<COM_INFO*> Child);

    void RestoreParent();
    void ReleaseParent();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals:
    void updateControl();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
    QMutex                             *m_Mutex;
    bool                                m_SelectFlag;

    QGraphicsItemGroup                 *m_ItemGroup;
    COM_INFO                           *m_CurCom;
    COM_OPERTATE                        m_SavedComOperate;

    QColor                              m_Color;

    QList<QRectF>                       m_ChildRectList;
    QRectF                              m_ChildGroupRect;

    QPointF                             m_ChildParentDiff;  //>@×Ó¸¸¿Ø¼þµÄÆ«ÒÆ
    QPointF                             m_SavedPoint;

    QRectF                              m_CurRect;

    qreal m_LeftLimitMin,   m_LeftLimitMax,     m_CurLeft;
    qreal m_TopLimitMin,    m_TopLimitMax,      m_CurTop;
    qreal m_RightLimitMin,  m_RightLimitMax,    m_CurRight;
    qreal m_BottomLimitMin, m_BottomLimitMax,   m_CurBottom;

    qreal m_ParentLeft, m_ParentTop, m_ParentRight, m_ParentBottom;
};

class DiagramFrameItem : public QGraphicsLineItem
{
public:
    explicit DiagramFrameItem(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    void setColor(const QColor &color)
        { myColor = color; }
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void updatePosition(const QPolygonF &rect, int startIndex, int endIndex);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    qreal myValidLen;
    QColor myColor;
};

#endif // DIAGRAMFRAME_H
