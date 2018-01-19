#include "physicalparallel.h"

PhysicalSodimasSAF::PhysicalSodimasSAF(QObject *parent) :
    PhysicalDevice(parent)
{
    IDE_TRACE_STR("--------------------------------------------------");
    mDevType = P_SodimasSAF;
    m_MonitorThread = 0;
    m_OldFloor = 0x7fffffff;
    m_File = 0;
    m_File = new QFile;
    m_FloorFunctionCoincide = false;
    m_DooropenSoundEnable = false;
    m_Frame51Type = NONE;
    m_Frame51Freq = 0;
    m_Frame51Timeout = 0;
    m_Frame51Count = 0;
    m_Frame51ChangeTimer = new QTimer(this);
    m_Frame51Timer = new QTimer(this);
    connect(m_Frame51ChangeTimer, SIGNAL(timeout()), this, SLOT(slot_Frame51Change()));
    sendtalk = 0;
    m_NewSgoFlag = 0;
    autotest = 0;
}

PhysicalSodimasSAF::~PhysicalSodimasSAF()
{
    m_Frame51Timer->stop();
    ReleaseMonitor();
}

quint32 PhysicalSodimasSAF::InitPara()
{
    PhysicalDevice::InitPara();
    int count = m_ParaElementList.count();
    if(count < 1)
        return false;
    for(int i=0;i<count;i++)
    {
        QDomElement tmpElement = m_ParaElementList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(!tmpName.compare("Coincide"))
        {
            m_FloorFunctionCoincide = (tmpElement.text().compare("True", Qt::CaseInsensitive))?(false):(true);
        }
        else if(!tmpName.compare("DooropenSound"))
        {
            m_DooropenSoundEnable = (tmpElement.text().compare("True", Qt::CaseInsensitive))?(false):(true);
            GlobalPara.DoorOpenEnable = m_DooropenSoundEnable;
        }
        else if(!tmpName.compare("Frame51"))
        {
            QString tmpType = tmpElement.text();
            if(tmpType.compare("Floor", Qt::CaseInsensitive)==0)
            {
                m_Frame51Type = FLOOR;
            }
            else if(tmpType.compare("Inspection", Qt::CaseInsensitive)==0)
            {
                m_Frame51Type = INSPECTION;
            }
            else
            {
                m_Frame51Type = NONE;
            }
        }
        else if(!tmpName.compare("Frame51Timeout"))
        {
            m_Frame51Timeout = tmpElement.text().toUInt();
        }
        else if(!tmpName.compare("Frame51Freq"))
        {
            m_Frame51Freq = tmpElement.text().toUInt();
        }
    }
    return true;
}

