#include "deviceitem.h"

//mSetWidget = new QWidget;
//QVBoxLayout *tmpLayout = new QVBoxLayout(mSetWidget);
//tmpLayout->addWidget(mDevice.mLGDev->m_SettingGroup);
//tmpLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));


DeviceEllipse::DeviceEllipse(QGraphicsItem *parent) :
    QGraphicsEllipseItem(parent)
{

}

DeviceEllipse::~DeviceEllipse()
{

}

DeviceItem::DeviceItem(DEV_TYPE pType, PhysicalDevice *pPHDev, LogicDevice *pLGDev, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    setAcceptHoverEvents(true);
    if(pType == DEV_PHYSICAL && pLGDev)
    {
        mDevType = DEV_SUBPH;
        setParentItem(pLGDev->m_DeviceItem);
    }
    else
    {
        mDevType = pType;
    }
    mPHDev = pPHDev;
    mLGDev = pLGDev;
}

DeviceItem::~DeviceItem()
{
    scene()->removeItem(this);
}

//>@如果pDstPos为空，则计算到其轨迹上的轨迹点
void DeviceItem::Initialize(QPointF pPos, QPointF pDstPos)
{
    if(mDevType == DEV_SUBPH) //>@如果当前设备为物理设备，且逻辑设备如果不为空，则以此逻辑设备为圆心
        mCirclePoint = mLGDev->m_DeviceItem->scenePos();
    else
        mCirclePoint = QSizeF(D_SCENESIZE/2, D_SCENESIZE/2);
    mRadius = GetDevRadius(mDevType);

    setPos(pPos);
    mTrackPoints.clear();
    if(pPos == pDstPos)
        return;
    //>@创建D_TIMELINERANGE个轨迹点
    if(pDstPos.isNull())
    {
        QLineF tmpLine(mCirclePoint, pPos);
        pDstPos = tmpLine.pointAt((qreal)mRadius/tmpLine.length());
        tmpLine.setPoints(pPos, pDstPos);
        for(int i=1;i<=D_TIMELINERANGE;i++)
        {
            mTrackPoints.append(tmpLine.pointAt((qreal)i/(qreal)D_TIMELINERANGE));
        }
    }
    else
    {
        QLineF tmpLine1(mCirclePoint, pPos);
        QLineF tmpLine2(mCirclePoint, pDstPos);
        qreal tmpDiffAngle = tmpLine2.angleTo(tmpLine1); //>@逆时针角度
        qreal tmpCurAngle = tmpLine1.angle();
        if(tmpDiffAngle - PI)  //>@
        {
            qreal tmpDiff = (2*PI-tmpDiffAngle)/(qreal)D_TIMELINERANGE;
            for(int i=1;i<=D_TIMELINERANGE;i++)
            {
                qreal tmpAngle = RegularAngle(tmpCurAngle - tmpDiff*i);
                mTrackPoints.append(mCirclePoint + QPointF(mRadius*qCos(tmpAngle), mRadius*qSin(tmpAngle)));
            }
        }
    }
}

void DeviceItem::updateAnimate(int pFrame)
{
    int tmpFrame = pFrame - 1;
    if(mTrackPoints.count() > tmpFrame)
        setPos(mTrackPoints.at(tmpFrame));
}

void DeviceItem::UpdateTooltip(QString pContent)
{
    setToolTip(pContent);
}

QRectF DeviceItem::boundingRect() const
{
    qreal adjust = 2.0;
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

int DeviceItem::GetDevRadius(DEV_TYPE pType)
{
    switch (pType)
    {
        case DEV_PHYSICAL:
            return D_PHRadius;
        case DEV_LOGIC:
            return D_LGRadius;
        case DEV_SUBPH:
            return D_SubPHRadius;
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
    if(!mDevName.isEmpty())
    {
        painter->save();
        painter->setFont(mFont);
        painter->setPen(mColor);
        QFontMetrics fm(mFont);
        int pixelsWide = fm.width(name);
        int pixelsHigh = fm.height();
        QRectF tragRectF(rect().left()-(pixelsWide-rect().width()+3)/2,
                        rect().top()+rect().height()+5,
                        pixelsWide+6,
                        pixelsHigh+3);
        painter->drawText(tragRectF, Qt::AlignCenter, data(0).toString());
        painter->restore();
    }
}
