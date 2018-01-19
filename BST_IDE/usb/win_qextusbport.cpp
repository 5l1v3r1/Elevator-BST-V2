#include "win_qextusbport.h"

/*!
\fn QextSerialPort::QextSerialPort(const QString & name)
Constructs a serial port attached to the port specified by devName.
devName is the name of the device, which is windowsystem-specific,
e.g."COM2" or "/dev/ttyS0".
*/
//"PIPE00"  "PIPE01"

QextUsbPort::QextUsbPort(GUID pGuid, QString pInPipe, QString pOutPipe, QextUsbBase::QueryMode mode):
    QextUsbBase(pGuid)
{
    m_Handle=0;
    m_InPipe = (char*)malloc(sizeof(char)*32);
    m_OutPipe = (char*)malloc(sizeof(char)*32);
    m_CompleteDeviceName = (char*)malloc(sizeof(char)*256);
    strcpy(m_InPipe, pInPipe.toLatin1().data());         // pipe name for bulk input pipe on our test board
    strcpy(m_OutPipe, pOutPipe.toLatin1().data());        // pipe name for bulk output pipe on our test board
    strcpy(m_CompleteDeviceName, "");   // generated from the GUID registered by the driver itself

    overlapThread = 0;

    hWrite = INVALID_HANDLE_VALUE;
    hRead = INVALID_HANDLE_VALUE;

    setQueryMode(mode);
    init();
}

void QextUsbPort::init()
{
	_bytesToWrite = 0;
    if (queryMode() == QextUsbBase::EventDriven)
    {
        overlapThread = new QextUsbThread(this);
        bytesToWriteLock = new QReadWriteLock;
    }
}

/*!
\fn QextSerialPort::~QextSerialPort()
Standard destructor.
*/
QextUsbPort::~QextUsbPort() {
    if (isOpen())
    {
        close();
    }
    if (queryMode() == QextUsbBase::EventDriven)
    {
        delete overlapThread;
        delete bytesToWriteLock;
    }
}

