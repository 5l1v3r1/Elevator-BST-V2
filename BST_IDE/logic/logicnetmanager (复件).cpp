#include "logicnetmanager.h"
#include "physicalinc.h"
#ifdef IDE
    #include "devicescene.h"
#elif DISPLAY
    #include "devicemanager.h"
#endif

/*************************************************************
//>@C_NFD
**************************************************************/
NetFrameParser::NetFrameParser(QObject *parent) :
    LogicDevice(parent)
{
    m_SFConn = false;

    m_MaxBlockSize = D_MAXSOCKETBLOCKSIZE;
    m_FrameState = State_Wait;
    m_CurFrameType = Flag_Normal;

    m_ReadFileOp = new ReadFileOp(this);
    m_WriteFileOp = new WriteFileOp(this);

    m_PackNum = 0;
    m_ByteArray.clear();
    m_DataBuffer.clear();
    m_FrameBuffer.clear();

    connect(&m_PackageDieTimer, SIGNAL(timeout()), this, SLOT(slot_PackageTimeout()));
    m_PackageDieCnt = 0;

    m_PHDev = 0;

    connect(&mHBTimer,SIGNAL(timeout()),this,SLOT(slot_HBTimeout()));
}

NetFrameParser::~NetFrameParser()
{
    m_SFConn = false;

    if(m_ReadFileOp)
        m_ReadFileOp->deleteLater();
    if(m_ReadFileOp)
        m_WriteFileOp->deleteLater();

    m_ByteArray.clear();
    m_DataBuffer.clear();
    m_FrameBuffer.clear();

    disconnect(&m_PackageDieTimer, SIGNAL(timeout()), 0, 0);
    m_PackageDieTimer.deleteLater();

    disconnect(&mHBTimer,SIGNAL(timeout()),0,0);
}

void NetFrameParser::SyncDev(QIODevice *pIODevice)
{
    m_PHDev = pIODevice;
}

void NetFrameParser::AppendPHDevices(QStringList pPHDevs)
{
    DeletePHDevices();
    for(int i=0;i<pPHDevs.count();i++)
    {
        PH_Type tmpType = GetPHType(pPHDevs.at(i));
        if(tmpType == P_INVALID)
            continue;
#ifdef IDE
        PhysicalDevice *tmpPHDev = m_DeviceScene->m_PHDevList.value(tmpType);
#elif DISPLAY
        PhysicalDevice *tmpPHDev = m_DevManager->m_PHDevList.value(tmpType);
#endif
        if(!tmpPHDev)
            continue;
        if(!tmpPHDev->mLGDevices.contains(mDevType))
            tmpPHDev->mLGDevices.insert(mDevType, this);
        //>@
        switch(tmpType)
        {
            case P_RMII:
            case P_GPRS:
            case P_WIFI:
            case P_USBNET:
            {
                PhysicalNetwork *tmpNetwork = VAR_CAST<PhysicalNetwork*>(tmpPHDev);
                if(!tmpNetwork)
                    continue;
                connect(tmpNetwork->m_SharedTcp, SIGNAL(readyRead()), this, SLOT(ParseDataStream()));
                break;
            }
            case P_SodimasCAN:
            {
                connect(tmpPHDev, SIGNAL(readyRead()), this, SLOT(ParseDataStream()));
                break;
            }
            default:continue;
        }
        mPHDevices.insert(tmpType, tmpPHDev);
    }
}

void NetFrameParser::DeletePHDevices()
{
    QList<PH_Type> tmpList = mPHDevices.keys();
    for(int i=0;i<tmpList.count();i++)
    {
        PhysicalDevice* tmpPHDev = mPHDevices.take(tmpList.at(i));
        if(!tmpPHDev)
            continue;
        switch(tmpPHDev->mDevType)
        {
            case P_RMII:
            case P_GPRS:
            case P_WIFI:
            case P_USBNET:
            {
                PhysicalNetwork *tmpNetwork = VAR_CAST<PhysicalNetwork*>(tmpPHDev);
                if(!tmpNetwork)
                    continue;
                disconnect(tmpNetwork->m_SharedTcp, SIGNAL(readyRead()), this, SLOT(ParseDataStream()));
                break;
            }
            case P_SodimasCAN:
            {
                disconnect(tmpPHDev, SIGNAL(readyRead()), this, SLOT(ParseDataStream()));
                break;
            }
            default:continue;
        }
    }
    mPHDevices.clear();
}

quint32 NetFrameParser::InitLGPara()
{
    QDomNodeList tmpParaNodeList = m_ParaElement.childNodes();
    if(tmpParaNodeList.count() < 1)
        return 0;
    quint32 validcount = 0;
    QHBoxLayout* tmpHBoxLayout;
    for(int i=0;i<tmpParaNodeList.count();i++)
    {
        QDomElement tmpElement = tmpParaNodeList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(!tmpName.compare("Device"))
        {
            QStringList tmpDevList = tmpElement.text().split(';');
            AppendPHDevices(tmpDevList);
            tmpHBoxLayout = CreateCheck(tmpName, tmpElement.attribute("List").split(';'), tmpDevList);
        }
        else
        {
            continue;
        }
        validcount++;
        m_SettingGroup->m_VLayout->addLayout(tmpHBoxLayout);
    }
    validcount++;
    return validcount;
}

void NetFrameParser::UpdateLGPara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("Device"))
    {
        CheckList *tmpComponent = VAR_CAST<CheckList *>(pObject);
        if(tmpComponent)
        {
            QStringList tmpDevList = tmpComponent->GetSelectList();
            QString tmpDevString = tmpComponent->GetSelectText(tmpDevList);
            if(SetElementText(tmpElement, tmpDevString))
            {
                AppendPHDevices(tmpDevList);
                tmpComponent->m_SelectList = tmpDevList;
            }
            else
            {
                tmpComponent->RestoreCheckList();
            }
        }
    }
}

void NetFrameParser::SendCMD(QString pInstruc,FRAME_FLAG pFlag)
{
    NFDFRAME_STRUCT tmpFrame;
    tmpFrame.mCmd = Type_SetCmd;
    tmpFrame.mCmdPara = 0x01; //>@命令参数
    tmpFrame.mData.clear();
    QByteArray tmpByteArray;
    tmpByteArray.clear();
    tmpByteArray.append(pInstruc);
    quint16 tmpLength = tmpByteArray.size();
    tmpByteArray.clear();
    quint8 tmpLenH = tmpLength / 256;
    quint8 tmpLenL = tmpLength % 256;
    tmpFrame.mData.append(tmpLenH);
    tmpFrame.mData.append(tmpLenL);
    tmpFrame.mData.append(pInstruc.toLocal8Bit());
    SendFrame(tmpFrame, pFlag);
    tmpFrame.mData.clear();
}

void NetFrameParser::SendCMD(QStringList pCmdSubList,FRAME_FLAG pFlag)
{
    QString tmpString = pCmdSubList.at(0);
    for(int i=1;i<pCmdSubList.count();i++)
    {
        tmpString.append(D_CMDSUBSPLIT);
        tmpString.append(pCmdSubList.at(i));
    }
    SendCMD(tmpString, pFlag);
}

bool NetFrameParser::SendFrame(NFDFRAME_STRUCT pFrame, FRAME_FLAG pFlag)
{
    QByteArray tmpSendData;
    tmpSendData.clear();
    if(pFlag == Flag_Normal)
        tmpSendData.append(D_FRAMEHEADER);
    else
        tmpSendData.append(D_FRAMEHEADER_ACK);
#ifdef CRC
    quint8 tmpCRC = 0;
    tmpSendData.append(pFrame.mCmd);
    tmpCRC ^= pFrame.mCmd;
    tmpSendData.append(pFrame.mCmdPara);
    tmpCRC ^= pFrame.mCmdPara;
    if(!pFrame.mData.isEmpty())
    {
        tmpSendData.append(pFrame.mData);
        for(int i=0;i<pFrame.mData.count();i++)
        {
            tmpCRC ^= pFrame.mData.at(i);
        }
    }
    tmpCRC = tmpCRC&0x7f;
    tmpSendData.append(tmpCRC);
#endif
#ifdef NOCRC
    tmpSendData.append(pFrame.mCmd);
    tmpSendData.append(pFrame.mCmdPara);
    if(!pFrame.mData.isEmpty())
    {
        tmpSendData.append(pFrame.mData);
        tmpSendData.append(0x7f);
    }
#endif
    if(pFlag == Flag_Normal)
        tmpSendData.append(D_FRAMEFOOTER);
    else
        tmpSendData.append(D_FRAMEFOOTER_ACK);
    SendDataStream(tmpSendData);
    tmpSendData.clear();
    return true;
}