void PhysicalSodimasSAF::UpdatePara(QWidget* pWidget)
{
    QString tmpObjectName = pWidget->objectName();
    for(int i=0;i<m_ParaElementList.count();i++)
    {
        QDomElement tmpElement = m_ParaElementList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(tmpObjectName.compare(tmpName))
            continue;
        if(!tmpName.compare("Coincide"))
        {
            Switch *tmpComponent = qobject_cast<Switch *>(pWidget);
            if(tmpComponent)
            {
                bool down = tmpComponent->isChecked();
                QString tmpString = (down==true)?QString("true"):QString("false");
                if(GlobalPara.m_MdpManager->SetText(tmpElement, tmpString))
                    m_FloorFunctionCoincide = down;
                else
                    tmpComponent->setChecked(!down);
            }
        }
        else if(!tmpName.compare("DooropenSound"))
        {
            Switch *tmpComponent = qobject_cast<Switch *>(pWidget);
            if(tmpComponent)
            {
                bool down = tmpComponent->isChecked();
                if(GlobalPara.m_MdpManager->SetText(tmpElement, (down)?QString("true"):QString("false")))
                    m_DooropenSoundEnable = down;
                else
                    tmpComponent->setChecked(!down);
            }
        }
        else if(!tmpName.compare("Frame51"))
        {
            ComboBox *tmpComponent = static_cast<ComboBox *>(pWidget);
            if(tmpComponent)
            {
                QString tmpType = tmpComponent->currentText();
                if(tmpType.compare("Floor", Qt::CaseInsensitive)==0)
                {
                    if(GlobalPara.m_MdpManager->SetText(tmpElement, tmpType))
                    {
                        m_Frame51Type = FLOOR;
                    }
                    else
                    {
                        tmpComponent->setCurrentIndex(m_Frame51Type);
                    }
                }
                else if(tmpType.compare("Inspection", Qt::CaseInsensitive)==0)
                {
                    if(GlobalPara.m_MdpManager->SetText(tmpElement, tmpType))
                    {
                        m_Frame51Type = INSPECTION;
                    }
                    else
                    {
                        tmpComponent->setCurrentIndex(m_Frame51Type);
                    }
                }
                else
                {
                    if(GlobalPara.mdpManager->SetText(tmpElement, tmpType))
                    {
                        m_Frame51Type = NONE;
                        m_Frame51Count = 0;
                        if(m_Frame51Timer->isActive())
                            m_Frame51Timer->stop();
                        if(m_Frame51ChangeTimer->isActive())
                            m_Frame51ChangeTimer->stop();
                    }
                    else
                    {
                        tmpComponent->setCurrentIndex(m_Frame51Type);
                    }
                }
            }
        }
        else if(!tmpName.compare("Frame51Timeout"))
        {
            SpinEdit *tmpComponent = static_cast<SpinEdit *>(pWidget);
            if(tmpComponent)
            {
                if(GlobalPara.m_MdpManager->SetText(tmpElement, tmpComponent->text()))
                {
                    m_Frame51Timeout = tmpComponent->value();
                }
                else
                {
                    tmpComponent->setValue(m_Frame51Timeout);
                }
            }
        }
        else if(!tmpName.compare("Frame51Freq"))
        {
            SpinEdit *tmpComponent = static_cast<SpinEdit *>(pWidget);
            if(tmpComponent)
            {
                if(GlobalPara.m_MdpManager->SetText(tmpElement, tmpComponent->text()))
                {
                    m_Frame51Freq = tmpComponent->value();
                }
                else
                {
                    tmpComponent->setValue(m_Frame51Freq);
                }
            }
        }
        break;
    }
}

bool PhysicalSodimasSAF::open(OpenMode mode)
{
    if (m_File)
    {
        /*open the port*/
        m_File->setFileName(QString("/dev/hmi"));
        if (m_File->open(QIODevice::ReadWrite|QIODevice::Unbuffered))
        {
            /*set open mode*/
            QIODevice::open(mode);
            StartMonitor();
        }
        else
        {
            qDebug("Could not open File! Error code : %d", m_File->error());
        }
    }
    return isOpen();
}

void PhysicalSodimasSAF::close()
{
    if(isOpen())
        m_File->close();
    QIODevice::close();
}

