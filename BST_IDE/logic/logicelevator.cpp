#include "logicelevator.h"
#include "physicalparallel.h"

/*************************************************************
//>@C_IB
**************************************************************/
IBFrameParser::IBFrameParser(QObject *parent) :
    LogicDevice(parent)
{
    Init();
}

IBFrameParser::~IBFrameParser()
{
    Release();
}

void IBFrameParser::Init()
{
    m_PHDev = 0;
    m_FrameState = State_Wait;
    m_CurFrameType = Flag_Normal;
    m_FrameBuffer.clear();
}

void IBFrameParser::Release()
{
    m_FrameState = State_Wait;
    m_CurFrameType = Flag_Normal;
    m_FrameBuffer.clear();
}

quint64 IBFrameParser::SendDataStream(QByteArray pData)
{
    if(m_PHDev==0 || pData.isEmpty())
    {
        IDE_TRACE();
        return 0;
    }
    quint64 len = 0;
    len += m_PHDev->write(pData);
    pData.clear();
    return len;
}

bool IBFrameParser::SendFrame(IBFRAME_STRUCT pFrame, FRAME_FLAG pFlag)
{
    QByteArray tmpSendData;
    tmpSendData.clear();
    switch(pFlag)
    {
        case Flag_Normal:tmpSendData.append(D_FRAMEHEADER);break;
        case Flag_Ack:tmpSendData.append(D_FRAMEHEADER_ACK);break;
        default:tmpSendData.clear();return false;
    }
    quint8 tmpCRC = 0;
    tmpSendData.append(pFrame.mCmd);
    tmpCRC ^= pFrame.mCmd;
    for(int i=0;i<7;i++)
    {
        tmpSendData.append(pFrame.mData[i]);
        tmpCRC ^= pFrame.mData[i];
    }
    tmpSendData.append(tmpCRC&0x7f);
    switch(pFlag)
    {
        case Flag_Normal:tmpSendData.append(D_FRAMEFOOTER);break;
        case Flag_Ack:tmpSendData.append(D_FRAMEFOOTER_ACK);break;
        default:tmpSendData.clear();return false;
    }
    SendDataStream(tmpSendData);
    tmpSendData.clear();
    return true;
}

void IBFrameParser::ParseDataStream()
{
    PhysicalDevice* tmpPHDev = VAR_CAST<PhysicalDevice*>(sender());
    if(tmpPHDev == 0)
        return;
    if(!mPHDevices.contains(tmpPHDev->mDevType))
        return;
    m_PHDev = tmpPHDev;
    QByteArray tmpBufferData;
    tmpBufferData.clear();
    tmpBufferData = m_PHDev->readAll();
    int tmpCount = tmpBufferData.count();
    if(tmpCount < D_IBFRAMELEN)
        return;
    int i=0;
    quint8 tmpValue;
    while(i<tmpCount)
    {
        switch(m_FrameState)
        {
            case State_Wait:
            {
                m_FrameBuffer.clear();
                for(;i<tmpCount;i++)
                {
                    tmpValue = (quint8)(tmpBufferData.at(i));
                    if(tmpValue == (quint8)(D_FRAMEHEADER))
                    {
                        m_CurFrameType = Flag_Normal;
                        m_FrameState = State_Continus;
                        i++;
                        break;
                    }
                    else if(tmpValue == (quint8)(D_FRAMEHEADER_ACK))
                    {
                        m_CurFrameType = Flag_Ack;
                        m_FrameState = State_Continus;
                        i++;
                        break;
                    }
                }
                break;
            }
            case State_Continus:
            {
                for(;i<tmpCount;i++)
                {
                    tmpValue = (quint8)tmpBufferData.at(i);
                    if(m_CurFrameType == Flag_Normal && tmpValue == (quint8)(D_FRAMEFOOTER))
                    {
                        FrameVerify();
                        m_FrameBuffer.clear();
                        m_FrameState = State_Wait;
                        i++;
                        break;
                    }
                    else if(m_CurFrameType == Flag_Ack && tmpValue == (quint8)(D_FRAMEFOOTER_ACK))
                    {
                        FrameVerify();
                        m_FrameBuffer.clear();
                        m_FrameState = State_Wait;
                        i++;
                        break;
                    }
                    m_FrameBuffer.append(tmpValue);
                }
                break;
            }
            default:
                return;
        }
    }
    tmpBufferData.clear();
}