quint64 NetFrameParser::SendDataStream(QByteArray pData)
{
    if(!m_PHDev)
        return 0;
    if(pData.isEmpty())
        return 0;
    quint64 size = pData.count();
    m_ByteArray.clear();
    m_ByteArray.append(0x5a);
    m_ByteArray.append(0x5a);
    m_ByteArray.append(size/256);
    m_ByteArray.append(size%256);
    m_ByteArray.append(pData);
    pData.clear();

    quint64 tmpLenth = 0;
    //>@要么为网络设备中的TCP，要么为其他设备
    QTcpSocket *tmpTcp = VAR_CAST<QTcpSocket*>(m_PHDev);
    if(tmpTcp)
    {
        tmpLenth = tmpTcp->write(m_ByteArray);
    }
    else
    {
        PhysicalDevice* tmpPHDev = VAR_CAST<PhysicalDevice*>(m_PHDev);
        if(!tmpPHDev)
            return 0;
        tmpLenth = tmpPHDev->write(m_ByteArray);
    }
    m_ByteArray.clear();
    return tmpLenth;
}

void NetFrameParser::SendFileData()
{
    static quint8 NumH = 0;
    static quint8 NumL = 0;
    static quint8 LenthH = 0;
    static quint8 LenthL = 0;
    NFDFRAME_STRUCT tmpFrame;
    tmpFrame.mCmd = Type_FileTrans;
    tmpFrame.mCmdPara = Flag_FileData; //>@命令参数
    tmpFrame.mData.clear();
    if(!m_ReadFileOp->read(qMin(m_ReadFileOp->mLeftSize, m_MaxBlockSize), m_ByteArray))
    {
        IDE_TRACE_STR("SendFileDataErr");
        return;
    }
    m_PackNum++;
    NumH = (m_PackNum)>>8;;
    NumL = (m_PackNum)&0xff;
    if(m_ReadFileOp->mLeftSize<m_MaxBlockSize)
    {
        m_ReadFileOp->mLeftSize = 0;
        m_PackNum = 0;
        tmpFrame.mCmdPara = Flag_FileFooter; //>@命令参数
    }
    m_ReadFileOp->mLeftSize -= m_ByteArray.size();
    LenthH = (m_ByteArray.size())>>8;
    LenthL = (m_ByteArray.size())&0xff;
    tmpFrame.mData.append(NumH);
    tmpFrame.mData.append(NumL);
    tmpFrame.mData.append(LenthH);
    tmpFrame.mData.append(LenthL);
    tmpFrame.mData.append(m_ByteArray);
    m_ByteArray.clear();
    SendFrame(tmpFrame, Flag_Normal);
    tmpFrame.mData.clear();
    if(m_ReadFileOp->mSize > 0)
    {
        static int tmpVaule = 0;
        static quint64 tmpSize = 0;
        if(m_PackNum != 0)
            tmpSize = (quint64)m_PackNum * m_MaxBlockSize;
        tmpVaule = (tmpSize / (m_ReadFileOp->mSize/100));
        UpdateProgress(tmpVaule, false);
    }
    if(tmpFrame.mCmdPara == Flag_FileFooter)
        RestartPackageTimer(D_PackageTime * 10);
    else
        RestartPackageTimer(D_PackageTime);
}

void NetFrameParser::SendFileMD5()
{
    NFDFRAME_STRUCT tmpFrame;
    quint8 NumH = 0;
    quint8 NumL = 0;
    quint8 LenthH = 0;
    quint8 LenthL = 0;
    tmpFrame.mCmd = Type_FileTrans;
    tmpFrame.mCmdPara = Flag_FileMD5; //>@命令参数
    tmpFrame.mData.clear();
    tmpFrame.mData.append(NumH);
    tmpFrame.mData.append(NumL);
    tmpFrame.mData.append(LenthH);
    tmpFrame.mData.append(LenthL);
    SendFrame(tmpFrame, Flag_Normal);
    tmpFrame.mData.clear();
//    RestartPackageTimer(D_PackageTime);
}

void NetFrameParser::GetFileData(QByteArray pData)
{
    quint64 tmpLen = pData.length();
    QByteArray tmpData;
    tmpData.clear();
    tmpData = pData.mid(6, tmpLen-7);
    if(!m_WriteFileOp->write(tmpData))
    {
        IDE_TRACE_STR("GetFileDataErr");
        return;
    }
    tmpData.clear();
    NFDFRAME_STRUCT tmpFrame;
    tmpFrame.mCmd = Type_FileTrans;
    tmpFrame.mCmdPara = pData.at(1); //>@命令参数
    tmpFrame.mData.clear();
    tmpFrame.mData.append(pData.mid(2,2));
    SendFrame(tmpFrame, Flag_Ack);
    tmpFrame.mData.clear();
    if(m_WriteFileOp->mSize > 0)
    {
        static int tmpVaule = 0;
        static quint64 tmpSize = 0;
        quint16 tmpNum = (quint8)(m_FrameBuffer.at(2));
        tmpNum = (tmpNum * 256) + (quint8)(m_FrameBuffer.at(3));
        tmpSize = (quint64)tmpNum * m_MaxBlockSize;
        tmpVaule = (tmpSize / (m_WriteFileOp->mSize / 100));
        UpdateProgress(tmpVaule, false);
    }
    RestartPackageTimer(D_PackageTime);
}

void NetFrameParser::GetFileFooter(QByteArray pData)
{
    quint64 tmpLen = pData.length();
    QByteArray tmpData;
    tmpData.clear();
    tmpData = pData.mid(6, tmpLen-7);
    if(!m_WriteFileOp->write(tmpData))
    {
        IDE_TRACE_STR("GetFileFooterErr");
        return;
    }
    tmpData.clear();
    NFDFRAME_STRUCT tmpFrame;
    tmpFrame.mCmd = Type_FileTrans;
    tmpFrame.mCmdPara = pData.at(1);
    tmpFrame.mData.clear();
    tmpFrame.mData.append(pData.mid(2,2));
    SendFrame(tmpFrame, Flag_Ack);
    tmpFrame.mData.clear();
    RestartPackageTimer(D_PackageTime * 10);
}

void NetFrameParser::GetFileMD5(QByteArray pData)
{
    NFDFRAME_STRUCT tmpFrame;
    tmpFrame.mCmd = Type_FileTrans;
    tmpFrame.mCmdPara = pData.at(1); //>@命令参数
    tmpFrame.mData.clear();
    tmpFrame.mData.append(pData.mid(2,2));
    SendFrame(tmpFrame, Flag_Ack);
    tmpFrame.mData.clear();
}

void NetFrameParser::RestartPackageTimer(int pTime)
{
    if(m_PackageDieTimer.isActive())
        m_PackageDieTimer.stop();
    m_PackageDieTimer.start(pTime);
    m_PackageDieCnt = 0;
}

void NetFrameParser::StopPackageTimer()
{
    if(m_PackageDieTimer.isActive())
        m_PackageDieTimer.stop();
    m_PackageDieCnt = 0;
}

void NetFrameParser::slot_PackageTimeout()
{
    IDE_TRACE_INT(m_PackageDieCnt);
    m_PackageDieCnt++;
    if(m_PackageDieCnt > 10)
    {
        StopPackageTimer();
        m_ReadFileOp->close();
        m_WriteFileOp->close();
        switch(m_CurSendEvent.mEventType)
        {
            case Event_StartSendFile:
            case Event_StartGetFile:
            {
                IDE_TRACE_STR("HostPackageTimeout");
                m_CurSendEvent.mEventPara<<QString("PackageTimeout")<<D_Failed;
                ProcessEventInfo(m_CurSendEvent);
                return;
            }
            default:break;
        }
        switch(m_CurRcvEvent.mEventType)
        {
            case Event_SendFileTest:
            case Event_GetFileTest:
            {
                IDE_TRACE_STR("DevicePackageTimeout");
                m_CurRcvEvent.mEventPara<<QString("PackageTimeout")<<D_Failed;
                ProcessEventInfo(m_CurRcvEvent);
                return;
            }
            default:break;
        }
    }
}