void PhysicalSodimasSAF::Monitor()
{
    if(m_File)
    {
#if 1
        QByteArray tmpByteArray = m_File->read(12);
        //QString tmpData  = QString("+++++++++ SrcCode : 0x%1  0x%2  0x%3  0x%4  0x%5  0x%6  0x%7  0x%8  0x%9  0x%10  0x%11  +++++++++++").arg(tmpByteArray.at(1),2,QLatin1Char('0')).arg(tmpByteArray.at(2),2,QLatin1Char('0')).arg(tmpByteArray.at(3),2,QLatin1Char('0')).arg(tmpByteArray.at(4),2,QLatin1Char('0')).arg(tmpByteArray.at(5),2,QLatin1Char('0')).arg(tmpByteArray.at(6),2,QLatin1Char('0')).arg(tmpByteArray.at(7),2,QLatin1Char('0')).arg(tmpByteArray.at(8),2,QLatin1Char('0')).arg(tmpByteArray.at(9),2,QLatin1Char('0')).arg(tmpByteArray.at(10),2,QLatin1Char('0')).arg(tmpByteArray.at(11),2,QLatin1Char('0'));
//        IDE_TRACE_STR(tmpData);
        //GlobalPara.m_Logger->info(tmpData);
        //printf("+++++++++ SrcCode : 0x%2x  0x%2x  0x%2x  0x%2x  0x%2x  0x%2x  0x%2x  0x%2x  0x%2x  0x%2x  0x%2x  +++++++++++\r\n",(int)tmpByteArray.at(1),(int)tmpByteArray.at(2),(int)tmpByteArray.at(3),(int)tmpByteArray.at(4),(int)tmpByteArray.at(5),(int)tmpByteArray.at(6),(int)tmpByteArray.at(7),(int)tmpByteArray.at(8),(int)tmpByteArray.at(9),(int)tmpByteArray.at(10),(int)tmpByteArray.at(11));
#else
        //IDE_TRACE_INT(autotest);
        QtSleep(2000);
        QByteArray tmpByteArray;
        quint8 floor;
        quint8 arrow;
        quint8 overflow;

        if(autotest == 0)
        {
            floor = 0x01;
            arrow = 0x09;
            autotest = 1;
        }
        else if(autotest == 1)
        {
            floor = 0x02;
            arrow = 0x09;
            autotest = 2;
        }
        else if(autotest == 2)
        {
            floor = 0x03;
            arrow = 0x09;
            autotest = 3;
        }
        else if(autotest == 3)
        {
            overflow = 0x40;
            autotest = 4;
        }
        else if(autotest == 4)
        {
            arrow = 0x05;
            floor = 0x03;
            autotest = 5;
        }
        else if(autotest == 5)
        {
            floor = 0x02;
            arrow = 0x05;
            autotest  = 6;
        }
        else if(autotest == 6)
        {
            floor = 0x01;
            arrow = 0x05;
            autotest = 0;
        }
        else
        {
            floor = 0x01;
            arrow = 0x09;
            autotest = 1;
        }
        //IDE_TRACE_INT(floor);
        //IDE_TRACE_INT(arrow);
        //IDE_TRACE_INT(overflow);
        tmpByteArray.append((char)0x80);
        tmpByteArray.append((char)0x00);            //data 1
        tmpByteArray.append((char)0x00);           //data 2
        tmpByteArray.append((char)(floor));                  //data 3
        tmpByteArray.append((char)(arrow));                   //data 4      //arrow+sgo
        tmpByteArray.append((char)(overflow));                   //data 5  //0x04
        tmpByteArray.append((char)0x40);                         //data 6
        tmpByteArray.append((char)0x00);
        tmpByteArray.append((char)0x00);
        tmpByteArray.append((char)0x00);
        tmpByteArray.append((char)0x00);
        tmpByteArray.append((char)0x81);
#endif
        if(ConvertProtocol(tmpByteArray))
            emit readyRead();
    }
}

void PhysicalSodimasSAF::slot_Frame51()
{
    //    IDE_TRACE();
        if(m_Frame51ChangeTimer->isActive())
            m_Frame51ChangeTimer->stop();
        m_Frame51ChangeTimer->start(m_Frame51Freq);
    //    IDE_TRACE();
}


