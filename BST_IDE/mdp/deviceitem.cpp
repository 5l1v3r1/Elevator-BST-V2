#include "deviceitem.h"

//mSetWidget = new QWidget;
//QVBoxLayout *tmpLayout = new QVBoxLayout(mSetWidget);
//tmpLayout->addWidget(mDevice.mLGDev->m_SettingGroup);
//tmpLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));


DeviceLine::DeviceLine(DeviceItem *startItem, DeviceItem *endItem, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsLineItem(parent, scene)
{
    myStartItem = startItem;
    myEndItem = endItem;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::black;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

QRectF DeviceLine::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath DeviceLine::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}

void DeviceLine::updatePosition()
{
    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);
}

void DeviceLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (myStartItem->collidesWithItem(myEndItem))
        return;

    QPen myPen = pen();
    myPen.setColor(myColor);
    qreal arrowSize = 20;
    painter->setPen(myPen);
    painter->setBrush(myColor);

    QPointF StartItemPos = myStartItem->pos();
    StartItemPos.setX(StartItemPos.x()+myStartItem->boundingRect().width()/2);
    StartItemPos.setY(StartItemPos.y()+myStartItem->boundingRect().height()/2);
    QPointF EndItemPos = myEndItem->pos();
    EndItemPos.setX(EndItemPos.x()+myEndItem->boundingRect().width()/2);
    EndItemPos.setY(EndItemPos.y()+myEndItem->boundingRect().height()/2);
    QLineF centerLine(StartItemPos, EndItemPos);
    setLine(centerLine);
    painter->drawLine(line());

    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (PI * 2) - angle;

    QPointF arrowP1 = line().p1() + QPointF(sin(angle + PI / 3) * arrowSize,
                                    cos(angle + PI / 3) * arrowSize);
    QPointF arrowP2 = line().p1() + QPointF(sin(angle + PI - PI / 3) * arrowSize,
                                    cos(angle + PI - PI / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line().p1() << arrowP1 << arrowP2;

    painter->drawLine(line());
    painter->drawPolygon(arrowHead);
    if (isSelected())
    {
        painter->setPen(QPen(myColor, 1, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0,-8.0);
        painter->drawLine(myLine);
    }
}

DeviceItem::DeviceItem(DEV_TYPE pType, PhysicalDevice *pPHDev, LogicDevice *pLGDev, QGraphicsItem *parent) :
    QGraphicsWidget(parent)
{
    setAcceptHoverEvents(true);
    mDevType = pType;
    mPHDev = pPHDev;
    mLGDev = pLGDev;
    connect(&mAnimator, SIGNAL(frameChanged(int)), this, SLOT(updateAnimate(int)));
    connect(&mAnimator, SIGNAL(finished()), this, SLOT(finishAnimate()));
    if(mDevType == DEV_PHYSICAL && mPHDev)
    {
        mName = mPHDev->mDevName;
        //>@增加物理设备与逻辑设备的连接
        if(mLGDev)
        {
            mPHDev->mLGDevices.insert(mLGDev->mDevType, mLGDev);
            mLGDev->mPHDevices.insert(mPHDev->mDevType, mPHDev);
        }
    }
    else if(mDevType == DEV_LOGIC && mLGDev)
        mName = mLGDev->mDevName;
}

DeviceItem::~DeviceItem()
{
    removeLines();
    scene()->removeItem(this);
}

//>@如果pDstPos为空，则计算到其轨迹上的轨迹点
void DeviceItem::Initialize(QPointF pPos)
{
    mCirclePoint = QPointF(D_SCENESIZE/2, D_SCENESIZE/2);
    mRadius = GetDevRadius(mDevType);
    QLineF tmpLine(mCirclePoint, pPos);
    qreal len = tmpLine.length();
    mTrackLine.setPoints(pPos, tmpLine.pointAt((qreal)mRadius/len));
    setPos(pPos);
    StartTimeline();
}

void DeviceItem::updateAnimate(int pFrame)
{
    setPos(mTrackLine.pointAt((qreal)pFrame / (qreal)D_TIMELINERANGE));
}

void DeviceItem::finishAnimate()
{
    setPos(mTrackLine.p2());
}

void DeviceItem::UpdateTooltip(QString pContent)
{
    setToolTip(pContent);
}

void DeviceItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;
    StopTimeline();
    DeviceParaForm tmpParaForm(this);
    int ret = tmpParaForm.exec();
    if(ret == QDialog::Accepted)
    {
    }
    else
    {
    }
    event->accept();
}

QRectF DeviceItem::boundingRect() const
{
    qreal adjust = 2.0;
    QFontMetrics fm(mFont);
    int pixelsWide = fm.width(mName);
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

int DeviceItem::GetDevRadius(DEV_TYPE pType)
{
    switch (pType)
    {
        case DEV_PHYSICAL:
            return D_PHRadius;
        case DEV_LOGIC:
            return D_LGRadius;
        default:
            break;
    }
    return 0;
}

void DeviceItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!mImg.isNull())
    {
        painter->save();
        painter->drawImage(rect(), mImg, mImg.rect());
        painter->restore();
    }
    if(!mName.isEmpty())
    {
        painter->save();
        painter->setFont(mFont);
        painter->setPen(mColor);
        QFontMetrics fm(mFont);
        int pixelsWide = fm.width(mName);
        int pixelsHigh = fm.height();
        QRectF tragRectF(rect().left()-(pixelsWide-rect().width()+3)/2,
                        rect().top()+rect().height()+5,
                        pixelsWide+6,
                        pixelsHigh+3);
        painter->drawText(tragRectF, Qt::AlignCenter, mName);
        painter->restore();
    }
}

void DeviceItem::StartTimeline()
{
    if(mTrackLine.length() <= 0)
        return;
    mAnimator.stop();
    mAnimator.setFrameRange(0, D_TIMELINERANGE);
    mAnimator.setDuration(mTrackLine.length());
    mAnimator.setCurveShape(QTimeLine::EaseInOutCurve);
    mAnimator.start();
}

void DeviceItem::StopTimeline()
{
    mAnimator.stop();
}

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

void DeviceItem::removeLine(DeviceLine *line)
{
    int index = mDeviceLines.indexOf(line);
    if (index != -1)
        mDeviceLines.removeAt(index);
}

void DeviceItem::removeLines()
{
//    foreach (DeviceLine *line, mDeviceLines)
//    {
//        line->startItem()->removeLine(line);
//        line->endItem()->removeLine(line);
//        scene()->removeItem(line);
//        delete line;
//    }
}

void DeviceItem::addLine(DeviceLine *line)
{
    mDeviceLines.append(line);
}
