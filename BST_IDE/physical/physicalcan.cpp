#include "physicalcan.h"

PhysicalCAN::PhysicalCAN(QObject *parent = 0) :
    PhysicalDevice(parent)
{
    m_Old_Check = 0;
    m_CanData = new PhysicalSerial(0);
    m_CanData->OpenSerial(0,BAUD115200);
    connect(m_CanData,SIGNAL(readyRead()),this,SLOT(Monitor()));
}

bool PhysicalCAN::open(OpenMode mode)
{
    bool flag = m_CanData->open(mode);//QIODevice::ReadOnly
    if(flag)
    {
        IDE_TRACE_STR("Open Serial Succeeded!");
        return true;
    }
    return false;
}

void PhysicalCAN::close()
{
    if(m_CanData)
    {
        m_CanData->close();
    }
}

QByteArray PhysicalCAN::readAll()
{
        return m_CanByteArray;
}

void PhysicalCAN::Monitor()
{
    QByteArray tmpByteArray = m_CanData->readAll();
    if(!tmpByteArray.isEmpty())
    {
       if( ConvertProtocol(tmpByteArray))
       {
           emit readyRead();
           QtSleep(50);
       }
    }
    tmpByteArray.clear();
}

bool PhysicalCAN::ConvertProtocol(QByteArray pByteArray)
{
    int tmpCount = pByteArray.count();
    if(tmpCount < 11)
    {
        return false;
    }
    quint8 tmpValue;
    int i=0,flag=0;
    for(;i<tmpCount;i++)
    {
        tmpValue = (quint8)(pByteArray.at(i));
        if(tmpValue == (quint8)(0x80) )
        {
            tmpValue = (quint8)(pByteArray.at(i+10));// Floor Frame
            if(tmpValue == (quint8)(0x81))
            {
                break;
            }
        }
    }
    if(tmpCount - i < 10)
    {
        return false;
    }

    quint8 CanData1,CanData2,CanData3,CanData4,CanData5,CanData6,CanData7,CanData8,CanCheck;
    CanData1 = (quint8)pByteArray.at(i+1) & 0x7f;
    CanData2 = (quint8)pByteArray.at(i+2) & 0x7f;       //baiwei
    CanData3 = (quint8)pByteArray.at(i+3) & 0x7f;       //shiwei
    CanData4 = (quint8)pByteArray.at(i+4) & 0x7f;       //gewei
    CanData5 = (quint8)pByteArray.at(i+5) & 0x7f;       //basic func
    CanData6 = (quint8)pByteArray.at(i+6) & 0x7f;       //func
    CanData7 = (quint8)pByteArray.at(i+7) & 0x7f;       //sound
    CanData8 = (quint8)pByteArray.at(i+8) & 0x7f;       //AreaChange
    CanCheck = (quint8)pByteArray.at(i+9) & 0x7f;      //check
    quint8 m_Check = CanData1^CanData2^CanData3^CanData4^CanData5^CanData6^CanData7^CanData8&0x7f;
    if(CanCheck !=m_Check)
    {
        IDE_TRACE_STR("Received Wrong Data!");
        return false;
    }
    if(m_Old_Check == CanCheck)
    {
        return false;
    }
    else
    {
        m_Old_Check = CanCheck;
    }

    quint8 baiwei = 0, shiwei = 0, gewei = 0,basicfun = 0,func = 0,sound = 0,AreaChange;

    baiwei      =   CanData2;
    shiwei      =   CanData3;
    gewei       =   CanData4;

    basicfun = CanData5;
    func = CanData6;
    sound = CanData7;
    AreaChange = CanData8 & 0x03;

    if(gewei == 0)
    {
        gewei = shiwei;
        shiwei = baiwei;
    }

    baiwei = 0;

    if(shiwei == 32)
    {
        shiwei = 0;
    }
    else if(shiwei == 45)
    {
        shiwei = 3;
    }
    else
    {
        shiwei = shiwei - 48;
    }

    if(gewei == 32)
    {
        gewei = 0;
    }
    else
    {
        gewei = gewei - 48;
    }

    IDE_TRACE_INT(baiwei);
    IDE_TRACE_INT(shiwei);
    IDE_TRACE_INT(gewei);
    if(AreaChange == 0x00)//Floor Changed
    {
        func = 0;
    }
    else if(AreaChange  == 0x01)
    {
        IDE_TRACE();
        baiwei = 0x7f;
        shiwei = 0x7f;
        gewei = 0x7f;
    }

    if(basicfun == 0)
    {
        basicfun = m_Old_basicfun;
    }
    else
    {
        m_Old_basicfun = basicfun;
    }

    quint8 check    = 0x00 ^ baiwei ^ shiwei ^ gewei ^ basicfun ^ func ^ sound ^ AreaChange & 0X7F;
    pByteArray.clear();
    m_CanByteArray.clear();
    m_CanByteArray.append((char)0x80);
    m_CanByteArray.append((char)0x00);
    m_CanByteArray.append((char)baiwei);
    m_CanByteArray.append((char)shiwei);
    m_CanByteArray.append((char)gewei);
    m_CanByteArray.append((char)basicfun);
    m_CanByteArray.append((char)func);
    m_CanByteArray.append((char)sound);
    m_CanByteArray.append((char)AreaChange);
    m_CanByteArray.append((char)check);
    m_CanByteArray.append((char)0x81);

    return true;
}
