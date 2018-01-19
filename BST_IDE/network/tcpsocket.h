#ifndef NODETCP_H
#define NODETCP_H

#include "define.h"
#include <QNetworkInterface>
#include <QTcpServer>
#include <QTcpSocket>

typedef struct{
    QTcpSocket* mSrcTcp;
    QTcpSocket* mDstTcp;
} RouterInfo;

//>@维护一个路由表，key为IP源地址
class TcpServer : public QObject
{
    Q_OBJECT
public:
    TcpServer(QObject *parent = 0);
    ~TcpServer();

    bool OpenServer(QHostAddress pIP, quint16 pPort);
    void CloseServer();

signals:
    void sConnState(bool pState);
    void sNewDataCome(QTcpSocket* pSocket);
public slots:
    void slot_PHConn();
    void slot_PHDisconn(QObject* pObject);
    void slot_ReadAll(QObject* pObject);

public:
    QSignalMapper                      *m_DisConnMapper, *m_ReadReadyMapper;
    QTcpServer                         *mTcpserver;
    QMultiHash<QString, QTcpSocket*>    m_SocketList;
    QHash<QString, RouterInfo*>         m_RouterMap;
};


#endif // NODETCP_H
