#ifndef NODEUDT_H
#define NODEUDT_H

#include "globalfun.h"

#ifdef WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#include "udt/Windows/include/WSPiApi.h"
#include "udt/Windows/include/udt.h"
#include "udt/Windows/include/cc.h"
#endif
#ifdef UBUNTU
#include "udt/Ubuntu/include/cc.h"
#include "udt/Ubuntu/include/udt.h"
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#endif
#ifdef LINUX
#include "udt/Linux/include/cc.h"
#include "udt/Linux/include/udt.h"
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#endif

#include <iostream>

class UdtServerMonitorThread;
class UdtClientMonitorThread;

class NodeUdtClient : public QIODevice
{
    Q_OBJECT
public:
    NodeUdtClient(QObject *parent = 0);
    ~NodeUdtClient();
    void Close()
    {
        StopMonitor();
        delete [] m_ByteBuffer;
        if(m_Client)
        {
            UDT::close(m_Client);
            m_Client = 0;
        }
    }

    bool OpenClient(QHostAddress pIP, quint16 pPort, quint32 pTimeout);

    bool isOpen() const
    {
        if(m_Client != 0)
            return true;
        return false;
    }

    inline qint64 write(const QByteArray &data)
    {
        if(m_Client == 0)
            return 0;
        int handle = 0;
        int ret = UDT::send(m_Client, data.data(), data.size(), 0, &handle);
        if (UDT::ERROR == ret)
        {
           cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
           return 0;
        }
        return ret;
    }

    qint64 readData(char *data, qint64 maxlen) {return -1;}
    qint64 writeData(const char *data, qint64 len) {return -1;}

    QByteArray readAll()
    {
        return QByteArray(m_ByteBuffer, m_RecvCount);
    }



    void Monitor();
    void StartMonitor();
    void StopMonitor();

signals:
    void sConnState(bool pState);

public slots:
    void slot_PHDisconn();

public:
    UDTSOCKET               m_Client;
    char*                   m_ByteBuffer;
    int                     m_RecvCount;
    UdtClientMonitorThread* m_MonitorThread;
};

class UdtClientMonitorThread : public QThread
{
    Q_OBJECT
public:
    UdtClientMonitorThread(NodeUdtClient *qesp)
        :QThread()
    {
        mQesp = qesp;
        mRun = true;
    }
    ~UdtClientMonitorThread()
    {
        Stop();
    }
    void Stop()
    {
        mRun = false;
    }
protected:
    virtual void run()
    {
        while (mRun)
        {
            mQesp->Monitor();
            msleep(1);
        }
        mRun = true;
    }
public:
    NodeUdtClient*     mQesp;
    bool                mRun;
};

class NodeUdtServer : public QIODevice
{
    Q_OBJECT
public:
    NodeUdtServer(QObject *parent = 0);
    ~NodeUdtServer();

    void Close();
    bool OpenServer(QHostAddress pIP, quint16 pPort);

    bool isOpen() const
    {
        if(m_Server != 0)
            return true;
        return false;
    }

    inline qint64 write(const QByteArray &data)
    {
        if(m_Client)
            return m_Client->write(data);
        return 0;
    }

    QByteArray readAll()
    {
        if(m_Client)
            return (m_Client->readAll());
        return QByteArray();
    }

    void Monitor();
    void StartMonitor();
    void StopMonitor();

signals:
    void sConnState(bool pState);

public:
    UDTSOCKET       m_Server;
    NodeUdtClient  *m_Client;
    UdtServerMonitorThread* m_MonitorThread;
};

class UdtServerMonitorThread : public QThread
{
    Q_OBJECT
public:
    UdtServerMonitorThread(NodeUdtServer *qesp)
        :QThread()
    {
        mQesp = qesp;
        mRun = true;
    }
    ~UdtServerMonitorThread()
    {
        Stop();
    }
    void Stop()
    {
        mRun = false;
    }
protected:
    virtual void run()
    {
        while (mRun)
        {
            mQesp->Monitor();
            msleep(300);
        }
        mRun = true;
    }
public:
    NodeUdtServer*      mQesp;
    bool                mRun;
};

#endif // NODEUDT_H