void IBFrameParser::FrameVerify()
{
    quint32 len = m_FrameBuffer.count();
    if(len != D_IBFRAMELEN-2)  //>@??¨¢?¦Ì?3¡è?¨¨?a11-2
    {
        //IDE_TRACE_INT(len);
        return;
    }
    quint8 tmpValue = (quint8)(m_FrameBuffer.at(len-1))&0x7f;
    quint8 tmpCRC = 0;
    for(int m=0;m<len-1;m++)
    {
        tmpCRC ^= (quint8)(m_FrameBuffer.at(m));
    }
    tmpCRC &= 0x7f;
    if(tmpCRC != tmpValue)
    {
        IDE_TRACE_INT(tmpCRC);
        IDE_TRACE_INT(tmpValue);
        return;
    }
    NewFrameDetected();
}

ElevatorSimulator::ElevatorSimulator(QObject *parent) :
    IBFrameParser(parent)
{
    m_WorkMode = MODE_NORMAL;
    for(int m=0;m<3;m++)
    {
        for(int n=0;n<2;n++)
        {
            m_BufferFrame[m][n].mCmd = Type_NONE;
            for(int i=0;i<7;i++)
                m_BufferFrame[m][n].mData[i] = 0;
        }
    }
    for(int i =0;i<6;i++)
    {
        m_ButtonState[i] = 0;
    }
    m_DisablityMode = false;
    m_TouchOutput = 0;
    m_SwitchOutput = 0;

    m_AutoRunTime = 0;
    m_MinRange = 0;
    m_MaxRange = 0xff;
    connect(&m_AutoRunTimer,SIGNAL(timeout()),this,SLOT(AutoRunTimer()));
}

ElevatorSimulator::~ElevatorSimulator()
{
    disconnect(&m_AutoRunTimer,SIGNAL(timeout()),this,SLOT(AutoRunTimer()));
    Destory();

    m_WorkMode = MODE_NORMAL;
    for(int m=0;m<3;m++)
    {
        for(int n=0;n<2;n++)
        {
            m_BufferFrame[m][n].mCmd = Type_NONE;
            for(int i=0;i<7;i++)
                m_BufferFrame[m][n].mData[i] = 0;
        }
    }
    for(int i =0;i<6;i++)
    {
        m_ButtonState[i] = 0;
    }
    m_DisablityMode = false;
    m_TouchOutput = 0;
    m_SwitchOutput = 0;
}

void ElevatorSimulator::NewFrameDetected()
{
    FRAME_TYPE tmpType = (FRAME_TYPE)(m_FrameBuffer.at(0));
    switch(tmpType)
    {
        case Type_FloorStatus:
        {
            m_BufferFrame[tmpType][Type_Rev].mCmd = tmpType;
            for(int n=1;n<8;n++)
            {
                m_BufferFrame[tmpType][Type_Rev].mData[n-1] = m_FrameBuffer.at(n);
            }
            if(m_CurFrameType == Flag_Ack)
            {
                ParseFloorFrame(m_BufferFrame[tmpType][Type_Rev].mData);
            }
            break;
        }
        case Type_ButtonStatus:
        {
            m_BufferFrame[tmpType][Type_Rev].mCmd = tmpType;
            for(int n=1;n<8;n++)
            {
                m_BufferFrame[tmpType][Type_Rev].mData[n-1] = m_FrameBuffer.at(n);
            }
            if(m_CurFrameType == Flag_Ack)
            {
                ParseButtonFrame(m_BufferFrame[tmpType][Type_Rev].mData);
            }
            break;
        }
        case Type_DisabilityBtnStatus:
        {
            m_BufferFrame[tmpType][Type_Rev].mCmd = tmpType;
            for(int n=1;n<8;n++)
            {
                m_BufferFrame[tmpType][Type_Rev].mData[n-1] = m_FrameBuffer.at(n);
            }
            if(m_CurFrameType == Flag_Ack)
            {
                ParseDisBtnFrame(m_BufferFrame[tmpType][Type_Rev].mData);
            }
            break;
        }
        default:
            IDE_TRACE();
            return;
    }
    return;
}
void ElevatorSimulator::ParseFloorFrame(quint8 *pData)
{
    //>@?D??¡ä£¤¦Ì?¨º?3?
    quint8 tmpTouchOutput = pData[0]&0x3f;
    if(tmpTouchOutput != m_TouchOutput)
    {
        emit TouchOutputChange(tmpTouchOutput);
        m_TouchOutput = tmpTouchOutput;
    }
    //>@?D???a1?¨º?3?
    quint8 tmpSwitchOutput = pData[1]&0x1f;
    if(tmpSwitchOutput != m_SwitchOutput)
    {
        emit SwitchOutputChange(tmpSwitchOutput);
        m_SwitchOutput = tmpSwitchOutput;
    }
    //>@?D??1|?¨¹¨¦¨¨??
    quint8 tmpValue = pData[2]&0x08;
    IBWORKMODE tmpWorkmode = m_WorkMode;
    if(tmpValue == 0x04)
    {
        tmpWorkmode = MODE_CONFIGURE;
    }
    else if(tmpValue == 0x02)
    {
        tmpWorkmode = MODE_DEBUG;
    }
    else if(tmpValue == 0x01)
    {
        tmpWorkmode = MODE_APPUPGRADE;
    }
    else if(tmpValue == 0x00)
    {
        tmpWorkmode = MODE_NORMAL;
    }
    if(tmpWorkmode != m_WorkMode)
    {
        emit WorkModeChange(tmpWorkmode);
        m_WorkMode = tmpWorkmode;
    }
}
void ElevatorSimulator::ParseButtonFrame(quint8 *pData)
{
    //>@?a??¡ã¡ä?£¤¡ä£¤¦Ì?D?o?
    quint8 tmpButtonNum = -1;
    quint8 tmpButtonState[6];
    for(int n=0;n<6;n++)
    {
        tmpButtonState[n] = pData[n]&0x7f;
        for(int m=0;m<7;m++)
        {
            tmpButtonNum = (n+1)*(m+1);
            quint8 tmpNewBtnState = tmpButtonState[n]&(1<<m);
            quint8 tmpOldBtnState = m_ButtonState[n]&(1<<m);
            if(tmpOldBtnState != tmpNewBtnState)
            {
                if(tmpOldBtnState == 0)
                {
                    emit ButtonStateChange(tmpButtonNum, STATE_PRESS);
                    m_ButtonState[n] |= (1<<m);
                }
                else
                {
                    emit ButtonStateChange(tmpButtonNum, STATE_RELEASE);
                    m_ButtonState[n] &= ~(1<<m);
                }
            }
        }
    }
}
void ElevatorSimulator::ParseDisBtnFrame(quint8 *pData)
{
    ParseButtonFrame(pData);
}

