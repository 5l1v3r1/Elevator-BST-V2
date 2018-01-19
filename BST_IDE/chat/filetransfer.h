#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QWidget>
#include "tcpclient.h"
#include "tcpserver.h"
#include <QLayout>
#include <QDebug>

namespace Ui {
class FileTransfer;
}

class FileTransfer : public QWidget
{
    Q_OBJECT
    
public:
    explicit FileTransfer(QWidget *parent = 0);
    ~FileTransfer();

    TcpServer *Server;
    TcpClient *Client;
    QString Ip;
    void VariableInit();
    void UiInit();
    QString GetServerIp();
protected:
    int m_nTimerId;
    void timerEvent(QTimerEvent *event);
private slots:
    void on_btn_connect_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::FileTransfer *ui;
};

#endif // FILETRANSFER_H
