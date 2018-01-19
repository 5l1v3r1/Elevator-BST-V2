#include "physicalparallel.h"

PhysicalSodimasSAF::PhysicalSodimasSAF(QObject *parent) :
    PhysicalDevice(parent)
{
    mMonitorThread = 0;
    mOldFloor = 0x7fffffff;
    mFile = 0;
    mFile = new QFile;
    mFloorFunctionCoincide = false;
    mDooropenSoundEnable = false;
    mFrame51Type = NONE;
    mFrame51Freq = 0;
    mFrame51Count = 0;
    connect(&mFrame51Timer, SIGNAL(timeout()), this, SLOT(slot_Frame51()));
}

PhysicalSodimasSAF::~PhysicalSodimasSAF()
{
    mFrame51Timer.stop();
    ReleaseMonitor();
}

quint32 PhysicalSodimasSAF::InitPara()
{
    QDomNodeList tmpParaNodeList = m_ParaElement.childNodes();
    if(tmpParaNodeList.count() < 1)
        return 0;
    quint32 validcount = 0;
    QHBoxLayout* tmpHBoxLayout;
    for(int i=0;i<tmpParaNodeList.count()-1;i++)
    {
        QDomElement tmpElement = tmpParaNodeList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(!tmpName.compare("Coincide"))
        {
            QString tmpValue = tmpElement.text();
            mFloorFunctionCoincide = (tmpValue.compare("true", Qt::CaseInsensitive))?(false):(true);
            tmpHBoxLayout = CreateBool(tmpName, tmpValue);
        }
        else if(!tmpName.compare("DooropenSound"))
        {
            QString tmpValue = tmpElement.text();
            mDooropenSoundEnable = (tmpValue.compare("true", Qt::CaseInsensitive))?(false):(true);
            tmpHBoxLayout = CreateBool(tmpName, tmpValue);
        }
        else if(!tmpName.compare("Frame51"))
        {
            QString tmpType = tmpElement.text();
            mFrame51Type = GetFrame51Type(tmpType);
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), tmpType);
        }
        else if(!tmpName.compare("Frame51Timeout"))
        {
            mFrame51Timeout = tmpElement.text().toInt();
            QStringList tmpValueList = tmpElement.attribute("List").split(';');
            if(tmpValueList.count()!=2)
            {
                mFrame51Timeout = 0;
                mMinFrame51Timeout = 0;
                mMaxFrame51Timeout = 0;
            }
            else
            {
                mMinFrame51Timeout = tmpValueList.at(0).toUInt();
                mMaxFrame51Timeout = tmpValueList.at(1).toUInt();
                if(mFrame51Timeout < mMinFrame51Timeout || mFrame51Timeout > mMaxFrame51Timeout)  //>@判断值是否有效
                    mFrame51Timeout = 0;
            }
        }
        else if(!tmpName.compare("Frame51Freq"))
        {
            mFrame51Freq = tmpElement.text().toInt();
            QStringList tmpValueList = tmpElement.attribute("List").split(';');
            if(tmpValueList.count()!=2)
            {
                mFrame51Freq = 0;
                mMinFrame51Freq = 0;
                mMaxFrame51Freq = 0;
            }
            else
            {
                mMinFrame51Freq = tmpValueList.at(0).toUInt();
                mMaxFrame51Freq = tmpValueList.at(1).toUInt();
                if(mFrame51Freq < mMinFrame51Freq || mFrame51Freq > mMaxFrame51Freq)  //>@判断值是否有效
                    mFrame51Freq = 0;
            }
        }
        else
        {
            continue;
        }
        validcount++;
        m_SettingGroup->m_VLayout->addWidget(CreateLine());
        m_SettingGroup->m_VLayout->addLayout(tmpHBoxLayout);
    }
    //>@后处理
    open(QIODevice::ReadWrite);
    return validcount;
}

void PhysicalSodimasSAF::UpdatePara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("Coincide"))
    {
        Switch *tmpComponent = qobject_cast<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, tmpElement, mFloorFunctionCoincide, tmpComponent->isChecked()))
            {
            }
        }
    }
    else if(!tmpName.compare("DooropenSound"))
    {
        Switch *tmpComponent = qobject_cast<Switch *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyBool(tmpComponent, tmpElement, mDooropenSoundEnable, tmpComponent->isChecked()))
            {
            }
        }
    }
    else if(!tmpName.compare("Frame51"))
    {
        ComboBox *tmpComponent = qobject_cast<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpFrame51Type;
            if(!ModifyEnum(tmpComponent, tmpElement, tmpFrame51Type, tmpComponent->currentText()))
            {
            }
            else
            {
                mFrame51Type = GetFrame51Type(tmpFrame51Type);
            }
        }
    }
    else if(!tmpName.compare("Frame51Timeout"))
    {
        SpinEdit *tmpComponent = qobject_cast<SpinEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyNum(tmpComponent, tmpElement, mFrame51Timeout, tmpComponent->value(), mMinFrame51Timeout, mMaxFrame51Timeout))
            {
            }
        }
    }
    else if(!tmpName.compare("Frame51Freq"))
    {
        SpinEdit *tmpComponent = qobject_cast<SpinEdit *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyNum(tmpComponent, tmpElement, mFrame51Freq, tmpComponent->value(), mMinFrame51Freq, mMaxFrame51Freq))
            {
            }
        }
    }
}