void ElevatorSimulator::SetFloor(quint32 pFloorNum)
{
    m_BufferFrame[Type_FloorStatus][Type_Send].mCmd = Type_FloorStatus;
    m_BufferFrame[Type_FloorStatus][Type_Send].mData[0] = pFloorNum & 0x7f;
    m_BufferFrame[Type_FloorStatus][Type_Send].mData[1] = (pFloorNum >> 8) & 0x7f;
    m_BufferFrame[Type_FloorStatus][Type_Send].mData[2] = (pFloorNum >> 16) & 0x7f;
}

void ElevatorSimulator::ReservedSetFloor(quint32 pFloorNum)
{
    quint8 tmpValue;
    tmpValue = pFloorNum / 100;
    m_BufferFrame[Type_FloorStatus][Type_Send].mCmd = Type_FloorStatus;
    m_BufferFrame[Type_FloorStatus][Type_Send].mData[0] = tmpValue & 0x7f;
    tmpValue = pFloorNum % 100 / 10;
    m_BufferFrame[Type_FloorStatus][Type_Send].mData[1] = tmpValue & 0x7f;
    tmpValue = pFloorNum % 10;
    m_BufferFrame[Type_FloorStatus][Type_Send].mData[2] = tmpValue & 0x7f;
}

void ElevatorSimulator::SetDoorState(DOOR_STATE pDoorState)
{

}

void ElevatorSimulator::SetArrowState(ARROWSTATE pArrowState)
{
    m_BufferFrame[Type_FloorStatus][Type_Send].mCmd = Type_FloorStatus;
    m_BufferFrame[Type_FloorStatus][Type_Send].mData[3] = pArrowState;
}

void ElevatorSimulator::SetStationArrive(bool pEnable)
{
    m_BufferFrame[Type_FloorStatus][Type_Send].mCmd = Type_FloorStatus;
    switch(pEnable)
    {
        case true:
            m_BufferFrame[Type_FloorStatus][Type_Send].mData[3] |= D_SignalArrive;break;
        case false:
            m_BufferFrame[Type_FloorStatus][Type_Send].mData[3] &= ~D_SignalArrive;break;
    }
}

