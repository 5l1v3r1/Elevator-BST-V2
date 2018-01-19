#ifndef DEVICEITEM_H
#define DEVICEITEM_H

#include "global.h"
#include "physicalinc.h"
#include "logicinc.h"

class DeviceEllipse : public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    DeviceEllipse(QGraphicsItem *parent = 0);
    ~DeviceEllipse();

    void setHovered(bool pFlag, QColor pColor)
    {
        if(pFlag)
            setPen(QPen(QBrush(pColor), 4, Qt::SolidLine));
        else
            setPen(QPen(QBrush(pColor), 1, Qt::DashDotDotLine));
    }
};

class DeviceItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    DeviceItem(PhysicalDevice *pPHDev, QGraphicsItem *parent = 0);
    DeviceItem(LogicDevice *pLGDev, QGraphicsItem *parent = 0);
    ~DeviceItem();

    int type() const { return UserType + mType; }

    void setFinalScale(qreal factor);
    void setDeleteAfterAnimation(bool deleteAfter);

    void calculateForces();

    bool advance();
    QRectF boundingRect() const;

    static qreal getRadiusForDevType(DEV_TYPE pType);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private Q_SLOTS:
    void stateChanged();
    void newDeviceAdded();

public:
    QGraphicsTextItem *text;
    QGraphicsPixmapItem *icon;

    qreal finalOpacity;
    qreal finalScale;
    qreal currentScale;

    QPointF newPos;

    bool deleteAfterAnimation;

    union _DEVICE{
        PhysicalDevice *mPHDev;
        LogicDevice *mLGDev;
    }mDevice;

    DEV_TYPE    mType;
};

#endif // DEVICEITEM_H
