#include "physicalnfc.h"

NFCCardReaderThread::NFCCardReaderThread(PhysicalNFC *qesp, nfc_context* pNfcContext, nfc_connstring pNfcConnString)
    :QThread()
{
    mQesp = qesp;
    strcpy(mNFCConnString, pNfcConnString);
    mRun = true;
    mPnd = NULL;
    mNfcContext = pNfcContext;
    mNFCTargetInfo = 0;
    InitNFC();
}
NFCCardReaderThread::~NFCCardReaderThread()
{
    Stop();
    if(mPnd)
    {
        nfc_close(mPnd);
        mPnd = NULL;
    }
    if(mNFCTargetInfo)
        delete mNFCTargetInfo;
}
bool NFCCardReaderThread::InitNFC()
{
    if(mPnd)
        return true;
    mPnd = nfc_open(mNfcContext, mNFCConnString);
    if (mPnd == NULL)
    {
        IDE_DEBUG(QString("Unable to open NFC device:%1").arg(QString(mNFCConnString)));
        return false;
    }
    if (nfc_initiator_init(mPnd) < 0)
    {
        IDE_DEBUG(nfc_strerror(mPnd));
        nfc_perror(mPnd, "nfc_initiator_init");
        nfc_close(mPnd);
        mPnd = NULL;
        IDE_DEBUG("NFC Initiator failed!");
        return false;
    }
    IDE_DEBUG(QString(nfc_device_get_name(mPnd)));
    return true;
}
void NFCCardReaderThread::Stop()
{
    mRun = false;
}
bool NFCCardReaderThread::isEqual(nfc_connstring pNfcConnString)
{
    int ret = strcmp(mNFCConnString, pNfcConnString);
    if(ret == 0)
        return true;
    return false;
}

void NFCCardReaderThread::run()
{
    uint8_t uiPollNr = 1;
    uint8_t uiPeriod = 1;
    nfc_modulation nmModulations[5] ={
                                      {NMT_ISO14443A, NBR_106 },
                                      {NMT_ISO14443B, NBR_106 },
                                      {NMT_FELICA, NBR_212 },
                                      {NMT_FELICA, NBR_424 },
                                      {NMT_JEWEL, NBR_106 },
                                    };
    size_t szModulations = 5;
    mNFCTargetInfo = new nfc_target;
    while (mRun)
    {
        if(!InitNFC())
        {
            IDE_DEBUG("nfc_initiator init fail, thread exit!");
            break;
        }
        nfc_device_set_property_bool(mPnd, NP_INFINITE_SELECT, false);
        //printf("NFC device will poll during %ld ms (%u pollings of %lu ms for %" PRIdPTR " modulations)\n", (unsigned long) uiPollNr * szModulations * uiPeriod * 150, uiPollNr, (unsigned long) uiPeriod * 150, szModulations);
        if (( nfc_initiator_poll_target(mPnd, nmModulations, szModulations, uiPollNr, uiPeriod, mNFCTargetInfo))  == 1)
        {
            emit sFindCard(mNFCTargetInfo);
        }
        msleep(50);
    }
    mRun = true;
}

NFCCardTagThread::NFCCardTagThread(PhysicalNFC *qesp, nfc_context* pNfcContext, nfc_connstring pNfcConnString)
    :QThread()
{
    mQesp = qesp;
    strcpy(mNFCConnString, pNfcConnString);
    mRun = true;
    mPnd = NULL;
    mNfcContext = pNfcContext;
    mNFCTargetInfo = 0;
    InitNFC();
}
NFCCardTagThread::~NFCCardTagThread()
{
    Stop();
    if(mPnd)
    {
        nfc_close(mPnd);
        mPnd = NULL;
    }
    if(mNFCTargetInfo)
        delete mNFCTargetInfo;
}
bool NFCCardTagThread::InitNFC()
{
    if(mPnd)
        return true;
    mPnd = nfc_open(mNfcContext, mNFCConnString);
    if (mPnd == NULL)
    {
        IDE_DEBUG(QString("Unable to open NFC device:%1").arg(QString(mNFCConnString)));
        return false;
    }
    if (nfc_initiator_init(mPnd) < 0)
    {
        IDE_DEBUG(nfc_strerror(mPnd));
        nfc_perror(mPnd, "nfc_initiator_init");
        nfc_close(mPnd);
        mPnd = NULL;
        IDE_DEBUG("NFC Initiator failed!");
        return false;
    }
    IDE_DEBUG(QString(nfc_device_get_name(mPnd)));
    return true;
}
void NFCCardTagThread::Stop()
{
    mRun = false;
}
bool NFCCardTagThread::isEqual(nfc_connstring pNfcConnString)
{
    int ret = strcmp(mNFCConnString, pNfcConnString);
    if(ret == 0)
        return true;
    return false;
}

