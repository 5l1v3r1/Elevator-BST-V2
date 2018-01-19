#include "win_qextusbport.h"

/*!
\fn Win_QextSerialPort::Win_QextSerialPort(const QString & name)
Constructs a serial port attached to the port specified by devName.
devName is the name of the device, which is windowsystem-specific,
e.g."COM2" or "/dev/ttyS0".
*/
Win_QextUsbPort::Win_QextUsbPort(const int & pVID, const int & pPID, QextUsbBase::QueryMode mode):
    QextUsbBase(pVID, pPID)
{
    Win_Handle=0;
    setQueryMode(mode);
    init();
}

/*!
\fn Win_QextSerialPort::Win_QextSerialPort(const PortSettings& settings)
Constructs a port with default name and specified settings.
*/
Win_QextUsbPort::Win_QextUsbPort(const UsbPortSettings& settings, QextUsbBase::QueryMode mode)
{
    Win_Handle=0;
    setVID(settings.mVID);
    setPID(settings.mPID);
    setTimeout(settings.Timeout_Millisec);
    setQueryMode(mode);
    init();
}

void Win_QextUsbPort::init()
{
	_bytesToWrite = 0;
    overlapThread = new Win_QextUsbThread(this);
	bytesToWriteLock = new QReadWriteLock;

    usb_init(); /* initialize the library */
    usb_find_busses(); /* find all busses */
    usb_find_devices(); /* find all connected devices */
}

/*!
\fn Win_QextSerialPort::~Win_QextSerialPort()
Standard destructor.
*/
Win_QextUsbPort::~Win_QextUsbPort() {
    if (isOpen())
    {
        close();
    }
    delete overlapThread;
    delete bytesToWriteLock; 
}

/*!
\fn bool Win_QextSerialPort::open(OpenMode mode)
Opens a serial port.  Note that this function does not specify which device to open.  If you need
to open a device by name, see Win_QextSerialPort::open(const char*).  This function has no effect
if the port associated with the class is already open.  The port is also configured to the current
settings, as stored in the Settings structure.
*/
bool Win_QextUsbPort::open(OpenMode mode)
{
    LOCK_MUTEX();
    if (!isOpen())
    {
        /*open the port*/
        struct usb_bus *bus;
        struct usb_device *dev;
        for (bus = usb_get_busses(); bus; bus = bus->next)
        {
            for (dev = bus->devices; dev; dev = dev->next)
            {
                if (dev->descriptor.idVendor == Settings.mVID
                    && dev->descriptor.idProduct == Settings.mPID)
                {
                    Win_Handle = usb_open(dev);
                }
            }
        }
        if (Win_Handle!=0)
        {
            /*set up parameters*/
            setVID(Settings.mVID);
            setPID(Settings.mPID);
            setTimeout(Settings.Timeout_Millisec);

            #ifdef TEST_SET_CONFIGURATION
            if (usb_set_configuration(Win_Handle, Settings.mConfig) < 0)
            {
                //("error setting config #%d: %s\n", MY_CONFIG, usb_strerror());
                usb_close(Win_Handle);
                return false;
            }
            #endif

            #ifdef TEST_CLAIM_INTERFACE
            if (usb_claim_interface(Win_Handle, 0) < 0)
            {
                //printf("error claiming interface #%d:\n%s\n", MY_INTF, usb_strerror());
                usb_close(Win_Handle);
                return false;
            }
            #endif

            //init event driven approach
            if (queryMode() == QextUsbBase::EventDriven)
            {
            	overlapThread->start();
            }
			QIODevice::open(mode);
        }
    }
    else
    {
		UNLOCK_MUTEX();
    	return false;
    }
    UNLOCK_MUTEX();
    return isOpen();
}

/*!
\fn void Win_QextSerialPort::close()
Closes a serial port.  This function has no effect if the serial port associated with the class
is not currently open.
*/
void Win_QextUsbPort::close()
{
    LOCK_MUTEX();

    if (isOpen())
    {
		flush();
        if (overlapThread->isRunning())
        {
			overlapThread->stop();
			if (QThread::currentThread() != overlapThread)
				overlapThread->wait();
		}
        //if (Win_Handle != INVALID_HANDLE_VALUE)
        {
            if(usb_close(Win_Handle))
                Win_Handle = 0;
        }
		_bytesToWrite = 0;
		QIODevice::close();
    }

    UNLOCK_MUTEX();
}

/*!
\fn void Win_QextSerialPort::flush()
Flushes all pending I/O to the serial port.  This function has no effect if the serial port
associated with the class is not currently open.
*/
void Win_QextUsbPort::flush()
{
    LOCK_MUTEX();
    if (isOpen())
    {
        //FlushFileBuffers(Win_Handle);
    }
    UNLOCK_MUTEX();
}

/*!
\fn qint64 Win_QextSerialPort::size() const
This function will return the number of bytes waiting in the receive queue of the serial port.
It is included primarily to provide a complete QIODevice interface, and will not record errors
in the lastErr member (because it is const).  This function is also not thread-safe - in
multithreading situations, use Win_QextSerialPort::bytesAvailable() instead.
*/
qint64 Win_QextUsbPort::size() const
{
    int availBytes;

    return (qint64)availBytes;
}

