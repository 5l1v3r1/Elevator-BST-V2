#include "deviceitem.h"
#include "devicescene.h"

DeviceItem::DeviceItem(DeviceScene* scene, QGraphicsWidget *parent):
    QGraphicsWidget(parent),
    mScene(0),
    mImg(QString()),
    mFont(QFont()),
    mColor(QColor())
{
    mScene = scene;
    setAcceptHoverEvents(true);
}

DeviceItem::~DeviceItem(){}

QRectF DeviceItem::boundingRect() const
{
    qreal adjust = 2.0;
    QString name = data(0).toString();
    QFontMetrics fm(mFont);
    int pixelsWide = fm.width(name);
    int pixelsHigh = fm.height();
    int iconWidth = rect().width();
    int left = {(iconWidth>=pixelsWide)?
                rect().left():
                rect().left()-(pixelsWide-rect().width()+3)/2};
    int width = {(iconWidth>=pixelsWide)?
                 rect().width():
                 pixelsWide+6};
    QRectF tragRectF(left-adjust,
                    rect().top()-adjust,
                    width+2*adjust,
                    rect().height()+pixelsHigh+8+2*adjust);
    return tragRectF;
}

void DeviceItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    UpdateItem(painter);
    UpdateItemName(painter);
//        painter->drawRect(boundingRect());
}
