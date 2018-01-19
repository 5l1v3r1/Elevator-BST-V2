#ifndef LOGICNETMANAGER_H
#define LOGICNETMANAGER_H

#include "logicbase.h"
#include "zipfun.h"

#define D_PackageTime       3000

#define D_TYPEMESSAGE       QString("Message")
#define D_TYPECOMMAND       QString("Command")
#define D_TYPEINFORM        QString("Inform")

/*************************************************************
//>@ 分为缓冲指令、非缓冲指令
//>@---------------------------------------------------------
//>@--              非缓冲指令列表 : 与UDP的指令系统部分兼容
//>@--  1、Message--xxx
//>@--  2、Command;xxx
//>@--  3、Inform;xxx;1000  数字表示消息显示1000ms后消失，只在Display中有效
//>@--  4、SFConn/SFDisconn/Reboot
//>@--  5、Set/Get    <Physical><CPU Threshold="85"/></Physical>
//>@--                <Logic></Logic>
//>@--                <Theme><SubUI><FuncInfo><Floor>13</Floor></FuncInfo></SubUI></Theme>
//>@--                <EventMap></EventMap>
//>@--  6、Upload/Download
//>@---------------------------------------------------------
//>@
**************************************************************/

class NetFrameParser : public LogicDevice
{
    Q_OBJECT
public:
    NetFrameParser(QObject *parent = 0);
    ~NetFrameParser();

    void SyncDev(PhysicalDevice *pDevice);

    quint32 InitPara();

    QStringList AppendPHDevices(QStringList pPHDevs);
    void DeletePHDevices();

    void SetMaxPackageblock(qint64 pSize) { m_MaxBlockSize = pSize; }

    quint64 SendDataStream(QByteArray pData);  //>@底层发送函数

    bool SendFrame(NFDFRAME_STRUCT pFrame, FRAME_FLAG pFlag);  //>@发送函数
    void SendCMD(QString pInstruc,FRAME_FLAG pFlag);  //>@发送配置指令
    void SendCMD(QStringList pCmdSubList,FRAME_FLAG pFlag);

    void SendDebug(QByteArray pFrame,FRAME_FLAG pFlag);  //>@发送调试数据

    void SendFileData();
    void SendFileMD5();
    void GetFileData(QByteArray pData);
    void GetFileFooter(QByteArray pData);
    void GetFileMD5(QByteArray pData);

    void RestartPackageTimer(int pTime);
    void StopPackageTimer();

    void ParseFrameStream();
    void FrameVerify();
    void NewFiletransDetected();
    void NewSetDetected();  //>@检测到一个新的配置指令
    virtual void NewDebugDetected() {}  //>@检测到一个Debug指令
    void SendEvent(EventInfo pEventInfo);             //>@ 发送堆栈中缓存的指令
    virtual bool ProcessEventInfo(EventInfo &pEventInfo) { return false; }

    //>@非缓冲函数
    void Set(QString pContent, FRAME_FLAG pFlag=Flag_Normal)    { SendCMD(QStringList()<<"Set"<<pContent, pFlag); }
    void Get(QString pContent, FRAME_FLAG pFlag=Flag_Normal)    { SendCMD(QStringList()<<"Get"<<pContent, pFlag); }

    void SendMessage(QString pMessage, FRAME_FLAG pFlag=Flag_Normal) { SendCMD(QStringList()<<"Message"<<pMessage, pFlag); }
    void SendCommand(QString pMessage, FRAME_FLAG pFlag=Flag_Normal) { SendCMD(QStringList()<<"Command"<<pMessage, pFlag); }
    void SendInform(QString pMessage, FRAME_FLAG pFlag=Flag_Normal) { SendCMD(QStringList()<<"Inform"<<pMessage, pFlag); }

    void UpdateProgress(quint8 pVaule, bool pGlobalRatio=false);

signals:
    void sConnState(bool pState);
    void sProgress(int pVaule);

    void sZip(QString pSrc, QString pDir, bool);
    void sUnzip(QString pSrc, QString pDir, bool);

    void sSet(QString pContent);
    void sGet(QString pContent);
    void sMessage(QString pMessage);
    void sCommand(QString pMessage);
    void sInform(QString pMessage);

public slots:
    void UpdatePara(QObject* pObject);
    void ParseDataStream();
    void slot_PackageTimeout();
    virtual void slot_HBTimeout() = 0;

public:
    qint64                  m_MaxBlockSize;
    FRAME_FLAG              m_CurFrameType;
    FRAME_STATE             m_FrameState;
    QByteArray              m_ByteArray, m_FrameBuffer;
    EventInfo               m_CurSendEvent, m_CurRcvEvent;
    ReadFileOp             *m_ReadFileOp;
    WriteFileOp            *m_WriteFileOp;
    quint16                 m_PackNum;
    QTimer                  m_PackageDieTimer;
    quint8                  m_PackageDieCnt;

    quint8                  m_Preprogress;

    PhysicalDevice         *m_PHDev;
    bool                    m_SFConn;

    bool                    mDebugMode;

public:
    QTimer                  mHBTimer;
    TaskState               mZipTaskState;
};

class HostNetManager : public NetFrameParser
{
    Q_OBJECT
public:
    HostNetManager(QObject *parent = 0);
    ~HostNetManager();

