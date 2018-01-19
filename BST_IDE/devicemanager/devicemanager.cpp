#include "devicemanager.h"

DeviceManager::DeviceManager(QRect pRect, QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint)
{
    m_DeviceWathcer = 0;
    mMdpManager = 0;
    setGeometry(pRect);
    //>@?????
    m_PHDevList.clear();
    m_LGDevList.clear();
    //>@??????grid
    QGridLayout *tmpGridLayout = new QGridLayout;
    tmpGridLayout->setSpacing(0);
    tmpGridLayout->setMargin(0);
    setLayout(tmpGridLayout);
    //>@????????
    m_MsgPrgInfo.mProgressValue = 0;
    m_MsgPrgInfo.mMessageQueue.append(QString("Welcome to use Display!"));
    m_MsgPrgInfo.mMessageQueue.append(GetAppVersion());
    m_PaintEnable = true;
    //>@??????????
    QTimer::singleShot(1, this, SLOT(InitDevices()));
}

DeviceManager::~DeviceManager()
{
    ReleaseDevices();
    CloseBootMdp();
}

bool DeviceManager::OpenDeviceWatcher()
{
    m_DeviceWathcer = new DeviceWatcher;
    connect(m_DeviceWathcer, SIGNAL(sDeviceAdded(QString)), this, SLOT(slot_DeviceAdded(QString)));
    connect(m_DeviceWathcer, SIGNAL(sDeviceRemoved(QString)), this, SLOT(slot_DeviceRemoved(QString)));
}

void DeviceManager::CloseDeviceWatcher()
{
    if(m_DeviceWathcer)
    {
        disconnect(m_DeviceWathcer, SIGNAL(sDeviceAdded(QString)), 0, 0);
        disconnect(m_DeviceWathcer, SIGNAL(sDeviceRemoved(QString)), 0, 0);
        m_DeviceWathcer->quit();
        m_DeviceWathcer->deleteLater();
    }
}

void DeviceManager::slot_DeviceAdded(QString dev)
{
    IDE_TRACE_STR(dev);
    QStringList tmpList = dev.split('>');
    if(tmpList.count()!=2)
    {
        IDE_TRACE_STR(dev);
        return;
    }
    QString tmpType = tmpList.at(0);
    if(tmpType.compare("storage", Qt::CaseInsensitive) == 0)  //>@U盘
    {
        PhysicalMobileDisk* tmpMDisk = VAR_CAST<PhysicalMobileDisk *>(m_PHDevList.value(P_MDISK));
        if(tmpMDisk)
        {
            QString tmpDriverPath = tmpList.at(1);   //>@/dev/sda  /dev/sda1

            system("mkdir /mnt/usbdevice ");
            if(tmpDriverPath.endsWith("1"))
            {
                QString tmpCmd = QString("mount %1 %2").arg(tmpDriverPath).arg("/mnt/usbdevice");
                system(tmpCmd.toLatin1().data());
                tmpMDisk->slot_DevStateChanged(S_PLUGGED,"/mnt/usbdevice" );
            }
        }
    }
}

void DeviceManager::slot_DeviceRemoved(QString dev)
{
    IDE_TRACE_STR(dev);
    QStringList tmpList = dev.split('>');
    if(tmpList.count()!=2)
    {
        IDE_TRACE_STR(dev);
        return;
    }
    QString tmpType = tmpList.at(0);
    if(tmpType.compare("storage", Qt::CaseInsensitive) == 0)  //>@U盘
    {
        PhysicalMobileDisk* tmpMDisk = VAR_CAST<PhysicalMobileDisk *>(m_PHDevList.value(P_MDISK));
        if(tmpMDisk)
        {
            tmpMDisk->slot_DevStateChanged(S_UNPLUGGED, tmpList.at(1));
        }
    }
}

void DeviceManager::UpdateProgress(QString pMsg)
{
    if(!pMsg.isEmpty())
    {
        if(m_MsgPrgInfo.mMessageQueue.count() < m_MsgPrgInfo.mMaxMsgRow)
        {
            m_MsgPrgInfo.mMessageQueue.append(pMsg);
        }
        else
        {
            m_MsgPrgInfo.mMessageQueue.removeFirst();
            m_MsgPrgInfo.mMessageQueue.append(pMsg);
        }
        update();
    }
}