void NFCCardTagThread::run()
{
    uint8_t uiPollNr = 1;
    uint8_t uiPeriod = 1;
    nfc_modulation nmModulations[5] ={
                                      {NMT_ISO14443A, NBR_106 },
                                      {NMT_ISO14443B, NBR_106 },
                                      {NMT_FELICA, NBR_212 },
                                      {NMT_FELICA, NBR_424 },
                                      {NMT_JEWEL, NBR_106 },
                                    };
    size_t szModulations = 5;
    mNFCTargetInfo = new nfc_target;
    while (mRun)
    {
        if(!InitNFC())
        {
            IDE_DEBUG("nfc_initiator init fail, thread exit!");
            break;
        }
        nfc_device_set_property_bool(mPnd, NP_INFINITE_SELECT, false);
        //printf("NFC device will poll during %ld ms (%u pollings of %lu ms for %" PRIdPTR " modulations)\n", (unsigned long) uiPollNr * szModulations * uiPeriod * 150, uiPollNr, (unsigned long) uiPeriod * 150, szModulations);
        if (( nfc_initiator_poll_target(mPnd, nmModulations, szModulations, uiPollNr, uiPeriod, mNFCTargetInfo))  == 1)
        {
            emit sFindCard(mNFCTargetInfo);
        }
        msleep(50);
    }
    mRun = true;
}

NFCInitiatorThread::NFCInitiatorThread(PhysicalNFC *qesp, nfc_context* pNfcContext, nfc_connstring pNfcConnString)
    :QThread()
{
    mQesp = qesp;
    strcpy(mNFCConnString, pNfcConnString);
    mRun = true;
    mPnd = NULL;
    mCurCmd = NONE;
    mNfcContext = pNfcContext;
    InitNFC();
}
NFCInitiatorThread::~NFCInitiatorThread()
{
    Stop();
    if(mPnd)
    {
        nfc_close(mPnd);
        mPnd = NULL;
    }
}
bool NFCInitiatorThread::InitNFC()
{
    if(mPnd)
        return true;
    mPnd = nfc_open(mNfcContext, mNFCConnString);
    if (mPnd == NULL)
    {
        IDE_DEBUG(QString("Unable to open NFC device:%1").arg(QString(mNFCConnString)));
        return false;
    }
    if (nfc_initiator_init(mPnd) < 0)
    {
        IDE_DEBUG(nfc_strerror(mPnd));
        nfc_perror(mPnd, "nfc_initiator_init");
        nfc_close(mPnd);
        mPnd = NULL;
        IDE_DEBUG("NFC Initiator failed!");
        return false;
    }
    IDE_DEBUG(QString(nfc_device_get_name(mPnd)));
    mCurCmd = SCANCARD;
    return true;
}

void NFCInitiatorThread::Stop()
{
    mRun = false;
}

bool NFCInitiatorThread::isEqual(nfc_connstring pNfcConnString)
{
    int ret = strcmp(mNFCConnString, pNfcConnString);
    if(ret == 0)
        return true;
    return false;
}

void NFCInitiatorThread::ExchangeData(QString pData)
{
    if(mCurCmd == WAITTRANSIT)
    {
        char *tmpPointer = (char *)abtTx;
        strcpy(tmpPointer, pData.toLatin1().data());
        mCurCmd = TRANSIT;
    }
}

void NFCInitiatorThread::DisselectTarget()
{
    if(mCurCmd == WAITTRANSIT)
    {
        mCurCmd = DISSELECT;
    }
}

void NFCInitiatorThread::run()
{
    while (mRun)
    {
        if(!InitNFC())
        {
            IDE_DEBUG("nfc_initiator init fail, thread exit!");
            break;
        }
        switch(mCurCmd)
        {
            case SELECT:  //>@标签扫描均为两次握手，第一次发送“你想干嘛？”第二次发送“给你的还满意吗？”
            {
                nfc_device_set_property_bool(mPnd, NP_INFINITE_SELECT, false);
                int res = 0;
                if (res = nfc_initiator_select_dep_target(mPnd, NDM_PASSIVE, NBR_212, NULL, &mNFCTargetInfo, 10) == 1)
                {
                    char *tmpPointer = (char *)abtTx;
                    strcpy(tmpPointer, "SHBST NFC Query");
                }
                else
                {
                    break;
                }
            }
            case TRANSIT:
            {
                int res=0;
                if ((res = nfc_initiator_transceive_bytes(mPnd, abtTx, sizeof(abtTx), abtRx, sizeof(abtRx), 0)) < 0)
                {
                    nfc_perror(mPnd, "nfc_initiator_transceive_bytes");
                    nfc_close(mPnd);
                    mPnd = NULL;
                    IDE_DEBUG("nfc_initiator_transceive_bytes fail!");
                }
                else
                {
                    abtRx[res] = 0;
                    mCurCmd = WAITTRANSIT;
                    emit sInitiator();
                }
                break;
            }
            case DISSELECT:
            {
                if (nfc_initiator_deselect_target(mPnd) < 0)
                {
                    nfc_perror(mPnd, "nfc_initiator_deselect_target");
                    nfc_close(mPnd);
                    mPnd = NULL;
                    IDE_DEBUG("nfc_initiator_deselect_target fail!");
                }
                mCurCmd = SCANCARD;
                break;
            }
            default:
                break;
        }
        msleep(50);
    }
    mRun = true;
}

