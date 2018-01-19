#ifndef HBPARSER_H
#define HBPARSER_H

#include "parserbase.h"

/*************************************************************
//>@C_HB
**************************************************************/
class HBFrameParser : public LogicDevice
{
    Q_OBJECT
public:
    HBFrameParser(QObject *parent);
    ~HBFrameParser();

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
        IDE_TRACE_STR(tmpString);
        ReturnInfo(tmpString);
    }
    virtual bool ReturnInfo(QString &pInfo)
    {
        return false;
    }
public:
    FRAME_STATE             m_DataState;
    FRAME_FLAG              m_CurFrameType;
    QByteArray              m_Block,m_DataBuffer, m_FrameBuffer;
public slots:
    quint64 SendDataStream(QByteArray pData);
    void ParseDataStream(QObject* pSender);
};
/****************************************************/
class HBHostParser : public HBFrameParser
{
    Q_OBJECT
public:
    HBHostParser(QObject *parent):
        HBFrameParser(parent)
    {
        connect(&mHBTimer,SIGNAL(timeout()),this,SLOT(slot_HBTimeout()));
    }
    ~HBHostParser()
    {
        disconnect(&mHBTimer,SIGNAL(timeout()),0,0);
    }
    void SendHB(quint16 pTime)
    {
        m_HBPara = pTime;
        StartHBTime(m_HBPara);
        QTimer::singleShot(m_HBPara / 2, this, SLOT(slot_SendHB()));
    }
    virtual bool ReturnInfo(QString &pInfo);
private:
    void StartHBTime(quint64 pTime)
    {
        if(mHBTimer.isActive())
            mHBTimer.stop();
        mHBTimer.start(pTime);
    }
    void StopHBTime()
    {
        if(mHBTimer.isActive())
            mHBTimer.stop();
    }
signals:
    void sConnState(CONN_STATE state);
    void sHBErr();
public slots:
    void slot_SendHB()
    {
        QString tmpString = QString("HB;%1").arg(m_HBPara);
        SendFrame(tmpString, Flag_Normal);
    }
    void slot_HBTimeout()
    {
        StopHBTime();
        emit sHBErr();
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
public:
    QTimer                  mHBTimer;
    quint16                 m_HBPara;
};
/****************************************************/
class HBDeviceParser : public HBFrameParser
{
    Q_OBJECT
public:
    HBDeviceParser(QObject *parent):
        HBFrameParser(parent)
    {
        connect(&mHBTimer,SIGNAL(timeout()),this,SLOT(slot_HBTimeout()));
    }
    ~HBDeviceParser()
    {
        disconnect(&mHBTimer,SIGNAL(timeout()),0,0);
    }
    void SendACK(QString pAck)
    {
        SendFrame(pAck, Flag_Ack);
    }
    virtual bool ReturnInfo(QString &pInfo);
public:
    QTimer                  mHBTimer;
private:
    void StartHBTime(quint64 pTime)
    {
        if(mHBTimer.isActive())
            mHBTimer.stop();
        mHBTimer.start(pTime);
    }
    void StopHBTime()
    {
        if(mHBTimer.isActive())
            mHBTimer.stop();
    }
signals:
    void sHBErr();
public slots:
    void slot_HBTimeout()
    {
        StopHBTime();
        emit sHBErr();
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

#endif // HBPARSER_H
