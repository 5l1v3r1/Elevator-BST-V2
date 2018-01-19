#include "devicescene.h"
#include "devicemanageform.h"

DeviceScene::DeviceScene(QString pMdpPath, QObject *parent):
    QGraphicsScene(parent)
{
    setItemIndexMethod(QGraphicsScene::NoIndex);
    setSceneRect(QRectF(0,0,D_SCENESIZE,D_SCENESIZE));

    mMdpManager = 0;
    m_CurDeviceItem = 0;
    //>@创建背景图
    m_BackgroundItem = new QGraphicsPixmapItem(QPixmap(":/Device/rc/Device/background.png"));
    addItem(m_BackgroundItem);

    mMDPPath = pMdpPath;
    QTimer::singleShot(0, this, SLOT(Initilize()));
}

DeviceScene::~DeviceScene()
{
    //>@删除scene下的所有item

    CloseBootMdp();
}

bool DeviceScene::OpenBootMdp(QString pPath)
{
    CloseBootMdp();
    mMdpManager = new mdpManager(pPath);
    if(!(mMdpManager->OpenMDP() || mMdpManager->NewMDP()))
    {
        mMdpManager->m_IsChanged = false;
        delete mMdpManager;
        mMdpManager = 0;
        IDE_TRACE_STR("Open or Create BootMdp#Failed");
        return false;
    }
    return true;
}
void DeviceScene::CloseBootMdp()
{
    if(mMdpManager)
    {
        delete mMdpManager;
        mMdpManager = 0;
    }
}
bool DeviceScene::SaveBootMdp()
{
    if(mMdpManager)
    {
        mMdpManager->m_IsChanged = true;
        if(!mMdpManager->SaveMDP())
        {
            IDE_TRACE_STR("SaveBootMDP#Failed");
            return false;
        }
        return true;
    }
    return false;
}

void DeviceScene::Connect(PhysicalDevice *pPHDev, LogicDevice *pLGDev)
{
    if(pPHDev && pLGDev)
    {
        //>@查看是否已经建立线了？
        for(int i=0;i<pPHDev->m_DeviceItem->mDeviceLines.count();i++)
        {
            if(pPHDev->m_DeviceItem->mDeviceLines.at(i)->endItem() == pLGDev->m_DeviceItem)
                return;
        }
        DeviceLine *tmpDeviceLine = new DeviceLine(pPHDev->m_DeviceItem, pLGDev->m_DeviceItem);
        tmpDeviceLine->setColor(Qt::magenta);
        pPHDev->m_DeviceItem->addLine(tmpDeviceLine);
        pLGDev->m_DeviceItem->addLine(tmpDeviceLine);
        tmpDeviceLine->setZValue(-1000.0);
        addItem(tmpDeviceLine);
        tmpDeviceLine->updatePosition();
    }
}

void DeviceScene::Initilize()
{
    if(!OpenBootMdp(mMDPPath))
        return;
    int count = mMdpManager->m_PHElementGroup.count() + mMdpManager->m_LGElementGroup.count();
    qreal diff = 100.0/(qreal)count;
    qreal angleDiff = 2*PI/(qreal)count;
    int value = 0;
    //>@先加载物理设备
    QList<PH_Type> tmpPHTypeList = mMdpManager->m_PHElementGroup.keys();
    for(int i=0;i<tmpPHTypeList.count();i++)
    {
        PH_Type tmpType = tmpPHTypeList.at(i);
        if(tmpType == P_INVALID)
            continue;
        QDomElement tmpElement = mMdpManager->m_PHElementGroup.value(tmpType);
        qreal tmpAngle = angleDiff*i;
        AddDevice(tmpType, QPointF(D_SCENESIZE/2 + D_LGRadius*qCos(tmpAngle), D_SCENESIZE/2 + D_LGRadius*qSin(tmpAngle)), (LogicDevice*)0, tmpElement);
        GlobalPara.updateProgress(tr("Loading %1 ...").arg(tmpType), value=i*diff);
    }
    //>@再加载逻辑设备
    QList<LG_Type> tmpLGTypeList = mMdpManager->m_LGElementGroup.keys();
    for(int i=0;i<tmpLGTypeList.count();i++)
    {
        LG_Type tmpType = tmpLGTypeList.at(i);
        if(tmpType == L_INVALID)
            continue;
        QDomElement tmpElement = mMdpManager->m_LGElementGroup.value(tmpType);
        qreal tmpAngle = angleDiff*i;
        AddDevice(tmpType, QPointF(D_SCENESIZE/2 + D_PHRadius*qCos(tmpAngle), D_SCENESIZE/2 + D_PHRadius*qSin(tmpAngle)), tmpElement);
        GlobalPara.updateProgress(tr("Loading %1 ...").arg(tmpType), value+i*diff);
    }
    GlobalPara.updateProgress(100);
}