void PhysicalSodimasSAF::slot_Frame51Change()
{
    if(m_Frame51Type == NONE)
    {
        m_Frame51Count = 0;
        if(m_Frame51ChangeTimer->isActive())
            m_Frame51ChangeTimer->stop();
        return;
    }
    m_Frame51Count ++;

    quint8 baiwei = 0, shiwei = 0, gewei = 0, basicfun=0, func1=0, func2=0, func3=0;
    if(m_Frame51Type == FLOOR)
    {
        if(m_Frame51Count%2)       //>@ÏÔÊŸLOGO
        {
            func1 = 0;
            func2 = (1<<6);
            func3 = 0;
        }
        else                       //>@ÏÔÊŸÖ®Ç°ÏÔÊŸ?ýµÄÂ??ã
        {
            shiwei = (m_OldFloor >> 8)&0x0000007f;
            gewei = (m_OldFloor)&0x0000007f;
        }
    }
    else if(m_Frame51Type == INSPECTION)
    {
        if(m_Frame51Count%2)       //>@ÏÔÊŸLOGO
        {
            func1 = (1<<3);
            func2 = 0;
            func3 = 0;
        }
        else                       //>@ÏÔÊŸInspection
        {
            func1 = 0;
            func2 = (1<<6);
            func3 = 0;
        }
    }
    quint8 check    = 0x00 ^ baiwei ^ shiwei ^ gewei ^ basicfun ^ func1 ^ func2 ^ func3 & 0X7F;

    m_ReadByteArray.clear();
    m_ReadByteArray.append((char)0x80);
    m_ReadByteArray.append((char)0x00);       //data 1
    m_ReadByteArray.append((char)baiwei);     //data 2
    m_ReadByteArray.append((char)shiwei);     //data 3
    m_ReadByteArray.append((char)gewei);      //data 4
    m_ReadByteArray.append((char)basicfun);   //data 5
    m_ReadByteArray.append((char)func1);      //data 6
    m_ReadByteArray.append((char)func2);      //data 7
    m_ReadByteArray.append((char)func3);      //data 8
    m_ReadByteArray.append((char)check);
    m_ReadByteArray.append((char)0x81);

    emit readyRead();
}

