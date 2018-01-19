#include "devicescene.h"

////>@增加两条轨道
//qreal radius = DeviceItem::GetDevRadius(DEV_PHYSICAL);
//DeviceEllipse *orbit = new DeviceEllipse(-radius, -radius, 2*radius, 2*radius);
//orbit->setHovered(false, QColor(Qt::green));
//addItem(orbit);

DeviceScene::DeviceScene(QString pMdpPath, QObject *parent):
    QGraphicsScene(parent)
{
    setItemIndexMethod(QGraphicsScene::NoIndex);
    setSceneRect(QRectF(0,0,D_SCENESIZE,D_SCENESIZE));
    m_CurDeviceItem = 0;

    //>@创建背景图
    m_BackgroundItem = new QGraphicsPixmapItem(QPixmap(":/Device/rc/Device/background.png"));
    addItem(m_BackgroundItem);
    qsrand(QDateTime::currentDateTime().toTime_t());
    offset[DEV_PHYSICAL] = 2 * M_PI * qrand() / RAND_MAX;
    offset[DEV_LOGIC] = offset[DEV_PHYSICAL] + M_PI / 6;

    mMDPPath = pMdpPath;
    connect(this, SIGNAL(sDeviceAdd()), this, SLOT(slot_DeviceAdd()));
    connect(this, SIGNAL(sDeviceDel()), this, SLOT(slot_DeviceDel()));
    QTimer::singleShot(0, this, SLOT(Initilize()));
}

DeviceScene::~DeviceScene()
{
    //>@删除scene下的所有item

}

bool DeviceScene::OpenBootMdp(QString pPath)
{
    CloseBootMdp();
    mMdpManager = new mdpManager(pPath);
    GlobalPara.m_MdpManager = mMdpManager;
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
        mMdpManager->m_IsChanged = false;
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

void DeviceScene::Initilize()
{
    if(!OpenBootMdp(mMDPPath))
        return;
    int count = mMdpManager->m_PHElementGroup.count() + mMdpManager->m_LGElementGroup.count();
    qreal diff = 70.0 / (qreal)count;
    int value = 0;
    //>@先加载物理设备
    QList<PH_Type> tmpPHTypeList = mMdpManager->m_PHElementGroup.keys();
    for(int i=0;i<tmpPHTypeList.count();i++)
    {
        PH_Type tmpType = tmpPHTypeList.at(i);
        QDomElement tmpElement = mMdpManager->m_PHElementGroup.value(tmpType);
        if(tmpElement.isNull())
            continue;
        AddDeviceItem(tmpType, tmpElement);
        GlobalPara.updateProgress(tr("Loading %1 ...").arg(tmpElement.tagName()), value=i*diff);
    }
    //>@再加载逻辑设备
    QList<LG_Type> tmpLGTypeList = mMdpManager->m_LGElementGroup.keys();
    for(int i=0;i<tmpLGTypeList.count();i++)
    {
        PH_Type tmpType = tmpLGTypeList.at(i);
        QDomElement tmpElement = mMdpManager->m_LGElementGroup.value(tmpType);
        if(tmpElement.isNull())
            continue;
        AddDeviceItem(tmpType, tmpElement);
        GlobalPara.updateProgress(tr("Loading %1 ...").arg(tmpElement.tagName()), value+i*diff);
    }
    //>@初始化界面


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
        case P_NAND:
            tmpPHDev = new PhysicalNand(this);
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
        case P_USBNET:
            tmpPHDev = new PhysicalUSBNET(this);
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
        case P_MMC:
            tmpPHDev = new PhysicalMMC(this);
            break;
        case P_UDisk:
            tmpPHDev = new PhysicalUDisk(this);
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
    tmpPHDev->InitPHDevice(pType, pElement);
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
        case L_CardReader:
            tmpLGDev = new LogicCardReader(this);
            break;
        case L_AutoConnect:
            tmpLGDev = new LogicAutoConnect(this);
            break;
        case L_ThemeManager:
            tmpLGDev = new LogicThemeManager(this);
            break;
        case L_ContentUpdates:
            tmpLGDev = new LogicContentUpgrades(this);
            break;
        case L_NetManager:
            tmpLGDev = new LogicNetManager(this);
            break;
        default:return tmpLGDev;
    }
    tmpLGDev->InitLGDevice(pType, pElement);
    return tmpLGDev;
}