void NetFrameParser::ParseDataStream()
{
    static quint16 sCurFrameDataLen = 0;
    QByteArray tmpBufferData;

    //>@要么为网络设备中的TCP，要么为其他设备
#ifdef IDE
    QTcpSocket *tmpTcp = VAR_CAST<QTcpSocket*>(sender());
    if(tmpTcp)
    {
        m_PHDev = tmpTcp;
        tmpBufferData = tmpTcp->readAll();
    }
#elif DISPLAY
    ToolTcpServer *tmpServer = VAR_CAST<ToolTcpServer*>(sender());
    if(tmpServer)
    {
        QTcpSocket *tmpTcp = tmpServer->mCurSocket;
        if(!tmpTcp)
            return;
        m_PHDev = tmpTcp;
        tmpBufferData = tmpTcp->readAll();
    }
#endif
    else
    {
        PhysicalDevice* tmpPHDev = VAR_CAST<PhysicalDevice*>(sender());
        if(!tmpPHDev)
            return;
        m_PHDev = tmpPHDev;
        tmpBufferData = tmpPHDev->readAll();
    }

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
            case State_Wait:
            {
                m_DataBuffer.clear();
                sCurFrameDataLen = 0;
                for(;i<tmpCount;i++)
                {
                    tmpValue = (quint8)(tmpBufferData.at(i));
                    if(tmpValue == 0x5a)
                    {
                        m_FrameState = State_Header;
                        i++;
                        break;
                    }
                }
                break;
            }
            case State_Header:
            {
                tmpValue = (quint8)(tmpBufferData.at(i++));
                if(tmpValue == 0x5a)
                    m_FrameState = State_DatalenH;
                else
                    m_FrameState = State_Wait;
                break;
            }
            case State_DatalenH:
            {
                tmpValue = (quint8)(tmpBufferData.at(i++));
                sCurFrameDataLen = tmpValue*256;
                m_FrameState = State_DatalenL;
                break;
            }
            case State_DatalenL:  //>@查找数据长度
            {
                tmpValue = (quint8)(tmpBufferData.at(i++));
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
                return;
        }
    }
    tmpBufferData.clear();
}