bool PhysicalSodimasSAF::ConvertProtocol(QByteArray pByteArray)
{
    volatile int tmpCount = pByteArray.count();
    if(tmpCount == 0)
    {
        if(m_Frame51Type != NONE)
        {
            IDE_TRACE();
            m_Frame51Count = 0;
            if(m_Frame51ChangeTimer->isActive())
            {
                m_Frame51ChangeTimer->stop();
            }
            if(m_Frame51Timer->isActive())
            {
                m_Frame51Timer->stop();
            }
            m_Frame51Timer->singleShot(m_Frame51Timeout,this,SLOT(slot_Frame51()));
        }
    }

    if(tmpCount < 12)
    {
        IDE_TRACE_INT(tmpCount);
        return false;
    }
    volatile quint8 tmpValue;
    volatile int i=0;
    for(;i<tmpCount;i++)
    {
        tmpValue = (quint8)(pByteArray.at(i));
        if(tmpValue == (quint8)(0x80))
        {
            break;
        }
    }
    if((tmpCount - i )< 12)
        return false;

    volatile quint8 baiwei = 0, shiwei = 0, gewei = 0, basicfun=0, func1=0, func2=0, func3=0;
    shiwei   = (quint8)pByteArray.at(i+2) & 0x7f;
    gewei    = (quint8)pByteArray.at(i+3) & 0x7f;

    volatile bool floorchanged = false;
    //>@sodimas»ù±¾ÉÏÃ»ÓÐ°ÙÎ»Â¥²ã£¬Òò´Ë´Ë´¦Ö±½ÓÈÏÎªµÚ¶þ×Ö½ÚÎª¹¦ÄÜ×Ö½Ú
    //>@Èç¹ûÂ¥²ãÓÐ±ä»¯£¬Ôò²»±Ø²é¿´¹¦ÄÜÁË£¬Ö±½Ó·¢ËÍÂ¥²ã¸Ä±äµÄÐÅºÅ£¬²¢Ê¹¹¦ÄÜÇøÓòÇåÁã

    //IDE_TRACE_INT(m_FloorFunctionCoincide);

    volatile  quint32 tmpNewFloor = (shiwei<<8) + gewei;
    if(tmpNewFloor != m_OldFloor)
    {
        sendtalk = 0;
        m_OldFloor = tmpNewFloor;
        if(m_FloorFunctionCoincide)
        {
            floorchanged = true;
            func1=0, func2=0, func3=0;
        }
    }

    tmpValue     = (quint8)pByteArray.at(i+10);
    m_NewSgoFlag = tmpValue&0x01;

    volatile  quint8 Battery = 0, DigiCode = 0, doorclose = 0, dooropen = 0, arrive = 0, talk = 0;
    if(!floorchanged)
    {
        tmpValue     = (quint8)pByteArray.at(i+1);
        Battery      = (tmpValue>>1) & 0x01;
        DigiCode     = tmpValue & 0x01;

        doorclose = (tmpValue >>5 ) & 0x01; //59fps
    }

    tmpValue            = (quint8)pByteArray.at(i+4);
    arrive = tmpValue & 0x01;
    if(m_DooropenSoundEnable)
    {
        dooropen = arrive;
    }

    volatile quint8 down         = (tmpValue>>2) & 0x01;
    volatile quint8 up           = (tmpValue>>3) & 0x01;
    volatile quint8 run          = (tmpValue>>1) & 0x01;
    volatile quint8 cellute, forceclose, accessforbid;

    if(!floorchanged)
    {
        cellute      = (tmpValue>>4) & 0x01;
        forceclose   = (tmpValue>>5) & 0x01;
        accessforbid = (tmpValue>>6) & 0x01;
    }

    volatile quint8 smoke, earthquick, overload, fullload, maintenance, fire;
    if(!floorchanged)
    {
        tmpValue = (quint8)pByteArray.at(i+5);
        smoke        = (tmpValue) & 0x01;
        earthquick   = (tmpValue>>1) & 0x01;
        overload     = (tmpValue>>2) & 0x01;
        fullload     = (tmpValue>>3) & 0x01;
        maintenance  = (tmpValue>>4) & 0x01;
        fire         = (tmpValue>>5) & 0x01;
    }

    tmpValue = (quint8)pByteArray.at(i+6);
    volatile quint8  liftier, fireService;
    if(!floorchanged)
    {
        liftier         = (tmpValue>>3) & 0x01;
        fireService     = (tmpValue>>3) & 0x01;
    }

    if(sendtalk == 0)   //>@talk can only read once
    {
        talk = (tmpValue>>6) & 0x01;
        if(talk == 1)
        {
            sendtalk = 1;
        }
    }
    else
    {
        if(doorclose)
        {
            talk = (tmpValue>>6) & 0x01;
        }
        else
        {
            talk = 0;
        }
    }
    IDE_TRACE_INT(talk);
    IDE_TRACE_INT(arrive);
    basicfun = (talk<<4) + (arrive<<3) + (run<<2) + (down<<1) + up;
    if(!floorchanged)
    {
        func1    = cellute+ (DigiCode<<1)+ (Battery<<2)+ (maintenance<<3) + (accessforbid<<4) + (overload<<5) + (fullload<<6);
        func2    = (liftier<<1) + (fire<<2)+( forceclose<<3)+ (smoke<<5);
        func3    = doorclose /*+ (dooropen<<1) */;
    }

    if(!floorchanged)
    {
        volatile quint32 tmpNewFunction = (func3<<16) + (func2<<8) + func1;
        if(tmpNewFunction != m_OldFunction)
        {
            m_OldFunction = tmpNewFunction;
            if(tmpNewFunction)
            {
                if(m_FloorFunctionCoincide)
                {
                    if(!(doorclose || dooropen))
                    {
                        shiwei = 0x7f;
                        gewei = 0x7f;
                    }
                }
            }
            if(dooropen)
                func3 = func3 | (dooropen<<1);
            else
                func3 = func3 & (~(dooropen<<1));
        }
        else
        {
            tmpValue = (quint8)pByteArray.at(i+9);
            volatile quint8 tmpfloorChanged = tmpValue&0x01;
            volatile quint8 tmpFuncChanged = tmpValue&0x04;
            if(tmpfloorChanged)
            {
                shiwei = (m_OldFloor >> 8) & 0x0000007f;
                gewei = (m_OldFloor) & 0x0000007f;
                func1=0, func2=0, func3=0;
            }
            else if(tmpFuncChanged)
            {
                if(tmpNewFunction)
                {
                    if(m_FloorFunctionCoincide)
                    {
                        if(!(doorclose || dooropen))
                        {
                            shiwei = 0x7f;
                            gewei = 0x7f;
                        }
                    }
                }
                if(dooropen)
                    func3 = func3 | (dooropen<<1);
                else
                    func3 = func3 & (~(dooropen<<1));
            }
            else
            {
                 shiwei = m_ReadByteArray.at(3);
                 gewei = m_ReadByteArray.at(4);
                 func1 = m_ReadByteArray.at(6);
                 func2 = m_ReadByteArray.at(7);
                 if(dooropen)
                     func3 = m_ReadByteArray.at(8) | (dooropen<<1);
                 else
                     func3 = m_ReadByteArray.at(8) & (~(dooropen<<1));
            }
        }
    }

    //QString tmpData  = QString("+++++++++ DstCode :baiwei:0x%1  shiwei:0x%2  gewei:0x%3  basicfun:0x%4  func1:0x%5  func2:0x%6  func3:0x%7 ++++++++++").arg(baiwei,2,QLatin1Char('0')).arg(shiwei,2,QLatin1Char('0')).arg(gewei,2,QLatin1Char('0')).arg(basicfun,2,QLatin1Char('0')).arg(func1,2,QLatin1Char('0')).arg(func2,2,QLatin1Char('0')).arg(func3,2,QLatin1Char('0'));
    //IDE_TRACE_STR(tmpData);
    //GlobalPara.m_Logger->info(tmpData);

    //printf("+++++++++ DstCode :baiwei:0x%2x shiwei:0x%2x gewei:0x%2x basicfun:0x%2x func1:0x%2x func2:0x%2x func3:0x%2x ++++++++++\r\n",(int)baiwei,(int)shiwei,(int)gewei,(int)basicfun,(int)func1,(int)func2,(int)func3) ;

    volatile quint8 check    = 0x00 ^ baiwei ^ shiwei ^ gewei ^ basicfun ^ func1 ^ func2 ^ func3 & 0X7F;

    m_ReadByteArray.clear();
    m_ReadByteArray.append((char)0x80);
    m_ReadByteArray.append((char)0x00);       //data 1
    m_ReadByteArray.append((char)baiwei);     //data 2
    m_ReadByteArray.append((char)shiwei);     //data 3
    m_ReadByteArray.append((char)gewei);      //data 4
    m_ReadByteArray.append((char)basicfun);   //data 5
    m_ReadByteArray.append((char)func1);      //data 6
    m_ReadByteArray.append((char)func2);      //data 7
    m_ReadByteArray.append((char)func3);      //data 8
    m_ReadByteArray.append((char)check);
    m_ReadByteArray.append((char)0x81);

    return true;
}

QByteArray PhysicalSodimasSAF::readAll()
{
    return m_ReadByteArray;
}

void PhysicalSodimasSAF::StartMonitor()
{
    ReleaseMonitor();
    m_MonitorThread = new SAFMonitorThread(this);
    m_MonitorThread->start(QThread::LowPriority);
}

void PhysicalSodimasSAF::ReleaseMonitor()
{
    if(m_MonitorThread != 0)
    {
        StopMonitor();
        m_MonitorThread->quit();
        m_MonitorThread->deleteLater();
        m_MonitorThread = 0;
    }
}

void PhysicalSodimasSAF::StopMonitor()
{
    if(m_MonitorThread != 0)
    {
        if (mMonitorThread->isRunning())
        {
            mMonitorThread->Stop();
            if (QThread::currentThread() != mMonitorThread)
                mMonitorThread->wait();
        }
    }
}