void DeviceManager::UpdateProgress(QString pMsg, int pValue)
{
    if(m_PaintEnable == false)
        return;
    if(pValue == m_MsgPrgInfo.mProgressValue && pMsg.isEmpty())
        return;
    m_MsgPrgInfo.mProgressValue = pValue;
    if(!pMsg.isEmpty())
    {
        if(m_MsgPrgInfo.mMessageQueue.count() < m_MsgPrgInfo.mMaxMsgRow)
        {
            m_MsgPrgInfo.mMessageQueue.append(pMsg);
        }
        else
        {
            m_MsgPrgInfo.mMessageQueue.removeFirst();
            m_MsgPrgInfo.mMessageQueue.append(pMsg);
        }
    }
    update();
    QtSleep(D_PROGRESSDELAYTIME);
    if(m_MsgPrgInfo.mProgressValue >= 100)
        m_PaintEnable = false;
}

QPixmap DeviceManager::GetBufferImage()
{
    QImage tmpImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
    tmpImage.fill(Qt::transparent); //>@创建透明图层
    QPainter p(&tmpImage);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.setRenderHint(QPainter::Antialiasing);
    //>@绘制背景
    p.save();
    QLinearGradient bgGradient(QPointF(0,0), QPointF(0,height()));
    bgGradient.setColorAt(0, QColor(38,38,38,255));
    bgGradient.setColorAt(1, QColor(28,28,28,255));//设置渐变的颜色和路径比例
    p.setBrush(QBrush(bgGradient));
    p.drawRect(rect());
    p.restore();
    int tmpPenWid = 10;
    int tmpX = width()/6.0;
    int tmeY = height()*7.0/10.0;
    int tmeW = width()*2.0/3.0;
    int tmeH = 44;//tmeW/15.0;
    QRect tmpPrgRect = QRect(tmpX, tmeY, tmeW, tmeH);
    //>@绘制进度条外框
    p.save();
    p.setBrush(QBrush(QColor(55,70,79)));
    p.drawRoundedRect(tmpPrgRect, 25, 25);
    p.restore();
    //>@绘制进度条底图及顶图
    p.save();
    QLinearGradient pgBaseGradient(QPointF(0,tmeY+tmpPenWid/2), QPointF(0,tmeY+tmeH-tmpPenWid));
    pgBaseGradient.setColorAt(0, QColor(241,241,241,255));
    pgBaseGradient.setColorAt(0.5, QColor(202,202,202,255));
    pgBaseGradient.setColorAt(1, QColor(241,241,241,255));//设置渐变的颜色和路径比例
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(pgBaseGradient));
    p.drawRoundedRect(tmpPrgRect.adjusted(tmpPenWid, tmpPenWid, -tmpPenWid, -tmpPenWid), 25, 25);
    p.restore();
    m_MsgPrgInfo.mProgressRect = tmpPrgRect.adjusted(tmpPenWid, tmpPenWid, -tmpPenWid, -tmpPenWid);
    int fontheight = m_MsgPrgInfo.mProgressRect.height()*2.0/3.0;
    m_MsgPrgInfo.mPrgTextFontSize = CalcFontSizeFromSize(QString("Arial"), QSize(100,fontheight), PRIOR_HEIGHT);
    m_MsgPrgInfo.mPrgTextFont = QFont("Arial", m_MsgPrgInfo.mPrgTextFontSize, QFont::DemiBold, false);
    //>@绘制消息底图
    p.save();
    tmpX = width()/4.0;
    tmeY = height()*3.0/10.0;
    tmeW = width()/2.0;
    tmeH = height()*3.0/10.0;//tmeW/15.0;
    QRect tmpTextRect = QRect(tmpX, tmeY, tmeW, tmeH);
    p.setBrush(QBrush(QColor(238, 239, 239)));
    p.drawRoundedRect(tmpTextRect, 23, 23);
    int ydiff = 5;
    int xdiff = 5;
    p.drawRoundedRect(tmpTextRect.adjusted(xdiff,ydiff,-xdiff,-ydiff), 20, 20);
    tmpX += xdiff*3;
    tmeY += ydiff*4;
    tmeW -= xdiff*6;
    tmeH -= 8*ydiff;
    m_MsgPrgInfo.mMessageRect = QRect(tmpX, tmeY, tmeW, tmeH);
#if(QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
    m_MsgPrgInfo.mMsgTextFont = QFont("Arial", 13, QFont::DemiBold, false);
#else
    m_MsgPrgInfo.mMsgTextFont = QFont("Arial", 52, QFont::DemiBold, false);
#endif
    QFontMetricsF fm(m_MsgPrgInfo.mMsgTextFont);
    m_MsgPrgInfo.mMaxMsgRow = m_MsgPrgInfo.mMessageRect.height() / fm.height();
    p.end();
    return QPixmap::fromImage(tmpImage);
}

