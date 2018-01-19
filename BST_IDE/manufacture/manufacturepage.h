#ifndef MANUFACTUREPAGE_H
#define MANUFACTUREPAGE_H

#include "tabpage.h"
#include <QFileDialog>
#include "usbport.h"

namespace Ui {
class ManufacturePage;
}

class ManufacturePage : public TabPage
{
    Q_OBJECT
public:
    explicit ManufacturePage(QWidget *parent = 0);
    ~ManufacturePage();

    void DeviceConnected();
    void DeviceDisconnected();

    void DownLoadRun();
    void DownLoadPause();
    void DownLoadStop();

    void InitDFU();
    void ReleaseDFU();

    void InitDataBaseList();
    void ReleaseDataBaseList();

    void OnSending();
    void OffSending();

public slots:
    void on_BtnBufferPathSelect_clicked();
    void on_BtnStart_clicked();
    void on_BtnPause_clicked();
    void on_BtnStop_clicked();

    void on_BtnScan_clicked(bool checked);
    void slot_BootModeChanged(int pIndex);
    void on_BtnSendCmd_clicked();

    void slot_SQLConnState(bool pSuccess, QString pIP);
    void slot_ConnState(bool pState);
    void slot_TransProgress(QString pMsg,int pValue);

private:
    Ui::ManufacturePage *ui;
    QTimer               m_ScanTimer;

    UsbDFU              *m_UsbDfu;
    bool                 m_DfuStopFlag;

    QButtonGroup        *m_RadioGroup;
    int                  m_CurRadioIndex;
};

#endif // MANUFACTUREPAGE_H
