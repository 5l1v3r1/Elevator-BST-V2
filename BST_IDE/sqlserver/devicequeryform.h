#ifndef DEVICEQUERYFORM_H
#define DEVICEQUERYFORM_H

#include "sqlquery.h"
#include <QTreeWidget>

namespace Ui {
class DeviceQueryForm;
}

class DeviceQueryForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit DeviceQueryForm(QWidget *parent = 0);
    ~DeviceQueryForm();
private slots:
    void on_ServerConnectBtn_clicked();
    void on_BarcodeEdit_editingFinished();
    void on_BtnBarcodeSearch_clicked();
    void SqlConnected();
    void SqlDisconnected();
    void InitDeviceInfo(QString pBarCode);
    void on_DeviceTableCombox_currentIndexChanged(int index);
signals:
    void sConnstate(bool pSuccess, QString pIP);
public:
    Ui::DeviceQueryForm *ui;
    SqlQuery            *m_SqlQuery;
};

#endif // DEVICEQUERYFORM_H