NFCTargetThread::NFCTargetThread(PhysicalNFC *qesp, nfc_context* pNfcContext, nfc_connstring pNfcConnString)
    :QThread()
{
    mQesp = qesp;
    strcpy(mNFCConnString, pNfcConnString);
    mRun = true;
    mPnd = NULL;
    mNfcContext = pNfcContext;
    InitNFC();
}
NFCTargetThread::~NFCTargetThread()
{
    Stop();
    if(mPnd)
    {
        nfc_close(mPnd);
        mPnd = NULL;
    }
}
bool NFCTargetThread::InitNFC()
{
    if(mPnd)
        return true;
    mPnd = nfc_open(mNfcContext, mNFCConnString);
    if (mPnd == NULL)
    {
        IDE_DEBUG(QString("Unable to open NFC device:%1").arg(QString(mNFCConnString)));
        return false;
    }
    InitTargetPara();
    IDE_DEBUG(QString(nfc_device_get_name(mPnd)));
    mCurCmd = INIT;
    return true;
}
void NFCTargetThread::Stop()
{
    mRun = false;
}
bool NFCTargetThread::isEqual(nfc_connstring pNfcConnString)
{
    int ret = strcmp(mNFCConnString, pNfcConnString);
    if(ret == 0)
        return true;
    return false;
}

void NFCTargetThread::InitTargetPara()
{
    nt.nti.ndi.abtNFCID3[0] = 0x12;
    nt.nti.ndi.abtNFCID3[1] = 0x34;
    nt.nti.ndi.abtNFCID3[2] = 0x56;
    nt.nti.ndi.abtNFCID3[3] = 0x78;
    nt.nti.ndi.abtNFCID3[4] = 0x9a;
    nt.nti.ndi.abtNFCID3[5] = 0xbc;
    nt.nti.ndi.abtNFCID3[6] = 0xde;
    nt.nti.ndi.abtNFCID3[7] = 0xff;
    nt.nti.ndi.abtNFCID3[8] = 0x00;
    nt.nti.ndi.abtNFCID3[9] = 0x00;
    nt.nti.ndi.szGB = 4;
    nt.nti.ndi.abtGB[0] = 0x12;
    nt.nti.ndi.abtGB[1] = 0x34;
    nt.nti.ndi.abtGB[2] = 0x56;
    nt.nti.ndi.abtGB[3] = 0x78;
    nt.nti.ndi.ndm = NDM_UNDEFINED;
    /* These bytes are not used by nfc_target_init:
        the chip will provide them automatically to the initiator */
    nt.nti.ndi.btDID = 0x00;
    nt.nti.ndi.btBS = 0x00;
    nt.nti.ndi.btBR = 0x00;
    nt.nti.ndi.btTO = 0x00;
    nt.nti.ndi.btPP = 0x01;
    nt.nm.nmt = NMT_DEP;
    nt.nm.nbr = NBR_UNDEFINED;
}

void NFCTargetThread::SendNFCData(QString pData)
{
    if(mCurCmd == WAITTRANSIT)
    {
        char *tmpPointer = (char *)abtTx;
        strcpy(tmpPointer, pData.toLatin1().data());
        mCurCmd = TRANSIT;
    }
}

