#ifndef USBPORT_H
#define USBPORT_H

#include "globalfun.h"

#ifdef WINDOWS

#include "usb/usbdi.h"
#include "usb/devioctl.h"

//////////////////////////////////////////////////////////////////////////////
// TEST SETUP (User configurable)

#define BULKUSB_IOCTL_INDEX  0x0000

#define IOCTL_BULKUSB_GET_CONFIG_DESCRIPTOR     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                           BULKUSB_IOCTL_INDEX,\
                           METHOD_BUFFERED,  \
                           FILE_ANY_ACCESS)

#define IOCTL_BULKUSB_RESET_DEVICE   CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                           BULKUSB_IOCTL_INDEX+1,\
                           METHOD_BUFFERED,  \
                           FILE_ANY_ACCESS)

#define IOCTL_BULKUSB_RESET_PIPE  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                           BULKUSB_IOCTL_INDEX+2,\
                           METHOD_BUFFERED,  \
                           FILE_ANY_ACCESS)

extern const GUID GUID_CLASS_I82930_BULK;

class UsbPortThread;

class UsbPort : public QIODevice
{
    Q_OBJECT

public:
    UsbPort(GUID pGuid, QString pInPipe, QString pOutPipe, QObject *parent=0);
    ~UsbPort();

    void construct();
    bool open(OpenMode mode);
    void close();
    void flush();
    bool isConnected();
    qint64 size();

    void monitor();

    void setInPipe(QString pGuid);
    QString getInPipe();

    void setOutPipe(QString pGuid);
    QString getOutPipe();

    void setGUID(GUID pGuid);
    GUID getGUID();

    bool isSequential() const;
    bool atEnd();

    void ungetChar(char c);
    qint64 readLine(char * data, qint64 maxSize);

    qint64 bytesAvailable();
    qint64 bytesToWrite() {return 0;}

    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);
    bool readFeedback(char* pFeedback, quint32 pTimeout);

    bool    isError() {return errorFlag;}
    ulong   lastError() {return lastErr;}
    QString lastErrorString() {return lastErrString;}
    void    translateError(ulong error) {return ;}

    void setTimeout(long millisec) { m_Timeout_Millisec = millisec; }
    ulong lineStatus() {return 0;}
    bool waitForReadyRead(int msecs) {return false;}	///< @todo implement.

    void InitMonitorThread();
    void ReleaseMonitorThread();
    void StartMonitorThread();
    void StopMonitorThread();

    HANDLE OpenOneDevice (  IN       HDEVINFO                    HardwareDeviceInfo,
                            IN       PSP_INTERFACE_DEVICE_DATA   DeviceInfoData,
                            IN       char *devName);

    HANDLE OpenUsbDevice(LPGUID  pGuid, char *outNameBuf);

    BOOL GetUsbDeviceFileName( LPGUID  pGuid, char *outNameBuf);

    HANDLE OpenDevFile(char *filename);

    char *usbDescriptorTypeString(UCHAR bDescriptorType );
    char *usbEndPointTypeString(UCHAR bmAttributes);
    char *usbConfigAttributesString(UCHAR bmAttributes);
    void print_USB_ENDPOINT_DESCRIPTOR(PUSB_ENDPOINT_DESCRIPTOR ed, int i);
    void print_USB_INTERFACE_DESCRIPTOR(PUSB_INTERFACE_DESCRIPTOR id, UINT ix);
    void print_USB_CONFIGURATION_DESCRIPTOR(PUSB_CONFIGURATION_DESCRIPTOR cd);

signals:
    void sConnState(bool pState);
public:
    QMutex* mutex;
    HANDLE  m_Handle; /* the device handle */

    bool    errorFlag;
    QString lastErrString;
    ulong   lastErr;

    CONN_STATE   m_ConnState;

    GUID    m_Guid;
    char *  m_CompleteDeviceName;
    char *  m_InPipe;
    char *  m_OutPipe;

    QReadWriteLock * bytesToWriteLock;	///< @todo maybe move to QextSerialBase.
    qint64 _bytesToWrite;		///< @todo maybe move to QextSerialBase (and implement in POSIX).
    qint64 _bytesForRead;
    UsbPortThread * overlapThread; ///< @todo maybe move to QextSerialBase (and implement in POSIX).

    quint32 m_Timeout_Millisec;
};

class UsbPortThread: public QThread
{
public:
    UsbPortThread(UsbPort * qesp, QObject *parent=0)
        :QThread(parent)
    {
        mUsbPort = qesp;
        if(mUsbPort)
            terminate = false;
        else
            terminate = true;
    }
    void stop()
    {
        terminate = true;
    }
protected:
    virtual void run()
    {
        while(!terminate)
        {
            mUsbPort->monitor();
            msleep(500);
        }
        terminate = true;
    }
public:
    UsbPort * mUsbPort;
    bool terminate;
};

#endif

#endif
