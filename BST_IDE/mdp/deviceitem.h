#ifndef DEVICEITEM_H
#define DEVICEITEM_H

#include "global.h"
#include "physicalinc.h"
#include "logicinc.h"
#include "deviceparaform.h"

#define D_SCENESIZE       1000
#define D_TIMELINERANGE   20

#define D_PHRadius        170
#define D_LGRadius        355
#define D_DelRadius       450

#define D_PHSize          QSizeF(64,64)
#define D_LGSize          QSizeF(76,76)

#define D_InitColor     QColor(170,90,45,230)
#define D_ModifyColor   QColor(180,160,100,230)
#define D_ShadowColor   QColor(150,130,100,120)
#define D_SelectColor   QColor(100,0,0,150)

class DeviceItem;

class DeviceLine : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4 };

    DeviceLine(DeviceItem *startItem, DeviceItem *endItem, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    int type() const  { return Type; }
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setColor(const QColor &color) { myColor = color; }
    DeviceItem *startItem() const  { return myStartItem; }
    DeviceItem *endItem() const   { return myEndItem; }

    void updatePosition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,  QWidget *widget = 0);

private:
    DeviceItem *myStartItem;
    DeviceItem *myEndItem;
    QColor myColor;
    QPolygonF arrowHead;
};

class DeviceItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    DeviceItem(DEV_TYPE pType, PhysicalDevice *pPHDev, LogicDevice *pLGDev, QGraphicsItem *parent = 0);
    ~DeviceItem();

    int type() const { return UserType + mDevType; }
    void removeLine(DeviceLine *line);
    void removeLines();
    void addLine(DeviceLine *line);

    void Initialize(QPointF pPos);

    void UpdateTooltip(QString pContent);

    void setPos(const QPointF &pos);
    void setPos(qreal x, qreal y);
    void setGeometry(const QRectF &rect);
    void setGeometry(qreal x, qreal y, qreal w, qreal h);

    void setImg(QImage pImg){mImg = pImg;}
    void setFont(QFont pFont){mFont = pFont;}
    void SetColor(QColor pColor){mColor = pColor;}

    static int GetDevRadius(DEV_TYPE pType);
    void StartTimeline();
    void StopTimeline();

public slots:
    void updateAnimate(int pFrame);
    void finishAnimate();

public:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
    DEV_TYPE                mDevType;
    PhysicalDevice         *mPHDev;
    LogicDevice            *mLGDev;

    QString                 mName;

    QTimeLine               mAnimator;
    QLineF                  mTrackLine;
    QPointF                 mCirclePoint;  //>@Ô²ÐÄ
    int                     mRadius;

    QImage                  mImg;
    QFont                   mFont;
    QColor                  mColor;

    QList<DeviceLine *>     mDeviceLines;
};

#endif // DEVICEITEM_H
