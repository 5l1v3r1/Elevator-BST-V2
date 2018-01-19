#ifndef PHYSICALSERIAL_H
#define PHYSICALSERIAL_H

#include "physicalbase.h"
#include "serialport.h"

class PhysicalSerial : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalSerial(QObject *parent = 0);
    virtual ~PhysicalSerial();

public:
    bool OpenSerial(QString pPortNum, BaudRateType pBaud);
    quint32 InitPara();
    virtual bool InitSubPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout) { return true; }
    virtual void UpdateSubPara(QString pName, QObject* pObject, QDomElement pElement) {}
    QString GetInformation()
    {
        if(!mSerialPort)
            return QString("%1").arg(mDevName);
        return QString("%1:%2  %3").arg(mDevName).arg(mSerialPort->portName()).arg(mSerialPort->baudRate());
    }

    qint64 write(const QByteArray &data);
    QByteArray readAll() { return mFrameArray; }

public slots:
    void UpdatePara(QObject* pObject);
    void slot_ProcessData();

public:
    SerialPort      *mSerialPort;
    QByteArray       mFrameArray;
};

class PhysicalSodimasCAN : public PhysicalSerial
{
    Q_OBJECT
public:
    typedef enum {Bare=0, DeviceNet, CANOpen, SDS} PROTOCOL;
    PhysicalSodimasCAN(QObject *parent = 0);
    virtual ~PhysicalSodimasCAN();

    PROTOCOL GetProtocol(QString pProtocol)
    {
        if(pProtocol.compare("Bare", Qt::CaseInsensitive) == 0)
            return Bare;
        else if(pProtocol.compare("DeviceNet", Qt::CaseInsensitive) == 0)
            return DeviceNet;
        else if(pProtocol.compare("CANOpen", Qt::CaseInsensitive) == 0)
            return CANOpen;
        else if(pProtocol.compare("SDS", Qt::CaseInsensitive) == 0)
            return SDS;
    }
    QString GetProtocolString(PROTOCOL pProtocol)
    {
        if(pProtocol == Bare)
            return "Bare";
        else if(pProtocol == DeviceNet)
            return "DeviceNet";
        else if(pProtocol == CANOpen)
            return "CANOpen";
        else if(pProtocol == SDS)
            return "SDS";
    }

public:
    bool InitSubPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout) {return true;}
    void UpdateSubPara(QString pName, QObject* pObject, QDomElement pElement) {}

public:
    PROTOCOL      mCanProtocol;
};

class PhysicalBSTRS232 : public PhysicalSerial
{
    Q_OBJECT
public:
    PhysicalBSTRS232(QObject *parent = 0);
    virtual ~PhysicalBSTRS232();
};

class PhysicalYungtayRS485 : public PhysicalSerial
{
    Q_OBJECT
public:
    PhysicalYungtayRS485(QObject *parent = 0);
    virtual ~PhysicalYungtayRS485();

public:
    bool InitSubPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout) {return true;}
    void UpdateSubPara(QString pName, QObject* pObject, QDomElement pElement) {}

public:
    bool      mAutoDirection;
};


#endif // NODESERIAL_H






