#include "devicelistform.h"
#include "ui_devicelistform.h"
#include "global.h"
#include "mainwindowclass.h"

devicelistform::devicelistform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::devicelistform)
{
    ui->setupUi(this);
    ui->CommandRecvTextEdit->setHidden(true);

    mNets.clear();
    mUdisks.clear();
    mComs.clear();

    TempDirAct = 0;
    BroadcastAct = 0;
    NetconnAct = 0;
    NetDisconnAct = 0;
    MessageAct = 0;

    mNetitem = 0;
    mUdiskitem = 0;
    mComitem = 0;

    InitAct();
    InitTreeWidget();
    InitNetCmdline();

}

devicelistform::~devicelistform()
{
    mScanTimer.stop();
    ReleaseNetCmdline();
    ReleaseTreeWidget();
    ReleaseAct();
    delete ui;
}

QStringList devicelistform::GetConnectedNets()
{
    QStringList tmpIPList;
    QStringList netKeyList = mNets.keys();
    for(int i=0; i<netKeyList.count(); i++)
    {
        NetInfo *tmpNet = mNets.value(netKeyList.at(i));
        if(!tmpNet)
            continue;
        if(tmpNet->mConn = Connected)
            tmpIPList.append(tmpNet->mIP);
    }
    return tmpIPList;
}

void devicelistform::ReleaseAct()
{
    if(TempDirAct != 0)
    {
        disconnect(TempDirAct, SIGNAL(triggered()), 0, 0);
        TempDirAct->deleteLater();
        TempDirAct = 0;
    }
    if(BroadcastAct != 0)
    {
        disconnect(BroadcastAct, SIGNAL(triggered()), 0, 0);
        BroadcastAct->deleteLater();
        BroadcastAct = 0;
    }
    if(NetconnAct != 0)
    {
        disconnect(NetconnAct, SIGNAL(triggered()), 0, 0);
        NetconnAct->deleteLater();
        NetconnAct = 0;
    }
    if(NetDisconnAct != 0)
    {
        disconnect(NetDisconnAct, SIGNAL(triggered()), 0, 0);
        NetDisconnAct->deleteLater();
        NetDisconnAct = 0;
    }
    if(MessageAct != 0)
    {
        disconnect(MessageAct, SIGNAL(triggered()), 0, 0);
        MessageAct->deleteLater();
        MessageAct = 0;
    }
}

void devicelistform::InitAct()
{
    ReleaseAct();

    NetconnAct = new QAction(QIcon(":/Menu/rc/Menu/deviceonline.png"), tr("&Connect"), this);
    NetconnAct->setStatusTip(tr("Connect to Device"));
    connect(NetconnAct, SIGNAL(triggered()), this, SLOT(slot_NetConn()));

    NetDisconnAct = new QAction(QIcon(":/Menu/rc/Menu/deviceoffline.png"), tr("&Disconnect"), this);
    NetDisconnAct->setStatusTip(tr("Disconnect to Device"));
    connect(NetDisconnAct, SIGNAL(triggered()), this, SLOT(slot_NetDisconn()));

    TempDirAct = new QAction(QIcon(":/Menu/rc/Menu/save.png"), tr("&SavePath"), this);
    TempDirAct->setStatusTip(tr("Set Default TempDir"));
    connect(TempDirAct, SIGNAL(triggered()), this, SLOT(slot_ModifyTempPath()));

    BroadcastAct = new QAction(QIcon(":/Menu/rc/Menu/chat.png"), tr("&Broadcast"), this);
    BroadcastAct->setStatusTip(tr("Send a Broadcast"));
    connect(BroadcastAct, SIGNAL(triggered()), this, SLOT(slot_Broadcast()));

    MessageAct = new QAction(QIcon(":/Menu/rc/Menu/chat.png"), tr("&Message"), this);
    MessageAct->setStatusTip(tr("Sebd a Message"));
    connect(MessageAct, SIGNAL(triggered()), this, SLOT(slot_Message()));
}

void devicelistform::ReleaseTreeWidget()
{
    disconnect(ui->treeWidget, SIGNAL(itemPressed(QTreeWidgetItem*, int)), 0, 0);
    if(mNetitem != 0)
    {
        delete mNetitem;
        mNetitem = 0;
    }
    if(mUdiskitem != 0)
    {
        delete mUdiskitem;
        mUdiskitem = 0;
    }
    if(mComitem != 0)
    {
        delete mComitem;
        mComitem = 0;
    }
}

