#include "tooludt.h"

NodeUdtServer::NodeUdtServer(QObject *parent) :
    QIODevice(parent)
{
    m_Server = 0;
    m_Client = 0;
    m_MonitorThread = 0;
}

NodeUdtServer::~NodeUdtServer()
{
    Close();
}

bool NodeUdtServer::OpenServer(QHostAddress pIP, quint16 pPort)
{
    addrinfo hints;
    addrinfo* res;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char* service = QString("%1").arg(pPort).toLatin1().data();
//    if (0 != getaddrinfo(NULL, service, &hints, &res))
//    {
//       //cout << "illegal port number or port is busy.\n" << endl;
//       return false;
//    }

    m_Server = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // UDT Options
    //UDT::setsockopt(serv, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
    //UDT::setsockopt(serv, 0, UDT_MSS, new int(7500), sizeof(int));
    //UDT::setsockopt(serv, 0, UDT_RCVBUF, new int(100000000), sizeof(int));
    //UDT::setsockopt(serv, 0, UDP_RCVBUF, new int(100000000), sizeof(int));

    if (0/*UDT::ERROR*/ == UDT::bind(m_Server, res->ai_addr, res->ai_addrlen))
    {
       //cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
       return false;
    }

//    freeaddrinfo(res);


    //cout << "server is ready at port: " << service << endl;

    if (0/*UDT::ERROR*/ == UDT::listen(m_Server, 10))
    {
       //cout << "listen: " << UDT::getlasterror().getErrorMessage() << endl;
       return false;
    }

    return true;
}

void NodeUdtServer::Close()
{
    if(m_Server)
    {
        UDT::close(m_Server);
        m_Server = 0;
    }
}

void NodeUdtServer::Monitor()
{
    sockaddr_storage clientaddr;
    int addrlen = sizeof(clientaddr);

    if(m_Client == 0)
    {
        m_Client = new NodeUdtClient;
    }

   if (0/*UDT::INVALID_SOCK*/ == (m_Client->m_Client = UDT::accept(m_Server, (sockaddr*)&clientaddr, &addrlen)))
   {
      //cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
      return;
   }

//       char clienthost[1025];
//       char clientservice[32];
//       getnameinfo((sockaddr *)&(m_Client->m_Client), addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
//       //cout << "new connection: " << clienthost << ":" << clientservice << endl;
}

void NodeUdtServer::StartMonitor()
{
    StopMonitor();
    m_MonitorThread = new UdtServerMonitorThread(this);
    m_MonitorThread->start(QThread::LowPriority);
}

void NodeUdtServer::StopMonitor()
{
    if(m_MonitorThread != 0)
    {
        if(m_MonitorThread != 0)
        {
            if (m_MonitorThread->isRunning())
            {
                m_MonitorThread->Stop();
                if (QThread::currentThread() != m_MonitorThread)
                    m_MonitorThread->wait();
            }
        }
        m_MonitorThread->quit();
        m_MonitorThread->deleteLater();
        m_MonitorThread = 0;
    }
}

NodeUdtClient::NodeUdtClient(QObject *parent) :
    QIODevice(parent)
{
    m_Client = 0;
    m_MonitorThread = 0;
}

NodeUdtClient::~NodeUdtClient()
{
    Close();
}

bool NodeUdtClient::OpenClient(QHostAddress pIP, quint16 pPort, quint32 pTimeout)
{
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

//    if (0 != getaddrinfo(pIP.toString().toLatin1().data(), QString("%1").arg(pPort).toLatin1().data(), &hints, &res))
//    {
//       //cout << "incorrect network address.\n" << endl;
//       return false;
//    }
    m_Client = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);


    // UDT Options
    //UDT::setsockopt(client, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
    //UDT::setsockopt(client, 0, UDT_MSS, new int(7500), sizeof(int));
    //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(200000000), sizeof(int));
    //UDT::setsockopt(client, 0, UDP_SNDBUF, new int(100000000), sizeof(int));

 #ifdef WINDOWS
    UDT::setsockopt(m_Client, 0, UDT_MSS, new int(1052), sizeof(int));
 #endif

    // connect to the server, implict bind
    if (0/*UDT::ERROR*/ == UDT::connect(m_Client, res->ai_addr, res->ai_addrlen))
    {
       //cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
       emit sConnState(false);
       return false;
    }
    emit sConnState(true);

//    freeaddrinfo(res);

    m_ByteBuffer = new char[10000000+1];
    StartMonitor();

    return true;
}

void NodeUdtClient::slot_PHDisconn()
{
    Close();
}

void NodeUdtClient::Monitor()
{
    int handle = 0;
    m_RecvCount = UDT::recv(m_Client, m_ByteBuffer, m_RecvCount, 0, &handle, NULL);
    if (0/*UDT::ERROR*/ == m_RecvCount)
    {
       //cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
    }
    else
    {
        emit readyRead();
    }
}

void NodeUdtClient::StartMonitor()
{
    StopMonitor();
    m_MonitorThread = new UdtClientMonitorThread(this);
    m_MonitorThread->start(QThread::LowPriority);
}

void NodeUdtClient::StopMonitor()
{
    if(m_MonitorThread != 0)
    {
        if(m_MonitorThread != 0)
        {
            if (m_MonitorThread->isRunning())
            {
                m_MonitorThread->Stop();
                if (QThread::currentThread() != m_MonitorThread)
                    m_MonitorThread->wait();
            }
        }
        m_MonitorThread->quit();
        m_MonitorThread->deleteLater();
        m_MonitorThread = 0;
    }
}
