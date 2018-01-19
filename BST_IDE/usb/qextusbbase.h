#ifndef _QEXTUSBBASE_H_
#define _QEXTUSBBASE_H_

#include "global.h"
#include <QIODevice>
#include <QMutex>
#include <windows.h>
#include <setupapi.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "devioctl.h"
#include "setupapi.h"
#include <basetyps.h>
#include <initguid.h>
#include "usbdi.h"
#include <QMessageBox>

#include <commctrl.h>
#include <tchar.h>
#include <string.h>
#include <process.h>
#include <stdlib.h>

#include <winbase.h>
#include <windows.h>
#include <QThread>
#include <Process.h>
#include <QEvent>
#include <QReadWriteLock>

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

/*macros for thread support*/
#ifndef LOCK_MUTEX()
#define LOCK_MUTEX() mutex->lock()
#endif
#ifndef UNLOCK_MUTEX()
#define UNLOCK_MUTEX() mutex->unlock()
#endif


extern const GUID GUID_CLASS_I82930_BULK;

/*!
 * 
 * A common base class for Win_QextSerialBase, Posix_QextSerialBase and QextSerialPort.
 */
class QextUsbBase : public QIODevice
{
Q_OBJECT

public:

protected:
    QMutex* mutex;

    ulong lastErr;
    QextUsbBase::QueryMode _queryMode;

    virtual qint64 readData(char * data, qint64 maxSize)=0;
    virtual qint64 writeData(const char * data, qint64 maxSize)=0;

public:
    QextUsbBase();
    QextUsbBase(GUID pGuid);
    virtual ~QextUsbBase();
    virtual void construct();

    virtual void setGUID(GUID pGuid);
    virtual GUID getGUID();

    GUID    m_Guid;
    char *  m_CompleteDeviceName;

    /**!
     * Get query mode.
     * 	\return query mode.
     */
    inline QextUsbBase::QueryMode queryMode() const {  return _queryMode; }

    virtual void setQueryMode(QueryMode mode);

    virtual void setTimeout(long)=0;

    virtual bool open(OpenMode mode)=0;
    virtual bool isSequential() const;
    virtual void close()=0;
    virtual void flush()=0;

    virtual qint64 size() const=0;
    virtual qint64 bytesAvailable()=0;
    virtual bool atEnd() const;

    virtual void ungetChar(char c)=0;
    virtual qint64 readLine(char * data, qint64 maxSize);

    virtual ulong lastError() const;
    virtual void translateError(ulong error)=0;

    virtual ulong lineStatus()=0;

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
};

#endif
