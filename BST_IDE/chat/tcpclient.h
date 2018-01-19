#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QDialog>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QTime>
namespace Ui {
    class TcpClient;
}

class TcpClient : public QDialog
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = 0);
    ~TcpClient();

    int ConnectStaus;

    void InitVariable();
    void InitClient();
    void InitUi();
    void setHostAddress(QHostAddress address);
    void setFileName(QString fileName){localFile = new QFile(fileName);}

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TcpClient *ui;
    QTcpSocket *tcpClient;
    quint16 blockSize;
    QHostAddress hostAddress;
    qint16 tcpPort;

    qint64 TotalBytes;
    qint64 bytesReceived;
    qint64 bytesToReceive;
    qint64 bytesToWrite;
    qint64 bytesWritten;
    qint64 loadSize;
    qint64 fileNameSize;
    QString fileName;
    QFile *localFile;
    QByteArray inBlock;
    QByteArray outBlock;

    QTime time;

private slots:
    void on_tcpClientCancleBtn_clicked();
    void on_tcpClientCloseBtn_clicked();
    void newConnect();
    void slot_readMessage();
    void slot_sendMessage();
    void slot_connect_Success();
    void slot_updateClientProgress(qint64 numBytes);
    void displayError(QAbstractSocket::SocketError);
    void on_tcpClientSendBtn_clicked();
    void on_tcpClientOpenBtn_clicked();

signals:
    void sendFileName();
};

#endif // TCPCLIENT_H