PhysicalDevice* DeviceScene::CreateDevItem(PH_Type pType, QDomElement pElement)
{
    PhysicalDevice *tmpPHDev = 0;
    switch(pType)
    {
        case P_CPU:
            tmpPHDev = new PhysicalCPU(this);
            break;
        case P_MEM:
            tmpPHDev = new PhysicalMEM(this);
            break;
        case P_DISK:
            tmpPHDev = new PhysicalDisk(this);
            break;
        case P_RTC:
            tmpPHDev = new PhysicalRTC(this);
            break;
        case P_Audio:
            tmpPHDev = new PhysicalAudio(this);
            break;
        case P_RMII:
            tmpPHDev = new PhysicalRMII(this);
            break;
        case P_LED:
            tmpPHDev = new PhysicalLED(this);
            break;
        case P_LCD:
            tmpPHDev = new PhysicalLCD(this);
            break;
        case P_SodimasSAF:
            tmpPHDev = new PhysicalSodimasSAF(this);
            break;
        case P_SodimasCAN:
            tmpPHDev = new PhysicalSodimasCAN(this);
            break;
        case P_BSTRS232:
            tmpPHDev = new PhysicalBSTRS232(this);
            break;
        case P_YungtayRS485:
            tmpPHDev = new PhysicalYungtayRS485(this);
            break;
        case P_MDISK:
            tmpPHDev = new PhysicalMobileDisk(this);
            break;
        case P_Camera:
            tmpPHDev = new PhysicalCamera(this);
            break;
        case P_Mouse:
            tmpPHDev = new PhysicalMouse(this);
            break;
        case P_Touch:
            tmpPHDev = new PhysicalTouch(this);
            break;
        case P_Keyboard:
            tmpPHDev = new PhysicalKeyboard(this);
            break;
        case P_GPRS:
            tmpPHDev = new PhysicalGPRS(this);
            break;
        case P_WIFI:
            tmpPHDev = new PhysicalWIFI(this);
            break;
        case P_Bluetooth:
            tmpPHDev = new PhysicalBluetooth(this);
            break;
        case P_NFC:
            tmpPHDev = new PhysicalNFC(this);
            break;
        default:return tmpPHDev;
    }
    if(!tmpPHDev)
        return tmpPHDev;
    tmpPHDev->InitDevice(pType, pElement);
    return tmpPHDev;
}

LogicDevice* DeviceScene::CreateDevItem(LG_Type pType, QDomElement pElement)
{
    LogicDevice *tmpLGDev = 0;
    switch(pType)
    {
        case L_DLNA:
            tmpLGDev = new LogicDLNA(this);
            break;
        case L_Bonjour:
            tmpLGDev = new LogicBonjour(this);
            break;
        case L_FTP:
            tmpLGDev = new LogicFTP(this);
            break;
        case L_SSH:
            tmpLGDev = new LogicSSH(this);
            break;
        case L_Monitor:
            tmpLGDev = new LogicMonitor(this);
            break;
        case L_VideoPhone:
            tmpLGDev = new LogicVideoPhone(this);
            break;
        case L_Elevator:
            tmpLGDev = new LogicElevator(this);
            break;
        case L_CardReader:
            tmpLGDev = new LogicCardReader(this);
            break;
        case L_NFCP2P:
            tmpLGDev = new LogicNFCP2P(this);
            break;
        case L_ThemeManager:
            tmpLGDev = new LogicThemeManager(this);
            break;
        case L_ContentUpdates:
            tmpLGDev = new LogicContentUpgrades(this);
            break;
        case L_NetDiscovery:
            tmpLGDev = new LogicNetDiscovery(this);
            break;
        case L_NetManager:
            tmpLGDev = new GuestNetManager(this);
            break;
        default:return tmpLGDev;
    }
    if(!tmpLGDev)
        return tmpLGDev;
    tmpLGDev->InitDevice(pType, pElement);
    return tmpLGDev;
}