void ElevatorSimulator::SetVoiceEnable(bool pEnable)
{
    m_BufferFrame[Type_FloorStatus][Type_Send].mCmd = Type_FloorStatus;
    switch(pEnable)
    {
        case true:
            m_BufferFrame[Type_FloorStatus][Type_Send].mData[3] |= D_SignalVoice;break;
        case false:
            m_BufferFrame[Type_FloorStatus][Type_Send].mData[3] &= ~D_SignalVoice;break;
    }
}

void ElevatorSimulator::SetLowPower(bool pEnable)
{
    m_BufferFrame[Type_FloorStatus][Type_Send].mCmd = Type_FloorStatus;
    switch(pEnable)
    {
        case true:
            m_BufferFrame[Type_FloorStatus][Type_Send].mData[3] |= D_SignalLowPower;break;
        case false:
            m_BufferFrame[Type_FloorStatus][Type_Send].mData[3] &= ~D_SignalLowPower;break;
    }
}

void ElevatorSimulator::SetFuncState(FUNCTION_STATE pFuncState, quint8 pFunData)
{
    m_BufferFrame[Type_FloorStatus][Type_Send].mCmd = Type_FloorStatus;
    switch(pFuncState)
    {
        case FunctionUnknow:
            break;
        case Function1:
            m_BufferFrame[Type_FloorStatus][Type_Send].mData[4] = pFunData;break;
        case Function2:
            m_BufferFrame[Type_FloorStatus][Type_Send].mData[5] = pFunData;break;
        case Function3:
            m_BufferFrame[Type_FloorStatus][Type_Send].mData[6] = pFunData;break;
    }
}

void ElevatorSimulator::SetButtonState(quint8 pBtnNum, BUTTONSTATE pState)
{
    int m = 0, n = 0;
    m = pBtnNum / 7 - 1;
    n = pBtnNum % 7 - 1;
    m_BufferFrame[Type_ButtonStatus][Type_Send].mCmd = Type_ButtonStatus;
    quint8 tmpValue = m_BufferFrame[Type_ButtonStatus][Type_Send].mData[m];
    if(pState == STATE_LIGHT)
    {
        m_BufferFrame[Type_ButtonStatus][Type_Send].mData[m] = tmpValue | (1<<n);
    }
    else if(pState == STATE_NORMAL)
    {
        m_BufferFrame[Type_ButtonStatus][Type_Send].mData[m] = tmpValue & ~(1<<n);
    }
}

void ElevatorSimulator::SetDisablityMode(bool pDisablity)
{
    m_DisablityMode = pDisablity;
}

void ElevatorSimulator::Destory()
{
    if(m_AutoRunTimer.isActive())
        m_AutoRunTimer.stop();
    m_AutoRunTime = 0;
}

void ElevatorSimulator::SetAutoRunRange(quint8 pMin, quint8 pMax)
{
    if(pMin >= pMax)
        return;
    m_MinRange = pMin;
    m_MaxRange = pMax;
}

void ElevatorSimulator::SetAutoRunTime(quint16 pTime)
{
    if(pTime<100)
        pTime = 100;
    if(m_AutoRunTimer.isActive())
        m_AutoRunTimer.stop();
    m_AutoRunTimer.start(pTime);
}

void ElevatorSimulator::StopAutoRun()
{
    if(m_AutoRunTimer.isActive())
        m_AutoRunTimer.stop();
}

void ElevatorSimulator::SendFloorPackage()
{
    IBFRAME_STRUCT tmpBuf;
    tmpBuf = m_BufferFrame[Type_FloorStatus][Type_Send];
    SendFrame(tmpBuf,Flag_Normal);
}

void ElevatorSimulator::SendBtnPackage()
{
    IBFRAME_STRUCT tmpBuf;
    tmpBuf = m_BufferFrame[Type_ButtonStatus][Type_Send];
    SendFrame(tmpBuf,Flag_Normal);
}

void ElevatorSimulator::SendDisabilityPackage()
{}

void ElevatorSimulator::SendAppUpgradePackage()
{}

void ElevatorSimulator::SendDebugPackage()
{}

void ElevatorSimulator::SendConfigurePackage()
{}

