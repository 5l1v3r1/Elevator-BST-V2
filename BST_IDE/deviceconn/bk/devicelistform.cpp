#include "devicelistform.h"
#include "ui_devicelistform.h"

devicelistform::devicelistform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::devicelistform)
{
    ui->setupUi(this);

    mTempPath = QString("C:/NFDBuffer/");

    mNFDHostApp = 0;
    mTcpclient = 0;

    mHostUdp = 0;
    mHostBroad = 0;
    mScanDevThread = 0;

    TempDirAct = 0;
    BroadcastAct = 0;
    SyncTimeAct = 0;
    NetconnAct = 0;
    NetDisconnAct = 0;
    MessageAct = 0;

    mNetitem = 0;
    mUdiskitem = 0;
    mComitem = 0;

    InitAct();
    InitTreeWidget();
    InitUdp();
    InitNFD();
    QtSleep(100);
    StartScanThread();
}

devicelistform::~devicelistform()
{
    KillScanThread();
    QtSleep(100);
    ReleaseUdp();
    ReleaseNFD();
    ReleaseTreeWidget();
    ReleaseAct();
    delete ui;
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
    if(SyncTimeAct != 0)
    {
        disconnect(SyncTimeAct, SIGNAL(triggered()), 0, 0);
        SyncTimeAct->deleteLater();
        SyncTimeAct = 0;
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

    TempDirAct = new QAction(QIcon(":/Menu/rc/Menu/save.png"), tr("&SavePath"), this);
//    TempDirAct->setShortcuts(QKeySequence::Open);
    TempDirAct->setStatusTip(tr("Set Default TempDir"));
    connect(TempDirAct, SIGNAL(triggered()), this, SLOT(slot_ModifyTempPath()));

    BroadcastAct = new QAction(QIcon(":/Menu/rc/Menu/chat.png"), tr("&Broadcast"), this);
//    TempDirAct->setShortcuts(QKeySequence::Open);
    BroadcastAct->setStatusTip(tr("Send a Broadcast"));
    connect(BroadcastAct, SIGNAL(triggered()), this, SLOT(slot_Broadcast()));

    SyncTimeAct = new QAction(QIcon(":/Menu/rc/Menu/refresh.png"), tr("&SyncTime"), this);
//    TempDirAct->setShortcuts(QKeySequence::Open);
    SyncTimeAct->setStatusTip(tr("Sync system time"));
    connect(SyncTimeAct, SIGNAL(triggered()), this, SLOT(slot_SyncTime()));

    NetconnAct = new QAction(QIcon(":/Menu/rc/Menu/textbold.png"), tr("&Conn"), this);
//    NetconnAct->setShortcuts(QKeySequence::Open);
    NetconnAct->setStatusTip(tr("Connect to Device"));
    connect(NetconnAct, SIGNAL(triggered()), this, SLOT(slot_NetConn()));

    NetDisconnAct = new QAction(QIcon(":/Menu/rc/Menu/textbold.png"), tr("&Disconn"), this);
//    NetDisconnAct->setShortcuts(QKeySequence::Open);
    NetDisconnAct->setStatusTip(tr("Disconnect to Device"));
    connect(NetDisconnAct, SIGNAL(triggered()), this, SLOT(slot_NetDisconn()));

    MessageAct = new QAction(QIcon(":/Menu/rc/Menu/chat.png"), tr("&Message"), this);
//    NetDisconnAct->setShortcuts(QKeySequence::Open);
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
    ui->treeWidget->setWindowTitle("DeviceList");
    //    ui->treeWidget->setSortingEnabled(true);//排序
    //    ui->treeWidget->setDragEnabled(true);
    ui->treeWidget->header()->setSortIndicatorShown(true);
    ui->treeWidget->header()->setSortIndicator(0, Qt::AscendingOrder);
    ui->treeWidget->setColumnCount(3);
    ui->treeWidget->setColumnWidth(0, 75);
    ui->treeWidget->setColumnWidth(1, 75);
    ui->treeWidget->setColumnWidth(2, 75);
    QList<QString>HeaderLabelList;
    HeaderLabelList.clear();
    HeaderLabelList<<"Type"<<"UserName"<<"Node";
    ui->treeWidget->setHeaderLabels(HeaderLabelList);

    mNetitem = new QTreeWidgetItem;
    mNetitem->setIcon(0,QIcon(":/Menu/rc/Menu/user.png"));
    mNetitem->setText(1, "Devices");
    mNetitem->setText(2, mTempPath);
    ui->treeWidget->addTopLevelItem(mNetitem);

    mUdiskitem = new QTreeWidgetItem;
    mUdiskitem->setIcon(0,QIcon(":/Menu/rc/Menu/floodfill.png"));
    mUdiskitem->setText(1, "Udisks");
    ui->treeWidget->addTopLevelItem(mUdiskitem);

    mComitem = new QTreeWidgetItem;
    mComitem->setIcon(0,QIcon(":/Menu/rc/Menu/comm.ico"));
    mComitem->setText(1, "Coms");
    ui->treeWidget->addTopLevelItem(mComitem);

    connect(ui->treeWidget,SIGNAL(itemPressed(QTreeWidgetItem*, int)),
            this,SLOT(Slot_ItemClicked(QTreeWidgetItem*, int)));
    slot_RefreshTreelist();
}

void devicelistform::Slot_ItemClicked(QTreeWidgetItem* pItem, int pColumn)
{
    if(QApplication::mouseButtons() == Qt::LeftButton)
    {
        return;
    }
    if (QApplication::mouseButtons() == Qt::RightButton)
    {
        if((pItem == mNetitem))
        {
            QMenu menu(tr("&Menu"),ui->treeWidget);
            menu.addAction(TempDirAct);
            menu.addAction(BroadcastAct);
            menu.addAction(SyncTimeAct);
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

void devicelistform::slot_RefreshTreelist()
{
//    mNetitem->takeChildren();
    QStringList IPList;
    for(int i=0;i<mNetitem->childCount();i++)
    {
        IPList<<mNetitem->child(i)->text(2);
    }
    QStringList netKeyList = mDevList.mNets.keys();
    for(int j=0; j<netKeyList.count(); j++)
    {
        NetInfo Netpara = mDevList.mNets.value(netKeyList.at(j));
        if(Netpara == NetInfo())
            continue;
        if(IPList.contains(Netpara.mIP))
        {
            int dex = IPList.lastIndexOf(Netpara.mIP);
            if(dex == -1)
                continue;
            QTreeWidgetItem* cldItem = mNetitem->child(dex);
            if(cldItem == 0)
                continue;
            NetInfo preNetpara;
            QString conn = cldItem->toolTip(0);
            if(conn.compare("Offline", Qt::CaseInsensitive) == 0)
                preNetpara.mConn = Offline;
            else if(conn.compare("Online", Qt::CaseInsensitive) == 0)
                preNetpara.mConn = Online;
            else if(conn.compare("Connected", Qt::CaseInsensitive) == 0)
                preNetpara.mConn = Connected;
            preNetpara.mName = cldItem->text(1);
            preNetpara.mIP = cldItem->text(2);
            if(preNetpara == Netpara)
                continue;
            switch(Netpara.mConn)
            {
                case Offline:
                    cldItem->setToolTip(0, "Offline");
                    cldItem->setIcon(0,QIcon(":/Menu/rc/Menu/notpass.png"));
                    break;
                case Online:
                    cldItem->setToolTip(0, "Online");
                    cldItem->setIcon(0,QIcon(":/Menu/rc/Menu/pass.png"));
                    break;
                case Connected:
                    cldItem->setToolTip(0, "Connected");
                    cldItem->setIcon(0,QIcon(":/Menu/rc/Menu/success.png"));
                    break;
            }
            cldItem->setText(1, Netpara.mName);
            cldItem->setText(2, Netpara.mIP);
        }
        else
        {
            QTreeWidgetItem* Netsetitem = new QTreeWidgetItem;
            Netsetitem->setCheckState(0, Qt::Unchecked);
            switch(Netpara.mConn)
            {
                case Offline:
                   Netsetitem->setIcon(0,QIcon(":/Menu/rc/Menu/notpass.png"));
                   break;
                case Online:
                   Netsetitem->setIcon(0,QIcon(":/Menu/rc/Menu/pass.png"));
                   break;
                case Connected:
                   Netsetitem->setIcon(0,QIcon(":/Menu/rc/Menu/success.png"));
                   break;
            }
            Netsetitem->setText(1, Netpara.mName);
            Netsetitem->setText(2, Netpara.mIP);
            mNetitem->addChild(Netsetitem);
        }
        if(mNetitem->isExpanded())
            mNetitem->setExpanded(true);
    }
}

void devicelistform::slot_PHConnState(bool pState)
{
    if(!pState)
    {
        QStringList netKeyList = mDevList.mNets.keys();
        for(int i=0; i<netKeyList.count(); i++)
        {
            if(mDevList.GetNetBusState(netKeyList.at(i)) == Connected)
            {
                emit sConnstate(false, netKeyList.at(i));
                mDevList.ModifyNetsBusState(netKeyList.at(i), Online);
            }
        }
        slot_RefreshTreelist();
    }
}

void devicelistform::slot_NFDProgress(quint8 pProgress)
{
    GlobalPara.updateProgress(pProgress);
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
    mNetitem->setText(2, mTempPath);
}

void devicelistform::slot_NetConn()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    for(int i=0; i<mNetitem->childCount(); i++)
    {
        if(item = mNetitem->child(i))
        {
            if(NFDConnect(QHostAddress(item->text(2)), D_NFCPORT, 100))
            {
                mDevList.ModifyNetsBusState(item->text(2), Connected);
                slot_RefreshTreelist();
                emit sConnstate(true, item->text(2));
            }
        }
    }
}

void devicelistform::slot_NetDisconn()
{
    if(NFDDisconnect(100))
    {
        QStringList netKeyList = mDevList.mNets.keys();
        for(int i=0; i<netKeyList.count(); i++)
        {
            if(mDevList.GetNetBusState(netKeyList.at(i)) == Connected)
            {
                mDevList.ModifyNetsBusState(netKeyList.at(i), Online);
                slot_RefreshTreelist();
                emit sConnstate(false, netKeyList.at(i));
            }
        }
    }
}

void devicelistform::slot_Broadcast()
{}

void devicelistform::slot_SyncTime()
{}

void devicelistform::slot_Message()
{}

void devicelistform::ScanIP()
{
    if(mHostBroad == 0)
        return;
    QStringList netKeyList = mDevList.mNets.keys();
    for(int i=0; i<netKeyList.count(); i++)
    {
        if(mDevList.GetNetBusState(netKeyList.at(i)) != Connected)
            mDevList.ModifyNetsBusState(netKeyList.at(i), Offline);
    }
    QStringList netList = mHostBroad->ScanDevIP(50);
    for(int i=0;i<netList.count();i++)
    {
        QString netParaList = netList.at(i);
        QStringList netPara = netParaList.split(";");
        if(netPara.count() != 4)
            continue;
        NetInfo netInfo;
        netInfo.mConn = Online;
        netInfo.mName = netPara.at(1);
        netInfo.mIP = netPara.at(2);
        mDevList.AppendNet(netInfo);
    }
}
void devicelistform::EnumerateUdisk(){}

void devicelistform::EnumerateCom(){}

bool devicelistform::Scan()
{
    QReadWriteLock lock;
    lock.lockForWrite();
    bool result = false;
    DevList preDevList = mDevList;
    ScanIP();
    EnumerateUdisk();
    EnumerateCom();
    if(mDevList != preDevList)
        result = true;
    lock.unlock();
    return result;
}

void devicelistform::StopScanThread()
{
    if(mScanDevThread != 0)
    {
        if (mScanDevThread->isRunning())
        {
            mScanDevThread->Stop();
            if (QThread::currentThread() != mScanDevThread)
                mScanDevThread->wait();
        }
    }
}

void devicelistform::KillScanThread()
{
    if(mScanDevThread != 0)
    {
        StopScanThread();
        disconnect(mScanDevThread, SIGNAL(sRefreshTreelist()), 0, 0);
        mScanDevThread->quit();
        mScanDevThread->deleteLater();
        mScanDevThread = 0;
    }
}

void devicelistform::StartScanThread()
{
    KillScanThread();
    mScanDevThread = new ScanDevThread(this);
    connect(mScanDevThread, SIGNAL(sRefreshTreelist()), this, SLOT(slot_RefreshTreelist()));
    mScanDevThread->start();
}