    //>@UnQueueEvent
    void SFConn();
    bool SFConn(quint64 pLife);
    void SFDisconn();
    bool SFDisconn(quint64 pLife);
    void Stop();
    void SendHB(quint16 pTime);
    bool GetDevInfo(quint64 pLife);

    //>@QueueEvent
    void SendFileTest(QString pCover, QString pSrc, QString pDst,
                      quint64 pDietime, qreal pRation,
                      EVENT_TYPE pFatherType, bool pEnd);
    void StartSendFile(quint64 pDietime, qreal pRation,
                       EVENT_TYPE pFatherType, bool pEnd);
    void GetFileTest(QString pCover, QString pSrc, QString pDst,
                     quint64 pDietime, qreal pRation,
                     EVENT_TYPE pFatherType, bool pEnd);
    void StartGetFile(quint64 pDietime, qreal pRation,
                      EVENT_TYPE pFatherType, bool pEnd);
    void Ls(QString pPath,
            quint64 pDietime, qreal pRation,
            EVENT_TYPE pFatherType, bool pEnd);
    void GrabScreen(quint64 pDietime, qreal pRation,
                    EVENT_TYPE pFatherType, bool pEnd);
    void DelFile(QString pFilePath,
                 quint64 pDietime, qreal pRation,
                 EVENT_TYPE pFatherType, bool pEnd);
    void CreatFile(QString pFilePath,
                   quint64 pDietime, qreal pRation,
                   EVENT_TYPE pFatherType, bool pEnd);
    void MovePath(QString pSrc, QString pDst,
                  quint64 pDietime, qreal pRation,
                  EVENT_TYPE pFatherType, bool pEnd);
    void DelPath(QString pFilePath,
                 quint64 pDietime, qreal pRation,
                 EVENT_TYPE pFatherType, bool pEnd);
    void CreatPath(QString pFilePath,
                   quint64 pDietime, qreal pRation,
                   EVENT_TYPE pFatherType, bool pEnd);
    void Compress(QString pSrc, QString pDst,
                  quint64 pDietime, qreal pRation,
                  EVENT_TYPE pFatherType, bool pEnd);
    void Decompress(QString pSrc, QString pDst,
                    quint64 pDietime, qreal pRation,
                    EVENT_TYPE pFatherType, bool pEnd);

    //>@指令堆栈操作
    void ClearStack();
    void PushStack(EventInfo pEventInfo);
    void PopStatck(EventInfo pEventInfo);
    bool UnQueueEventAckHandle(EventInfo &pEventInfo);
    bool QueueEventAckHandle(EventInfo &pEventInfo);

    //>@指令超时操作
    void StartDieTime(quint64 pTime);
    void StopDieTime();
    void StartHBTime(quint64 pTime);
    void StopHBTime();

    //>@复合函数
    bool SendFile(QString pCover, QString pSrc, QString pDst, quint64 pTime);
    bool GetFile(QString pCover, QString pSrc, QString pDst, quint64 pTime);
    bool GetDesktop(QString pDevSavePath, QString pDst, quint64 pTime);
    bool DownLoadThemeZip(QString pSrc, QString pDir, quint64 pTime);
    bool UpLoadThemeZip(QString pSrc, QString pDir, quint64 pTime);

    bool ProcessEventInfo(EventInfo &pEventInfo);

    void NewDebugDetected();

signals:
    void sSendFileTest(bool pState);
    void sGetFileTest(bool pState);
    void sStartSendFile(bool pState);
    void sStartGetFile(bool pState);
    void sDebug(bool pState);
    void sLs(QString pStr);
    void sGrabScreen(bool pState);
    void sDelFile(bool pState);
    void sCreatFile(bool pState);
    void sDelPath(bool pState);
    void sCreatPath(bool pState);
    void sCompress(bool pState);
    void sDecompress(bool pState);

    void sSendFile(bool pState);
    void sGetFile(bool pState);
    void sGetDesktop(bool pState);
    void sDownLoadThemeZip(bool pState);
    void sUpLoadThemeZip(bool pState);

    //>@发送去掉头尾以及校验的帧数据
    void sParseFrame(QByteArray pFrame);

public slots:
    void slot_SendHB();
    void slot_Dietime();
    void slot_HBTimeout();

public:
    QHash<QString, QString> m_DevInfoList;

    QTimer                  m_DieTimer;
    quint16                 m_HBPara;
    QQueue<EventInfo>       m_EventQueue;

    TaskState               m_SendFileTaskState;
    TaskState               m_GetFileTaskState;
    TaskState               m_GetDesktopTaskState;
    TaskState               m_DownLoadTaskState;
    TaskState               m_UpLoadTaskState;
};


/****************************************************/
class GuestNetManager : public NetFrameParser
{
    Q_OBJECT
public:
    GuestNetManager(QObject *parent);
    ~GuestNetManager();

    void SendAck(QString pAck);
    void ReStartHBTimer(quint64 pTime);

    bool ProcessEventInfo(EventInfo &pEventInfo);

public slots:
    void slot_HBTimeout();

    void slot_ZipThread();
    void slot_UnzipThread();
    void slot_ZipTaskEnd();
    void slot_ZipTaskErr();
    void slot_ZipTaskProgress(int ratio);
};

#endif // NFDPARSER_H