void ElevatorSimulator::AutoRunTimer()
{
    static quint8 tmpNum = 0;
    static bool tmpUpDown = true;
    static quint8 tmpState = STATE_Start;
    quint8 tmpFloor = tmpNum/4 + m_MinRange;
    if(tmpUpDown)
    {
        if(tmpFloor >= m_MaxRange)
        {
            tmpNum--;
            tmpUpDown = false;
            ReservedSetFloor(tmpFloor);
            SetArrowState(ArrowUpDownroll);
            SetFuncState(Function1,0xff);
            SetFuncState(Function2,0xff);
            SetFuncState(Function3,0xff);
            SendFloorPackage();
            return;
        }
        switch(tmpState)
        {
            case STATE_Start:
                ReservedSetFloor(tmpFloor);
                SetArrowState(ArrowUp);
                SetFuncState(Function1,1<<tmpFloor%7);
                SetFuncState(Function2,0);
                SetFuncState(Function3,0);
                tmpState = STATE_Run;
                break;
            case STATE_Run:
                SetArrowState(ArrowUproll);
                SetFuncState(Function1,0);
                SetFuncState(Function2,1<<tmpFloor%7);
                SetFuncState(Function3,0);
                tmpState = STATE_Stop;
                break;
            case STATE_Stop:
                SetArrowState(ArrowUp);
                SetStationArrive(true);
                SetFuncState(Function1,0);
                SetFuncState(Function2,0);
                SetFuncState(Function3,1<<tmpFloor%7);
                tmpState = STATE_Open;
                break;
            case STATE_Open:
                SetArrowState(ArrowUnknow);
                SetVoiceEnable(true);
                tmpState = STATE_Start;
                break;
        }
        tmpNum++;
    }
    else
    {
        if(tmpFloor <= m_MinRange)
        {
            tmpNum++;
            tmpUpDown = true;
            ReservedSetFloor(tmpFloor);
            SetArrowState(ArrowUpDownroll);
            SetFuncState(Function1,0xff);
            SetFuncState(Function2,0xff);
            SetFuncState(Function3,0xff);
            SendFloorPackage();
            return;
        }
        switch(tmpState)
        {
            case STATE_Start:
                ReservedSetFloor(tmpFloor);
                SetArrowState(ArrowDown);
                SetFuncState(Function1,1<<tmpFloor%7);
                SetFuncState(Function2,0);
                SetFuncState(Function3,0);
                tmpState = STATE_Run;
                break;
            case STATE_Run:
                SetArrowState(ArrowDownroll);
                SetFuncState(Function1,0);
                SetFuncState(Function2,1<<tmpFloor%7);
                SetFuncState(Function3,0);
                tmpState = STATE_Stop;
                break;
            case STATE_Stop:
                SetArrowState(ArrowDown);
                SetStationArrive(true);
                SetFuncState(Function1,0);
                SetFuncState(Function2,0);
                SetFuncState(Function3,1<<tmpFloor%7);
                tmpState = STATE_Open;
                break;
            case STATE_Open:
                SetArrowState(ArrowUnknow);
                SetVoiceEnable(true);
                tmpState = STATE_Start;
                break;
        }
        tmpNum--;
    }
    SendFloorPackage();
}

LogicElevator::LogicElevator(QObject *parent) :
    IBFrameParser(parent)
{
    Init();
}

LogicElevator::~LogicElevator()
{
    Init();
}

void LogicElevator::Init()
{
        m_OldSGOTime = new QTime();
        *m_OldSGOTime = QTime::currentTime();
        m_CurSGOTime = new QTime();
        SGOCount = 0;
        ArrowChanged = false;
        m_AudioTimer = new QTimer(0);

        connect(m_AudioTimer,SIGNAL(timeout()),this,SLOT(RepeatAudio()));
        for(int m=0;m<3;m++)
        {
            for(int n=0;n<2;n++)
            {
                m_BufferFrame[m][n].mCmd = Type_NONE;
                for(int i=0;i<7;i++)
                    m_BufferFrame[m][n].mData[i] = 0;
            }
        }

        m_FloorNum = INVALIDRC;
        m_ArrowState = ArrowUnknow;
        m_DoorState = DoorUnknow;
        m_Sound = INVALIDRC;
        m_DisablityMode = false;
        m_LowPowerEnable = false;
        m_DisablityMode = false;
        m_Fun = 0;

        for(int i=0;i<6;i++)
            m_ButtonState[i] = 0;
        for(int i=0;i<36;i++)
            m_ButtonCount[i] = D_ButtonAntiShakeNum;
}

void LogicElevator::RepeatAudio()
{
    QString tmpCmd = QString("/THEME/%1.WAV").arg(m_Fun + AUDIO_FUNCTION);
    IDE_TRACE_STR(tmpCmd);
    SystemAudio(tmpCmd,false,false,0);
}

