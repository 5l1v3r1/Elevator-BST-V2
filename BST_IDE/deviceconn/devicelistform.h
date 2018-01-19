#ifndef DEVICELISTFORM_H
#define DEVICELISTFORM_H

#include "logicnetmanager.h"
#include "logicnetdiscovery.h"
#include "devicemessagedlg.h"

#include <QWidget>
#include <QMultiHash>
#include <QThread>
#include <QReadWriteLock>

namespace Ui {
class devicelistform;
}

/*
    扫描到的节点均为offline状态，
*/
class devicelistform : public QWidget
{
    Q_OBJECT
public:
    explicit devicelistform(QWidget *parent = 0);
    ~devicelistform();

    void ReleaseAct();
    void InitAct();

    void ReleaseTreeWidget();
    void InitTreeWidget();

    void ReleaseNetCmdline();
    void InitNetCmdline();

    void EnumerateNet();
    void EnumerateUSB();
    void EnumerateCOM();

    QStringList GetConnectedNets();
    NetInfo* GetCurNetInfo();
    NetInfo* GetNetInfo(QString pIP);
    NetInfo* AddNetInfo(QString pIP);
    QTreeWidgetItem* AddNetTreeItem(NetInfo *pNetInfo);
    QTreeWidgetItem* GetTreeItem(NetInfo *pNetInfo);
    void SetTreeItemBusState(QTreeWidgetItem *pItem, BusState pState);
    BusState GetTreeItemBusState(QTreeWidgetItem *pItem);

signals:
    void sConnstate(NetInfo *pNet);

public slots:
    void slot_NetConn();
    void slot_NetDisconn();

    //>@用于轮询网络、U盘以及串口设备
    void slot_Scan();
    void slot_ConnStateChanged();

    void slot_ModifyTempPath();
    void slot_Broadcast();
    void slot_SyncTime(){}
    void slot_Message();
    void Slot_ItemClicked(QTreeWidgetItem* pItem, int pColumn);
    void slot_RefreshTreelist();
    void on_BtnHideCmdRecv_clicked(bool checked);
    void on_BtnAdd_clicked();

public:
    //>@缓存路径
    QString             mTempPath;

    //>@列表项
    QTreeWidgetItem     *mNetitem,
                        *mUseritem,
                        *mUdiskitem,
                        *mComitem;
    //>@设置
    QAction             *TempDirAct,   //>@修改临时目录
                        *BroadcastAct, //>@广播
                        *MessageAct,   //>@发送消息
                        //>@
                        *NetconnAct,   //>@建立NFD连接
                        *NetDisconnAct;

public:
    //>@设备表
    QHash<QString, NetInfo*>    mNets;
    QHash<QString, UdiskInfo*>  mUdisks;
    QHash<QString, ComInfo*>    mComs;

private:
    Ui::devicelistform *ui;
};

HostNetManager* GetNetManager();

#endif // DEVICELISTFORM_H