void devicelistform::InitTreeWidget()
{
    ReleaseTreeWidget();
    ui->treeWidget->setRootIsDecorated(true);//折叠
    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);//多选
    ui->treeWidget->setAutoFillBackground(true);
    ui->treeWidget->setStyleSheet("QTreeWidget::item{height:18px}");  //格式
    ui->treeWidget->setWindowTitle(tr("Device List"));
    //    ui->treeWidget->setSortingEnabled(true);//排序
    //    ui->treeWidget->setDragEnabled(true);
    ui->treeWidget->header()->setSortIndicatorShown(true);
    ui->treeWidget->header()->setSortIndicator(0, Qt::AscendingOrder);
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setColumnWidth(0, 80);
    ui->treeWidget->setColumnWidth(1, 170);
    QList<QString>HeaderLabelList;
    HeaderLabelList.clear();
    HeaderLabelList<<tr("Type")<<tr("Address");
    ui->treeWidget->setHeaderLabels(HeaderLabelList);

    mNetitem = new QTreeWidgetItem;
    mNetitem->setBackgroundColor(0, QColor(254,235,208));
    mNetitem->setBackgroundColor(1, QColor(254,235,208));
    mNetitem->setIcon(0,QIcon(":/Device/rc/Device/net.png"));
    mNetitem->setFont(1, QFont("Arial", 9, QFont::Bold));
    mNetitem->setText(1, tr("Network"));
    mNetitem->setText(2, mTempPath);
    ui->treeWidget->addTopLevelItem(mNetitem);

    mTempPath = QString("C:/NFDBuffer/");
    mNetitem->setToolTip(1, mTempPath);

    mUdiskitem = new QTreeWidgetItem;
    mUdiskitem->setBackgroundColor(0, QColor(255,250,179));
    mUdiskitem->setBackgroundColor(1, QColor(255,250,179));
    mUdiskitem->setIcon(0,QIcon(":/Device/rc/Device/udisk.png"));
    mUdiskitem->setFont(1, QFont("Arial", 9, QFont::Bold));
    mUdiskitem->setText(1, tr("USB"));
    ui->treeWidget->addTopLevelItem(mUdiskitem);

    mComitem = new QTreeWidgetItem;
    mComitem->setBackgroundColor(0, QColor(254,235,208));
    mComitem->setBackgroundColor(1, QColor(254,235,208));
    mComitem->setIcon(0,QIcon(":/Device/rc/Device/rs232.png"));
    mComitem->setFont(1, QFont("Arial", 9, QFont::Bold));
    mComitem->setText(1, tr("COM"));
    ui->treeWidget->addTopLevelItem(mComitem);

    connect(ui->treeWidget,SIGNAL(itemPressed(QTreeWidgetItem*, int)),
            this,SLOT(Slot_ItemClicked(QTreeWidgetItem*, int)));
    slot_RefreshTreelist();
}

void devicelistform::Slot_ItemClicked(QTreeWidgetItem* pItem, int pColumn)
{
    if(QApplication::mouseButtons() == Qt::LeftButton)
    {
        for(int i=0; i<mNetitem->childCount(); i++)
        {
            if(pItem == mNetitem->child(i))
            {
                ui->DevAddress->setText(pItem->text(1));
                return;
            }
        }
        return;
    }
    if (QApplication::mouseButtons() == Qt::RightButton)
    {
        if((pItem == mNetitem))
        {
            QMenu menu(tr("&Menu"),ui->treeWidget);
            menu.addAction(TempDirAct);
            menu.addAction(BroadcastAct);
            menu.exec(QCursor::pos());
            return;
        }
        else if((pItem == mUdiskitem))
        {
            return;
        }
        else if((pItem == mComitem))
        {
            return;
        }
        else
        {
            for(int i=0; i<mNetitem->childCount(); i++)
            {
                if(pItem = mNetitem->child(i))
                {
                    QMenu menu(tr("&Menu"),ui->treeWidget);
                    menu.addAction(NetconnAct);
                    menu.addAction(NetDisconnAct);
                    menu.addAction(MessageAct);
                    menu.exec(QCursor::pos());
                    return;
                }
            }
        }
    }
}

