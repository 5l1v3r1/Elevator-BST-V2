#include "parserut.h"

/*************************************************************
//>@C_UT
**************************************************************/
UTFrameParser::UTFrameParser(QObject *parent) :
    LogicDevice(parent)
{
    m_CurFrameType = Flag_Normal;
    m_FrameState = State_Wait;
    m_FrameBuffer.clear();
}

UTFrameParser::~UTFrameParser()
{
    m_CurFrameType = Flag_Normal;
    m_FrameState = State_Wait;
    m_FrameBuffer.clear();
}

bool UTFrameParser::SendFrame(QString pFrame, FRAME_FLAG pFlag)
{
    QByteArray tmpSendData;
    tmpSendData.clear();
    switch(pFlag)
    {
        case Flag_Normal:tmpSendData.append(0x60);break;
        case Flag_Ack:tmpSendData.append(0xe0);break;
        default:return false;
    }
    tmpSendData.append(pFrame);
    SendDataStream(tmpSendData);
    tmpSendData.clear();
    return true;
}

quint64 UTFrameParser::SendDataStream(QByteArray pData)
{
    if(pData.isEmpty())
        return 0;
    quint64 size = pData.count();
    m_Block.clear();
    m_Block.append(0x5a);
    m_Block.append(0x5a);
    m_Block.append(size/256);
    m_Block.append(size%256);
    m_Block.append(pData);
    pData.clear();
    if(!ISDevActive())
    {
        IDE_TRACE();
        return 0;
    }
    quint64 sendedLenth = m_PHDev->write(m_Block);
    m_Block.clear();
    return sendedLenth;
}

void UTFrameParser::ParseDataStream(QObject* pSender)
{
    PhysicalDevice* PHDev = VAR_CAST<PhysicalDevice*>(pSender);
    if(PHDev == 0)
    {
        IDE_TRACE_STR("System#Err");
        return;
    }
    SyncDev(PHDev);
    static quint16 sCurFrameDataLen = 0;
    QByteArray tmpBufferData;
    tmpBufferData.clear();
    if(!ISDevActive())
        return;
    tmpBufferData = m_PHDev->readAll();
    int tmpCount = tmpBufferData.count();
    if(tmpCount < sizeof(quint32))
    {
        tmpBufferData.clear();
        return;
    }
    int i=0;
    quint8 tmpValue;
    while(i<tmpCount)
    {
        switch(m_FrameState)
        {
            case State_Wait:  //>@查找帧头
            {
                m_DataBuffer.clear();
                sCurFrameDataLen = 0;
                for(;i<tmpCount;i++)
                {
                    tmpValue = (quint8)tmpBufferData.at(i);
                    if(tmpValue == 0x5a)
                    {
                        m_FrameState = State_Header;
                        i++;
                        break;
                    }
                }
                break;
            }
            case State_Header:  //>@检测下一字节帧头是否合法
            {
                tmpValue = tmpBufferData.at(i++);
                if(tmpValue == 0x5a)
                    m_FrameState = State_DatalenH;
                else
                    m_FrameState = State_Wait;
                break;
            }
            case State_DatalenH:  //>@查找数据长度
            {
                tmpValue = tmpBufferData.at(i++);
                sCurFrameDataLen = tmpValue*256;
                m_FrameState = State_DatalenL;
                break;
            }
            case State_DatalenL:  //>@查找数据长度
            {
                tmpValue = tmpBufferData.at(i++);
                sCurFrameDataLen += tmpValue;
                m_FrameState = State_Continus;
                break;
            }
            case State_Continus:
            {
                quint16 tmpLen = tmpCount-i;
                if(tmpLen  >= sCurFrameDataLen)
                {
                    m_DataBuffer.append(tmpBufferData.mid(i, sCurFrameDataLen));
                    m_FrameState = State_Wait;
                    i += sCurFrameDataLen;
                    sCurFrameDataLen = 0;
                    ParseFrameStream();
                    m_DataBuffer.clear();
                }
                else if(tmpLen < sCurFrameDataLen)
                {
                    m_DataBuffer.append(tmpBufferData.mid(i));
                    i += tmpLen;
                    sCurFrameDataLen -= tmpLen;
                }
                break;
            }
            default:
                tmpBufferData.clear();
                m_DataBuffer.clear();
                return;
        }
    }
    tmpBufferData.clear();
}

void UTFrameParser::ParseFrameStream()
{
    quint32 tmpCount = m_DataBuffer.count();
    if(tmpCount < 1)
    {
        IDE_TRACE();
        return;
    }
    quint8 tmpValue = m_DataBuffer.at(0);
    switch(tmpValue)
    {
        case 0x60:m_CurFrameType = Flag_Normal;break;
        case 0xe0:m_CurFrameType = Flag_Ack;break;
        default:IDE_TRACE();return;
    }
    m_FrameBuffer.clear();
    QString tmpInstru;
    tmpInstru.clear();
    tmpInstru = QString::fromLocal8Bit(m_DataBuffer.mid(1));
    m_FrameBuffer.append(tmpInstru);
    NewFrameDetected();
    m_FrameBuffer.clear();
}


