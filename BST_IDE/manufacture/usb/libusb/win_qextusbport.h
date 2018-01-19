#ifndef _WIN_QEXTUSBPORT_H_
#define _WIN_QEXTUSBPORT_H_

#include "qextusbbase.h"
#include <winbase.h>
#include <windows.h>
#include <QThread>
#include <Process.h>
#include <QEvent>
#include <QReadWriteLock>

class QReadWriteLock;
class Win_QextUsbThread;

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE 0
#endif

class Win_QextUsbPort: public QextUsbBase
{
	Q_OBJECT
    friend class Win_QextUsbThread;
	
	private:
		/*!
		 * This method is a part of constructor.
		 */
		void init();
		
	protected:
        usb_dev_handle *Win_Handle; /* the device handle */

        QBuffer m_ReadBuffer;

		QReadWriteLock * bytesToWriteLock;	///< @todo maybe move to QextSerialBase.
		qint64 _bytesToWrite;		///< @todo maybe move to QextSerialBase (and implement in POSIX).
        Win_QextUsbThread * overlapThread; ///< @todo maybe move to QextSerialBase (and implement in POSIX).
		 	
        void monitorCommEvent();
	    virtual qint64 readData(char *data, qint64 maxSize);
        virtual qint64 writeData(char *data, qint64 maxSize);

	public:
        Win_QextUsbPort(const int & pVID, const int & pPID, QextUsbBase::QueryMode mode = QextUsbBase::Polling);
        Win_QextUsbPort(const UsbPortSettings& settings, QextUsbBase::QueryMode mode = QextUsbBase::Polling);

        virtual ~Win_QextUsbPort();
	    virtual bool open(OpenMode mode);
	    virtual void close();
	    virtual void flush();
	    virtual qint64 size() const;
	    virtual void ungetChar(char c);

        virtual void setTimeout(long);

        quint32 QUsbRead(usb_dev_handle *dev, char* data, quint32 maxSize, int timeout);
        quint32 QUsbWrite(usb_dev_handle *dev, char* data, quint32 maxSize, int timeout);

	    virtual qint64 bytesAvailable();
	    
		virtual qint64 bytesToWrite() const;
		
		virtual bool waitForReadyRead(int msecs);	///< @todo implement.

        void monitorUsbEvent();
};

/*!
 * This thread monitors communication events.
 */
class Win_QextUsbThread: public QThread
{
    Win_QextUsbPort * qesp;
	bool terminate;

	public:
		/*!
		 * Constructor.
		 * 
		 * \param qesp valid serial port object.
		 */
        Win_QextUsbThread(Win_QextUsbPort * qesp);
		
		/*!
		 * Stop the thread.
		 */
		void stop();
	
	protected:
		//overriden
		virtual void run();
	
};

#endif