void LogicElevator::NewFrameDetected()
{
    FRAME_TYPE tmpType = (FRAME_TYPE)(m_FrameBuffer.at(0));
    switch(tmpType)
    {
        case Type_FloorStatus:
        {
            m_BufferFrame[tmpType][Type_Rev].mCmd = tmpType;
            for(int n=1;n<8;n++)
            {
                m_BufferFrame[tmpType][Type_Rev].mData[n-1] = m_FrameBuffer.at(n);
            }
            if(m_CurFrameType == Flag_Normal)
            {
                ParseFloorFrame(m_BufferFrame[tmpType][Type_Rev].mData);
            }
            break;
        }
        case Type_ButtonStatus:
        {
            m_DisablityMode = false;
            m_BufferFrame[tmpType][Type_Rev].mCmd = tmpType;
            for(int n=1;n<8;n++)
            {
                m_BufferFrame[tmpType][Type_Rev].mData[n-1] = m_FrameBuffer.at(n);
            }
            if(m_CurFrameType == Flag_Normal)
            {
                ParseButtonFrame(m_BufferFrame[tmpType][Type_Rev].mData);
            }
            break;
        }
        case Type_DisabilityBtnStatus:
        {
            m_DisablityMode = true;
            m_BufferFrame[tmpType][Type_Rev].mCmd = tmpType;
            for(int n=1;n<8;n++)
            {
                m_BufferFrame[tmpType][Type_Rev].mData[n-1] = m_FrameBuffer.at(n);
            }
            if(m_CurFrameType == Flag_Normal)
            {
                ParseDisBtnFrame(m_BufferFrame[tmpType][Type_Rev].mData);
            }
            break;
        }
        default:
//            IDE_TRACE();
            return;
    }
    return;
}

