#include "physicalserial.h"

PhysicalSerial::PhysicalSerial(QObject *parent) :
    PhysicalDevice(parent)
{
    mSerialPort = new SerialPort;
    connect(mSerialPort, SIGNAL(readyRead()), this, SLOT(slot_ProcessData()));
}

PhysicalSerial::~PhysicalSerial()
{
    if(mSerialPort)
        mSerialPort->deleteLater();
}

void PhysicalSerial::slot_ProcessData()
{
    mFrameArray.clear();
    if(mSerialPort)
    {
        mFrameArray = mSerialPort->readAll();
        emit readyRead();
    }
}

qint64 PhysicalSerial::write(const QByteArray &data)
{
    if(mSerialPort)
        return mSerialPort->write(data);
    return -1;
}

QString GetSerialPort(QString num)
{
    QString tmpPortName;
#ifdef WINDOWS
    tmpPortName = QString("COM%1").arg(num);
#else
    tmpPortName = QString("/dev/ttyS%1").arg(num);
#endif
    return tmpPortName;
}

QString GetSerialPortNum(QString portname)
{
    QString tmpPortNum;
#ifdef WINDOWS
    tmpPortNum = portname.remove(QString("COM"));
#else
    tmpPortNum = portname.remove(QString("/dev/ttyS"));
#endif
    return tmpPortNum;
}

bool PhysicalSerial::OpenSerial(QString pPortNum, BaudRateType pBaud)
{
    if(!mSerialPort)
        return false;
    mSerialPort->setPortName(GetSerialPort(pPortNum));
    mSerialPort->Settings.BaudRate = pBaud;
    return mSerialPort->open(QIODevice::ReadWrite);
}

quint32 PhysicalSerial::InitPara()
{
    QDomNodeList tmpParaNodeList = m_ParaElement.childNodes();
    if(tmpParaNodeList.count() < 1)
        return 0;
    quint32 validcount = 0;
    for(int i=0;i<tmpParaNodeList.count()-1;i++)
    {
        QDomElement tmpElement = tmpParaNodeList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QHBoxLayout* tmpHBoxLayout = 0;
        QString tmpName = tmpElement.tagName();
        if(!tmpName.compare("Port"))
        {
            QString tmpValue = tmpElement.text();
            mSerialPort->setPortName(GetSerialPort(tmpValue));
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), tmpValue);
        }
        else if(!tmpName.compare("Buad"))
        {
            QString tmpValue = tmpElement.text();
            mSerialPort->Settings.BaudRate = (BaudRateType)(tmpValue.toInt());
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), tmpValue);
        }
        else if(!tmpName.compare("DataLen"))
        {
            QString tmpValue = tmpElement.text();
            mSerialPort->Settings.DataBits = (DataBitsType)(tmpValue.toInt());
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), tmpValue);
        }
        else if(!tmpName.compare("StopBit"))
        {
            QString tmpValue = tmpElement.text();
            mSerialPort->Settings.StopBits = (StopBitsType)(tmpValue.toInt());
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), tmpValue);
        }
        else if(!tmpName.compare("Parity"))
        {
            QString tmpValue = tmpElement.text();
            mSerialPort->Settings.Parity = (ParityType)(tmpValue.toInt());
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), tmpValue);
        }
        else if(!tmpName.compare("Flowcon"))
        {
            QString tmpValue = tmpElement.text();
            mSerialPort->Settings.FlowControl = (FlowType)(tmpValue.toInt());
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), tmpValue);
        }
        else if(!InitSubPara(tmpName, tmpElement, tmpHBoxLayout))
        {
            continue;
        }
        validcount++;
        m_SettingGroup->m_VLayout->addWidget(CreateLine());
        m_SettingGroup->m_VLayout->addLayout(tmpHBoxLayout);
    }
    //>@后处理
    mSerialPort->open(QIODevice::ReadWrite);
    return validcount;
}

void PhysicalSerial::UpdatePara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    mSerialPort->close();
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("Port"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpPortNum;
            if(!ModifyEnum(tmpComponent, tmpElement, tmpPortNum, tmpComponent->currentText()))
            {
            }
            else
            {
                mSerialPort->setPortName(GetSerialPort(tmpPortNum));
            }
        }
    }
    else if(!tmpName.compare("Buad"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpBuad;
            if(!ModifyEnum(tmpComponent, tmpElement, tmpBuad, tmpComponent->currentText()))
            {
            }
            else
            {
                mSerialPort->Settings.BaudRate = (BaudRateType)(tmpBuad.toInt());
            }
        }
    }
    else if(!tmpName.compare("DataLen"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpDataBits;
            if(!ModifyEnum(tmpComponent, tmpElement, tmpDataBits, tmpComponent->currentText()))
            {
            }
            else
            {
                mSerialPort->Settings.DataBits = (DataBitsType)(tmpDataBits.toInt());
            }
        }
    }
    else if(!tmpName.compare("StopBit"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpStopBit;
            if(!ModifyEnum(tmpComponent, tmpElement, tmpStopBit, tmpComponent->currentText()))
            {
            }
            else
            {
                mSerialPort->Settings.StopBits = (StopBitsType)(tmpStopBit.toInt());
            }
        }
    }
    else if(!tmpName.compare("Parity"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpParity;
            if(!ModifyEnum(tmpComponent, tmpElement, tmpParity, tmpComponent->currentText()))
            {
            }
            else
            {
                mSerialPort->Settings.Parity = (ParityType)(tmpParity.toInt());
            }
        }
    }
    else if(!tmpName.compare("Flowcon"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpFlowcon;
            if(!ModifyEnum(tmpComponent, tmpElement, tmpFlowcon, tmpComponent->currentText()))
            {
            }
            else
            {
                mSerialPort->Settings.FlowControl = (FlowType)(tmpFlowcon.toInt());
            }
        }
    }
    else
    {
        UpdateSubPara(tmpName, pObject, tmpElement);
    }
    //>@后处理
    mSerialPort->open(QIODevice::ReadWrite);
}

PhysicalSodimasCAN::PhysicalSodimasCAN(QObject *parent) :
    PhysicalSerial(parent)
{

}

PhysicalSodimasCAN::~PhysicalSodimasCAN()
{

}

PhysicalBSTRS232::PhysicalBSTRS232(QObject *parent) :
    PhysicalSerial(parent)
{

}

PhysicalBSTRS232::~PhysicalBSTRS232()
{

}


PhysicalYungtayRS485::PhysicalYungtayRS485(QObject *parent) :
    PhysicalSerial(parent)
{

}

PhysicalYungtayRS485::~PhysicalYungtayRS485()
{

}

