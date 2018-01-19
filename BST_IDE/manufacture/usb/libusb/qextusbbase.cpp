#include "qextusbbase.h"

/*!
\fn QextUsbBase::QextUsbBase()
Default constructor.
*/
QextUsbBase::QextUsbBase()
 : QIODevice()
{
    setVID(0);
    setPID(0);
    construct();
}

/*!
\fn QextSerialBase::QextSerialBase(const QString & name)
Construct a port and assign it to the device specified by the name parameter.
*/
QextUsbBase::QextUsbBase(const int & pVID, const int & pPID)
 : QIODevice()
{
    setVID(pVID);
    setPID(pPID);
    construct();
}

/*!
\fn QextSerialBase::~QextSerialBase()
Standard destructor.
*/
QextUsbBase::~QextUsbBase()
{
	delete mutex;
}

/*!
\fn void QextSerialBase::construct()
Common constructor function for setting up default port settings.
(115200 Baud, 8N1, Hardware flow control where supported, otherwise no flow control, and 0 ms timeout).
*/
void QextUsbBase::construct()
{
    Settings.mVID = 0;
    Settings.mPID = 0;
    Settings.Timeout_Millisec=500;
	mutex = new QMutex( QMutex::Recursive );
	setOpenMode(QIODevice::NotOpen);
}

void QextUsbBase::setQueryMode(QueryMode mechanism)
{
	_queryMode = mechanism;
}

/*!
\fn void QextSerialBase::setPortName(const QString & name)
Sets the name of the device associated with the object, e.g. "COM1", or "/dev/ttyS0".
*/
void QextUsbBase::setVID(const int & vid)
{
    m_VID = vid;
}

int QextUsbBase::getVID()
{
    return m_VID;
}
void QextUsbBase::setPID(const int & pid)
{
    m_PID = pid;
}

int QextUsbBase::getPID()
{
    return m_PID;
}

/*!
\fn bool QextSerialBase::isSequential() const
Returns true if device is sequential, otherwise returns false. Serial port is sequential device
so this function always returns true. Check QIODevice::isSequential() documentation for more 
information.
*/
bool QextUsbBase::isSequential() const
{
	return true;
}

/*!
\fn bool QextSerialBase::atEnd() const
This function will return true if the input buffer is empty (or on error), and false otherwise.
Call QextSerialBase::lastError() for error information.
*/
bool QextUsbBase::atEnd() const
{
    if (size()) {
        return true;
    }
    return false;
}

/*!
\fn qint64 QextSerialBase::readLine(char * data, qint64 maxSize)
This function will read a line of buffered input from the port, stopping when either maxSize bytes
have been read, the port has no more data available, or a newline is encountered.
The value returned is the length of the string that was read.
*/
qint64 QextUsbBase::readLine(char * data, qint64 maxSize)
{
    qint64 numBytes = bytesAvailable();
    char* pData = data;

	if (maxSize < 2)	//maxSize must be larger than 1
		return -1;

    /*read a byte at a time for MIN(bytesAvail, maxSize - 1) iterations, or until a newline*/
    while (pData<(data+numBytes) && --maxSize) {
        readData(pData, 1);
        if (*pData++ == '\n') {
            break;
        }
    }
    *pData='\0';

    /*return size of data read*/
    return (pData-data);
}

/*!
\fn ulong QextSerialBase::lastError() const
Returns the code for the last error encountered by the port, or E_NO_ERROR if the last port
operation was successful.  Possible error codes are:

\verbatim
Error                           Explanation
---------------------------     -------------------------------------------------------------
E_NO_ERROR                      No Error has occured
E_INVALID_FD                    Invalid file descriptor (port was not opened correctly)
E_NO_MEMORY                     Unable to allocate memory tables (POSIX)
E_CAUGHT_NON_BLOCKED_SIGNAL     Caught a non-blocked signal (POSIX)
E_PORT_TIMEOUT                  Operation timed out (POSIX)
E_INVALID_DEVICE                The file opened by the port is not a character device (POSIX)
E_BREAK_CONDITION               The port detected a break condition
E_FRAMING_ERROR                 The port detected a framing error
                                (usually caused by incorrect baud rate settings)
E_IO_ERROR                      There was an I/O error while communicating with the port
E_BUFFER_OVERRUN                Character buffer overrun
E_RECEIVE_OVERFLOW              Receive buffer overflow
E_RECEIVE_PARITY_ERROR          The port detected a parity error in the received data
E_TRANSMIT_OVERFLOW             Transmit buffer overflow
E_READ_FAILED                   General read operation failure
E_WRITE_FAILED                  General write operation failure
\endverbatim
*/
ulong QextUsbBase::lastError() const
{
    return lastErr;
}
