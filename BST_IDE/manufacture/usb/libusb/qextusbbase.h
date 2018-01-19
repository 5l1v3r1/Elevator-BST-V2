#ifndef _QEXTUSBBASE_H_
#define _QEXTUSBBASE_H_

#include "global.h"
#include <QIODevice>
#include <QMutex>
#include "lusb0_usb.h"

//////////////////////////////////////////////////////////////////////////////
// TEST SETUP (User configurable)

// Issues a Set configuration request
#define TEST_SET_CONFIGURATION

// Issues a claim interface request
#define TEST_CLAIM_INTERFACE

// Use the libusb-win32 async transfer functions. see
// transfer_bulk_async() below.
//#define TEST_ASYNC

// Attempts one bulk read.
#define TEST_BULK_READ

// Attempts one bulk write.
#define TEST_BULK_WRITE

//////////////////////////////////////////////////////////////////////////////
// DEVICE SETUP (User configurable)

// Device configuration and interface id.
#define MY_CONFIG 1
#define MY_INTF 0

// Device endpoint(s)
#define EP_IN 0x81
#define EP_OUT 0x01

// Device of bytes to transfer.
#define BUF_SIZE 64


/*macros for thread support*/
#ifndef LOCK_MUTEX()
#define LOCK_MUTEX() mutex->lock()
#endif
#ifndef UNLOCK_MUTEX()
#define UNLOCK_MUTEX() mutex->unlock()
#endif

/*!
 * Enums for port settings.
 */
enum NamingConvention 
{
    WIN_NAMES,
    IRIX_NAMES,
    HPUX_NAMES,
    SUN_NAMES,
    DIGITAL_NAMES,
    FREEBSD_NAMES,
    LINUX_NAMES
};

/**
 * structure to contain port settings
 */
typedef struct
{
    int     mVID;
    int     mPID;
    int     mConfig;
    int     mInterfaceId;
    int     mEpIn;
    int     mEpOut;
    long    Timeout_Millisec;
}UsbPortSettings;

/*!
 * 
 * A common base class for Win_QextSerialBase, Posix_QextSerialBase and QextSerialPort.
 */
class QextUsbBase : public QIODevice
{
	Q_OBJECT

	public:
		enum QueryMode {
			Polling,
			EventDriven
		};

	protected:
	    QMutex* mutex;
        int     m_VID;
        int     m_PID;
        UsbPortSettings Settings;
	    ulong lastErr;
        QextUsbBase::QueryMode _queryMode;

	    virtual qint64 readData(char * data, qint64 maxSize)=0;
	    virtual qint64 writeData(const char * data, qint64 maxSize)=0;

	public:
        QextUsbBase();
        QextUsbBase(const int & pVID, const int & pPID);
        virtual ~QextUsbBase();
	    virtual void construct();
        virtual void setVID(const int & vid);
        virtual int getVID();
        virtual void setPID(const int & pid);
        virtual int getPID();
	    
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

	signals:
		/**
		 * This signal is emitted whenever port settings are updated.
		 * 	\param valid \p true if settings are valid, \p false otherwise.
		 * 
		 * 	@todo implement.
		 */
//		void validSettings(bool valid);

		/*!
		 * This signal is emitted whenever dsr line has changed its state. You may
		 * use this signal to check if device is connected.
		 * 	\param status \p true when DSR signal is on, \p false otherwise.
		 * 
		 * 	\see lineStatus().
		 */
		void dsrChanged(bool status);
};

#endif