void LogicElevator::ParseFloorFrame(quint8 *pData)
{
#ifdef  SODIMAS_SAF
    quint32 floorNum = 0;
    floorNum += ((quint32)pData[0]&0x0000007f)<<16;
    floorNum += ((quint32)pData[1]&0x0000007f)<<8;
    floorNum += (quint32)pData[2]&0x0000007f;

    quint8 signal = pData[3] & D_SignalArea ;

    quint32 functionValue = 0, functionID = INVALIDRC;
    functionValue += ((quint32)pData[4]&0x000000ff);
    functionValue += ((quint32)pData[5]&0x000000ff)<<8;
    functionValue += ((quint32)pData[6]&0x000000ff)<<16;

//    IDE_TRACE();
    ARROWSTATE arrowState = (ARROWSTATE)(pData[3] & D_ArrowArea) ;
    IDE_TRACE_INT(arrowState);

    for(quint32 i=0;i<24;i++)
    {
        if((functionValue>>i)&0x01)
        {
            if(i < 7)
                functionID = i;
            else if(i>7 && i < 15)
                functionID = i-1;
            else if(i>15 && i < 23)
                functionID = i-2;
            break;
        }
    }

    if(floorNum != m_FloorNum)
    {
        emit sFloorEvent(floorNum);
        m_FloorNum = floorNum;
    }

    volatile quint32 m_FunChanged = 0;
    if(functionID != m_Fun)
    {
        emit sFunctionEvent(functionID);
        m_Fun = functionID;

        m_FunChanged =1;
    }

    //IDE_TRACE_INT(arrowState);
    if(arrowState != m_ArrowState)
    {
        IDE_TRACE_INT(arrowState);
        ArrowChanged = true;
        emit sArrowEvent(arrowState);
        m_ArrowState = arrowState;
    }

    if(functionID == 5 || functionID == 10)
    {
        m_AudioTimer->start(200);
    }
    else
    {
        if(m_AudioTimer->isActive())
        {
            m_AudioTimer->stop();
            SystemAudio("",true,false,0);
        }
    }

    if(signal & D_SignalVoice && functionID != 14)
    {
        IDE_TRACE();
        quint32 audioNum = 0;
        audioNum += ((quint32)(pData[1]&0x0000007f))*10;
        audioNum += pData[2]&0x0000007f;
        QString tmpname = QString("/THEME/%1.WAV").arg(audioNum);
        IDE_TRACE_STR(tmpname);
        SystemAudio(tmpname,false,false,0);
    }

//    IDE_TRACE();
    PhysicalSodimasSAF *tmpSaf = static_cast<PhysicalSodimasSAF *>(m_PHDev);
    if(tmpSaf)
    {
        if(tmpSaf->m_NewSgoFlag) // xia jiang yan
        {
            //IDE_TRACE();
            m_OldSGO = 0;
        }
    }

    if((m_FunChanged || functionID == 14 ) && functionID !=15)
    {
        m_FunChanged = 0;
        QString tmpname;
        if(functionID == 14)
        {
            if((signal & D_SignalVoice) != 0)
            {
                tmpname = QString("/THEME/%1.WAV").arg(functionID + 200);
                if(isFileExist(tmpname))
                {
                    SystemAudio(tmpname,false,false,0);
                }
            }
        }
        else
        {
            if(functionID != 5 && functionID != 10)
            {
//                IDE_TRACE();
                m_AudioTimer->stop();
                tmpname = QString("/THEME/%1.WAV").arg(functionID + 200);
                if(isFileExist(tmpname))
                {
                    SystemAudio(tmpname,false,false,0);
                }
            }
        }
    }

    if(signal & D_SignalArrive) //>@??????+??????=?????
    {
        QString tmpname;
        if(m_OldSGO == 0)
        {
            m_OldSGO = 1;
            if(m_ArrowState == ArrowUp || m_ArrowState == ArrowUproll)
            {
                tmpname = QString("/THEME/DING1.WAV");
                SystemAudio(tmpname,false,false,0);
            }
            else if(m_ArrowState == ArrowDown || m_ArrowState == ArrowDownroll)
            {
                tmpname = QString("/THEME/DING2.WAV");
                SystemAudio(tmpname,false,false,0);
            }
            else if(m_ArrowState == ArrowUpDown)
            {
                tmpname = QString("/THEME/DING3.WAV");
                SystemAudio(tmpname,false,false,0);
            }
            if(GlobalPara.DoorOpenEnable)   //>@door open
            {
                IDE_TRACE();
                tmpname = QString("/THEME/%1.WAV").arg(15 + 200);
                IDE_TRACE_STR(tmpname);
                SystemAudio(tmpname,false,false,0);
            }
        }
    }
    else
    {
        m_OldSGO = 0;
    }

//>@
    bool energySaveEnable = (bool)(signal & D_SignalLowPower);
    if(energySaveEnable != m_LowPowerEnable)
    {
        if(energySaveEnable)
            emit sEnergySaveEvent();
        m_LowPowerEnable = energySaveEnable;
    }
#endif

#ifdef SODIMAS_CAN
    quint32 floorNum = 0;
    floorNum += ((quint32)pData[0]&0x0000007f)<<16;
    floorNum += ((quint32)pData[1]&0x0000007f)<<8;
    floorNum += (quint32)pData[2]&0x0000007f;
    quint8 signal = pData[3] & D_SignalArea ;

    quint32 functionValue = 0, functionID = INVALIDRC;
    functionValue = ((quint32)pData[4]&0x000000ff);
    quint32 sound =((quint32)pData[5]&0x000000ff);

    if(sound != m_Sound && floorNum != m_FloorNum)
    {
        //TUDO:Add no sound state
        QString tmpCmd = QString("%1%2%3.WAV").arg(D_FSRCPATH).arg(D_THEMEDIR).arg(sound);
        IDE_TRACE_STR(tmpCmd);
        SystemAudio(tmpCmd,false,false,0);
        m_Sound = sound;
    }

    ARROWSTATE arrowState = (ARROWSTATE)(pData[3] & D_ArrowArea) ;
    volatile quint32 m_FunChanged = 0;
    functionID = functionValue;

    if(floorNum != m_FloorNum)
    {
        IDE_TRACE_INT(floorNum);
        IDE_TRACE_INT(m_FloorNum);
        //GlobalPara.m_Logger->info(QString("Floor Changed : Previous : %1 Current : %2").arg(m_FloorNum).arg(floorNum));
        emit sFloorEvent(floorNum);
        m_FloorNum = floorNum;
    }

    if(functionID != m_Fun)
    {
        //GlobalPara.m_Logger->info(QString("Function Changed : Previous : %1 Current : %2").arg(m_Fun).arg(functionID));
        emit sFunctionEvent(functionID);
        m_Fun = functionID;
        m_FunChanged =1;
    }

    if(arrowState != m_ArrowState)
    {
        //GlobalPara.m_Logger->info(QString("Arrow Changed : Previous : %1 Current : %2").arg(m_ArrowState).arg(arrowState));
        ArrowChanged = true;
        emit sArrowEvent(arrowState);
        m_ArrowState = arrowState;
    }

    //>@
    bool energySaveEnable = (bool)(signal & D_SignalLowPower);
    if(energySaveEnable != m_LowPowerEnable)
    {
        if(energySaveEnable)
            emit sEnergySaveEvent();
        m_LowPowerEnable = energySaveEnable;
    }
#endif
}