/*!
\fn qint64 Win_QextSerialPort::bytesAvailable()
Returns the number of bytes waiting in the port's receive queue.  This function will return 0 if
the port is not currently open, or -1 on error.  Error information can be retrieved by calling
Win_QextSerialPort::getLastError().
*/
qint64 Win_QextUsbPort::bytesAvailable()
{
    LOCK_MUTEX();
    if (isOpen())
    {
        UNLOCK_MUTEX();
        return /*Status.cbInQue +*/ QIODevice::bytesAvailable();
    }
    UNLOCK_MUTEX();
    return 0;
}

void Win_QextUsbPort::ungetChar(char c)
{
    LOCK_MUTEX();
    if (isOpen())
    {
        UNLOCK_MUTEX();
    }
    UNLOCK_MUTEX();
}

/*!
\fn void Win_QextSerialPort::setTimeout(ulong millisec);
Sets the read and write timeouts for the port to millisec milliseconds.
Setting 0 for both sec and millisec indicates that timeouts are not used for read nor
write operations. Setting -1 indicates that read and write should return immediately.

\note this function does nothing in event driven mode.
*/
void Win_QextUsbPort::setTimeout(long millisec) {
    LOCK_MUTEX();
    Settings.Timeout_Millisec = millisec;

    if (millisec == -1)
    {
    }
    else
    {
    }
    if (queryMode() != QextUsbBase::EventDriven)
    {

    }
    UNLOCK_MUTEX();
}

quint32 Win_QextUsbPort::QUsbRead(usb_dev_handle *dev, char* data, quint32 maxSize, int timeout)
{
    return usb_bulk_read(dev, Settings.mEpIn, data, maxSize, timeout);
}

quint32 Win_QextUsbPort::QUsbWrite(usb_dev_handle *dev, char* data, quint32 maxSize, int timeout)
{
    return usb_interrupt_write(dev, Settings.mEpOut, data, maxSize, timeout);
}

/*!
\fn qint64 Win_QextSerialPort::readData(char *data, qint64 maxSize)
Reads a block of data from the serial port.  This function will read at most maxlen bytes from
the serial port and place them in the buffer pointed to by data.  Return value is the number of
bytes actually read, or -1 on error.

\warning before calling this function ensure that serial port associated with this class
is currently open (use isOpen() function to check if port is open).
*/
qint64 Win_QextUsbPort::readData(char *data, qint64 maxSize)
{
    qint64 retVal;
	
    LOCK_MUTEX();
    
    retVal = 0;
    if (queryMode() == QextUsbBase::EventDriven)
    {
        retVal = QUsbRead(Win_Handle, data, (qint64)maxSize, Settings.Timeout_Millisec);
        if (retVal <= 0)
        {
            retVal = (qint64)-1;
        }
    }
    else
    {
        retVal = QUsbRead(Win_Handle, data, (qint64)maxSize, 0);
        if (retVal <= 0)
        {
            retVal = (qint64)-1;
        }
    }

	UNLOCK_MUTEX();
    return (qint64)retVal;
}

/*!
\fn qint64 Win_QextSerialPort::writeData(const char *data, qint64 maxSize)
Writes a block of data to the serial port.  This function will write len bytes
from the buffer pointed to by data to the serial port.  Return value is the number
of bytes actually written, or -1 on error.

\warning before calling this function ensure that serial port associated with this class
is currently open (use isOpen() function to check if port is open).
*/
qint64 Win_QextUsbPort::writeData(char *data, qint64 maxSize)
{
    qint64 retVal;
    LOCK_MUTEX();

    retVal = 0;
    if (queryMode() == QextUsbBase::EventDriven)
    {
    	bytesToWriteLock->lockForWrite();
    	_bytesToWrite += maxSize;
        bytesToWriteLock->unlock();

        retVal = QUsbWrite(Win_Handle, data, (qint64)maxSize, Settings.Timeout_Millisec);
        if (retVal <= 0)
        {
            retVal = (qint64)-1;
        }
        else
	   		retVal = maxSize;
    }
    else
    {
        retVal = QUsbWrite(Win_Handle, data, (qint64)maxSize, 0);
        if (retVal <= 0)
        {
            retVal = (qint64)-1;
        }
   	}
   	
    UNLOCK_MUTEX();
    return (qint64)retVal;
}

bool Win_QextUsbPort::waitForReadyRead(int msecs)
{
	//@todo implement
	return false;
}

qint64 Win_QextUsbPort::bytesToWrite() const
{
	return _bytesToWrite;
}


void Win_QextUsbPort::monitorUsbEvent()
{
    char tmpBuffer[1024];
    int retval = readData(tmpBuffer, 1024);
    if (retval > 0)
    {
        emit readyRead();
    }
}

Win_QextUsbThread::Win_QextUsbThread(Win_QextUsbPort * qesp):
	QThread()
{
	this->qesp = qesp;
	terminate = false;
}

void Win_QextUsbThread::stop()
{
    terminate = true;
}

void Win_QextUsbThread::run()
{
	while (!terminate)
    {
        qesp->monitorUsbEvent();
    }
	terminate = false;
}