bool PhysicalSodimasSAF::open(OpenMode mode)
{
    if (mFile)
    {
        /*open the port*/
        mFile->setFileName(QString("/dev/hmi"));
        if (
#ifndef SAFTEST
                mFile->open(QIODevice::ReadWrite|QIODevice::Unbuffered)
#else
                1
#endif
                )
        {
            /*set open mode*/
            QIODevice::open(mode);
            StartMonitor();
        }
        else
        {
            qDebug("Could not open File! Error code : %d", mFile->error());
        }
    }
    return isOpen();
}

void PhysicalSodimasSAF::close()
{
    if(isOpen())
        mFile->close();
    QIODevice::close();
}

void PhysicalSodimasSAF::Monitor()
{
    if(mFile)
    {
#ifndef SAFTEST
        QByteArray tmpByteArray = mFile->read(12);
#else
        QtSleep(1000);
        QByteArray tmpByteArray;
        static quint32 floor = 0;
        static quint32 function = 1;
        static quint32 i = 0;
        if(i++%2 == 0)
        {
            floor++;
            if(floor > 70)
                floor = 0;
        }
        else
        {
            function <<= 1;
        }
        tmpByteArray.append((char)0x80);
        tmpByteArray.append((char)(function>>24));     //data 2
        tmpByteArray.append((char)(0));                 //data 3
        tmpByteArray.append((char)(floor/10));      //data 4
        tmpByteArray.append((char)(floor%10));      //data 5
        tmpByteArray.append((char)(function));      //data 6
        tmpByteArray.append((char)(function>>8));      //data 7
        tmpByteArray.append((char)(function>>16));      //data 8
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
    if(mFrame51Type == NONE)
    {
        mFrame51Count = 0;
        mFrame51Timer.stop();
        return;
    }
    mFrame51Count ++;

    quint8 baiwei = 0, shiwei = 0, gewei = 0, basicfun=0, func1=0, func2=0, func3=0;
    if(mFrame51Type == FLOOR)
    {
        if(mFrame51Count%2)       //>@显示LOGO
        {
            func1 = 0;
            func2 = (1<<6);
            func3 = 0;
        }
        else                       //>@显示之前显示过的楼层
        {
            shiwei = (mOldFloor >> 8)&0x0000007f;
            gewei = (mOldFloor)&0x0000007f;
        }
    }
    else if(mFrame51Type == INSPECTION)
    {
        if(mFrame51Count%2)       //>@显示LOGO
        {
            func1 = (1<<3);
            func2 = 0;
            func3 = 0;
        }
        else                       //>@显示Inspection
        {
            func1 = 0;
            func2 = (1<<6);
            func3 = 0;
        }
    }
    quint8 check    = 0x00 ^ baiwei ^ shiwei ^ gewei ^ basicfun ^ func1 ^ func2 ^ func3 & 0X7F;

    mReadByteArray.clear();
    mReadByteArray.append((char)0x80);
    mReadByteArray.append((char)0x00);       //data 1
    mReadByteArray.append((char)baiwei);     //data 2
    mReadByteArray.append((char)shiwei);     //data 3
    mReadByteArray.append((char)gewei);      //data 4
    mReadByteArray.append((char)basicfun);   //data 5
    mReadByteArray.append((char)func1);      //data 6
    mReadByteArray.append((char)func2);      //data 7
    mReadByteArray.append((char)func3);      //data 8
    mReadByteArray.append((char)check);
    mReadByteArray.append((char)0x81);

    emit readyRead();
}

bool PhysicalSodimasSAF::ConvertProtocol(QByteArray pByteArray)
{
    int tmpCount = pByteArray.count();
    if(tmpCount < 12)
        return false;
    quint8 tmpValue;
    int i=0;
    for(;i<tmpCount;i++)
    {
        tmpValue = (quint8)(pByteArray.at(i));
        if(tmpValue == (quint8)(0x80))
        {
            break;
        }
    }
    if(tmpCount - i < 12)
        return false;

    //>@如果m_Frame51Freq时间内没有新的帧到来，则执行Frame51的功能
    if(mFrame51Type != NONE)
    {
        mFrame51Count = 0;
        mFrame51Timer.stop();
        mFrame51Timer.start(mFrame51Freq);
    }

    quint8 baiwei = 0, shiwei = 0, gewei = 0, basicfun=0, func1=0, func2=0, func3=0;
    shiwei   = (quint8)pByteArray.at(i+2) & 0x7f;
    gewei    = (quint8)pByteArray.at(i+3) & 0x7f;

    bool floorchanged = false;
    //>@sodimas基本上没有百位楼层，因此此处直接认为第二字节为功能字节
    //>@如果楼层有变化，则不必查看功能了，直接发送楼层改变的信号，并使功能区域清零
    quint32 tmpNewFloor = (shiwei<<8) + gewei;
    if(tmpNewFloor != mOldFloor)
    {
        mOldFloor = tmpNewFloor;
        if(mFloorFunctionCoincide)
        {
            floorchanged = true;
            func1=0, func2=0, func3=0;
        }
    }

    quint8 Battery = 0, DigiCode = 0, doorclose = 0, dooropen = 0, arrive = 0, talk = 0;
    if(!floorchanged)
    {
        tmpValue     = (quint8)pByteArray.at(i+1);
        Battery      = (tmpValue>>1) & 0x01;
        DigiCode     = tmpValue & 0x01;

        doorclose = (tmpValue >>5 ) & 0x01;
    }

    tmpValue            = (quint8)pByteArray.at(i+4);
    arrive = tmpValue & 0x01;

    if(mDooropenSoundEnable)
    {
        dooropen = arrive;
    }

    quint8 down         = (~(tmpValue>>2)) & 0x01;
    quint8 up           = (~(tmpValue>>3)) & 0x01;
    quint8 run          = (tmpValue>>1) & 0x01;
    quint8 cellute, forceclose, accessforbid;
    if(!floorchanged)
    {
        cellute      = (tmpValue>>4) & 0x01;
        forceclose   = (tmpValue>>5) & 0x01;
        accessforbid = (tmpValue>>6) & 0x01;
    }

    quint8 smoke, earthquick, overload, fullload, maintenance, fire;
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
    quint8  EmergencyCalls, LockedCall, StopServer, liftier, fireService;
    if(!floorchanged)
    {
        EmergencyCalls  = tmpValue & 0x01;
        LockedCall      = (tmpValue>>1) & 0x01;
        StopServer      = (tmpValue>>2) & 0x01;
        liftier         = (tmpValue>>3) & 0x01;
        fireService     = (tmpValue>>3) & 0x01;
    }

    if(floorchanged)   //>@talk only valid on the state of floor changed!
        talk = (tmpValue>>6) & 0x01;
    else
        talk = 0;

    //>@basicfun与楼层、功能是同步获取的，因此需要正常解析基础功能区域
    basicfun = (talk<<4) + (arrive<<3) + (run<<2) + (down<<1) + up;
    if(!floorchanged)
    {
        func1    = cellute+ (DigiCode<<1)+ (Battery<<2)+ (maintenance<<3) + (accessforbid<<4) + (overload<<5) + (fullload<<6);
        func2    = EmergencyCalls +(liftier<<1) + (fire<<2)+( forceclose<<3)+ (smoke<<5);
        func3    = doorclose + (dooropen<<1) ;
    }

    quint32 tmpNewFunction = (func3<<16) + (func2<<8) + func1;
    if(tmpNewFunction != mOldFunction)  //>@如果楼层没有变化，则直接是楼层区域清零
    {
        mOldFunction = tmpNewFunction;
        if(mFloorFunctionCoincide)
        {
            if(!floorchanged)
            {
                if(!(doorclose || dooropen))
                {
                    baiwei = 0x7f, shiwei = 0x7f, gewei = 0x7f;
                }
            }
        }
    }

    quint8 check    = 0x00 ^ baiwei ^ shiwei ^ gewei ^ basicfun ^ func1 ^ func2 ^ func3 & 0X7F;

    mReadByteArray.clear();
    mReadByteArray.append((char)0x80);
    mReadByteArray.append((char)0x00);       //data 1
    mReadByteArray.append((char)baiwei);     //data 2
    mReadByteArray.append((char)shiwei);     //data 3
    mReadByteArray.append((char)gewei);      //data 4
    mReadByteArray.append((char)basicfun);   //data 5
    mReadByteArray.append((char)func1);      //data 6
    mReadByteArray.append((char)func2);      //data 7
    mReadByteArray.append((char)func3);      //data 8
    mReadByteArray.append((char)check);
    mReadByteArray.append((char)0x81);

    return true;
}

QByteArray PhysicalSodimasSAF::readAll()
{
    return mReadByteArray;
}

void PhysicalSodimasSAF::StartMonitor()
{
    ReleaseMonitor();
    mMonitorThread = new SAFMonitorThread(this);
    mMonitorThread->start(QThread::LowPriority);
}

void PhysicalSodimasSAF::ReleaseMonitor()
{
    if(mMonitorThread != 0)
    {
        StopMonitor();
        mMonitorThread->quit();
        mMonitorThread->deleteLater();
        mMonitorThread = 0;
    }
}

void PhysicalSodimasSAF::StopMonitor()
{
    if(mMonitorThread != 0)
    {
        if (mMonitorThread->isRunning())
        {
            mMonitorThread->Stop();
            if (QThread::currentThread() != mMonitorThread)
                mMonitorThread->wait();
        }
    }
}