QPixmap DeviceManager::GetProgressImage()
{
    if(m_PaintEnable == false)
    {
        return QPixmap();
    }
    if(m_BufferPixmap.isNull())
    {
        m_BufferPixmap = GetBufferImage();
    }
    QImage tmpBgImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
    tmpBgImage.fill(Qt::transparent); //>@创建透明图层
    QPainter p(&tmpBgImage);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawPixmap(rect(), m_BufferPixmap);
    if(m_MsgPrgInfo.mProgressValue >= 0 && m_MsgPrgInfo.mProgressValue <= 100)
    {
        p.save();
        QLinearGradient pgTopGradient(m_MsgPrgInfo.mProgressRect.topLeft(), m_MsgPrgInfo.mProgressRect.bottomLeft());
        pgTopGradient.setColorAt(0, QColor(254,145,29,255));
        pgTopGradient.setColorAt(0.5, QColor(238,99,0,255));
        pgTopGradient.setColorAt(1, QColor(254,145,29,255));//设置渐变的颜色和路径比例
        p.setPen(Qt::NoPen);
        p.setBrush(QBrush(pgTopGradient));
        int len = m_MsgPrgInfo.mProgressRect.width()*(100-m_MsgPrgInfo.mProgressValue)/100.0;
        p.drawRoundedRect(m_MsgPrgInfo.mProgressRect.adjusted(0, 0, -len, 0), 15, 15);
        p.restore();
        //>@绘制进度值
        p.save();
        QString tmpValue = QString("%1").arg(m_MsgPrgInfo.mProgressValue);
        QFontMetricsF fm(m_MsgPrgInfo.mPrgTextFont);
        qreal pixelsWide = fm.width(tmpValue);
        qreal pixelsHigh = fm.height();
        p.setFont(m_MsgPrgInfo.mPrgTextFont);
        p.setPen(Qt::white);
        p.drawText(QRect(m_MsgPrgInfo.mProgressRect.left()+m_MsgPrgInfo.mProgressRect.width()/2.0-pixelsWide/2.0,
                         m_MsgPrgInfo.mProgressRect.top()+m_MsgPrgInfo.mProgressRect.height()/2.0-pixelsHigh/2.0,
                         pixelsWide, pixelsHigh), tmpValue);
        p.restore();
    }
    //>@绘制文字
    if(m_MsgPrgInfo.mMessageQueue.count() > 0)
    {
        p.save();
        p.setFont(m_MsgPrgInfo.mMsgTextFont);
        p.setPen(Qt::darkGreen);
        int tmpTextHeight = (qreal)m_MsgPrgInfo.mMessageRect.height()/(qreal)m_MsgPrgInfo.mMaxMsgRow;
        for(int i=0;i<m_MsgPrgInfo.mMessageQueue.count();i++)
        {
            p.drawText(QRect(m_MsgPrgInfo.mMessageRect.left(),
                             m_MsgPrgInfo.mMessageRect.top()+tmpTextHeight*i,
                             m_MsgPrgInfo.mMessageRect.width(),
                             tmpTextHeight),
                       m_MsgPrgInfo.mMessageQueue.at(i));
        }
        p.restore();
    }
#ifdef RGB565  //>@主要是对图像进行像素调整
    if(ConvertImageTo565(tmpBgImage) == false)
        return QPixmap();
#endif
    return QPixmap::fromImage(tmpBgImage);
}

void DeviceManager::paintEvent(QPaintEvent *e)
{
    m_BgPixmap = GetProgressImage();
    if(m_BgPixmap.isNull())
    {
        QWidget::paintEvent(e);
    }
    else
    {
        QPainter p(this);
        p.drawPixmap(rect(), m_BgPixmap);
    }
}