DeviceItem* DeviceScene::AddDevice(PH_Type pType, QPointF pPos, LogicDevice* pLGDev, QDomElement pElement)
{
    if(pType == P_INVALID)
    {
        IDE_TRACE();
        return (DeviceItem*)0;
    }

    //>@如果已经添加过此物理设备，则不再添加
    PhysicalDevice* tmpPHDev = m_PHDevList.value(pType);
    if(tmpPHDev)
        return tmpPHDev->m_DeviceItem;
    //>@
    QDomElement tmpOriginElement = GlobalPara.m_PHonfigInfoGroup.value(pType);
    if(tmpOriginElement.isNull())
    {
        IDE_TRACE();
        return (DeviceItem*)0;
    }
    if(pElement.isNull())
        pElement = mMdpManager->AddDeviceDom(QString("/Physical/%1").arg(tmpOriginElement.tagName()), tmpOriginElement);
    if(pElement.isNull())
        return (DeviceItem*)0;
    tmpPHDev = CreateDevItem(pType, pElement);
    if(!tmpPHDev)
        return (DeviceItem*)0;
    m_PHDevList.insert(pType, tmpPHDev);

    tmpPHDev->m_DeviceItem = new DeviceItem(DEV_PHYSICAL, tmpPHDev, pLGDev);
    tmpPHDev->m_DeviceItem->setImg(QImage(tmpOriginElement.attribute("img")));
    tmpPHDev->m_DeviceItem->setFlag(QGraphicsWidget::ItemIsMovable, true);
    tmpPHDev->m_DeviceItem->setFont(QFont("Arial",9,QFont::Black,true));
    tmpPHDev->m_DeviceItem->SetColor(D_ModifyColor);
    SetShadow(tmpPHDev->m_DeviceItem, D_ShadowColor, 0.3, QPointF(10,10));
    tmpPHDev->m_DeviceItem->setZValue(1);
    addItem(tmpPHDev->m_DeviceItem);
    //>@建立线
    Connect(tmpPHDev, pLGDev);
    tmpPHDev->m_DeviceItem->Initialize(pPos);
    return tmpPHDev->m_DeviceItem;
}

void DeviceScene::DelDevice(PH_Type pType)
{
    PhysicalDevice *tmpItem = m_PHDevList.take(pType);
    if(tmpItem)
    {
        //>@当删除一个DeviceItem时，如果其代表一个物理节点，则需要对应删除此物理节点以及此物理节点驱动的逻辑节点中的此物理节点的指针。
        QList<LG_Type> tmpList = tmpItem->mLGDevices.keys();
        for(int i=0;i<tmpItem->mLGDevices.count();i++)
        {
            LogicDevice* tmpLGDev = tmpItem->mLGDevices.value(tmpList.at(i));
            if(tmpLGDev)
                tmpLGDev->mPHDevices.remove(tmpItem->mDevType);
        }
        mMdpManager->DelDeviceDom(pType, QString("/Physical/%1").arg(tmpItem->mDevName));
        tmpItem->deleteLater();
    }
}

