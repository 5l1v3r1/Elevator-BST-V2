#include "usbport.h"

UsbDFU::UsbDFU(GUID pGuid, QObject *parent) :
    QObject(parent)
{
    m_DownloadThread = 0;
    m_FileTxBuffer = 0;
    hRead = 0;
    hWrite = 0;

    m_DownloadErrorFlag = false;
    m_SendEnable = true;
    m_CurSendDataNum = 0;
    m_FileTxBufferSize = 0;

#ifdef WINTHREAD
    m_WinThreadFinished = true;
#endif

    m_UsbPort = 0;
    m_UsbPort = new UsbBase(pGuid, "PIPE00", "PIPE01", this);
    m_UsbPort->open(QIODevice::ReadWrite);
    connect(m_UsbPort, SIGNAL(sConnState(bool)), this, SIGNAL(sConnState(bool)), Qt::DirectConnection);

    InitDownloadThread();
}

UsbDFU::~UsbDFU()
{
    ReleaseDownloadThread();
    if(m_UsbPort)
    {
        disconnect(m_UsbPort, SIGNAL(sConnState(bool)), 0,0);
        m_UsbPort->close();
        m_UsbPort->deleteLater();
    }
}

bool UsbDFU::SendCMD(EXEC_CMD pCMD)
{
    char txBuf[10]="";
    *((unsigned char *)txBuf+0)=D_MODECMD;
    *((unsigned char *)txBuf+1)=pCMD;
    if(m_UsbPort)
    {
        m_UsbPort->writeData(txBuf, 8);
        QtSleep(100);
        if(m_UsbPort->readFeedback("Status_is_Ok!", 100))
            return true;
    }
    return false;
}

bool UsbDFU::SendData(QString pData)
{
    int tmpDataSize = pData.count()+1;
    char *txBuf = (char *)malloc(tmpDataSize+10);
    *((unsigned char *)txBuf+0)=D_MODEDATA;
    strcpy(txBuf+8, pData.toLatin1().data());
    *((DWORD *)txBuf+1) = tmpDataSize+10;   //文件总共大小 filesize(4+4+n+2)
    quint32 cs = 0;
    for(quint32 i=8;i<(tmpDataSize+8);i++)//和校验
       cs+=(BYTE)(txBuf[i]);
    *((DWORD *)(txBuf+8+tmpDataSize))=cs;   //添加和校验的值
    if(m_UsbPort)
    {
        m_UsbPort->writeData(txBuf, tmpDataSize+10);
        QtSleep(100);
        if(m_UsbPort->readFeedback("Status_is_Ok!", 100))
            return true;
    }
    return false;
}

bool UsbDFU::isDownloaded()
{
#ifdef WINTHREAD
    return m_WinThreadFinished;
#else
    if (m_DownloadThread && m_DownloadThread->isRunning())
    {
        return false;
    }
    return true;
#endif
}