//>@初始化设备列表
void DeviceManager::InitDevices()
{
    QString tmpMdpFile = D_FSRCPATH + D_BOOTDIR + D_BOOTFILE;
    if(!OpenBootMdp(tmpMdpFile))
    {
        UpdateProgress(tr("Open Boot File Failed, Will Use The Back File!"), 100);
        QString tmpDefaultMdpFile = D_FSRCPATH + D_BOOTDIR + D_BOOTFILE;
        if(!CopyFile(tmpDefaultMdpFile, tmpMdpFile))
            tmpMdpFile = tmpDefaultMdpFile;
        if(!OpenBootMdp(tmpMdpFile))
        {
            UpdateProgress(tr("Open Default Boot File Failed, System Exit!"), 100);
            return;
        }
    }
    //>@初始化物理设备
    UpdateProgress(QString("Start Load Physical Devices..."), 0);
    QList<PH_Type> tmpPHList = mMdpManager->m_PHElementGroup.keys();
    int tmpPgDiff = 25.0 / tmpPHList.count();
    for(int i=0;i<tmpPHList.count();i++)
    {
        PH_Type tmpType = tmpPHList.at(i);
        if(tmpType == P_INVALID)
            continue;
        QDomElement tmpElement = mMdpManager->m_PHElementGroup.value(tmpType);
        if(tmpElement.isNull())
            continue;
        UpdateProgress(QString("Loading %1 ...").arg(tmpElement.tagName()), tmpPgDiff*i);
        AddPHDevice(tmpType, tmpElement);
    }

    //>@初始化逻辑设备
    UpdateProgress(QString("Start Load Logic Devices..."), 25);
    QList<LG_Type> tmpLGList = mMdpManager->m_LGElementGroup.keys();
    tmpPgDiff = 25.0 / tmpLGList.count();
    for(int i=0;i<tmpLGList.count();i++)
    {
        LG_Type tmpType = tmpLGList.at(i);
        if(tmpType == L_INVALID)
            continue;
        QDomElement tmpElement = mMdpManager->m_LGElementGroup.value(tmpType);
        if(tmpElement.isNull())
            continue;
        UpdateProgress(QString("Loading %1 ...").arg(tmpElement.tagName()), 25+tmpPgDiff*i);
        AddLGDevice(tmpType, tmpElement);
    }
    UpdateProgress(QString("Load Completed!"), 50);
    //>@初始化监测服务
    OpenDeviceWatcher();
    //>@初始化信号连接
    emit sDeviceInitialized();
}

void DeviceManager::ReleaseDevices()
{
    QList<PH_Type> PHTypeList = m_PHDevList.keys();
    for(int i=0;i<PHTypeList.count();i++)
    {
        DelPHDevice(PHTypeList.at(i));
    }
    QList<LG_Type> LGTypeList = m_LGDevList.keys();
    for(int i=0;i<LGTypeList.count();i++)
    {
        DelLGDevice(LGTypeList.at(i));
    }
}

void DeviceManager::Restart()
{
    QApplication::quit();
}

bool DeviceManager::AddPHDevice(PH_Type pPHType, QDomElement pPHTypeElement, bool pCover)
{
    if(pPHType == P_INVALID || pPHTypeElement.isNull())
        return false;
    //>@查询此物理设备是否已经存在
    if(m_PHDevList.contains(pPHType))
    {
        if(pCover)
            DelPHDevice(pPHType);
        else
            return false;
    }
    PhysicalDevice *tmpPHDev = 0;
    switch(pPHType)
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
        case P_MDISK:
            tmpPHDev = new PhysicalMobileDisk(this);
            break;
        case P_Camera:
            tmpPHDev = new PhysicalCamera(this);
            break;
        case P_GPRS:
            tmpPHDev = new PhysicalGPRS(this);
            break;
        case P_WIFI:
            tmpPHDev = new PhysicalWIFI(this);
            break;
        case P_NFC:
#ifndef N329
            tmpPHDev = new PhysicalNFC(this);
#endif
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
        case P_Bluetooth:
            tmpPHDev = new PhysicalBluetooth(this);
            break;
        case P_LCD:
            tmpPHDev = new PhysicalLCD(this);
            break;
        case P_LED:
            tmpPHDev = new PhysicalLED(this);
            break;
        case P_Audio:
            tmpPHDev = new PhysicalAudio(this);
            break;
        case P_RMII:
            tmpPHDev = new PhysicalRMII(this);
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
        default:return false;
    }
    if(!tmpPHDev)
        return false;
    tmpPHDev->InitDevice(pPHType, pPHTypeElement);
    connect(this, SIGNAL(sDeviceInitialized()), tmpPHDev, SLOT(slot_ProcessConnect()));
    m_PHDevList.insert(pPHType, tmpPHDev);
    return true;
}

bool DeviceManager::DelPHDevice(PH_Type pPHType)
{
    PhysicalDevice *tmpPHDev = m_PHDevList.take(pPHType);
    if(tmpPHDev == 0)
        return true;
    tmpPHDev->deleteLater();
    return true;
}