DeviceItem* DeviceScene::AddDevice(LG_Type pType, QPointF pPos, QDomElement pElement)
{
    if(pType == L_INVALID)
    {
        IDE_TRACE();
        return (DeviceItem*)0;
    }
    //>@如果已经添加过此逻辑设备，则不再添加
    LogicDevice* tmpLGDev = m_LGDevList.value(pType);
    if(tmpLGDev)
        return tmpLGDev->m_DeviceItem;
    //>@
    QDomElement tmpOriginElement = GlobalPara.m_LGonfigInfoGroup.value(pType);
    if(tmpOriginElement.isNull())
    {
        IDE_TRACE();
        return (DeviceItem*)0;
    }
    if(pElement.isNull())
        pElement = mMdpManager->AddDeviceDom(QString("/Logic/%1").arg(tmpOriginElement.tagName()), tmpOriginElement);
    if(pElement.isNull())
        return (DeviceItem*)0;
    tmpLGDev = CreateDevItem(pType, pElement);
    if(!tmpLGDev)
        return (DeviceItem*)0;
    m_LGDevList.insert(pType, tmpLGDev);

    tmpLGDev->m_DeviceItem = new DeviceItem(DEV_LOGIC, (PhysicalDevice*)0, tmpLGDev);
    tmpLGDev->m_DeviceItem->setImg(QImage(tmpOriginElement.attribute("img")));
    tmpLGDev->m_DeviceItem->setFlag(QGraphicsWidget::ItemIsMovable, true);
    tmpLGDev->m_DeviceItem->setFont(QFont("Arial",9,QFont::Black,true));
    tmpLGDev->m_DeviceItem->SetColor(D_ModifyColor);
    SetShadow(tmpLGDev->m_DeviceItem, D_ShadowColor, 0.3, QPointF(10,10));
    tmpLGDev->m_DeviceItem->setZValue(0);
    addItem(tmpLGDev->m_DeviceItem);
    QList<PH_Type> tmpPHTypeList = tmpLGDev->mPHDevices.keys();
    for(int i=0;i<tmpPHTypeList.count();i++)
    {
        PhysicalDevice *tmpPHDev = tmpLGDev->mPHDevices.value(tmpPHTypeList.at(i));
        Connect(tmpPHDev, tmpLGDev);
    }
    tmpLGDev->m_DeviceItem->Initialize(pPos);
    return tmpLGDev->m_DeviceItem;
}

void DeviceScene::DelDevice(LG_Type pType)
{
    LogicDevice *tmpItem = m_LGDevList.take(pType);
    if(tmpItem)
    {
        //>@当删除一个DeviceItem时，如果其代表一个物理节点，则需要对应删除此物理节点以及此物理节点驱动的逻辑节点中的此物理节点的指针。
        QList<PH_Type> tmpList = tmpItem->mPHDevices.keys();
        for(int i=0;i<tmpItem->mPHDevices.count();i++)
        {
            PhysicalDevice* tmpPHDev = tmpItem->mPHDevices.value(tmpList.at(i));
            if(tmpPHDev)
                tmpPHDev->mLGDevices.remove(tmpItem->mDevType);
        }
        mMdpManager->DelDeviceDom(pType, QString("/Logic/%1").arg(tmpItem->mDevName));
        tmpItem->deleteLater();
    }
}

void DeviceScene::DelDevice(DeviceItem* pDevItem)
{
    if(!pDevItem)
        return;
    if(pDevItem->mDevType == DEV_PHYSICAL)
    {
        DelDevice(pDevItem->mPHDev->mDevType);
    }
    else if(pDevItem->mDevType == DEV_LOGIC)
    {
        DelDevice(pDevItem->mLGDev->mDevType);
    }
    pDevItem->deleteLater();
}

//>@添加时会有 种情况：
//>@        1、逻辑设备添加
//>@        2、物理设备放在逻辑设备上面
//>@        3、物理设备没有放在逻辑设备上面
//>@查询是否重复添加，物理设备允许重复添加，逻辑设备不允许

void DeviceScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if(!event->mimeData()->hasFormat("Device"))
        return;
    QByteArray itemData = event->mimeData()->data("Device");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);
    QString itemPath;
    dataStream>>itemPath;

    DEV_TYPE tmpDevType = GetDevType(itemPath);
    switch(tmpDevType)
    {
        case DEV_PHYSICAL:
        {
            QString tmpDevName = getFileName(itemPath);
            PH_Type tmpType = GetPHType(tmpDevName);
            LogicDevice *tmpLGDev = 0;
            DeviceItem* tmpItem = VAR_CAST<DeviceItem*>(itemAt(event->scenePos()));
            if(tmpItem && tmpItem->mDevType == DEV_LOGIC)
                tmpLGDev = tmpItem->mLGDev;
            AddDevice(tmpType, event->scenePos(), tmpLGDev, QDomElement());
            break;
        }
        case DEV_LOGIC:
        {
            QString tmpDevName = getFileName(itemPath);
            LG_Type tmpType = GetLGType(tmpDevName);
            AddDevice(tmpType, event->scenePos(), QDomElement());
            break;
        }
        default: break;
    }
}

void DeviceScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}

void DeviceScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;
    DeviceItem *tmpItem = VAR_CAST<DeviceItem*>(itemAt(event->scenePos()));
    if(tmpItem)
    {
        tmpItem->StopTimeline();
    }
    QGraphicsScene::mousePressEvent(event);
}

void DeviceScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;
    qreal rx = event->scenePos().rx();
    qreal ry = event->scenePos().ry();
    DeviceItem *tmpItem = VAR_CAST<DeviceItem*>(mouseGrabberItem());
    if(!tmpItem)
        return;
    switch(tmpItem->mDevType)
    {
        case DEV_PHYSICAL:
        {
            QList<QGraphicsItem *> tmpItemList = items(QPointF(rx, ry));
            for(int i=0;i<tmpItemList.count();i++)
            {
                DeviceItem *tmpLgItem = VAR_CAST<DeviceItem*>(tmpItemList.at(i));
                if(!tmpLgItem || tmpItem == tmpLgItem)
                    continue;
                if(tmpLgItem->mDevType == DEV_LOGIC)
                {
                    //>@建立物理与逻辑节点的连接
                    Connect(tmpItem->mPHDev, tmpLgItem->mLGDev);
                    tmpItem->Initialize(QPointF(rx, ry));
                    return;
                }
            }
            QLineF tmpLine(tmpItem->mCirclePoint, QPointF(rx, ry));
            if(tmpLine.length() > D_DelRadius)
            {
                DelDevice(tmpItem);
            }
            else
            {
                tmpItem->Initialize(event->scenePos());
            }
            break;
        }
        case DEV_LOGIC:
        {
            QLineF tmpLine(tmpItem->mCirclePoint, QPointF(rx, ry));
            if(tmpLine.length() > D_DelRadius)
            {
                DelDevice(tmpItem);
            }
            else
            {
                tmpItem->Initialize(event->scenePos());
            }
            break;
        }
        default: break;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void DeviceScene::keyPressEvent(QKeyEvent *event)
{
    QGraphicsScene::keyPressEvent(event);
    switch(event->key())
    {
        case Qt::Key_Left:
            break;
        case Qt::Key_Right:
            break;
        default:
            break;
    }
}

DeviceView::DeviceView(QWidget *parent) :
    QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(QBrush(Qt::gray));
    //setBackgroundBrush(QPixmap(":/Page/rc/Page/background.png"));
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setAcceptDrops(true);
    setDragMode(QGraphicsView::RubberBandDrag);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(true);
}

void DeviceView::paintEvent(QPaintEvent *event)
{
    static bool busy = false;
    if(!busy)
        busy = true;
    else
        return;
    QGraphicsView::paintEvent(event);
    busy = false;
}

void DeviceView::drawBackground(QPainter *painter, const QRectF &rect)
{
    static bool busy = false;
    if(!busy)
        busy = true;
    else
        return;
    QGraphicsView::drawBackground(painter, rect);
    QGraphicsScene* scene = this->scene();
    if(scene)
    {
        int w = (int)(scene->width()-RectPulse*2);
        int h = (int)(scene->height()-RectPulse*2);
        if(w <= 0 || h<=0)
            return;
        painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
        painter->setBrush(Qt::white);
        painter->drawRect(QRect(0, 0, (int)(scene->width()), (int)(scene->height())));
        painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
        painter->drawRect(QRect(RectPulse, RectPulse, w, h));
    }
    busy = false;
}

void DeviceView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
//    fitInView(sceneRect(), Qt:: KeepAspectRatio);
//    QTransform tmpTransform = transform();
//    DeviceManageForm *tmpForm = VAR_CAST<DeviceManageForm*>(parentWidget());
//    if(tmpForm)
//        tmpForm->UpdateZoomSlider(tmpTransform.m11()*100);
//    qDebug() << (tmpTransform.m11());
}
