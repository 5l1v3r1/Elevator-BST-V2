#ifndef PHYSICALINPUT_H
#define PHYSICALINPUT_H

#include "physicalmovable.h"

class PhysicalMouse : public PhysicalMovable
{
    Q_OBJECT
public:
    PhysicalMouse(QObject *parent = 0);
    ~PhysicalMouse();

    bool SetPlugged();
    bool SetUnplugged();

    bool InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout);
    void UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement);

public:
    bool            mVisible;
};

class PhysicalKeyboard : public PhysicalMovable
{
    Q_OBJECT
public:
    typedef enum {Virtual=0, Machinery} MODE;
    PhysicalKeyboard(QObject *parent = 0);
    ~PhysicalKeyboard();

    MODE GetMode(QString pMode)
    {
        if(pMode.compare("Virtual", Qt::CaseInsensitive) == 0)
            return Virtual;
        else if(pMode.compare("Machinery", Qt::CaseInsensitive) == 0)
            return Machinery;
    }
    QString GetModeString(MODE pMode)
    {
        if(pMode == Virtual)
            return "Virtual";
        else if(pMode == Machinery)
            return "Machinery";
    }

    bool SetPlugged();
    bool SetUnplugged();

    bool InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout);
    void UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement);
    QString GetInformation() { return QString("%1:%2  Pinyin(%3)")
                                      .arg(mDevName).arg(mMode)
                                      .arg(mSupportPinyin?QString("true"):QString("false")); }

public:
    MODE            mMode;
    bool            mSupportPinyin;
};


class PhysicalTouch : public PhysicalMovable
{
    Q_OBJECT
public:
    typedef enum {Resistance=0, Capacitance, Infrared, Ultrasonic, Optics} MODE;
    PhysicalTouch(QObject *parent = 0);
    ~PhysicalTouch();

    bool SetPlugged();
    bool SetUnplugged();

    bool InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout);
    void UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement);

    QString GetInformation() { return QString("%1:%2").arg(mDevName).arg(GetModeString(mMode)); }

    MODE GetMode(QString pMode)
    {
        if(pMode.compare("Resistance", Qt::CaseInsensitive) == 0)
            return Resistance;
        else if(pMode.compare("Capacitance", Qt::CaseInsensitive) == 0)
            return Capacitance;
        else if(pMode.compare("Infrared", Qt::CaseInsensitive) == 0)
            return Infrared;
        else if(pMode.compare("Ultrasonic", Qt::CaseInsensitive) == 0)
            return Ultrasonic;
        else if(pMode.compare("Optics", Qt::CaseInsensitive) == 0)
            return Optics;
    }
    QString GetModeString(MODE pMode)
    {
        if(pMode == Resistance)
            return "Resistance";
        else if(pMode == Capacitance)
            return "Capacitance";
        else if(pMode == Infrared)
            return "Infrared";
        else if(pMode == Ultrasonic)
            return "Ultrasonic";
        else if(pMode == Optics)
            return "Optics";
    }

public:
    MODE            mMode;
};

#endif // PHYSICALKEYBOARD_H