DeviceItem* DeviceScene::AddDevice(PH_Type pType, QPointF pPos, LogicDevice* pLGDev, QDomElement pElement)
{
    if(pType == P_INVALID || pElement.isNull())
    {
        IDE_TRACE();
        return (DeviceItem*)0;
    }
    //>@如果已经添加过此物理设备，则在DEV中再添加一个DeviceItem即可
    PhysicalDevice* tmpPHDev = m_PHDevList.value(pType);
    if(!tmpPHDev)
    {
        QDomElement tmpOriginElement = GlobalPara.m_PHonfigInfoGroup.value(pType);
        if(tmpOriginElement.isNull())
        {
            IDE_TRACE();
            return (DeviceItem*)0;
        }
        tmpPHDev = CreateDevItem(pType, pElement);
    }
    if(!tmpPHDev)
        return (DeviceItem*)0;
    m_PHDevList.insert(pType, tmpPHDev);

    DeviceItem *tmpDevItem = 0;
    tmpDevItem = new DeviceItem(DEV_PHYSICAL, tmpPHDev, pLGDev);  //>@在器件类中计算轨迹
    tmpDevItem->setImg(QImage(tmpOriginElement.attribute("img")));
    tmpDevItem->setFlag(QGraphicsWidget::ItemIsMovable, true);
    tmpDevItem->setFont(QFont("Arial",9,QFont::Black,true));
    tmpDevItem->SetColor(D_ModifyColor);
    SetShadow(tmpDevItem, D_ShadowColor, 0.3, QPointF(10,10));
    if(tmpPHDev->mDevType == DEV_PHYSICAL)
        m_PHDevItems.append(tmpDevItem);
    //>@
    tmpPHDev->m_DeviceItems.append(tmpDevItem);
    return tmpDevItem;
}

void DeviceScene::DelDevice(PH_Type pType)
{
    PhysicalDevice *tmpItem = m_PHDevList.take(pType);
    if(tmpItem)
    {
        for(int i=0;i<tmpItem->m_DeviceItems.count();i++)
            m_PHDevItems.removeOne(tmpItem->m_DeviceItems.at(i));
        tmpItem->deleteLater();
    }
}

DeviceItem* DeviceScene::AddDevice(LG_Type pType, QDomElement pElement)
{
    if(pType == L_INVALID || pElement.isNull())
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
    return tmpLGDev->m_DeviceItem;
}

void DeviceScene::DelDeviceItem(LG_Type pType)
{
    LogicDevice *tmpItem = m_LGDevList.take(pType);
    if(tmpItem)
    {
        tmpItem->deleteLater();
    }
}

void DeviceScene::DelDevice(DeviceItem* pDevItem)
{
    if(!pDevItem)
        return;
    //>@如果为物理设备，则需要查看此物理设备中的节点是否全部删除了，如果是则要把此PHDev也删除掉
    if(pDevItem->mDevType == DEV_PHYSICAL)
    {
        pDevItem->mPHDev->DeleteDevItem(pDevItem);
        m_PHDevItems.removeOne(pDevItem);
        if(pDevItem->mPHDev->m_DeviceItems.count()<=0)
            DelDevice(pDevItem->mPHDev->mDevType);
    }
    //>@如果为逻辑设备，因为只有一个图像节点，直接将LGDev也删除掉
    else if(pDevItem->mDevType == DEV_LOGIC)
    {
        pDevItem->mLGDev->DeleteDevItem();
        DelDevice(pDevItem->mLGDev->mDevType);
    }
}

void DeviceScene::OpenParaForm(QGraphicsItem* pItem)
{
    DeviceItem *tmpItem = VAR_CAST<DeviceItem*>(pItem);
    if(!tmpItem)
        return;
    if(tmpItem->mDevType == DEV_PHYSICAL)
    {
        PH_Type tmpType = GetPHType(tmpItem->mDevName);
        PhysicalDevice *tmpPHDev = m_PHDevList.value(tmpType);
        if(!tmpPHDev)
            return;
        DeviceParaForm tmpParaForm(tmpPHDev);
        int ret = tmpParaForm.exec();
        if(ret == QDialog::Accepted)
        {
        }
        else
        {
        }
    }
    else if(tmpItem->mDevType == DeviceItem::LOGIC && tmpItem->mDevice.mLGDev)
    {
        LG_Type tmpType = GetLGType(tmpItem->mDevName);
        LogicDevice *tmpLGDev = m_LGDevList.value(tmpType);
        if(!tmpLGDev)
            return;
        DeviceParaForm tmpParaForm(tmpLGDev);
        int ret = tmpParaForm.exec();
        if(ret == QDialog::Accepted)
        {
        }
        else
        {
        }
    }
    ClearColor(tmpItem);
    SetShadow(tmpItem, D_ShadowColor, 0.3, QPointF(10,10));
}

void DeviceScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if(m_Animator.state() != QTimeLine::NotRunning)
        return;
    if(event->mimeData()->hasFormat("Device"))
    {
        QByteArray itemData = event->mimeData()->data("Device");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        QString itemPath;
        dataStream>>itemPath;

        QDomElement tmpInitElement = GlobalPara.m_ConfigInfoGroup.value(itemPath);
        if(tmpInitElement.isNull())
            return;

        //>@添加时会有 种情况：
        //>@        1、逻辑设备添加
        //>@        2、物理设备放在逻辑设备上面
        //>@        3、物理设备没有放在逻辑设备上面
        //>@查询是否重复添加，物理设备允许重复添加，逻辑设备不允许
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
                DeviceItem *tmpDevItem = AddDevice(tmpType, tmpLGDev, tmpInitElement);
                if(tmpDevItem)
                {
                    InitAnimatePara(tmpDevItem, event->scenePos());
                    StartTimeline();
                }
                break;
            }
            case DEV_LOGIC:
            {
                QString tmpDevName = getFileName(itemPath);
                LG_Type tmpType = GetLGType(tmpDevName);
                DeviceItem *tmpDevItem = AddDevice(tmpType, tmpInitElement);
                if(tmpDevItem)
                {
                    InitAnimatePara(tmpDevItem, event->scenePos());
                    StartTimeline();
                }
                break;
            }
            default: break;
        }
    }
    event->ignore();
}

void DeviceScene::InitAnimatePara(DeviceItem *pDevItem, QPointF pPos)
{
    if(!pDevItem)
        return;
    if(pPos == pDstPos)
        return;

    QPointF tmpCirclePoint;  //>@圆心
    int tmpRadius;
    int tmpItemCount;
    QList<DeviceItem*> tmpItemList;
    if(pDevItem->mDevType == DEV_SUBPH) //>@如果当前设备为物理设备，且逻辑设备如果不为空，则以此逻辑设备为圆心
    {
        for(int i=0;i<pDevItem->mLGDev->mPHDevices.count();i++)
        {
            pDevItem->mLGDev->mPHDevices
        }
        tmpItemList = ;
        tmpCirclePoint = pDevItem->mLGDev->m_DeviceItem->scenePos();
    }
    else if(pDevItem->mDevType == DEV_PHYSICAL)
    {
        tmpItemCount = m_PHDevItems.count();
        tmpCirclePoint = QSizeF(D_SCENESIZE/2, D_SCENESIZE/2);
    }
    else
    {
        tmpItemCount = m_LGDevList.keys().count();
        tmpCirclePoint = QSizeF(D_SCENESIZE/2, D_SCENESIZE/2);
    }
    tmpRadius = GetDevRadius(pDevItem->mDevType);

    QLineF tmpLine(tmpCirclePoint, pPos);
    QPointF keyPoint = tmpLine.pointAt((qreal)tmpRadius/tmpLine.length());


    if(pDevItem->mDevType == DEV_SUBPH)
    {
        count =
    }
    qreal angle = 2 * PI / count;

    //>@计算相关节点的运动轨迹
    QList


    pDevItem->setPos(pPos);
    pDevItem->mTrackPoints.clear();

    //>@创建D_TIMELINERANGE个轨迹点
    if(pDstPos.isNull())
    {
        QLineF tmpLine(tmpCirclePoint, pPos);
        pDstPos = tmpLine.pointAt((qreal)tmpRadius/tmpLine.length());
        tmpLine.setPoints(pPos, pDstPos);
        for(int i=1;i<=D_TIMELINERANGE;i++)
        {
            pDevItem->mTrackPoints.append(tmpLine.pointAt((qreal)i/(qreal)D_TIMELINERANGE));
        }
    }
    else
    {
        QLineF tmpLine1(tmpCirclePoint, pPos);
        QLineF tmpLine2(tmpCirclePoint, pDstPos);
        qreal tmpDiffAngle = tmpLine2.angleTo(tmpLine1); //>@逆时针角度
        qreal tmpCurAngle = tmpLine1.angle();
        if(tmpDiffAngle - PI)  //>@
        {
            qreal tmpDiff = (2*PI-tmpDiffAngle)/(qreal)D_TIMELINERANGE;
            for(int i=1;i<=D_TIMELINERANGE;i++)
            {
                qreal tmpAngle = RegularAngle(tmpCurAngle - tmpDiff*i);
                pDevItem->mTrackPoints.append(tmpCirclePoint + QPointF(tmpRadius*qCos(tmpAngle), tmpRadius*qSin(tmpAngle)));
            }
        }
        else
        {
            qreal tmpDiff = (2*PI-tmpDiffAngle)/(qreal)D_TIMELINERANGE;
            for(int i=1;i<=D_TIMELINERANGE;i++)
            {
                qreal tmpAngle = RegularAngle(tmpCurAngle + tmpDiff*i);
                pDevItem->mTrackPoints.append(tmpCirclePoint + QPointF(tmpRadius*qCos(tmpAngle), tmpRadius*qSin(tmpAngle)));
            }
        }
    }
}

void DeviceScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}

void DeviceScene::SetFoucs(DeviceItem *pItem)
{
    if(m_CurDeviceItem == pItem)
        return;
    ClearFocus();
    m_CurDeviceItem = tmpItem;
    if(m_CurDeviceItem)
        SetColor(m_CurDeviceItem, D_SelectColor);
}

void DeviceScene::ClearFocus()
{
    if(m_CurDeviceItem)
    {
        ClearColor(m_CurDeviceItem);
        m_CurDeviceItem = 0;
    }
}

void DeviceScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if(event->button() != Qt::LeftButton)
        return;
    DeviceItem *tmpItem = VAR_CAST<DeviceItem*>(itemAt(event->scenePos().rx(), event->scenePos().ry()));
    SetFoucs(tmpItem);
}

void DeviceScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseDoubleClickEvent(event);
    if(event->button() != Qt::LeftButton)
        return;
    ClearFocus();
    DeviceItem *tmpItem = VAR_CAST<DeviceItem*>(itemAt(event->scenePos().rx(), event->scenePos().ry()));
    if(!tmpItem)
        return;
    OpenParaForm(tmpItem);
}

void DeviceScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if(event->button() != Qt::LeftButton)
        return;
    qreal rx = event->scenePos().rx();
    qreal ry = event->scenePos().ry();
    DeviceItem *tmpItem = VAR_CAST<DeviceItem*>(itemAt(rx, ry));
    if(!tmpItem)
        return;
    qreal r = sqrt(rx*rx + ry*ry);
    if(r > D_Radius+30)
    {
        switch(tmpItem->mDevType)
        {
            case DEV_PHYSICAL:
            {
                PH_Type tmpType = GetPHType(tmpItem->mDevName);
                mMdpManager->DelDeviceDom(tmpType, QString("/Physical/%1").arg(tmpItem->mDevName));
                DelDeviceItem(tmpType);
                break;
            }
            case DEV_LOGIC:
            {
                LG_Type tmpType = GetLGType(tmpItem->mDevName);
                mMdpManager->DelDeviceDom(tmpType, QString("/Logic/%1").arg(tmpItem->mDevName));
                DelDeviceItem(tmpType);
                break;
            }
            default: break;
        }
    }
}

void DeviceScene::slot_DeviceAdd()
{

}

void DeviceScene::slot_DeviceDel()
{

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

void DeviceScene::deviceMoved()
{
    if (!mModifyTimerId)
        mModifyTimerId = startTimer(1000 / 25);
}

void DeviceScene::timerEvent(QTimerEvent *)
{
    QList<DeviceItem *> tmpDeviceItems;
    foreach (QGraphicsItem *item, items())
    {
        if (DeviceItem *tmpItem = qgraphicsitem_cast<DeviceItem *>(item))
            tmpDeviceItems << tmpItem;
    }

    foreach (DeviceItem *tmpItem, tmpDeviceItems)
        tmpItem->calculateForces();

    bool cloudsMoved = false;
    foreach (DeviceItem *tmpItem, tmpDeviceItems)
        cloudsMoved |= tmpItem->advance();

    if (!cloudsMoved)
    {
        killTimer(mModifyTimerId);
        mModifyTimerId = 0;
    }
}

DeviceView::DeviceView(QWidget *parent) :
    UpQGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(QBrush(Qt::gray));
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
    UpQGraphicsView::paintEvent(event);
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