void devicelistform::slot_Broadcast()
{

}

void devicelistform::slot_Message()
{
    TabPageManager *tmpTabManager = VAR_CAST<TabPageManager *>(GlobalPara.m_TabPageManager);
    if(tmpTabManager)
    {
        NetInfo *tmpNet = GetCurNetInfo();
        if(!tmpNet)
            return;
        tmpTabManager->AddTabPage(tmpNet->mIP, TYPE_CHAT, ":/Menu/rc/Menu/chat.png");
    }
}

void devicelistform::slot_ModifyTempPath()
{
    QString dir = QFileDialog::getExistingDirectory
            (this, tr("Open Directory"),
             mTempPath,
             QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
        mTempPath = dir;
    if(!mTempPath.endsWith("/"))
        mTempPath.append("/");
    mNetitem->setToolTip(1, mTempPath);
}

NetInfo* devicelistform::GetCurNetInfo()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(!item)
        return 0;
    if(item->parent() != mNetitem)
        return 0;
    return mNets.value(item->text(1));
}

NetInfo* devicelistform::GetNetInfo(QString pIP)
{
    return mNets.value(pIP);
}

QTreeWidgetItem* devicelistform::GetTreeItem(NetInfo *pNetInfo)
{
    if(!mNetitem || !pNetInfo)
        return 0;
    for(int i=0;i<mNetitem->childCount();i++)
    {
        QTreeWidgetItem* item = mNetitem->child(i);
        if(!item)
            continue;
        if(item->text(1).compare(pNetInfo->mIP) == 0)
            return item;
    }
    return 0;
}

NetInfo* devicelistform::AddNetInfo(QString pIP)
{
    if(!NetInfo::CheckIP(pIP))
        return 0;
    NetInfo *tmpNet = new NetInfo(pIP);
    connect(tmpNet, SIGNAL(sConnState()), this, SLOT(slot_ConnStateChanged()));
    return tmpNet;
}

QTreeWidgetItem* devicelistform::AddNetTreeItem(NetInfo *pNetInfo)
{
    if(!pNetInfo)
        return (QTreeWidgetItem*)0;
    QTreeWidgetItem* tmpItem = new QTreeWidgetItem;
    SetTreeItemBusState(tmpItem, pNetInfo->mConn);
    tmpItem->setText(1, pNetInfo->mIP);
    return tmpItem;
}

void devicelistform::SetTreeItemBusState(QTreeWidgetItem *pItem, BusState pState)
{
    if(!pItem)
        return;
    switch(pState)
    {
        case Offline:
            pItem->setToolTip(0, tr("Offline"));
            pItem->setIcon(0,QIcon(":/Menu/rc/Menu/notpass.png"));
            break;
        case Online:
            pItem->setToolTip(0, tr("Online"));
            pItem->setIcon(0,QIcon(":/Menu/rc/Menu/pass.png"));
            break;
        case Connected:
            pItem->setToolTip(0, tr("Connected"));
            pItem->setIcon(0,QIcon(":/Menu/rc/Menu/success.png"));
            break;
        default:
            break;
    }
}

BusState devicelistform::GetTreeItemBusState(QTreeWidgetItem *pItem)
{
    if(!pItem)
        return Offline;
    QString tmpToolTip = pItem->toolTip(0);
    if(tmpToolTip.compare(tr("Offline")) == 0)
        return Offline;
    else if(tmpToolTip.compare(tr("Online")) == 0)
        return Online;
    else if(tmpToolTip.compare(tr("Connected")) == 0)
        return Connected;
    return Offline;
}

void devicelistform::slot_NetConn()
{
    NetInfo *tmpNet = GetCurNetInfo();
    if(!tmpNet)
        return;
    if(!tmpNet->NetManagerConnect(1000))
    {
        QMessageBox::warning(this, tr("Error"),
                              tr("Can't connect to %1, \nPlease Check it!").arg(tmpNet->mIP),
                               QMessageBox::Ok,
                               QMessageBox::Ok);
    }
}