void NetFrameParser::ParseFrameStream()
{
    quint32 tmpCount = m_DataBuffer.count();
    quint8 tmpValue = 0;
    m_FrameBuffer.clear();
    if(tmpCount < 5)
    {
        IDE_TRACE_INT(tmpCount);
        return;
    }
    tmpValue = (quint8)(m_DataBuffer.at(0));
    switch(tmpValue)
    {
        case (quint8)D_FRAMEHEADER:
            m_CurFrameType = Flag_Normal;
            tmpValue = (quint8)(m_DataBuffer.at(tmpCount-1));
            switch(tmpValue)
            {
                case (quint8)D_FRAMEFOOTER:
                    break;
                default:
                    IDE_TRACE_INT(tmpValue);
                    return;
            }
            break;
        case (quint8)D_FRAMEHEADER_ACK:
            m_CurFrameType = Flag_Ack;
            tmpValue = (quint8)(m_DataBuffer.at(tmpCount-1));
            switch(tmpValue)
            {
                case (quint8)D_FRAMEFOOTER_ACK:
                    break;
                default:
                    IDE_TRACE_INT(tmpValue);
                    return;
            }
            break;
        default:
            IDE_TRACE_INT(tmpCount);
            for(int i = 0;i < tmpCount; i++)
            {
                IDE_TRACE_INT((quint8)(m_DataBuffer.at(i)));
            }
            return;
    }
    tmpValue = (quint8)(m_DataBuffer.at(1));
    switch(tmpValue)
    {
        case Type_FileTrans:
            switch(m_CurFrameType)
            {
                case Flag_Normal:
                {
                    quint16 tmpDataLenth = 0;
                    quint8 tmpLenth = 0;
                    tmpLenth =(quint8)(m_DataBuffer.at(5));
                    tmpDataLenth = (((quint16)tmpLenth)&0x00ff)*256;
                    tmpDataLenth += (quint8)(m_DataBuffer.at(6));
                    if(tmpDataLenth>(tmpCount - 9))
                    {
                        IDE_TRACE_INT(tmpDataLenth);
                        return;
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        case Type_SetCmd:
        {
            quint16 tmpDataLenth = 0;
            quint8 tmpLenth = 0;
            tmpLenth =(quint8)(m_DataBuffer.at(3));
            tmpDataLenth = (((quint16)tmpLenth)&0x00ff)*256;
            tmpDataLenth += (quint8)(m_DataBuffer.at(4));
            if(tmpDataLenth>(tmpCount - 7))
            {
                IDE_TRACE_INT(tmpDataLenth);
                IDE_TRACE_INT(tmpCount);
                return;
            }
            break;
        }
        default:
            IDE_TRACE_INT(tmpValue);
            return;
    }
    m_FrameBuffer = m_DataBuffer.mid(1,tmpCount-2);
    FrameVerify();
    m_FrameBuffer.clear();
}

void NetFrameParser::FrameVerify()
{
    quint64 len = m_FrameBuffer.count();
    if(len < 3)
    {
        IDE_TRACE_INT(len);
        return;
    }
#ifdef CRC
    quint8 tmpValue = (quint8)(m_FrameBuffer.at(len-1))&0x7f;
    quint8 tmpCRC = 0;
    for(int m=0;m<len-1;m++)
    {
        tmpCRC ^= (quint8)(m_FrameBuffer.at(m));
    }
    tmpCRC &= 0x7f;
    if(tmpCRC != tmpValue)
    {
        IDE_TRACE_STR("VerifyErr");
        ProcessEventInfo("VerifyErr");
        m_TransStopFlag = 1;
    }
#endif
    FRAME_TYPE tmpType = (FRAME_TYPE)(m_FrameBuffer.at(0));
    switch(tmpType)
    {
        case Type_FileTrans:
        {
            if(m_CurFrameType == Flag_Ack) //>@如果是响应帧，需要发送下一包数据
            {
                quint8 tmpPara = m_FrameBuffer.at(1);
                switch(tmpPara)
                {
                    case Flag_FileHeader:
                    case Flag_FileData:
                    {
                        SendFileData();
                        return;
                    }
                    case Flag_FileFooter:
                    {
                        SendFileMD5();
                        StopPackageTimer();
                        return;
                    }
                    case Flag_FileMD5:
                    {
                        switch(m_CurSendEvent.mEventType)
                        {
                            case Event_StartSendFile:
                            {
                                m_ReadFileOp->close();
                                m_CurSendEvent.mEventPara.append(D_Success);
                                ProcessEventInfo(m_CurSendEvent);
                                break;
                            }
                            default:break;
                        }
                        switch(m_CurRcvEvent.mEventType)
                        {
                            case Event_StartGetFile:
                            {
                                m_ReadFileOp->close();
                                m_CurRcvEvent.mEventPara.append(D_Success);
                                ProcessEventInfo(m_CurRcvEvent);
                                break;
                            }
                            default:break;
                        }
                        return;
                    }
                    default:
                        return;
                }
            }
            else if(m_CurFrameType == Flag_Normal)
            {
                quint8 tmpPara = m_FrameBuffer.at(1);
                switch(tmpPara)
                {
                    case Flag_FileData:
                    {
                        GetFileData(m_FrameBuffer);
                        return;
                    }
                    case Flag_FileFooter:
                    {
                        GetFileFooter(m_FrameBuffer);
                        StopPackageTimer();
                        return;
                    }
                    case Flag_FileMD5:
                    {
                        GetFileMD5(m_FrameBuffer);
                        switch(m_CurRcvEvent.mEventType)
                        {
                            case Event_StartSendFile:
                            {
                                m_WriteFileOp->close();
                                m_CurRcvEvent.mEventPara.append(D_Success);
                                ProcessEventInfo(m_CurRcvEvent);
                                break;
                            }
                            default:break;
                        }
                        switch(m_CurSendEvent.mEventType)
                        {
                            case Event_StartGetFile:
                            {
                                m_WriteFileOp->close();
                                m_CurSendEvent.mEventPara.append(D_Success);
                                ProcessEventInfo(m_CurSendEvent);
                                break;
                            }
                            default:break;
                        }
                        return;
                    }
                    default:
                        return;
                }
            }
            return;
        }
        case Type_SetCmd:
        {
            NewFrameDetected();
            return;
        }
        default:
        {
            IDE_TRACE();
            return;
        }
    }
}

void NetFrameParser::NewFrameDetected()
{
    if(m_FrameBuffer.length() < 4)
    {
        IDE_TRACE();
        return;
    }
    quint16 tmpLength = 0;
    tmpLength += (quint8)(m_FrameBuffer.at(2))*256;
    tmpLength += (quint8)(m_FrameBuffer.at(3));
    if(m_FrameBuffer.length() < tmpLength + 4)
    {
        IDE_TRACE();
        return;
    }
    EventInfo eventInfo;
    eventInfo.fromString(QString(m_FrameBuffer.mid(4,tmpLength)));
    ProcessEventInfo(eventInfo);
}

void NetFrameParser::SendEvent(EventInfo pEventInfo)
{
    m_CurSendEvent = pEventInfo;
    switch(pEventInfo.mEventType)
    {
        case Event_SendFileTest:
        {
            m_PackageDieCnt = 0;
            if(pEventInfo.mEventPara.count() == 3)
            {
                if(m_ReadFileOp->isExist(pEventInfo.mEventPara.at(1)))
                {
                    if(m_ReadFileOp->open(pEventInfo.mEventPara.at(1)))
                    {
                        m_ReadFileOp->savePath(pEventInfo.mEventPara.at(1));
                        QString size = QString("%1").arg(m_ReadFileOp->mSize);
                        pEventInfo.mEventPara<<size;
                        break;
                    }
                    else
                    {
                        pEventInfo.mEventPara<<QString("Locked")<<D_Failed;
                        ProcessEventInfo(pEventInfo);
                        return;
                    }
                }
                else
                {
                    pEventInfo.mEventPara<<QString("UnExist")<<D_Failed;
                    ProcessEventInfo(pEventInfo);
                    return;
                }
            }
        }
        case Event_GetFileTest:
        {
            m_PackageDieCnt = 0;
            if(pEventInfo.mEventPara.count() == 3)
            {
                if(m_WriteFileOp->isExist(pEventInfo.mEventPara.at(2)))
                {
                    if(pEventInfo.mEventPara.at(0).compare("y", Qt::CaseInsensitive) == 0)
                    {
                        if(m_WriteFileOp->open(pEventInfo.mEventPara.at(2)))
                        {
                            m_WriteFileOp->savePath(pEventInfo.mEventPara.at(2));
                            break;
                        }
                        else
                        {
                            pEventInfo.mEventPara<<QString("Locked")<<D_Failed;
                            ProcessEventInfo(pEventInfo);
                            return;
                        }
                    }
                    else
                    {
                        pEventInfo.mEventPara<<QString("Exist")<<D_Failed;
                        ProcessEventInfo(pEventInfo);
                        return;
                    }
                }
                else
                {
                    if(m_WriteFileOp->isPathExist(pEventInfo.mEventPara.at(2)))
                    {
                        if(m_WriteFileOp->open(pEventInfo.mEventPara.at(2)))
                        {
                            m_WriteFileOp->savePath(pEventInfo.mEventPara.at(2));
                            break;
                        }
                        else
                        {
                            pEventInfo.mEventPara<<QString("PathWriteErr")<<D_Failed;
                            ProcessEventInfo(pEventInfo);
                            return;
                        }
                    }
                }
            }
            break;
        }
        case Event_StartSendFile:
        {
            m_PackageDieCnt = 0;
            SendCMD(pEventInfo.toFrame(), Flag_Normal);
            SendFileData();
            return;
        }
        case Event_StartGetFile:
        {
            m_PackageDieCnt = 0;
            SendCMD(pEventInfo.toFrame(), Flag_Normal);
            return;
        }
        default:
        {
            SendCMD(pEventInfo.toFrame(), Flag_Normal);
            break;
        }
    }
}

void NetFrameParser::UpdateProgress(quint8 pVaule, bool pGlobalRatio)
{
    if(pVaule > 100)
        pVaule = 100;
    if(pGlobalRatio)
        pVaule = pVaule * m_CurSendEvent.mRatio + m_Preprogress;
    IDE_TRACE_INT(pVaule);
    emit sProgress(pVaule);
}

/*****************************************************************
//>@ HostNetManager
*****************************************************************/
HostNetManager::HostNetManager(QObject *parent)
    : NetFrameParser(parent)
{
    m_Preprogress = 0;
    connect(&m_DieTimer,SIGNAL(timeout()),this,SLOT(slot_Dietime()));
}

HostNetManager::~HostNetManager()
{
    m_Preprogress = 0;
    disconnect(&m_DieTimer,SIGNAL(timeout()),0,0);
}

bool HostNetManager::SFConn(quint64 pLife)
{
    SendCMD(QStringList()<<"SFConn", Flag_Normal);
    while(pLife-- && !m_SFConn)
        QtSleep(1);
    return m_SFConn;
}

bool HostNetManager::SFDisconn(quint64 pLife)
{
    if(!m_SFConn)
        return true;
    Stop();
    SendCMD(QStringList()<<"SFDisconn", Flag_Normal);
    while(pLife-- && m_SFConn)
        QtSleep(1);
    return !m_SFConn;
}

void HostNetManager::Stop()
{
    if(!m_SFConn)
        return;
    ClearStack();
    SendCMD(QStringList()<<"Stop", Flag_Normal);
}

bool HostNetManager::GetDevInfo(quint64 pLife)
{
    m_DevInfoList.clear();
    if(!m_SFConn)
        return false;
    SendCMD(QStringList()<<"DevInfo", Flag_Normal);
    while(pLife-- && m_DevInfoList.isEmpty())
        QtSleep(1);
    if(m_DevInfoList.isEmpty())
        return false;
    return true;
}

bool HostNetManager::LocalCompress(QString pSrc, QString pDst, bool pCover)
{
    QThread ZipThread;
    ZipTask compressZipTask;
    connect(this,SIGNAL(sZip(QString, QString, bool)),&compressZipTask,SLOT(Zip(QString, QString, bool)));
    connect(&compressZipTask,SIGNAL(end()),this,SLOT(slot_ZipTaskEnd()));
    connect(&compressZipTask,SIGNAL(error()),this,SLOT(slot_ZipTaskErr()));
    connect(&compressZipTask,SIGNAL(progress(int)),this,SLOT(slot_ZipTaskProgress(int)));
    compressZipTask.moveToThread(&ZipThread);
    ZipThread.start(QThread::LowPriority);
    ZipThread.wait(10);
    mZipTaskState = Start;
    emit sZip(pSrc, pDst, pCover);
    while(mZipTaskState == Start)
        QtSleep(1);
    if(mZipTaskState == Success)
        return true;
    return false;
}
bool HostNetManager::LocalDecompress(QString pSrc, QString pDir, bool pCover)
{
    QThread ZipThread;
    ZipTask decompressZipTask;
    connect(this,SIGNAL(sUnzip(QString, QString, bool)),&decompressZipTask,SLOT(Unzip(QString, QString, bool)));
    connect(&decompressZipTask,SIGNAL(end()),this,SLOT(slot_ZipTaskEnd()));
    connect(&decompressZipTask,SIGNAL(error()),this,SLOT(slot_ZipTaskErr()));
    connect(&decompressZipTask,SIGNAL(progress(int)),this,SLOT(slot_ZipTaskProgress(int)));
    decompressZipTask.moveToThread(&ZipThread);
    ZipThread.start(QThread::LowPriority);
    ZipThread.wait(10);
    mZipTaskState = Start;
    emit sUnzip(pSrc, pDir, pCover);
    while(mZipTaskState == Start)
        QtSleep(1);
    if(mZipTaskState == Success)
        return true;
    return false;
}

void HostNetManager::SendFileTest
    (QString pCover, QString pSrc, QString pDst,
     quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_SendFileTest;
    tmpEventInfo.mEventPara<<pCover<<pSrc<<pDst;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}

void HostNetManager::StartSendFile
    (quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_StartSendFile;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}

void HostNetManager::GetFileTest
    (QString pCover, QString pSrc, QString pDst,
     quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_GetFileTest;
    tmpEventInfo.mEventPara<<pCover<<pSrc<<pDst;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}
void HostNetManager::StartGetFile
    (quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_StartGetFile;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}

void HostNetManager::Ls
    (QString pPath,
     quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_Ls;
    tmpEventInfo.mEventPara<<pPath;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}

void HostNetManager::GrabScreen
    (quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_GrabScreen;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}

void HostNetManager::DelFile
    (QString pFilePath,
     quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_DelFile;
    tmpEventInfo.mEventPara<<pFilePath;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}

void HostNetManager::CreatFile
    (QString pFilePath,
     quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_CreatFile;
    tmpEventInfo.mEventPara<<pFilePath;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}

void HostNetManager::MovePath
    (QString pSrc, QString pDst,
     quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_MovePath;
    tmpEventInfo.mEventPara<<pSrc<<pDst;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}

void HostNetManager::DelPath
    (QString pFilePath,
     quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_DelPath;
    tmpEventInfo.mEventPara<<pFilePath;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}

void HostNetManager::CreatPath
    (QString pFilePath,
     quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_CreatPath;
    tmpEventInfo.mEventPara<<pFilePath;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}

void HostNetManager::Compress
    (QString pSrc, QString pDst,
     quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_Compress;
    tmpEventInfo.mEventPara<<pSrc<<pDst;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}

void HostNetManager::Decompress
    (QString pSrc, QString pDst,
     quint64 pDietime, qreal pRation,
     EVENT_TYPE pFatherType, bool pEnd)
{
    EventInfo tmpEventInfo;
    tmpEventInfo.mEventType = Event_Decompress;
    tmpEventInfo.mEventPara<<pSrc<<pDst;
    tmpEventInfo.mRatio = pRation;
    tmpEventInfo.mEventDietime = pDietime * pRation;
    tmpEventInfo.mFatherType = pFatherType;
    tmpEventInfo.mEndFlag = pEnd;
    PushStack(tmpEventInfo);
}

bool HostNetManager::SendFile(QString pCover, QString pSrc, QString pDst, quint64 pTime)
{
    m_SendFileTaskState = Start;
    SendFileTest(pCover, pSrc, pDst, pTime, 0.05, Event_SendFile, false);
    StartSendFile(pTime, 0.95, Event_SendFile, true);
    while(pTime-- && (m_SendFileTaskState==Start))
        QtSleep(1);
    switch(m_SendFileTaskState)
    {
        case Success:return true;
        default:return false;
    }
    return false;
}

bool HostNetManager::GetFile(QString pCover, QString pSrc, QString pDst, quint64 pTime)
{
    m_GetFileTaskState = Start;
    GetFileTest(pCover, pSrc, pDst, pTime, 0.05, Event_GetFile, false);
    StartGetFile(pTime, 0.95, Event_GetFile, true);
    while(pTime-- && (m_GetFileTaskState==Start))
        QtSleep(1);
    switch(m_GetFileTaskState)
    {
        case Success:return true;
        default:return false;
    }
    return false;
}

bool HostNetManager::GetDesktop(QString pDevSavePath, QString pDst, quint64 pTime)
{
    m_GetDesktopTaskState = Start;
    GrabScreen(pTime, 0.6, Event_GetDesktop, false);
    GetFileTest("y", pDevSavePath, pDst, pTime, 0.05, Event_GetDesktop, false);
    StartGetFile(pTime, 0.25, Event_GetDesktop, false);
    DelFile(pDevSavePath, pTime, 0.1, Event_GetDesktop, true);
    while(pTime-- && (m_GetDesktopTaskState==Start))
        QtSleep(1);
    switch(m_GetDesktopTaskState)
    {
        case Success:return true;
        default:return false;
    }
    return false;
}

bool HostNetManager::DownLoadThemeZip(QString pSrc, QString pDir, quint64 pTime)
{
    m_DownLoadTaskState = Start;
    if(!pSrc.endsWith("/"))
        pSrc.append("/");
    if(!pDir.endsWith("/"))
        pDir.append("/");
    QString tempDir;
    QString dir;
    QString name;
    QString src;
    QString dst;
    QStringList list = pSrc.split("/");
    if(list.count() < 2)
        return false;
    for(int i=0; i<list.count()-2; i++)
    {
        dir.append(list.at(i));
        dir.append("/");
    }
    name = list.at(list.count()-2);
    //>@创建缓存目录
    tempDir = QString("%1Output/").arg(dir);
    DelPath(tempDir, pTime, 0.04, Event_DownLoadTheme, false);
    CreatPath(tempDir, pTime, 0.01, Event_DownLoadTheme, false);
    //>@压缩主题到当前目录
    dst = QString("%1%2").arg(tempDir).arg(name);
    Compress(pSrc, dst, pTime, 0.60, Event_DownLoadTheme, false);
    //>@获取压缩包到目的目录
    src = QString("%1%2").arg(dst).arg(".zip");
    dst = QString("%1%2%3").arg(pDir).arg(name).arg(".zip");
    GetFileTest("y", src, dst, pTime, 0.01, Event_DownLoadTheme, false);
    StartGetFile(pTime, 0.24, Event_DownLoadTheme, false);
    //>@清除缓存目录
    DelPath(tempDir ,pTime, 0.1, Event_DownLoadTheme, true);
    while(pTime-- && (m_DownLoadTaskState==Start))
        QtSleep(1);
    switch(m_DownLoadTaskState)
    {
        case Success:return true;
        default:return false;
    }
    return false;
}

bool HostNetManager::UpLoadThemeZip(QString pSrc, QString pDir, quint64 pTime)
{
    m_UpLoadTaskState = Start;
    if(!pDir.endsWith("/"))
        pDir.append("/");
    QString tempDir;
    QString name;
    QString unSuffixName;
    QString dst;
    QString src;
    //>@创建缓存目录
    tempDir = QString("%1Output/").arg(pDir);
    DelPath(tempDir, pTime, 0.04, Event_UpLoadTheme, false);
    CreatPath(tempDir, pTime, 0.01, Event_UpLoadTheme, false);
    //>@发送压缩包到缓存目录
    name = getFileName(pSrc);
    dst = QString("%1%2").arg(tempDir).arg(name);
    SendFileTest("y", pSrc, dst, pTime, 0.05, Event_UpLoadTheme, false);
    StartSendFile(pTime, 0.2, Event_UpLoadTheme, false);
    //>@压缩包解压到缓存目录
    src = dst;
    Decompress(src, tempDir, pTime, 0.4, Event_UpLoadTheme, false);
    //>@清除同名主题
    QStringList list = name.split(".");
    unSuffixName = list.at(0);
    dst = QString("%1%2/").arg(pDir).arg(unSuffixName);
    DelPath(dst, pTime, 0.05, Event_UpLoadTheme, false);
//>@移动缓存目录解压好的主题到主题目录
    src = QString("%1%2/").arg(tempDir).arg(unSuffixName);
    MovePath(src, pDir, pTime, 0.2, Event_UpLoadTheme, false);
//>@清除缓存目录
    DelPath(tempDir, pTime, 0.05, Event_UpLoadTheme, true);
//>@重启设备
//    RebootDev(Qt::QueuedConnection, Event_UpLoadTheme, true);
    while(pTime-- && (m_UpLoadTaskState==Start))
        QtSleep(1);
    switch(m_UpLoadTaskState)
    {
        case Success:return true;
        default:return false;
    }
    return false;
}


void HostNetManager::SendHB(quint16 pTime)
{
    if(!m_SFConn)
        return;
    m_HBPara = pTime;
    StartHBTime(m_HBPara);
    QTimer::singleShot(m_HBPara / 2, this, SLOT(slot_SendHB()));
}

void HostNetManager::ClearStack()
{
    m_ReadFileOp->close();
    m_WriteFileOp->close();
    m_EventQueue.clear();
}
void HostNetManager::PushStack(EventInfo pEventInfo)
{
    if((!m_SFConn)||
       (pEventInfo.mEventType == Event_Unknow)||
       (pEventInfo.mEventDietime < 5))
    {
        IDE_TRACE();
        return;
    }
    if(m_EventQueue.isEmpty())
    {
        m_EventQueue.enqueue(pEventInfo);
        PopStatck(pEventInfo);
    }
    else
    {
        m_EventQueue.enqueue(pEventInfo);
    }
}
void HostNetManager::PopStatck(EventInfo pEventInfo)
{
    StartDieTime(pEventInfo.mEventDietime);
    SendEvent(pEventInfo);
}

bool HostNetManager::UnQueueEventAckHandle(EventInfo &pEventInfo)
{
    quint8 paraCnt = pEventInfo.mEventPara.count();
    switch(pEventInfo.mEventType)
    {
        case Event_SFDisconn:
        {
            m_SFConn = false;
            emit sConnState(m_SFConn);
            return true;
        }
        case Event_SFConn:
        {
            m_SFConn = true;
            emit sConnState(m_SFConn);
            return true;
        }
        case Event_HB:
        {
            SendHB(D_HBTime);
            return true;
        }
        case Event_DevInfo:
        {
            for(int i=1;i<paraCnt;i++)
            {
                QStringList tmpList = pEventInfo.mEventPara.at(i).split(':');
                if(tmpList.count()!=2)
                    continue;
                m_DevInfoList.insert(tmpList.at(0), tmpList.at(1));
            }
            return true;
        }
        case Event_Set:
        {
            emit sSet(pEventInfo.toContent());
            return true;
        }
        case Event_Get:
        {
            emit sGet(pEventInfo.toContent());
            return true;
        }
        case Event_Message:
        {
            emit sMessage(pEventInfo.toContent());
            return true;
        }
        case Event_Inform:
        {
            if(paraCnt == 0)
                return false;
            QString tmpString = pEventInfo.mEventPara.at(0);
            if(tmpString.compare("ZipTaskProgress", Qt::CaseInsensitive) == 0)
            {
                if(paraCnt != 2)
                    return false;
                qint64 progress0 =0;
                if(!StrToLongLong(pEventInfo.mEventPara.at(1), progress0))
                    return false;
                UpdateProgress(progress0, true);
                return true;
            }
            emit sInform(pEventInfo.toContent());
            return true;
        }
        case Event_Command:
        {
            emit sCommand(pEventInfo.toContent());
            return true;
        }
        default:break;
    }
    return false;
}

bool HostNetManager::QueueEventAckHandle(EventInfo &pEventInfo)
{
    if(m_EventQueue.isEmpty())
    {
        IDE_TRACE_STR(pEventInfo.toFrame());
        return true;
    }
    EventInfo curEventInfo = m_EventQueue.head();
    if(!(curEventInfo & pEventInfo))
    {
        //curEvent
        IDE_TRACE_STR(curEventInfo.toFrame());
        //ackEvent
        IDE_TRACE_STR(pEventInfo.toFrame());
        return true;
    }
    StopDieTime();
    bool capture = pEventInfo.isSuccess();
    if(capture)
    {
        m_Preprogress += 100 * curEventInfo.mRatio;
        int progress = 0;
        UpdateProgress(progress);
        if(!m_EventQueue.isEmpty())
            m_EventQueue.dequeue();
        if(curEventInfo.mEndFlag)
        {
            progress = 255;
            UpdateProgress(progress);
            if(curEventInfo.mFatherType != Event_Unknow)
                pEventInfo.mEventType = curEventInfo.mFatherType;
            capture = false;
            m_Preprogress = 0;
        }
    }
    else
    {
        m_Preprogress = 0;
        const int queueCnt = m_EventQueue.count();
        for(int i=0;i<queueCnt;i++)
        {
            if(m_EventQueue.isEmpty())
                break;
            EventInfo headEvent = m_EventQueue.head();
            QString PopEvents = headEvent.toFrame();
            IDE_TRACE_STR(PopEvents);
            m_EventQueue.dequeue();
            if(headEvent.mEndFlag)
            {
                if(curEventInfo.mFatherType != Event_Unknow)
                    pEventInfo.mEventType = curEventInfo.mFatherType;
                break;
            }
        }
    }
    //>@当前指令收到响应后，发送下一条指令
    if(!m_EventQueue.isEmpty())
    {
        EventInfo nextEventInfo = m_EventQueue.head();
        PopStatck(nextEventInfo);
    }
    return capture;
}

void HostNetManager::StartDieTime(quint64 pTime)
{
    if(m_DieTimer.isActive())
        m_DieTimer.stop();
    m_DieTimer.start(pTime);
}
void HostNetManager::StopDieTime()
{
    if(m_DieTimer.isActive())
        m_DieTimer.stop();
}
void HostNetManager::StartHBTime(quint64 pTime)
{
    if(mHBTimer.isActive())
        mHBTimer.stop();
    mHBTimer.start(pTime);
}
void HostNetManager::StopHBTime()
{
    if(mHBTimer.isActive())
        mHBTimer.stop();
}

void HostNetManager::slot_ZipTaskEnd()
{
    mZipTaskState = Success;
}
void HostNetManager::slot_ZipTaskErr()
{
    mZipTaskState = Failed;
}
void HostNetManager::slot_ZipTaskProgress(int ratio)
{
#ifdef IDE
    GlobalPara.updateProgress(QString("ZipTaskProgress"), ratio);
#endif
}

void HostNetManager::slot_SendHB()
{
    SendCMD(QStringList()<<"HB"<<QString("%1").arg(m_HBPara), Flag_Normal);
}

void HostNetManager::slot_Dietime()
{
    if(m_DieTimer.isActive())
        m_DieTimer.stop();
    m_CurSendEvent.mEventPara<<QString("Timeout")<<D_Failed;
    ProcessEventInfo(m_CurSendEvent);
}

void HostNetManager::slot_HBTimeout()
{
    IDE_TRACE_STR("HBTimeout");
    StopHBTime();
    ClearStack();
    m_SFConn = false;
    emit sConnState(m_SFConn);
}

bool HostNetManager::ProcessEventInfo(EventInfo &pEventInfo)
{
    if(NetFrameParser::ProcessEventInfo(pEventInfo))
        return true;
    bool result = false;
    if(pEventInfo.mEventType >= Event_SFConn && pEventInfo.mEventType <= Event_Command)
        result = UnQueueEventAckHandle(pEventInfo);
    else if(pEventInfo.mEventType >= Event_SendFileTest && pEventInfo.mEventType <= Event_Decompress)
        result = QueueEventAckHandle(pEventInfo);

    bool success = pEventInfo.isSuccess();
    switch(pEventInfo.mEventType)
    {
        case Event_Ls:
        {
            return true;
        }
        case Event_GrabScreen:
        {
            emit sGrabScreen(success);
            return true;
        }
        case Event_GetFileTest:
        {
            m_PackageDieCnt = 0;
            quint8 paraCnt = pEventInfo.mEventPara.count();
            if(paraCnt>=5)
            {
                if(success)
                {
                    qint64 size = 0;
                    if(!StrToLongLong(pEventInfo.mEventPara.at(paraCnt - 2), size))
                    {
                        pEventInfo.mEventPara.replace((paraCnt - 1), "SizeErr");
                        pEventInfo.mEventPara<<D_Failed;
                    }
                    else
                    {
                        if(!CheckCapacity(getFileDirectory(pEventInfo.mEventPara.at(1)), size))
                        {
                            pEventInfo.mEventPara.replace((paraCnt - 1), "HostCapacityErr");
                            pEventInfo.mEventPara<<D_Failed;
                        }
                        else
                        {
                            m_WriteFileOp->mSize = size;
                        }
                    }
                }
                else
                {
                    m_WriteFileOp->close();
                }
            }
            break;
        }
        case Event_DelFile:
        {
            emit sDelFile(success);
            return true;
        }
        case Event_CreatFile:
        {
            emit sCreatFile(success);
            return true;
        }
        case Event_Compress:
        {
            emit sCompress(success);
            return true;
        }
        case Event_Decompress:
        {
            emit sDecompress(success);
            return true;
        }
        //>@
        case Event_SendFile:
        {
            m_SendFileTaskState = (TaskState)success;
            emit sSendFile(success);
            return true;
        }
        case Event_GetFile:
        {
            m_GetFileTaskState = (TaskState)success;
            emit sGetFile(success);
            return true;
        }
        case Event_GetDesktop:
        {
            m_GetDesktopTaskState = (TaskState)success;
            emit sGetDesktop(success);
            return true;
        }
        case Event_DownLoadTheme:
        {
            m_DownLoadTaskState = (TaskState)success;
            emit sDownLoadThemeZip(success);
            return true;
        }
        case Event_UpLoadTheme:
        {
            m_UpLoadTaskState = (TaskState)success;
            emit sUpLoadThemeZip(success);
            return true;
        }
    }
}


/******************************************************************************
//>@
******************************************************************************/
GuestNetManager::GuestNetManager(QObject *parent) :
    NetFrameParser(parent)
{
}

GuestNetManager::~GuestNetManager()
{
    disconnect(this,SIGNAL(sConnState(bool)),0,0);
    disconnect(this,SIGNAL(sNews(QString)),0,0);
    disconnect(this, SIGNAL(sGetDevInfo()), 0, 0);
    disconnect(this,SIGNAL(sReboot()),0,0);
}

void GuestNetManager::SendAck(QString pAck)
{
    SendCMD(pAck,Flag_Ack);
}

void GuestNetManager::ReStartHBTimer(quint64 pTime)
{
    if(mHBTimer.isActive())
        mHBTimer.stop();
    mHBTimer.start(pTime);
}

void GuestNetManager::slot_HBTimeout()
{
    IDE_TRACE_STR("HBTimeout");
    if(mHBTimer.isActive())
        mHBTimer.stop();
    m_SFConn = false;
    emit sConnState(m_SFConn);
}

void GuestNetManager::slot_ZipThread()
{
    QThread ZipThread;
    ZipTask compressZipTask;
    connect(this,SIGNAL(sZip(QString, QString, bool)),&compressZipTask,SLOT(Zip(QString, QString, bool)));
    connect(&compressZipTask,SIGNAL(end()),this,SLOT(slot_ZipTaskEnd()));
    connect(&compressZipTask,SIGNAL(error()),this,SLOT(slot_ZipTaskErr()));
    connect(&compressZipTask,SIGNAL(progress(int)),this,SLOT(slot_ZipTaskProgress(int)));
    compressZipTask.moveToThread(&ZipThread);
    ZipThread.start(QThread::LowPriority);
    ZipThread.wait(10);
    mZipTaskState = Start;
    emit sZip(m_CurRcvEvent.mEventPara.at(0), m_CurRcvEvent.mEventPara.at(1), true);
//        ZipThread.wait();
    while(mZipTaskState == Start)
        QtSleep(1);
    if(mZipTaskState = Success)
    {
        m_CurRcvEvent.mEventPara<<D_Success;
        SendAck(m_CurRcvEvent.toFrame());
    }
    else
    {
        m_CurRcvEvent.mEventPara<<D_Failed;
        SendAck(m_CurRcvEvent.toFrame());
    }
}
void GuestNetManager::slot_UnzipThread()
{
    QThread ZipThread;
    ZipTask decompressZipTask;
    connect(this,SIGNAL(sUnzip(QString, QString, bool)),&decompressZipTask,SLOT(Unzip(QString, QString, bool)));
    connect(&decompressZipTask,SIGNAL(end()),this,SLOT(slot_ZipTaskEnd()));
    connect(&decompressZipTask,SIGNAL(error()),this,SLOT(slot_ZipTaskErr()));
    connect(&decompressZipTask,SIGNAL(progress(int)),this,SLOT(slot_ZipTaskProgress(int)));
    decompressZipTask.moveToThread(&ZipThread);
    ZipThread.start(QThread::LowPriority);
    ZipThread.wait(10);
    mZipTaskState = Start;
    emit sUnzip(m_CurRcvEvent.mEventPara.at(0), m_CurRcvEvent.mEventPara.at(1), true);
    while(mZipTaskState == Start)
        QtSleep(1);
    if(mZipTaskState = Success)
    {
        m_CurRcvEvent.mEventPara<<D_Success;
        SendAck(m_CurRcvEvent.toFrame());
    }
    else
    {
        m_CurRcvEvent.mEventPara<<D_Failed;
        SendAck(m_CurRcvEvent.toFrame());
    }
}
void GuestNetManager::slot_ZipTaskEnd()
{
    mZipTaskState = Success;
}

void GuestNetManager::slot_ZipTaskErr()
{
    mZipTaskState = Failed;
}

void GuestNetManager::slot_ZipTaskProgress(int ratio)
{
    //>@发送本机压缩的进度
    SendInform(QString("ZipTaskProgress%1%2").arg(D_CMDSUBSPLIT).arg(ratio));
}

bool GuestNetManager::ProcessEventInfo(EventInfo &pEventInfo)
{
    if(NetFrameParser::ProcessEventInfo(pEventInfo))
        return true;
    quint8 paraCnt = pEventInfo.mEventPara.count();
    switch(pEventInfo.mEventType)
    {
        case Event_SFConn:
        {
            m_SFConn = true;
            emit sConnState(m_SFConn);
            pEventInfo.mEventPara<<D_Success;
            SendAck(pEventInfo.toFrame());
            return true;
        }
        case Event_SFDisconn:
        {
            m_SFConn = false;
            emit sConnState(m_SFConn);
            pEventInfo.mEventPara<<D_Success;
            SendAck(pEventInfo.toFrame());
        }
        case Event_HB:
        {
            bool flag = false;
            if(paraCnt == 2)
            {
                qint64 hBPara = 0;
                if(StrToLongLong(pEventInfo.mEventPara.at(1), hBPara))
                {
                    ReStartHBTimer(hBPara);
                    flag = true;
                }
            }
            pEventInfo.mEventPara << ((flag)?D_Success:D_Failed);
            SendAck(pEventInfo.toFrame());
            return true;
        }
        case Event_Stop:
        {
            switch(m_CurRcvEvent.mEventType)
            {
                case Event_SendFileTest:
                case Event_StartSendFile:
                case Event_GetFileTest:
                case Event_StartGetFile:
                {
                    m_ReadFileOp->close();
                    m_WriteFileOp->close();
                    break;
                }
                case Event_Compress:
                case Event_Decompress:
                {
                    break;
                }
                default:break;
            }
            return true;
        }
        case Event_DevInfo:
        {
            pEventInfo.mEventPara<<QString("%1:%2").arg("FSRC").arg(D_FSRCPATH);
#ifdef DISPLAY
            QList<PH_Type> tmpList = m_DevManager->m_PHDevList.keys();
            for(int i=0;i<tmpList.count();i++)
            {
                PhysicalDevice* tmpPHDev = m_DevManager->m_PHDevList.value(tmpList.at(i));
                if(tmpPHDev != 0)
                    pEventInfo.mEventPara<<tmpPHDev->GetInformation();
            }
            QList<LG_Type> tmpList = m_DevManager->m_LGDevList.keys();
            for(int i=0;i<tmpList.count();i++)
            {
                LogicDevice* tmpLGDev = m_DevManager->m_LGDevList.value(tmpList.at(i));
                if(tmpLGDev != 0)
                    pEventInfo.mEventPara<<tmpLGDev->GetInformation();
            }
#endif
            pEventInfo.mEventPara<<D_Success;
            SendAck(pEventInfo.toFrame());
            return true;
        }
        case Event_Set:
        {
            if(paraCnt != 2)
            {
                pEventInfo.mEventPara<<D_Failed;
                SendAck(pEventInfo.toFrame());
                return true;
            }
            xmlParse *tmpSetParse = new xmlParse;
            if(tmpSetParse->setContent(pEventInfo.mEventPara.at(1)))
            {
                QDomElement tmpElement = tmpSetParse->documentElement();
                if(!tmpElement.isNull())
                {
                    QString tmpCommonType = tmpElement.tagName();
                    if(tmpCommonType.compare("Physical") == 0)
                    {
#ifdef DISPLAY
                        QDomNodeList tmpPHDevList = tmpElement.childNodes();
                        for(int i=0;i<tmpPHDevList.count();i++)
                        {
                            QDomElement tmpPHElement = tmpPHDevList.at(i).toElement();
                            if(tmpPHElement.isNull())
                                continue;
                            PhysicalDevice* tmpPHDev = m_DevManager->m_PHDevList.value(GetPHType(tmpPHElement.tagName()));
                            if(!tmpPHDev)
                                continue;
                            tmpPHDev->SetPara(tmpPHElement, tmpSetParse);
                        }
#endif
                    }
                    else if(tmpCommonType.compare("Logic") == 0)
                    {
#ifdef DISPLAY
                        QDomNodeList tmpLGDevList = tmpElement.childNodes();
                        for(int i=0;i<tmpLGDevList.count();i++)
                        {
                            QDomElement tmpLGElement = tmpLGDevList.at(i).toElement();
                            if(tmpLGElement.isNull())
                                continue;
                            LogicDevice* tmpLGDev = m_DevManager->m_LGDevList.value(GetLGType(tmpLGElement.tagName()));
                            if(!tmpLGDev)
                                continue;
                            tmpLGDev->SetPara(tmpLGElement, tmpSetParse);
                        }
#endif
                    }
                    else if(tmpCommonType.compare("Theme") == 0)
                    {
#ifdef DISPLAY
                        LogicThemeManager* tmpThemeManager = VAR_CAST<LogicThemeManager*>(m_DevManager->m_LGDevList.value(L_ThemeManager));

#endif
                    }
                }
            }
            pEventInfo.mEventPara.replace(1, tmpSetParse->toString());
            pEventInfo.mEventPara<<D_Success;
            SendAck(pEventInfo.toFrame());
            return true;
        }
        case Event_Get:
        {
            if(paraCnt != 2)
            {
                pEventInfo.mEventPara<<D_Failed;
                SendAck(pEventInfo.toFrame());
                return true;
            }
            xmlParse *tmpSetParse = new xmlParse;
            if(tmpSetParse->setContent(pEventInfo.mEventPara.at(1)))
            {
                QDomElement tmpElement = tmpSetParse->documentElement();
                if(!tmpElement.isNull())
                {
                    QString tmpCommonType = tmpElement.tagName();
                    if(tmpCommonType.compare("Physical") == 0)
                    {
#ifdef DISPLAY
                        QDomNodeList tmpPHDevList = tmpElement.childNodes();
                        for(int i=0;i<tmpPHDevList.count();i++)
                        {
                            QDomElement tmpPHElement = tmpPHDevList.at(i).toElement();
                            if(tmpPHElement.isNull())
                                continue;
                            PhysicalDevice* tmpPHDev = m_DevManager->m_PHDevList.value(GetPHType(tmpPHElement.tagName()));
                            if(!tmpPHDev)
                                continue;
                            tmpPHDev->GetPara(tmpPHElement, tmpSetParse);
                        }
#endif
                    }
                    else if(tmpCommonType.compare("Logic") == 0)
                    {
#ifdef DISPLAY
                        QDomNodeList tmpLGDevList = tmpElement.childNodes();
                        for(int i=0;i<tmpLGDevList.count();i++)
                        {
                            QDomElement tmpLGElement = tmpLGDevList.at(i).toElement();
                            if(tmpLGElement.isNull())
                                continue;
                            LogicDevice* tmpLGDev = m_DevManager->m_LGDevList.value(GetLGType(tmpLGElement.tagName()));
                            if(!tmpLGDev)
                                continue;
                            tmpLGDev->GetPara(tmpLGElement, tmpSetParse);
                        }
#endif
                    }
                    else if(tmpCommonType.compare("Theme") == 0)
                    {
#ifdef DISPLAY
                        LogicThemeManager* tmpThemeManager = VAR_CAST<LogicThemeManager*>(m_DevManager->m_LGDevList.value(L_ThemeManager));
#endif
                    }
                }
            }
            pEventInfo.mEventPara.replace(1, tmpSetParse->toString());
            pEventInfo.mEventPara<<D_Success;
            SendAck(pEventInfo.toFrame());
            return true;
        }
        case Event_Message:
        {
            emit sMessage(pEventInfo.toContent());
            return true;
        }
        case Event_Inform:
        {
            emit sInform(pEventInfo.toContent());
            return true;
        }
        case Event_Command:
        {
#ifdef DISPLAY   //>@执行标准指令
            for(int i=0;i<pEventInfo.mEventPara.count();i++)
            {
                QString tmpCMD = pEventInfo.mEventPara.at(i);
                QProcess tmpProcess;
                tmpProcess.setProcessChannelMode(QProcess::MergedChannels);
                tmpProcess.start(tmpCMD);
                tmpProcess.waitForFinished();
                tmpCMD.append('\n');
                tmpCMD.append(tmpProcess.readAllStandardOutput());
                pEventInfo.mEventPara.replace(i, tmpCMD);
                tmpProcess.kill();
            }
#endif
            pEventInfo.mEventPara<<D_Success;
            SendAck(pEventInfo.toFrame());
            return true;
        }
        case Event_SendFileTest:
        {
            m_PackageDieCnt = 0;
            m_CurRcvEvent = pEventInfo;
            if(paraCnt == 4)
            {
                qint64 size = 0;
                if(!StrToLongLong(pEventInfo.mEventPara.at(3), size))
                {
                    pEventInfo.mEventPara<<QString("SizeErr")<<D_Failed;
                    goto SENDACk;
                }
                if(!CheckCapacity(getFileDirectory(pEventInfo.mEventPara.at(2)), size))
                {
                    pEventInfo.mEventPara<<QString("DevCapacityErr")<<D_Failed;
                    goto SENDACk;
                }
                m_WriteFileOp->mSize = size;
                if(m_WriteFileOp->isExist(pEventInfo.mEventPara.at(2)))
                {
                    if(pEventInfo.mEventPara.at(0).compare("y", Qt::CaseInsensitive) == 0)
                    {
                        if(m_WriteFileOp->open(pEventInfo.mEventPara.at(2)))
                        {
                            m_WriteFileOp->savePath(pEventInfo.mEventPara.at(2));
                            pEventInfo.mEventPara<<D_Success;
                        }
                        else
                        {
                            pEventInfo.mEventPara<<QString("Locked")<<D_Failed;
                        }
                    }
                    else
                    {
                        pEventInfo.mEventPara<<QString("Exist")<<D_Failed;
                    }
                }
                else
                {
                    if(m_WriteFileOp->isPathExist(pEventInfo.mEventPara.at(2)))
                    {
                        if(m_WriteFileOp->open(pEventInfo.mEventPara.at(2)))
                        {
                            m_WriteFileOp->savePath(pEventInfo.mEventPara.at(2));
                            pEventInfo.mEventPara<<D_Success;
                        }
                        else
                        {
                            pEventInfo.mEventPara<<QString("PathWriteErr")<<D_Failed;
                        }
                    }
                }
SENDACk:
                SendAck(pEventInfo.toFrame());
            }
            return true;
        }
        case Event_GetFileTest:
        {
            m_PackageDieCnt = 0;
            m_CurRcvEvent = pEventInfo;
            if(paraCnt == 3)
            {
                if(m_ReadFileOp->isExist(pEventInfo.mEventPara.at(1)))
                {
                    if(m_ReadFileOp->open(pEventInfo.mEventPara.at(1)))
                    {
                        m_ReadFileOp->savePath(pEventInfo.mEventPara.at(1));
                        QString size = QString("%1").arg(m_ReadFileOp->mSize);
                        pEventInfo.mEventPara<<size<<D_Success;
                    }
                    else
                    {
                        pEventInfo.mEventPara<<QString("Locked")<<D_Failed;
                    }
                }
                else
                {
                    pEventInfo.mEventPara<<QString("UnExist")<<D_Failed;
                }
                SendAck(pEventInfo.toFrame());
            }
            return true;
        }
        case Event_StartGetFile:
        {
            m_PackageDieCnt = 0;
            m_CurRcvEvent = pEventInfo;
            if(pEventInfo.mEventPara.count() == 0)
                SendFileData();
            return true;
        }
        case Event_GrabScreen:
        {
            m_CurRcvEvent = pEventInfo;
            if(PrintSence(D_GrabScreenPath+D_GrabScreenName))
                pEventInfo.mEventPara<<D_Success;
            else
                pEventInfo.mEventPara<<D_Failed;
            SendAck(pEventInfo.toFrame());
            return true;
        }
        case Event_DelFile:
        {
            m_CurRcvEvent = pEventInfo;
            if(paraCnt == 1)
            {
                if(DelFile(pEventInfo.mEventPara.at(0)))
                    pEventInfo.mEventPara<<D_Success;
                else
                    pEventInfo.mEventPara<<D_Failed;
                SendAck(pEventInfo.toFrame());
            }
            return true;
        }
        case Event_CreatFile:
        {
            m_CurRcvEvent = pEventInfo;
            if(paraCnt == 1)
            {
                if(CreatFile(pEventInfo.mEventPara.at(0)))
                    pEventInfo.mEventPara<<D_Success;
                else
                    pEventInfo.mEventPara<<D_Failed;
                SendAck(pEventInfo.toFrame());
            }
            return true;
        }
        case Event_MovePath:
        {
            m_CurRcvEvent = pEventInfo;
            if(paraCnt == 2)
            {
                if(MoveFolder(pEventInfo.mEventPara.at(0), pEventInfo.mEventPara.at(1), true))
                    pEventInfo.mEventPara<<D_Success;
                else
                    pEventInfo.mEventPara<<D_Failed;
                SendAck(pEventInfo.toFrame());
            }
            return true;
        }
        case Event_DelPath:
        {
            m_CurRcvEvent = pEventInfo;
            if(paraCnt == 1)
            {
                if(DelFolder(pEventInfo.mEventPara.at(0)))
                    pEventInfo.mEventPara<<D_Success;
                else
                    pEventInfo.mEventPara<<D_Failed;
                SendAck(pEventInfo.toFrame());
            }
            return true;
        }
        case Event_CreatPath:
        {
            m_CurRcvEvent = pEventInfo;
            if(paraCnt == 1)
            {
                if(CreatPath(pEventInfo.mEventPara.at(0)))
                    pEventInfo.mEventPara<<D_Success;
                else
                    pEventInfo.mEventPara<<D_Failed;
                SendAck(pEventInfo.toFrame());
            }
            return true;
        }
        case Event_Compress:
        {
            m_CurRcvEvent = pEventInfo;
            if(paraCnt == 2)
            {
            //>@QTimer::singleShot 默认就是异步槽
                QTimer::singleShot(1, this, SLOT(slot_ZipThread()));
            }
            return true;
        }
        case Event_Decompress:
        {
            m_CurRcvEvent = pEventInfo;
            if(paraCnt == 2)
            {
            //>@QTimer::singleShot 默认就是异步槽
                QTimer::singleShot(1, this, SLOT(slot_UnzipThread()));
            }
            return true;
        }
        case Event_Ls:
        {
            m_CurRcvEvent = pEventInfo;
            if(paraCnt == 1)
            {
                SendAck(Ls(pEventInfo.mEventPara.at(0)));
            }
            return true;
        }
        default:
            return false;
    }
}

