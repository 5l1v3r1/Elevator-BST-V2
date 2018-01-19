#ifndef DEVICEMESSAGEDLG_H
#define DEVICEMESSAGEDLG_H

#include <QDialog>

namespace Ui {
class DeviceMessageDlg;
}

class DeviceMessageDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit DeviceMessageDlg(QWidget *parent = 0);
    ~DeviceMessageDlg();
    
    void SetMemberOB(QString pStr);
    QString GetMessage();

    void accept();
public:
    QString         mMessage;
private:
    Ui::DeviceMessageDlg *ui;
};

#endif // DEVICEMESSAGEDLG_H
