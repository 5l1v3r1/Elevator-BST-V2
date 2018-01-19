#ifndef TOOLTCP_H
#define TOOLTCP_H

#include "globalfun.h"
#include <QNetworkInterface>
#include <QTcpServer>
#include <QTcpSocket>

class ThreadTcpSocket;

typedef QTcpSocket* TcpSocketPtr;
Q_DECLARE_METATYPE(TcpSocketPtr)
inline uint qHash(TcpSocketPtr pPtr) {  return (uint)pPtr; }

class ToolTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    ToolTcpServer(QObject *parent = 0);
    ~ToolTcpServer();

    bool OpenServer(QHostAddress pIP, quint16 pPort);
    void CloseServer();
    uint GetClientType(QTcpSocket *pSocket);
    void incomingConnection(int handle);

    qint64 write(uint pType, QByteArray data);

signals:
    void sNewSocket(QTcpSocket *pSocket, uint pType);
    void sRemoveSocket(QTcpSocket *pSocke);

public slots:
    void slot_RemoveClient();
    void slot_GetClientType();

public:
    //>@客户机句柄 uint表示具体应用
    QHash<TcpSocketPtr, uint> m_ClientSockets;        //>@当前连接的客户机，一种应用只允许一个客户机
};

class ToolTcpClient : public QTcpSocket
{
    Q_OBJECT
public:
    ToolTcpClient(uint pClientType = 0, QObject *parent = 0);
    ~ToolTcpClient();

    bool OpenClient(QHostAddress pIP, quint16 pPort, quint32 pTimeout);
    void OpenClient(QHostAddress pIP, quint16 pPort);
    void CloseClient();

public slots:
    void slot_Conn();
    void slot_Disconn();

signals:
    void sConnState(bool pState);

public:
    uint            mClientType;        //>@标示此客户机的用途，帧格式(6个字节):0x5a 0xa5 uint
};

#endif // NODETCP_H