void devicelistform::slot_NetDisconn()
{
    NetInfo *tmpNet = GetCurNetInfo();
    if(!tmpNet)
        return;
    if(!tmpNet->NetManagerDisconnect(100))
    {
        QMessageBox::warning(this, tr("Error"),
                              tr("Can't disconnect from %1, \nPlease Check it!").arg(tmpNet->mIP),
                               QMessageBox::Ok,
                               QMessageBox::Ok);
    }
#if 1
    SetTreeItemBusState(ui->treeWidget->currentItem(), Offline);
    tmpNet->ReleaseNetManager();
#endif
}

void devicelistform::slot_ConnStateChanged()
{
    NetInfo *tmpNet = VAR_CAST<NetInfo*>(sender());
    if(!tmpNet)
        return;
    QTreeWidgetItem* tmpItem = GetTreeItem(tmpNet);
    if(!tmpItem)
        return;
    if(GetTreeItemBusState(tmpItem) != tmpNet->mConn)
    {
        SetTreeItemBusState(tmpItem, tmpNet->mConn);
        emit sConnstate(tmpNet);

        //tmpNet->mNetManager->GetFile("Y", "/workspace/IDE/BST_DISPLAY/RuntimeRc/FSRC/TEMP/GrabScreen.JPG", "D:/GrabScreen.JPG", 10000);
    }
}


void devicelistform::slot_RefreshTreelist()
{
    QStringList tmpIPList;
    for(int i=0;i<mNetitem->childCount();i++)
        tmpIPList << mNetitem->child(i)->text(1);
    //>@添加网络节点
    QStringList netKeyList = mNets.keys();
    for(int j=0; j<netKeyList.count(); j++)
    {
        NetInfo *tmpNet = mNets.value(netKeyList.at(j));
        if(!tmpNet)
            continue;
        if(tmpIPList.contains(tmpNet->mIP))
        {
            int dex = tmpIPList.lastIndexOf(tmpNet->mIP);
            if(dex == -1)
                continue;
            QTreeWidgetItem* tmpItem = mNetitem->child(dex);
            if(tmpItem == 0)
                continue;
            SetTreeItemBusState(tmpItem, tmpNet->mConn);
        }
        else
        {
            QTreeWidgetItem* tmpItem = AddNetTreeItem(tmpNet);
            if(!tmpItem)
                continue;
            mNetitem->addChild(tmpItem);
        }
    }
    //>@添加U盘节点
    //ui->treeWidget->DelTreeAllItem(mUdiskitem);

    //>@添加串口节点
    //ui->treeWidget->DelTreeAllItem(mComitem);

    //>@展开
    ui->treeWidget->expandAll();
}

void devicelistform::slot_Scan()
{
    EnumerateNet();
    EnumerateUSB();
    EnumerateCOM();
    slot_RefreshTreelist();
}

void devicelistform::on_BtnAdd_clicked()
{
    QString tmpDevAddr = ui->DevAddress->text();
    if(tmpDevAddr.isEmpty())
        return;
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(!item)
    {
        QMessageBox::warning(this, tr("Error"),
                                   tr("Please select the type of device!"),
                             QMessageBox::Yes,
                             QMessageBox::Yes);
        return;
    }
    //>@判断是否添加到网络节点中
    if(item == mNetitem || item->parent() == mNetitem)
    {
        if(NetInfo::CheckIP(tmpDevAddr))
        {
            NetInfo* tmpNet = AddNetInfo(tmpDevAddr);
            if(tmpNet)
                mNets.insert(tmpDevAddr, tmpNet);
        }
    }
    //>@判断是否添加到U盘节点或者串口节点中

    slot_RefreshTreelist();
}

void devicelistform::on_BtnHideCmdRecv_clicked(bool checked)
{
    if(checked)
        ui->CommandRecvTextEdit->setHidden(false);
    else
        ui->CommandRecvTextEdit->setHidden(true);
}

HostNetManager* GetNetManager()
{
    DeviceToolBar *tmpToolBar = VAR_CAST<DeviceToolBar>(GlobalPara.m_DeviceToolBar);
    if(!(tmpToolBar && tmpToolBar->mDevListFrom))
        return (HostNetManager*)0;
    NetInfo *tmpNetInfo = tmpToolBar->mDevListFrom->GetCurNetInfo();
    if(!tmpNetInfo)
        return (HostNetManager*)0;
    return tmpNetInfo->mNetManager;
}