void LogicElevator::ParseButtonFrame(quint8 *pData)
{
    //>@?a??????¡ã¡ä?£¤¦Ì?¨¢¨¢
    quint8 tmpButtonNum = -1;
    quint8 tmpButtonState[6];
    for(int n=0;n<6;n++)
    {
        tmpButtonState[n] = pData[n]&0x7f;
        for(int m=0;m<7;m++)
        {
            tmpButtonNum = (n+1)*(m+1);
            quint8 tmpNewBtnState = tmpButtonState[n]&(1<<m);
            quint8 tmpOldBtnState = m_ButtonState[n]&(1<<m);
            if(tmpOldBtnState != tmpNewBtnState)
            {
                if(m_ButtonCount[tmpButtonNum-1] < D_ButtonAntiShakeNum)
                {
//                    IDE_DEBUG(QString("Vein : Button %1 change from %2 to %3").arg(tmpButtonNum).arg(tmpOldBtnState).arg(tmpNewBtnState));
                }
                m_ButtonCount[tmpButtonNum-1] = 0;

                if(tmpOldBtnState != 0)
                {
                    emit sButtonStateChange(tmpButtonNum, STATE_LIGHT);
                    m_ButtonState[n] |= (1<<m);
                }
                else
                {
                    emit sButtonStateChange(tmpButtonNum, STATE_NORMAL);
                    m_ButtonState[n] &= ~(1<<m);
                }
            }
        }
    }
}

void LogicElevator::ParseDisBtnFrame(quint8 *pData)
{
    ParseButtonFrame(pData);
}

void LogicElevator::SetWorkMode(IBWORKMODE pMode)
{

}

void LogicElevator::SetTouchOutput(quint8 pValue)
{
    m_BufferFrame[Type_FloorStatus][Type_Send].mData[0] = pValue&0x7f;
}

void LogicElevator::SetSwitchOutput(quint8 pValue)
{
    m_BufferFrame[Type_FloorStatus][Type_Send].mData[1] = pValue&0x7f;
}

void LogicElevator::SetButtonState(quint8 pBtnNum, BUTTONSTATE pState)
{
    int m = 0, n = 0;
    m = pBtnNum / 7 - 1;
    n = pBtnNum % 7 - 1;
    quint8 tmpValue = m_BufferFrame[Type_ButtonStatus][Type_Send].mData[m];
    if(pState == STATE_PRESS)
    {
        m_BufferFrame[Type_ButtonStatus][Type_Send].mData[m] = tmpValue | (1<<n);
    }
    else if(pState == STATE_RELEASE)
    {
        m_BufferFrame[Type_ButtonStatus][Type_Send].mData[m] = tmpValue & ~(1<<n);
    }
}

quint32 LogicElevator::InitPara()
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
        if(!tmpName.compare("StationTalk"))
        {
            QStringList tmpValueList = tmpElement.attribute("List").split(';');
            tmpHBoxLayout = CreateEnum(tmpName, tmpValueList, tmpElement.text());
        }
        else if(!tmpName.compare("StationArrive"))
        {
            QStringList tmpValueList = tmpElement.attribute("List").split(';');
            tmpHBoxLayout = CreateEnum(tmpName, tmpValueList, tmpElement.text());
        }
        else if(!tmpName.compare("Device"))
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
        m_SettingGroup->m_VLayout->addWidget(CreateLine());
        m_SettingGroup->m_VLayout->addLayout(tmpHBoxLayout);
    }
    validcount++;
    return validcount;
}

void LogicElevator::UpdatePara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("StationTalk"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
//            QString tmpType;
//            if(!ModifyEnum(tmpComponent, tmpElement, tmpType, tmpComponent->currentText()))
//            {
//            }
//            else
//            {
//                mStationTalk = GetTristateType(tmpType);
//            }
        }
    }
    else if(!tmpName.compare("StationArrive"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
//            QString tmpType;
//            if(!ModifyEnum(tmpComponent, tmpElement, tmpType, tmpComponent->currentText()))
//            {
//            }
//            else
//            {
//                mStationArrive = GetTristateType(tmpType);
//            }
        }
    }
    else if(!tmpName.compare("Device"))
    {
        CheckList *tmpComponent = VAR_CAST<CheckList *>(pObject);
        if(tmpComponent)
        {
            QStringList tmpDevList = tmpComponent->GetSelectList();
            QStringList tmpList = PHDevicesAdded(tmpDevList, tmpComponent);
            if(!ModifyCheck(tmpComponent, tmpElement, tmpComponent->m_SelectList, tmpList))
            {
            }
        }
    }
}
