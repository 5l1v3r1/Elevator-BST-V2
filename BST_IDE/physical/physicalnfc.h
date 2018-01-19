#ifndef PHYSICALNFC_H
#define PHYSICALNFC_H

#include "physicalmovable.h"

#ifdef WINDOWS
#include "nfc/Windows/nfc.h"
#endif
#ifdef UBUNTU
#include "nfc/Ubuntu/nfc.h"
#endif
#ifdef LINUX
#include "nfc/Linux/nfc.h"
#endif

#define MAX_DEVICE_COUNT 16
#define MAX_TARGET_COUNT 16
#define MAX_FRAME_LEN 264

class PhysicalNFC;

class NFCCardReaderThread : public QThread
{
    Q_OBJECT
public:
    NFCCardReaderThread(PhysicalNFC *qesp, nfc_context* pNfcContext, nfc_connstring pNfcConnString);
    ~NFCCardReaderThread();
    bool InitNFC();
    void Stop();
    bool isEqual(nfc_connstring pNfcConnString);

signals:
    void sFindCard(nfc_target *pTarget);

protected:
    virtual void run();

public:
    PhysicalNFC      *mQesp;
    nfc_device       *mPnd;
    nfc_context      *mNfcContext;
    nfc_target       *mNFCTargetInfo;
    nfc_connstring    mNFCConnString;
    bool              mRun;
};

class NFCCardTagThread : public QThread
{
    Q_OBJECT
public:
    NFCCardTagThread(PhysicalNFC *qesp, nfc_context* pNfcContext, nfc_connstring pNfcConnString);
    ~NFCCardTagThread();
    bool InitNFC();
    void Stop();
    bool isEqual(nfc_connstring pNfcConnString);

signals:
    void sFindCard(nfc_target *pTarget);

protected:
    virtual void run();

public:
    PhysicalNFC      *mQesp;
    nfc_device       *mPnd;
    nfc_context      *mNfcContext;
    nfc_target       *mNFCTargetInfo;
    nfc_connstring    mNFCConnString;
    bool              mRun;
};

class NFCInitiatorThread : public QThread
{
    Q_OBJECT
public:
    typedef enum CMD{NONE=0, SELECT, WAITTRANSIT, TRANSIT, DISSELECT, SCANCARD} CMD;
public:
    NFCInitiatorThread(PhysicalNFC *qesp, nfc_context* pNfcContext, nfc_connstring pNfcConnString);
    ~NFCInitiatorThread();
    bool InitNFC();
    void Stop();
    bool isEqual(nfc_connstring pNfcConnString);
    void ExchangeData(QString pData);
    void DisselectTarget();
signals:
    void sInitiator();
protected:
    virtual void run();
public:
    PhysicalNFC      *mQesp;
    nfc_device       *mPnd;
    nfc_context      *mNfcContext;
    nfc_target        mNFCTargetInfo;
    nfc_connstring    mNFCConnString;
    bool              mRun;

    uint8_t           abtRx[MAX_FRAME_LEN];
    int               szRx;
    uint8_t           abtTx[MAX_FRAME_LEN];
    CMD               mCurCmd;
};

class NFCTargetThread : public QThread
{
    Q_OBJECT
public:
    typedef enum{ NONE=0, INIT, RECV, WAITTRANSIT, TRANSIT } CMD;
    NFCTargetThread(PhysicalNFC *qesp, nfc_context* pNfcContext, nfc_connstring pNfcConnString);
    ~NFCTargetThread();
    bool InitNFC();
    void InitTargetPara();
    void Stop();
    bool isEqual(nfc_connstring pNfcConnString);
    void ProcessCMD();
    void SendNFCData(QString pData);
signals:
    void sTarget();
protected:
    virtual void run();
public:
    PhysicalNFC      *mQesp;
    nfc_device       *mPnd;
    nfc_context      *mNfcContext;
    nfc_target        mNFCTargetInfo;
    nfc_connstring    mNFCConnString;
    bool              mRun;

    CMD               mCurCmd;
    nfc_target        nt;
    uint8_t           abtRx[MAX_FRAME_LEN];
    int               szRx;
    uint8_t           abtTx[MAX_FRAME_LEN];
};

//>@可选多种模式，但每种模式对应一个NFC设备，一个设备仅能应用于一种模式
class PhysicalNFC : public PhysicalMovable
{
    Q_OBJECT
public:
    typedef enum{Initiator=0, Target, CardTag, CardReader} NFC_TYPE;
    PhysicalNFC(QObject *parent = 0);
    ~PhysicalNFC();

    QStringList InitNFCDevs(QStringList pDevTypes);
    QThread* AddNFCPoll(nfc_context* pNfcContext, NFC_TYPE pType, nfc_connstring pNfcConnString);
    void ReleaseNFCPoll(nfc_connstring pNfcConnString);
    void ReleaseNFCsPoll();

    bool InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout);
    void UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement);

    QString GetInformation()
    {
        QString tmpInfo = QString("%1:").arg(mDevName);
        QList<NFC_TYPE> tmpList = m_NFCDevs.keys();
        for(int i=0;i<tmpList.count();i++)
        {
            tmpInfo.append(GetTypeString(tmpList.at(i)));
            tmpInfo.append(" ");
        }
        return tmpInfo;
    }
    NFC_TYPE GetType(QString pType)
    {
        if(pType.compare("Initiator", Qt::CaseInsensitive) == 0)
            return Initiator;
        else if(pType.compare("Target", Qt::CaseInsensitive) == 0)
            return Target;
        else if(pType.compare("CardTag", Qt::CaseInsensitive) == 0)
            return CardTag;
        else if(pType.compare("CardReader", Qt::CaseInsensitive) == 0)
            return CardReader;
    }
    QString GetTypeString(NFC_TYPE pType)
    {
        if(pType == Initiator)
            return "Initiator";
        else if(pType == Target)
            return "Target";
        else if(pType == CardTag)
            return "CardTag";
        else if(pType == CardReader)
            return "CardReader";
    }

public:
    QHash<NFC_TYPE, QThread*>   m_NFCDevs;
    QStringList                 m_ValidTypeList;

    nfc_context*                mContext;
    QSignalMapper              *m_SignalMapper;
};

#endif // PHYSICALNFC_H
