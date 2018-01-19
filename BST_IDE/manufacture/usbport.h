#ifndef USBPORT_H
#define USBPORT_H

#include "usbbase.h"

typedef enum{
    CMD_XLOADUPGRADE = 0X01,
    CMD_UBOOTUPGRADE = 0X02,
    CMD_KERNELUPGRADE = 0X03,
    CMD_FILESYSTEMUPGRADE = 0X04,
    CMD_APPLICATIONUPGRADE = 0X05,
    CMD_UBOOTSTD = 0x0a,
    CMD_RESET = 0X7,
    CMD_GETSTATUS = 0X8,
    CMD_SETBOOTARGS = 0X9
} EXEC_CMD;

#define D_MODEDATA 0xAE
#define D_MODECMD  0xBF

#define TX_SIZE    (256*4)

#define Xload_Mode 0x01
#define Bootloader_Mode 0x02
#define Kernel_Mode 0x03
#define Rootfs_Mode 0x04
#define CleanBootargs_Mode 0x05
#define Bootcmd_Mode 0x06
#define Reset_Mode 0x07
#define Status_Mode 0x08

class DownloadFileThread;

class UsbDFU : public QObject
{
    Q_OBJECT
public:
    explicit UsbDFU(GUID pGuid, QObject *parent = 0);
    ~UsbDFU();

    bool DownloadFile(QString pFileName, EXEC_CMD pCMD);
    bool UploadFile(QString pFilePath);

    void ProcessData(void *args=0);
    void InitDownloadThread();
    void ReleaseDownloadThread();
    void StartDownloadThread();
    void StopDownloadThread();

    bool isDownloaded();

    bool SendCMD(EXEC_CMD pCMD);
    bool SendData(QString pData);
signals:
    void sProgress(QString pMessage=QString(), int ProgressVaule=0);
    void sConnState(bool pState);
    void sDownloadFinised();
public:
    UsbBase    *m_UsbPort;
    HANDLE      hWrite;
    HANDLE      hRead;
    bool        m_DownloadErrorFlag;
    QString     m_DownloadLastError;
    bool        m_SendEnable;
#ifdef WINTHREAD
    bool        m_WinThreadFinished;
#endif

    EXEC_CMD    m_CurExecCMD;

    //>@根据一下两项可以计算当前发送进度
    ULONG   m_CurSendDataNum;      //>@当前已发送字节数
    ULONG   m_FileTxBufferSize;    //>@总共需要发送字节数
    char *  m_FileTxBuffer;

    DownloadFileThread *m_DownloadThread;
};

class DownloadFileThread: public QThread
{
    Q_OBJECT
public:
    DownloadFileThread(UsbDFU *pUsbDFU, QObject *parent=0)
        :QThread(parent)
    {
        m_UsbDFU = pUsbDFU;
        if(m_UsbDFU)
            terminate = false;
        else
            terminate = true;
    }
    ~DownloadFileThread()
    {
        stop();
    }
    void stop()
    {
        terminate = true;
    }
protected:
    virtual void run()
    {
        if(!terminate)
            m_UsbDFU->ProcessData();
        terminate = false;
    }
public:
    UsbDFU      *m_UsbDFU;
    bool        terminate;
};

#endif // USBPORT_H