bool DeviceManager::AddLGDevice(LG_Type pLGType, QDomElement pLGTypeElement, bool pCover)
{
    if(pLGType == L_INVALID || pLGTypeElement.isNull())
        return false;
    //>@查询此物理设备是否已经存在
    if(m_LGDevList.contains(pLGType))
    {
        if(pCover)
            DelLGDevice(pLGType);
        else
            return false;
    }
    //>@
    LogicDevice *tmpLGDev = 0;
    switch(pLGType)
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
        case L_NFCP2P:
            tmpLGDev = new LogicNFCP2P(this);
            break;
        case L_ThemeManager:
            tmpLGDev = new LogicThemeManager(this);
            break;
        case L_Elevator:
            tmpLGDev = new LogicElevator(this);
            break;
        case L_ContentUpdates:
            tmpLGDev = new LogicContentUpgrades(this);
            break;
        case L_NetDiscovery:
            tmpLGDev = new GuestNetDiscovery(this);
            break;
        case L_NetManager:
            tmpLGDev = new GuestNetManager(this);
            break;
        default:return false;
    }
    if(!tmpLGDev)
        return false;
    tmpLGDev->InitDevice(pLGType, pLGTypeElement);
    connect(this, SIGNAL(sDeviceInitialized()), tmpLGDev, SLOT(slot_ProcessConnect()));
    m_LGDevList.insert(pLGType, tmpLGDev);
    return true;
}

bool DeviceManager::DelLGDevice(LG_Type pLGType)
{
    LogicDevice* tmpLGDev = m_LGDevList.take(pLGType);
    if(tmpLGDev == 0)
        return true;
    //>@删除物理设备
    tmpLGDev->deleteLater();
    return true;
}

PhysicalDevice* DeviceManager::GetPHDev(PH_Type pPHType)
{
    return m_PHDevList.value(pPHType);
}

LogicDevice* DeviceManager::GetLGDev(LG_Type pLGType)
{
    return m_LGDevList.value(pLGType);
}

bool DeviceManager::OpenBootMdp(QString pPath)
{
    CloseBootMdp();
    mMdpManager = new mdpManager(pPath);
    if(!mMdpManager->OpenMDP())
    {
        mMdpManager->m_IsChanged = false;
        delete mMdpManager;
        mMdpManager = 0;
        IDE_TRACE_STR("OpenBootMdp#Failed");
        return false;
    }
    return true;
}
void DeviceManager::CloseBootMdp()
{
    if(mMdpManager)
    {
        delete mMdpManager;
        mMdpManager = 0;
    }
}
bool DeviceManager::SaveBootMdp()
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

void DeviceManager::keyPressEvent(QKeyEvent *m_event)
{
    int key = m_event->key();

    switch(key)
    {
    case 'A':
    {
        qDebug()<<"The Pressed Key Is : A     ID Is :"<<key;
        //change THTME to horizontal
        //system("/mnt/nand1-1/ChangeDirection.sh");
        PhysicalLCD *tmpLcd = (PhysicalLCD *)(m_PHDevList.value(P_LCD));
        if(tmpLcd && !tmpLcd->m_ParaElement.isNull())
        {
            QDomElement tmpElement = tmpLcd->m_ParaElement.firstChildElement("Rotation");
            if(!tmpElement.isNull())
            {
                QString tmpValue;
                QString tmpString = tmpElement.text();
                if(tmpString.compare("180") == 0)
                    tmpValue = "270";
                else
                     tmpValue = "180";
                mMdpManager->ModifyItemElement(tmpElement,tmpValue );
            }
        }
        LogicThemeManager *tmpTheme = (LogicThemeManager *)(m_LGDevList.value(L_ThemeManager));
        if(tmpTheme && !tmpTheme->m_ParaElement.isNull())
        {
            QDomElement tmpElement = tmpTheme->m_ParaElement.firstChildElement("Direction");
            if(!tmpElement.isNull())
            {
                QString tmpValue;
                QString tmpString = tmpElement.text();
                if(tmpString.compare("H") == 0)
                    tmpValue = "V";
                else
                     tmpValue = "H";
                mMdpManager->ModifyItemElement(tmpElement,tmpValue );
            }
        }
        mMdpManager->SaveMDP();
        system("reboot");
        break;
    }
    case 'B':
    {
        qDebug()<<"The Pressed Key Is : B     ID Is :"<<key;
        //change THTME to vertical
    }
    case 'C':
    {
        qDebug()<<"The Pressed Key Is : C     ID Is :"<<key;
    }
    case 'D':
    {
        qDebug()<<"The Pressed Key Is : D     ID Is :"<<key;
    }
    default:
        break;
    }
    return;
}
