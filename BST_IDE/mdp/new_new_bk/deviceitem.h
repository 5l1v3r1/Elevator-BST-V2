#ifndef DEVICEITEM_H
#define DEVICEITEM_H

#include "global.h"
#include "physicalinc.h"
#include "logicinc.h"

#define D_SCENESIZE       1600
#define D_PHRadius        225
#define D_LGRadius        760
#define D_SubPHRadius     100

#define D_TIMELINERANGE   20

#define D_PHSize          QSizeF(64,64)
#define D_LGRadius        QSizeF(76,76)
#define D_SubPHRadius     QSizeF(48,48)

class DeviceEllipse : public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    DeviceEllipse(QGraphicsItem *parent = 0);
    ~DeviceEllipse();
};

class DeviceItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    DeviceItem(DEV_TYPE pType, PhysicalDevice *pPHDev, LogicDevice *pLGDev, QGraphicsItem *parent = 0);
    ~DeviceItem();

    int type() const { return UserType + mDevType; }

    void Initialize(QPointF pPos, QPointF pDstPos=QPointF());

    void UpdateTooltip(QString pContent);

    void setPos(const QPointF &pos);
    void setPos(qreal x, qreal y);
    void setGeometry(const QRectF &rect);
    void setGeometry(qreal x, qreal y, qreal w, qreal h);

    void setImg(QImage pImg){mImg = pImg;}
    void setFont(QFont pFont){mFont = pFont;}
    void SetColor(QColor pColor){mColor = pColor;}

    static int GetDevRadius(DEV_TYPE pType);

public slots:
    void updateAnimate(int pFrame);

private:
    QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
    QList<QPointF>          mTrackPoints;

    DEV_TYPE                mDevType;
    PhysicalDevice         *mPHDev;
    LogicDevice            *mLGDev;

    QImage                  mImg;
    QFont                   mFont;
    QColor                  mColor;
};

void DeviceItem::setPos(const QPointF &pos)
{
    QGraphicsWidget::setPos(QPointF(pos.x() - rect().width()/2, pos.y() - rect().height()/2));
}
void DeviceItem::setPos(qreal x, qreal y)
{
    setPos(QPointF(x, y));
}
void DeviceItem::setGeometry(const QRectF &rect)
{
    QRectF redressRectF(rect.left()-rect.width()/2,
                        rect.top()-rect.height()/2,
                        rect.width(),
                        rect.height());
    QGraphicsWidget::setGeometry(redressRectF);
}
void DeviceItem::setGeometry(qreal x, qreal y, qreal w, qreal h)
{
    setGeometry(QRectF(x, y, w, h));
}

#endif // DEVICEITEM_H
