#ifndef LOGICELEVATOR_H
#define LOGICELEVATOR_H

#include "logicbase.h"
#include "baseaudio.h"
/*************************************************************
//>@C_IB
**************************************************************/
class  IBFrameParser : public LogicDevice
{
    Q_OBJECT
public:
    IBFrameParser(QObject *parent);
    ~IBFrameParser();
    void Init();
    void Release();
    virtual void NewFrameDetected() {}
    void FrameVerify();
    bool SendFrame(IBFRAME_STRUCT pFrame, FRAME_FLAG pFlag);
public slots:
    quint64 SendDataStream(QByteArray pData);
    void ParseDataStream();
public:
    FRAME_STATE             m_FrameState;
    FRAME_FLAG              m_CurFrameType;
    QByteArray              m_FrameBuffer;

    PhysicalDevice         *m_PHDev;
};

/****************************************************/
class ElevatorSimulator : public IBFrameParser
{
    Q_OBJECT
public:
    ElevatorSimulator(QObject *parent);
    ~ElevatorSimulator();
    void NewFrameDetected();
    void ParseFloorFrame(quint8 *pData);
    void ParseButtonFrame(quint8 *pData);
    void ParseDisBtnFrame(quint8 *pData);

    void Destory();
    void SetAutoRunRange(quint8 pMin, quint8 pMax);
    void SetAutoRunTime(quint16 pTime);
    void StopAutoRun();

    void SendFloorPackage();
    void SendBtnPackage();
    void SendDisabilityPackage();
    void SendAppUpgradePackage();
    void SendDebugPackage();
    void SendConfigurePackage();

signals:
    void WorkModeChange(IBWORKMODE pMode);
    void TouchOutputChange(quint8 pValue);
    void SwitchOutputChange(quint8 pValue);
    void ButtonStateChange(quint8 pBtnNum, BUTTONSTATE pState);
public slots:
    void SetArrowState(ARROWSTATE pArrowState);
    void SetFloor(quint32 pFloorNum);
    void ReservedSetFloor(quint32 pFloorNum);
    void SetDoorState(DOOR_STATE pDoorState);
    void SetStationArrive(bool pEnable);
    void SetVoiceEnable(bool pEnable);
    void SetLowPower(bool pEnable);
    void SetFuncState(FUNCTION_STATE pFuncState, quint8 pFunData);
    void SetButtonState(quint8 pBtnNum, BUTTONSTATE pState);
    void SetDisablityMode(bool pDisablity);

    void AutoRunTimer();

public:
    IBWORKMODE              m_WorkMode;
    IBFRAME_STRUCT          m_BufferFrame[3][2];
    quint8                  m_ButtonState[6];
    bool                    m_DisablityMode;
    quint8                  m_TouchOutput;
    quint8                  m_SwitchOutput;

    QTimer                  m_AutoRunTimer;
    quint16                 m_AutoRunTime;
    quint8                  m_MinRange;
    quint8                  m_MaxRange;
};

/****************************************************/
class LogicElevator : public IBFrameParser
{
    Q_OBJECT
public:
    typedef enum{Forbid, Enable, Negation} TristateType;
    LogicElevator(QObject *parent);
    ~LogicElevator();

    TristateType GetTristateType(QString pMode)
    {
        if(pMode.compare("Forbid", Qt::CaseInsensitive) == 0)
            return Forbid;
        else if(pMode.compare("Enable", Qt::CaseInsensitive) == 0)
            return Enable;
        else if(pMode.compare("Negation", Qt::CaseInsensitive) == 0)
            return Negation;
    }
    QString GetTristateTypeString(TristateType pMode)
    {
        if(pMode == Forbid)
            return "Forbid";
        else if(pMode == Enable)
            return "Enable";
        else if(pMode == Negation)
            return "Negation";
    }

public:
    void Init();
    void NewFrameDetected();
    void ParseFloorFrame(quint8 *pData);
    void ParseButtonFrame(quint8 *pData);
    void ParseDisBtnFrame(quint8 *pData);

    quint32 InitPara();
    bool SetPara(QDomElement &pElement, xmlParse *pParse);
    bool GetPara(QDomElement &pElement, xmlParse *pParse);

public slots:
    void UpdatePara(QObject* pObject);
    void SetWorkMode(IBWORKMODE pMode);
    void SetTouchOutput(quint8 pValue);
    void SetSwitchOutput(quint8 pValue);
    void SetButtonState(quint8 pBtnNum, BUTTONSTATE pState);
    void RepeatAudio();

signals:
    void sArrowEvent(ARROWSTATE pArrowState);
    void sFloorEvent(quint32 pFloorNum);
    void sFunctionEvent(quint32 pFundex);
    void sAudioEvent(quint32 pAudioNum);

    void sEnergySaveEvent();

    void sDoorStateChange(DOOR_STATE pDoorState);
    void sButtonStateChange(quint8 pBtnNum, BUTTONSTATE pState);
    void sDisablityModeChange(bool pDisablity);

public:
    IBFRAME_STRUCT       m_BufferFrame[3][2];
    ARROWSTATE               m_ArrowState;
    DOOR_STATE                m_DoorState;
    bool                                m_LowPowerEnable;
    bool                                m_DisablityMode;
    quint32                          m_FloorNum;
    quint32                          m_Sound;
    quint32                          m_Fun;
    quint32                          m_ButtonCount[36];
    quint8                            m_ButtonState[6];
    QTimer                          *m_AudioTimer;
    bool                                ArrowChanged;
    int                                   SGOCount;
    QTime                            *m_OldSGOTime;
    QTime                            *m_CurSGOTime;
    int                                   m_OldSGO;
};

#endif // IBPARSER_H