bool UsbDFU::DownloadFile(QString pFilePath, EXEC_CMD pCMD)
{
    //>@查看是否有文件正在传输
#ifdef WINTHREAD
    if(!m_WinThreadFinished)
#else
    if (m_DownloadThread && m_DownloadThread->isRunning())
#endif
    {
        m_DownloadErrorFlag = true;
        m_DownloadLastError = QString("Has file transferring, please wait!");
        return false;
    }
    //>@检查文件
    if(pFilePath.isEmpty() || !isFileExist(pFilePath))
    {
        m_DownloadErrorFlag = true;
        m_DownloadLastError = QString("The File is broken, please check it!");
        return false;
    }
    //获得打开的文件名
    char tmpPath[40];
    QString tmpFileName = getFileName(pFilePath);
    QByteArray ba = tmpFileName.toLatin1();
    strcpy(tmpPath, ba.data());
    LPCWSTR tempLPCWSTR = (WCHAR*)(pFilePath.utf16());
    HANDLE hFile=CreateFile(tempLPCWSTR,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if(hFile==INVALID_HANDLE_VALUE)
    {
        m_DownloadErrorFlag = true;
        m_DownloadLastError = QString("Can't open file, please check it!");
        return false;
    }
    //获得文件的大小
    DWORD fileSize=GetFileSize(hFile,NULL);
    if(m_FileTxBuffer)
    {
        free(m_FileTxBuffer);
        m_FileTxBuffer = 0;
    }
    m_FileTxBuffer=(char *)malloc(fileSize+10);
    if(m_FileTxBuffer==0)
    {
        CloseHandle(hFile);
        m_DownloadErrorFlag = true;
        m_DownloadLastError = QString("Memory Allocation Fail!");
        return false;
    }
    //读取文件，查看文件大小是否正确
    DWORD txBufSize = 0;
    ReadFile(hFile,(void *)(m_FileTxBuffer+8),fileSize,&txBufSize,NULL);
    if(txBufSize!=fileSize)
    {
        free(m_FileTxBuffer);
        m_FileTxBuffer = 0;
        CloseHandle(hFile);
        m_DownloadErrorFlag = true;
        m_DownloadLastError = QString("File Size Error!");
        return false;
    }
    //>@计算头帧
    *((unsigned char *)m_FileTxBuffer+0)=D_MODEDATA;
    *((DWORD *)m_FileTxBuffer+1)=fileSize+10;   //文件总共大小 filesize(4+4+n+2)
    quint32 cs = 0;
    for(quint32 i=8;i<(fileSize+8);i++)//和校验
       cs+=(BYTE)(m_FileTxBuffer[i]);
    *((WORD *)(m_FileTxBuffer+8+fileSize))=cs;   //添加和校验的值
    txBufSize+=10;
    m_FileTxBufferSize = txBufSize;
    CloseHandle(hFile);
    //>@开始发送文件
    m_SendEnable = true;
    m_CurExecCMD = pCMD;
#ifdef WINTHREAD
    unsigned long threadResult = _beginthread( (void (*)(void *))(ProcessData), 0x2000,(void *)0);
    if(threadResult!=-1)
        return false;
#else
    //StartDownloadThread();
    ProcessData();
#endif
    return !m_DownloadErrorFlag;
}

void UsbDFU::ProcessData(void *args)
{
#ifdef WINTHREAD
    m_WinThreadFinished = false;
#endif
    m_DownloadErrorFlag = false;
    if(m_UsbPort)
    {
        //>@打开读写管道
        quint32 err_count = 200;
        do{
            hWrite = m_UsbPort->OpenDevFile(m_UsbPort->m_OutPipe);   //打开写管道
            hRead = m_UsbPort->OpenDevFile(m_UsbPort->m_InPipe);     //打开读管道
            if((hWrite!=INVALID_HANDLE_VALUE)&&(hRead!=INVALID_HANDLE_VALUE))
                break; //>@如果打开成功，则退出检测循环开始发送文件
            CloseHandle(hRead);
            CloseHandle(hWrite);
            if(!(err_count--))
            {
                free(m_FileTxBuffer);
                m_FileTxBuffer = 0;
                m_DownloadErrorFlag = true;
                m_DownloadLastError = QString("Can't open USB device!");
#ifdef WINTHREAD
                m_WinThreadFinished = true;
                _endthread();
#endif
                return;
            }
            QtSleep(500);
        }while(1);
        //>@开始传输文件
transmission:
        void *txBlk = 0;
        DWORD tmpCurSendDataNum = 0;
        DWORD txBlkSize = 0;
        DWORD nBytesWrite = 0;
        while(m_SendEnable)
        {
            if((m_FileTxBufferSize - tmpCurSendDataNum) > TX_SIZE)
                txBlkSize = TX_SIZE;
            else
                txBlkSize = m_FileTxBufferSize - tmpCurSendDataNum;
            txBlk=(void *)(m_FileTxBuffer + tmpCurSendDataNum);
            if(WriteFile(hWrite,txBlk,txBlkSize,&nBytesWrite,NULL) == false)
            {
                m_DownloadLastError = QString("Write [%1] Location [%2]--  Sending %3 ; Sended %4")
                                                                      .arg((int)hWrite)
                                                                      .arg(tmpCurSendDataNum)
                                                                      .arg(txBlkSize)
                                                                      .arg(nBytesWrite);
                m_DownloadErrorFlag = true;
                break;
            }
            tmpCurSendDataNum += nBytesWrite;
            if(tmpCurSendDataNum >= m_FileTxBufferSize)
            {
                break;
            }
            emit sProgress(QString(), (qreal)tmpCurSendDataNum * 100.0 / (qreal)m_FileTxBufferSize);
        }
        if(!m_DownloadErrorFlag)  //>@如果文件传输正确，则进入校验和烧录阶段
        {
            //>@判断文件校验是否正确
            err_count = 100;
            int loopcount = 0;
            while(1)
            {
                char rxBuf[20];
                DWORD nBytesWrite = 0;
                memset(rxBuf, 0, sizeof(rxBuf));
                ReadFile(hRead,rxBuf,13,&nBytesWrite,NULL);
                if(strcmp("Status_is_Ok!",rxBuf) == 0)
                {
                    m_DownloadErrorFlag = false;
                    break;
                }
                else if(strcmp("Status_is_err",rxBuf) == 0)
                {
                    if(err_count--)
                    {
                        m_DownloadLastError = QString("Data transfer error, Retransfer data!");
                        m_DownloadErrorFlag = true;
                        goto transmission;
                    }
                    else
                    {
                        m_DownloadLastError = QString("Data checksum error!");
                        m_DownloadErrorFlag = true;
                        goto Checksum_failed;
                    }
                }
                if(loopcount++ > 9999)
                {
                    CloseHandle(hWrite);
                    CloseHandle(hRead);
                    hWrite = m_UsbPort->OpenDevFile(m_UsbPort->m_OutPipe);   //打开写管道
                    hRead = m_UsbPort->OpenDevFile(m_UsbPort->m_InPipe);     //打开读管道
                    while((hWrite==INVALID_HANDLE_VALUE)||(hRead==INVALID_HANDLE_VALUE))
                    {
                        CloseHandle(hWrite);
                        CloseHandle(hRead);
                        QtSleep(1000);
                        hWrite = m_UsbPort->OpenDevFile(m_UsbPort->m_OutPipe);   //打开写管道
                        hRead = m_UsbPort->OpenDevFile(m_UsbPort->m_InPipe);     //打开读管道
                    }
                    QtSleep(1000);
                    loopcount = 0;
                    char txBuf[10];
                    *((unsigned char *)txBuf+0)=D_MODECMD;
                    *((unsigned char *)txBuf+1)=CMD_GETSTATUS;
                    WriteFile(hWrite,txBuf,8,&nBytesWrite,NULL);
                }
            }

            //>@发送文件烧录指令
            {
                char rxBuf[20];
                char txBuf[20];
                DWORD nBytesWrite = 0;
                *((unsigned char *)txBuf+0)=D_MODECMD;
                *((unsigned char *)txBuf+1)=m_CurExecCMD;
                WriteFile(hWrite,txBuf,8,&nBytesWrite,NULL);
                while(1)
                {
                    memset(rxBuf, 0, sizeof(rxBuf));
                    ReadFile(hRead,rxBuf,13,&nBytesWrite,NULL);
                    if(strcmp("Status_is_Ok!",rxBuf) == 0)
                        break;
                }
            }
        }
Checksum_failed:
        free((void *)m_FileTxBuffer);
        emit sDownloadFinised();
        CloseHandle(hWrite);
        CloseHandle(hRead);
    }
#ifdef WINTHREAD
    m_WinThreadFinished = true;
    _endthread();
#endif
    return;
}

bool UsbDFU::UploadFile(QString pFilePath)
{
    return true;
}


void UsbDFU::InitDownloadThread()
{
    ReleaseDownloadThread();
    m_DownloadThread = new DownloadFileThread(this);
}
void UsbDFU::ReleaseDownloadThread()
{
    if(m_DownloadThread != 0)
    {
        StopDownloadThread();
        m_DownloadThread->quit();
        m_DownloadThread->deleteLater();
        m_DownloadThread = 0;
    }
}
void UsbDFU::StartDownloadThread()
{
    if(m_UsbPort)
        m_UsbPort->StopMonitorThread();
    if(m_DownloadThread != 0)
    {
        if (m_DownloadThread->isRunning())
            return;
        m_DownloadThread->start(QThread::LowPriority);
    }
}

void UsbDFU::StopDownloadThread()
{
    if(m_DownloadThread != 0)
    {
        if (m_DownloadThread->isRunning())
        {
            m_DownloadThread->stop();
            if (QThread::currentThread() != m_DownloadThread)
                m_DownloadThread->wait();
        }
    }
}
