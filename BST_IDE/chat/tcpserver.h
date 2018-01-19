#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QDialog>
#include <QTcpServer>
#include <QFile>
#include <QTime>

namespace Ui {
    class TcpServer;
}

class TcpServer : public QDialog
{
    Q_OBJECT
public:
    explicit TcpServer(QWidget *parent = 0);
    ~TcpServer();

    void refused();
    void InitServer();
    void InitVariable();
    void InitUi();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TcpServer *ui;
    qint16 tcpPort;
    QTcpServer *tcpServer;
    QString fileName;
    QString theFileName;
    QFile *localFile;

    qint64 TotalBytes;
    qint64 bytesWritten;
    qint64 bytesToWrite;
    qint64 bytesReceived;
    qint64 fileNameSize;
    qint64 loadSize;
    QByteArray outBlock;//缓存一次发送的数据
    QByteArray inBlock;

    QTcpSocket *clientConnection;

    QTime time;//计时器

private slots:
    void slot_sendMessage();
    void slot_updateClientProgress(qint64 numBytes);
    void slot_GetNextPendingConnect();
    void slot_ReceiveProgress();
    void slot_displayError(QAbstractSocket::SocketError socketError);

    void on_serverSendBtn_clicked();

    void on_serverCloseBtn_clicked();

    void on_serverOpenBtn_clicked();   
signals:
    void sendFileName();

};

#endif // TCPSERVER_H