/*!
\fn bool QextSerialPort::open(OpenMode mode)
Opens a serial port.  Note that this function does not specify which device to open.  If you need
to open a device by name, see QextSerialPort::open(const char*).  This function has no effect
if the port associated with the class is already open.  The port is also configured to the current
settings, as stored in the Settings structure.
*/
bool QextUsbPort::open(OpenMode mode)
{
    LOCK_MUTEX();
    if (!isOpen())
    {
        /*open the port*/
        m_Handle = OpenUsbDevice( (LPGUID)&m_Guid, m_CompleteDeviceName);
        if (m_Handle != INVALID_HANDLE_VALUE)
        {
            IDE_TRACE_STR(QString(m_CompleteDeviceName));
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

bool QextUsbPort::isConnected()
{
    m_Handle = OpenUsbDevice( (LPGUID)&m_Guid, m_CompleteDeviceName);
    if (m_Handle != INVALID_HANDLE_VALUE)
    {
        return true;
    }
    return false;
}

/*!
\fn void QextSerialPort::close()
Closes a serial port.  This function has no effect if the serial port associated with the class
is not currently open.
*/
void QextUsbPort::close()
{
    LOCK_MUTEX();

    if (isOpen())
    {
		flush();
        if (queryMode() == QextUsbBase::EventDriven && overlapThread->isRunning())
        {
			overlapThread->stop();
			if (QThread::currentThread() != overlapThread)
				overlapThread->wait();
		}

        m_Handle = INVALID_HANDLE_VALUE;
		_bytesToWrite = 0;
		QIODevice::close();
    }

    UNLOCK_MUTEX();
}

/*!
\fn void QextSerialPort::flush()
Flushes all pending I/O to the serial port.  This function has no effect if the serial port
associated with the class is not currently open.
*/
void QextUsbPort::flush()
{
    LOCK_MUTEX();
    if (isOpen())
    {
        FlushFileBuffers(m_Handle);
    }
    UNLOCK_MUTEX();
}

/*!
\fn qint64 QextSerialPort::size() const
This function will return the number of bytes waiting in the receive queue of the serial port.
It is included primarily to provide a complete QIODevice interface, and will not record errors
in the lastErr member (because it is const).  This function is also not thread-safe - in
multithreading situations, use QextSerialPort::bytesAvailable() instead.
*/
qint64 QextUsbPort::size() const
{
    int availBytes;

    return (qint64)availBytes;
}

/*!
\fn qint64 QextSerialPort::bytesAvailable()
Returns the number of bytes waiting in the port's receive queue.  This function will return 0 if
the port is not currently open, or -1 on error.  Error information can be retrieved by calling
QextSerialPort::getLastError().
*/
qint64 QextUsbPort::bytesAvailable()
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

void QextUsbPort::ungetChar(char c)
{
    LOCK_MUTEX();
    if (isOpen())
    {
        UNLOCK_MUTEX();
    }
    UNLOCK_MUTEX();
}

/*!
\fn void QextSerialPort::setTimeout(ulong millisec);
Sets the read and write timeouts for the port to millisec milliseconds.
Setting 0 for both sec and millisec indicates that timeouts are not used for read nor
write operations. Setting -1 indicates that read and write should return immediately.

\note this function does nothing in event driven mode.
*/
void QextUsbPort::setTimeout(long millisec) {
    LOCK_MUTEX();
    m_Timeout_Millisec = millisec;

    if (millisec == -1)  //>@Á¢¼´·µ»Ø
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

DWORD QextUsbPort::QUsbRead(PVOID data, DWORD maxSize, int timeout)
{
    ULONG nBytesRead;
    char temp[256]="";
    strcpy(temp, m_CompleteDeviceName);
    strcat(temp, "\\");
    strcat(temp,m_InPipe);
    hRead = OpenDevFile(temp);
    if(hRead == INVALID_HANDLE_VALUE)
    {
        return -1;
    }
    ReadFile(hRead, data, maxSize, &nBytesRead, NULL);
    if(hRead != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hRead);
    }
    return (quint32)nBytesRead;
}

DWORD QextUsbPort::QUsbWrite(PCVOID data, DWORD maxSize, int timeout)
{
    ULONG nBytesRead;
    char temp[256]="";
    strcpy(temp, m_CompleteDeviceName);
    strcat(temp, "\\");
    strcat(temp,m_OutPipe);
    hWrite = OpenDevFile(temp);
    if(hWrite == INVALID_HANDLE_VALUE)
    {
        return -1;
    }
    WriteFile(hWrite, data, maxSize, &nBytesRead, NULL);
    if(hWrite != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hWrite);
    }
    return (quint32)nBytesRead;
}

/*!
\fn qint64 QextSerialPort::readData(char *data, qint64 maxSize)
Reads a block of data from the serial port.  This function will read at most maxlen bytes from
the serial port and place them in the buffer pointed to by data.  Return value is the number of
bytes actually read, or -1 on error.

\warning before calling this function ensure that serial port associated with this class
is currently open (use isOpen() function to check if port is open).
*/
qint64 QextUsbPort::readData(char *data, qint64 maxSize)
{
    qint64 retVal;
	
    LOCK_MUTEX();
    
    retVal = 0;
    if (queryMode() == QextUsbBase::EventDriven)
    {
        retVal = QUsbRead(data, (qint64)maxSize, m_Timeout_Millisec);
        if (retVal <= 0)
        {
            retVal = (qint64)-1;
        }
    }
    else
    {
        retVal = QUsbRead(data, (qint64)maxSize, 0);
        if (retVal <= 0)
        {
            retVal = (qint64)-1;
        }
    }

	UNLOCK_MUTEX();
    return (qint64)retVal;
}

/*!
\fn qint64 QextSerialPort::writeData(const char *data, qint64 maxSize)
Writes a block of data to the serial port.  This function will write len bytes
from the buffer pointed to by data to the serial port.  Return value is the number
of bytes actually written, or -1 on error.

\warning before calling this function ensure that serial port associated with this class
is currently open (use isOpen() function to check if port is open).
*/
qint64 QextUsbPort::writeData(const char *data, qint64 maxSize)
{
    qint64 retVal;
    LOCK_MUTEX();

    retVal = 0;
    if (queryMode() == QextUsbBase::EventDriven)
    {
    	bytesToWriteLock->lockForWrite();
    	_bytesToWrite += maxSize;
        bytesToWriteLock->unlock();

        retVal = QUsbWrite(data, (qint64)maxSize, m_Timeout_Millisec);
        if (retVal <= 0)
        {
            retVal = (qint64)-1;
        }
        else
	   		retVal = maxSize;
    }
    else
    {
        retVal = QUsbWrite(data, (qint64)maxSize, 0);
        if (retVal <= 0)
        {
            retVal = (qint64)-1;
        }
   	}
   	
    UNLOCK_MUTEX();
    return (qint64)retVal;
}

void QextUsbPort::monitorUsbEvent()
{
    DWORD eventMask = 0;

    ResetEvent(overlap.hEvent);
    if (!WaitCommEvent(m_Handle, & eventMask, & overlap))
        if (GetLastError() != ERROR_IO_PENDING)
            qCritical("WaitCommEvent error %ld\n", GetLastError());

    if (WaitForSingleObject(overlap.hEvent, INFINITE) == WAIT_OBJECT_0)
    {
        //overlap event occured
        DWORD undefined;
        if (!GetOverlappedResult(m_Handle, & overlap, & undefined, false))
        {
            qWarning("Comm event overlapped error %ld", GetLastError());
            return;
        }
        if (eventMask & EV_RXCHAR) {
            if (sender() != this)
                emit readyRead();
        }
        if (eventMask & EV_TXEMPTY) {
            DWORD numBytes;
            GetOverlappedResult(m_Handle, & overlapWrite, & numBytes, true);
            bytesToWriteLock->lockForWrite();
            if (sender() != this)
                emit bytesWritten(bytesToWrite());
            _bytesToWrite = 0;
            bytesToWriteLock->unlock();
        }
    }
}

bool QextUsbPort::waitForReadyRead(int msecs)
{
	//@todo implement
	return false;
}

qint64 QextUsbPort::bytesToWrite() const
{
	return _bytesToWrite;
}

QextUsbThread::QextUsbThread(QextUsbPort * qesp):
	QThread()
{
	this->qesp = qesp;
	terminate = false;
}

void QextUsbThread::stop()
{
    terminate = true;
}

void QextUsbThread::run()
{
	while (!terminate)
    {
        qesp->monitorUsbEvent();
    }
	terminate = false;
}
