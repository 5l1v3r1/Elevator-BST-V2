#ifndef MANUFACTUREPAGE_H
#define MANUFACTUREPAGE_H

#include "global.h"
#include <QFileDialog>
#include "sqlquery.h"
#include <QTreeWidget>

namespace Ui {
class ManufacturePage;
}

class ManufacturePage : public QDialog
{
    Q_OBJECT
public:
    explicit ManufacturePage(QWidget *parent = 0);
    ~ManufacturePage();

    void DelTreeAllItem(QTreeWidgetItem *parentItem);
    QTreeWidgetItem *DelTreeItem(QTreeWidgetItem *pItem);
    QTreeWidgetItem *AddTreeItem(QTreeWidgetItem *parentItem, QStringList contentList, bool Editable);

    QTreeWidgetItem     *m_CurBootloaderItem;
    QTreeWidgetItem     *m_CurKernelItem;
    QTreeWidgetItem     *m_CurFileSystemItem;
    QTreeWidgetItem     *m_CurApplicationItem;
    QStringList          DeviceTableList;

private slots:
    void on_ServerConnectBtn_clicked();
    void on_DeviceConnectBtn_clicked();

    void on_BarcodeEdit_editingFinished();
    void on_BtnBarcodeSearch_clicked();
    void on_BtnBufferPathSelect_clicked();

    void on_BtnDownloadStart_clicked();
    void on_BtnDownloadPause_clicked();
    void on_BtnDownloadStop_clicked();

    void SqlConnected();
    void SqlDisconnected();

    void InitBootloaderList();
    void InitKernelList();
    void InitFileSystemList();
    void InitApplicationList();

    void InitDeviceInfo(QString pBarCode);
    void on_DeviceTableCombox_currentIndexChanged(int index);

    void on_BtnBootloaderAdd_clicked();
    void on_BtnBootloaderDel_clicked();
    void on_BtnBootloaderRefresh_clicked();
    void on_BootloaderList_itemSelectionChanged();

    void on_BtnKernelAdd_clicked();
    void on_BtnKernelDel_clicked();
    void on_BtnKernelRefresh_clicked();
    void on_KernelList_itemSelectionChanged();

    void on_BtnFsAdd_clicked();
    void on_BtnFsDel_clicked();
    void on_BtnFsRefresh_clicked();
    void on_FileSystemList_itemSelectionChanged();

    void on_BtnAppAdd_clicked();
    void on_BtnAppDel_clicked();
    void on_BtnAppRefresh_clicked();
    void on_ApplicationList_itemSelectionChanged();

    void slot_SQLConnectState(CONN_STATE pState);
    void slot_TransFileProgress(quint8 pVaule);

private:
    Ui::ManufacturePage *ui;
    SqlQuery    *m_SqlQuery;

    bool        SFConnectEstablished;
    bool        m_FileTransferFinished;
};

#endif // MANUFACTUREPAGE_H
