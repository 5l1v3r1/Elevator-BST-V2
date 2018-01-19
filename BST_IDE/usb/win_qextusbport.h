#ifndef _WIN_QEXTUSBPORT_H_
#define _WIN_QEXTUSBPORT_H_

#include "qextusbbase.h"

class QReadWriteLock;
class QextUsbThread;

#define TX_SIZE (4096*4)

class QextUsbPort: public QextUsbBase
{
    Q_OBJECT
friend class QextUsbThread;

private:
    /*!
     * This method is a part of constructor.
     */
    void init();

public:
    QextUsbPort(GUID pGuid, QextUsbBase::QueryMode mode = QextUsbBase::Polling);

    ~QextUsbPort();
    bool open(OpenMode mode);
    void close();
    void flush();
    qint64 size() const;
    void ungetChar(char c);

    bool isConnected();

    void monitorUsbEvent();

    void setTimeout(long);

    DWORD QUsbRead(PVOID data, DWORD maxSize, int timeout);
    DWORD QUsbWrite(PCVOID data, DWORD maxSize, int timeout);

    qint64 bytesAvailable();

    qint64 bytesToWrite() const;

    qint64 readData(char *data, qint64 maxSize);

    qint64 writeData(const char *data, qint64 maxSize);

    void translateError(ulong error) {return ;}

    ulong lineStatus() {return 0;}

    bool waitForReadyRead(int msecs);	///< @todo implement.

protected:
    HANDLE m_Handle; /* the device handle */

    HANDLE threadStartEvent;
    OVERLAPPED overlap;
    OVERLAPPED overlapWrite;
    COMMCONFIG Win_CommConfig;
    COMMTIMEOUTS Win_CommTimeouts;

    char *m_InPipe;
    char *m_OutPipe;

    QReadWriteLock * bytesToWriteLock;	///< @todo maybe move to QextSerialBase.
    qint64 _bytesToWrite;		///< @todo maybe move to QextSerialBase (and implement in POSIX).
    QextUsbThread * overlapThread; ///< @todo maybe move to QextSerialBase (and implement in POSIX).

    HANDLE hWrite;
    HANDLE hRead;

    quint32     m_Timeout_Millisec;
};

/*!
 * This thread monitors communication events.
 */
class QextUsbThread: public QThread
{
    QextUsbPort * qesp;
    bool terminate;

public:
    /*!
     * Constructor.
     *
     * \param qesp valid serial port object.
     */
    QextUsbThread(QextUsbPort * qesp);

    /*!
     * Stop the thread.
     */
    void stop();

protected:
    //overriden
    virtual void run();
};

#endif
