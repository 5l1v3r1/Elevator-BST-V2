#ifndef CONNECTDEVICE_H
#define CONNECTDEVICE_H

#include <QDialog>
#include "global.h"
#include "physicalinc.h"
#include "logicinc.h"

namespace Ui {
class Connect;
}

class ConnectDevice : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectDevice(NFDHostApp *pNFD, NodeTcpclient *pTcpclient, QWidget *parent = 0);
    ~ConnectDevice();
    void accept();
    void reject();
public:
    Ui::Connect *ui;

    NFDHostApp*         m_NFDHostApp;
    NodeTcpclient*      mTcpclient;
};

#endif // CONNECTDEVICE_H
