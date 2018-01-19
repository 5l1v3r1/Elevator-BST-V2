#include "deviceitem.h"
#include "devicescene.h"
#include "deviceparaform.h"

DeviceEllipse::DeviceEllipse(QGraphicsItem *parent) :
    QGraphicsEllipseItem(parent)
{

}

DeviceEllipse::~DeviceEllipse()
{

}

//mSetWidget = new QWidget;
//QVBoxLayout *tmpLayout = new QVBoxLayout(mSetWidget);
//tmpLayout->addWidget(mDevice.mLGDev->m_SettingGroup);
//tmpLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

DeviceItem::DeviceItem(PhysicalDevice *pPHDev, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    mType = DEV_PHYSICAL;
    mDevice.mPHDev = pPHDev;
    Initialize();
}

DeviceItem::DeviceItem(LogicDevice *pLGDev, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    mType = DEV_LOGIC;
    mDevice.mLGDev = pLGDev;
    Initialize();
}

DeviceItem::~DeviceItem()
{
}

void DeviceItem::Initialize()
{
    deleteAfterAnimation = false;

    setFlag(ItemIsMovable);
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
    setFlag(ItemSendsGeometryChanges);
#endif
    setZValue(1);

    icon = new QGraphicsPixmapItem(this);
    text = new QGraphicsTextItem(this);

    currentScale = 0;
    finalScale = 1;
    setTransform(QTransform::fromScale(currentScale, currentScale), false);
    setOpacity(0);

    newDeviceAdded();
}

void DeviceItem::setFinalScale(qreal factor)
{
    finalScale = factor;
}

void DeviceItem::setDeleteAfterAnimation(bool deleteAfter)
{
    deleteAfterAnimation = deleteAfter;
}

void DeviceItem::calculateForces()
{
    if (!scene() || scene()->mouseGrabberItem() == this)
    {
        newPos = pos();
        return;
    }

    // sum up all the forces push this item away
    qreal xvel = 0;
    qreal yvel = 0;
    QLineF orbitForce;
    foreach (QGraphicsItem *item, scene()->items())
    {
        // other DeviceItems
        DeviceItem *tmpDeviceItem = qgraphicsitem_cast<DeviceItem *>(item);
        if (!tmpDeviceItem && item->data(0) != QLatin1String("This Device"))
            continue;

        qreal factor = 1.0;

        QLineF line(tmpDeviceItem ? item->mapToScene(0, 0) : QPointF(0, 0), mapToScene(0, 0));
        factor = tmpDeviceItem->currentScale;

        qreal dx = line.dx();
        qreal dy = line.dy();
        double l = 2.0 * (dx * dx + dy * dy);
        if (l > 0)
        {
            xvel += factor * dx * 200.0 / l;
            yvel += factor * dy * 200.0 / l;
        }
    }

    // tendency to stay at a fixed orbit
    qreal orbit = getRadiusForState(configuration.state());
    qreal distance = orbitForce.length();

    QLineF unit = orbitForce.unitVector();

    orbitForce.setLength(xvel * unit.dx() + yvel * unit.dy());

    qreal w = 2 - exp(-pow(distance-orbit, 2)/(2 * 50));

    if (distance < orbit)
    {
        xvel += orbitForce.dx() * w;
        yvel += orbitForce.dy() * w;
    }
    else
    {
        xvel -= orbitForce.dx() * w;
        yvel -= orbitForce.dy() * w;
    }

    if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
        xvel = yvel = 0;

    QRectF sceneRect = scene()->sceneRect();
    newPos = pos() + QPointF(xvel, yvel);
    newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
    newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
}

bool DeviceItem::advance()
{
    static const qreal scaleDelta = 0.01;

    bool animated = false;

    if (currentScale < finalScale)
    {
        animated = true;
        currentScale = qMin<qreal>(currentScale + scaleDelta, finalScale);
        setTransform(QTransform::fromScale(currentScale, currentScale), false);
    }
    else if (currentScale > finalScale)
    {
        animated = true;
        currentScale = qMax<qreal>(currentScale - scaleDelta, finalScale);
        setTransform(QTransform::fromScale(currentScale, currentScale), false);
    }

    if (newPos != pos())
    {
        setPos(newPos);
        animated = true;
    }

    if (opacity() != finalOpacity)
    {
        animated = true;
        if (qAbs(finalScale - currentScale) > 0.0)
        {
            // use scale as reference
            setOpacity(opacity() + scaleDelta * (finalOpacity - opacity()) / qAbs(finalScale - currentScale));
        }
        else
        {
            setOpacity(finalOpacity);
        }
    }

    if (!animated && deleteAfterAnimation)
        deleteLater();

    return animated;
}

QRectF DeviceItem::boundingRect() const
{
    return childrenBoundingRect();
}

QVariant DeviceItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemPositionHasChanged:
            if (DeviceScene *tmpDeviceScene = qobject_cast<DeviceScene *>(scene()))
                tmpDeviceScene->DeviceMoved();
            break;
        default:
            break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void DeviceItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        //>@打开设置窗口

        stateChanged();
    }
}

void DeviceItem::stateChanged()
{
    finalOpacity = 1.0;

    QString tooltip;
    QDomElement tmpElement;
    if(mType == DEV_PHYSICAL)
    {
        tooltip += tr("<b>%1</b><br>").arg(mDevice.mPHDev->mDevName);
        tmpElement = mDevice.mPHDev->m_ParaElement;
    }
    else if(mType == DEV_LOGIC)
    {
        tooltip += tr("<b>%1</b><br>").arg(mDevice.mLGDev->mDevName);
        tmpElement = mDevice.mLGDev->m_ParaElement;
    }

    QDomNodeList tmpList = tmpElement.childNodes();
    for(int i=0;i<tmpList.count();i++)
    {
        QDomElement tmpParaElement = tmpList.at(i).toElement();
        tooltip += tr("<br>%1: %2").arg(tmpParaElement.tagName()).arg(tmpParaElement.text());
    }
    setToolTip(tooltip);
}

void DeviceItem::newDeviceAdded()
{
    const qreal height = icon->boundingRect().height() + text->boundingRect().height();

    icon->setSharedRenderer(svgCache[bearerType]);
    icon->setPos(icon->boundingRect().width() / -2, height / -2);

    if(mType == DEV_PHYSICAL)
        text->setHtml(tr("<b>%1</b><br>").arg(mDevice.mPHDev->mDevName));
    else if(mType == DEV_LOGIC)
        text->setHtml(tr("<b>%1</b><br>").arg(mDevice.mLGDev->mDevName));
    text->setPos(text->boundingRect().width() / -2, height / 2 - text->boundingRect().height());

    stateChanged();
}

qreal DeviceItem::getRadiusForDevType(DEV_TYPE pType)
{
    switch (pType)
    {
        case DEV_PHYSICAL:
            return 100;
        case DEV_LOGIC:
            return 250;
        default:
            break;
    }
    return 0;
}