void NFCTargetThread::run()
{
    while (mRun)
    {
        if(!InitNFC())
        {
            IDE_TRACE();
            break;
        }
        switch(mCurCmd)
        {
            case INIT:
            {
                nfc_device_set_property_bool(mPnd, NP_INFINITE_SELECT, false);
                if (nfc_target_init(mPnd, &nt, abtRx, sizeof(abtRx), 0) < 0)
                {
                    nfc_perror(mPnd, "nfc_target_init");
                    IDE_DEBUG(nfc_strerror(mPnd));
                    nfc_close(mPnd);
                    mPnd = NULL;
                    IDE_DEBUG("NFC target failed!");
                }
                else
                {
                    mCurCmd = RECV;
                }
                break;
            }
            case RECV:
            {
                if ((szRx = nfc_target_receive_bytes(mPnd, abtRx, sizeof(abtRx), 0)) < 0)
                {
                    nfc_perror(mPnd, "nfc_target_receive_bytes");
                    nfc_close(mPnd);
                    mPnd = NULL;
                    IDE_DEBUG("nfc_target_receive_bytes failed!");
                }
                else
                {
                    abtRx[(size_t) szRx] = 0;
                    mCurCmd = WAITTRANSIT;
                    emit sTarget();
                }
                break;
            }
            case TRANSIT:
            {
                if (nfc_target_send_bytes(mPnd, abtTx, sizeof(abtTx), 0) < 0)
                {
                  nfc_perror(mPnd, "nfc_target_send_bytes");
                  nfc_close(mPnd);
                  mPnd = NULL;
                  IDE_DEBUG("nfc_target_send_bytes failed!");
                }
                else
                {
                    mCurCmd = RECV;
                }
                break;
            }
            default:
                break;
        }
        msleep(10);
    }
    mRun = true;
}

PhysicalNFC::PhysicalNFC(QObject *parent) :
    PhysicalMovable(parent)
{
    m_SignalMapper = 0;
    SYSTEM_CMD::SetNFC(true);
}

PhysicalNFC::~PhysicalNFC()
{
    ReleaseNFCsPoll();
    if(mContext != NULL)
    {
        nfc_exit(mContext);
    }
    SYSTEM_CMD::SetNFC(false);
}

QStringList PhysicalNFC::InitNFCDevs(QStringList pDevTypes)
{
    ReleaseNFCsPoll();
    nfc_init(&mContext);
    if (mContext == NULL)
    {
        IDE_TRACE_STR("NFCInit#Err");
        return QStringList();
    }
    IDE_TRACE_STR(QString(nfc_version()));
    nfc_connstring connstrings[MAX_DEVICE_COUNT];
    size_t localDevCnt = nfc_list_devices(mContext, connstrings, MAX_DEVICE_COUNT);
    IDE_TRACE_INT(localDevCnt);
    QStringList validDevs;
    for(int i=0;i<localDevCnt;i++)
    {
        if(i >= pDevTypes.count())
            break;
        QString tmpString = pDevTypes.at(i);
        AddNFCPoll(mContext, GetType(tmpString), connstrings[i]);
        validDevs.append(tmpString);
    }
    return validDevs;
}

bool PhysicalNFC::InitSubPHPara(QString pName, QDomElement pElement, QHBoxLayout*& pHBoxLayout)
{
    if(!pName.compare("Mode"))
    {
        QStringList tmpTypeList = pElement.text().split(';');
        QStringList tmpValidDevs = InitNFCDevs(tmpTypeList);
        pHBoxLayout = CreateCheck(pName, pElement.attribute("List").split(';'), tmpValidDevs);
    }
    else
    {
        return false;
    }
    return true;
}

void PhysicalNFC::UpdateSubPHPara(QString pName, QObject *pObject, QDomElement pElement)
{
    if(!pName.compare("Mode"))
    {
        CheckList *tmpComponent = VAR_CAST<CheckList *>(pObject);
        if(tmpComponent)
        {
            QStringList tmpValidDevs = InitNFCDevs(tmpComponent->GetSelectList());
            if(!ModifyCheck(tmpComponent, pElement, m_ValidTypeList, tmpValidDevs))
            {
            }
        }
    }
}

QThread* PhysicalNFC::AddNFCPoll(nfc_context* pNfcContext, NFC_TYPE pType, nfc_connstring pNfcConnString)
{
    QThread *tmpThread = 0;
    switch(pType)
    {
        case Initiator:
            tmpThread = new NFCInitiatorThread(this, pNfcContext, pNfcConnString);
            break;
        case Target:
            tmpThread = new NFCTargetThread(this, pNfcContext, pNfcConnString);
            break;
        case CardTag:
            tmpThread = new NFCCardTagThread(this, pNfcContext, pNfcConnString);
            break;
        case CardReader:
            tmpThread = new NFCCardReaderThread(this, pNfcContext, pNfcConnString);
            break;
        default:
            break;
    }
    return tmpThread;
}

void PhysicalNFC::ReleaseNFCsPoll()
{
    QList<NFC_TYPE> tmpList = m_NFCDevs.keys();
    for(int i=tmpList.count()-1;i>=0;i--)
    {
        QThread* tmpThread = VAR_CAST<QThread*>(m_NFCDevs.take(tmpList.at(i)));
        if(tmpThread != 0)
        {
            if (tmpThread->isRunning())
            {
                tmpThread->quit();
                if (QThread::currentThread() != tmpThread)
                    tmpThread->wait();
            }
            tmpThread->deleteLater();
        }
    }
    m_NFCDevs.clear();
}
