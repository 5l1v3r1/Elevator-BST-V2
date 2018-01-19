#ifndef DIAGRAMFRAME_H
#define DIAGRAMFRAME_H

#include "propertytree.h"

class DiagramScene;
class GraphicsOperate;

#define D_NORMAL_COLOR      Qt::blue
#define D_HVOVER_COLOR      Qt::green
#define D_FRAMEOVER_COLOR   Qt::red

class DiagramFrame : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit DiagramFrame(QGraphicsItem *parent = 0);

    void Connect();
    void Disconnect();

    void InitFrame(GraphicsOperate *pComData);
    void DrawFrame();

    COM_OPERTATE GetCursorShape(QPointF Point);
    void ReleaseCursorShape(COM_OPERTATE pOperate);

    void UpdateFrame(QGraphicsSceneMouseEvent *event);
    void UpdateCoordinate();

    void SetFrameRect(QRectF pRect);

    QColor color() const { return m_Color; }
    void setColor(const QColor &color) { m_Color = color; update(); }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public slots:
    void slot_SceneRectChanged(QRectF pRect);

public:
    GraphicsOperate                    *m_Component;

    QMutex                             *m_Mutex;
    bool                                m_SelectFlag;
    COM_OPERTATE                        m_SavedComOperate;
    QColor                              m_Color; 
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
