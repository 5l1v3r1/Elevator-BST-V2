#ifndef UTPARSER_H
#define UTPARSER_H

#include "parserbase.h"

/*************************************************************
//>@C_UT
**************************************************************/
class  UTFrameParser : public LogicDevice
{
    Q_OBJECT
public:
    UTFrameParser(QObject *parent);
    ~UTFrameParser();

    bool SendFrame(QString pFrame, FRAME_FLAG pFlag);
    void ParseFrameStream();
    void NewFrameDetected()
    {
        if(m_FrameBuffer.length() < 1)
        {
            IDE_TRACE();
            return;
        }
        QString tmpString;
        tmpString.clear();
        tmpString.append(m_FrameBuffer);
        ReturnInfo(tmpString);
    }
    virtual bool ReturnInfo(QString &pInfo)
    {
        return false;
    }
public slots:
    quint64 SendDataStream(QByteArray pData);
    void ParseDataStream(QObject* pSender);
public:
    FRAME_STATE             m_FrameState;
    FRAME_FLAG              m_CurFrameType;
    QByteArray              m_Block,m_DataBuffer, m_FrameBuffer;
};
/****************************************************/
class UTHostParser : public UTFrameParser
{
    Q_OBJECT
public:
    UTHostParser(QObject *parent): UTFrameParser(parent){}
    ~UTHostParser(){}
    void Send(QString pInstruc)
    {
        SendFrame(pInstruc, Flag_Normal);
    }
    bool ReturnInfo(QString &pInfo)
    {
        if(UTFrameParser::ReturnInfo(pInfo))
            return true;
        IDE_TRACE_STR(pInfo);
        return false;
    }
public:
    void SyncEventMap(EventMap *pEventMap)
    {
        LogicDevice::SyncEventMap(pEventMap);
    }
    bool InitPHPara(QDomElement pElement, EventMap *pEventMap=0)
    {
        LogicDevice::InitPHPara(pElement, pEventMap);
    }
};
/****************************************************/
class UTDeviceParser : public UTFrameParser
{
    Q_OBJECT
public:
    UTDeviceParser(QObject *parent): UTFrameParser(parent){}
    ~UTDeviceParser()
    {
        disconnect(this,SIGNAL(sInform(QString)),0,0);
    }
    bool ReturnInfo(QString &pInfo)
    {
        if(UTFrameParser::ReturnInfo(pInfo))
            return true;
        emit sInform(pInfo);
        SendFrame(pInfo, Flag_Ack);
        return false;
    }
public:
    void SyncEventMap(EventMap *pEventMap)
    {
        connect(this,SIGNAL(sInform(QString)),m_EventMap,SLOT(slot_UDPInform(QString)));
        LogicDevice::SyncEventMap(pEventMap);
    }
    bool InitPHPara(QDomElement pElement, EventMap *pEventMap=0)
    {
        LogicDevice::InitPHPara(pElement, pEventMap);
    }
signals:
    void sInform(QString pInfo);
};

#endif // UTPARSER_H
