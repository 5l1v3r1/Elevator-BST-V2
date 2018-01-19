#include "projectmanager.h"
#include <QMainWindow>
#include "deviceclass.h"
#include "uipage.h"

ProjectManager::ProjectManager(QWidget *parent) :
    UpQTreeWidget(parent)
{
    m_MenuOnItem = 0;
    mToolUdp = 0;
    mHostNetDiscovery = 0;
    mStorageWatcher = 0;
    setRootIsDecorated(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setStyleSheet("QTreeWidget::item{height:20px}");  //设置行宽
    setColumnCount(1);
    setColumnWidth(0, 120);
    setHeaderLabel(tr("Device List"));
    setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked );
    setSelectionMode(QAbstractItemView::SingleSelection);
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
            this, SLOT(slot_ItemDoubleClicked(QTreeWidgetItem *, int)));
    connect(this, SIGNAL(itemPressed(QTreeWidgetItem *, int)),
            this, SLOT(slot_ItemPressed(QTreeWidgetItem *, int)));

    CreateMenu();

    InitScan();

    //>@默认添加桌面为本地缓冲
    AddDevice(BaseDevice::TYPE_LOCAL, QString("LocalDisk"), QDir::homePath());
}

ProjectManager::~ProjectManager()
{
    ReleaseScan();
    if(m_DevToolBar)
        m_DevToolBar->deleteLater();  //设备控制工具条
    for(int i=topLevelItemCount()-1;i>=0;i--)
    {
        DeviceTreeItem *tmpItem = (DeviceTreeItem*)takeTopLevelItem(i);
        if(tmpItem)
        {
            tmpItem->mDelFileFlag = false;
            delete tmpItem;
        }
    }
}

void ProjectManager::CreateMenu()
{
    /*
        New
        Add
        ---------
        Clear
        ---------
        Options
    */
    m_BlankMenu = new QMenu;
    m_BlankMenu->addAction(QIcon(":/Menu/rc/Menu/new.png"), tr("&New"),
                               this, SLOT(slot_NewDevice()));
    m_BlankMenu->addAction(QIcon(":/Menu/rc/Menu/add.png"), tr("&Add"),
                               this, SLOT(slot_AddDevice()));
    m_BlankMenu->addSeparator();
    m_BlankMenu->addAction(QIcon(":/Menu/rc/Menu/clear.png"), tr("&Clear"),
                               this, SLOT(slot_ClearDevice()));
    m_BlankMenu->addSeparator();
    m_BlankMenu->addAction(QIcon(":/Menu/rc/Menu/toolView.png"), tr("&Options"),
                               this, SLOT(slot_Options()));
    /*
        Refresh
        ---------
        Sync
        ---------
        New
        Add
        ---------
        Delete
        ---------
        Explorer
        Options(包含缓冲路径以及硬件设备的类型设置)
    */
    m_DeviceMenu = new QMenu;
    m_DeviceMenu->addAction(QIcon(":/Menu/rc/Menu/refresh.png"), tr("&Refresh"),
                               this, SLOT(slot_RefreshDevice()));
    m_DeviceMenu->addSeparator();
    m_DeviceMenu->addAction(QIcon(":/Menu/rc/Menu/success.png"), tr("&Sync"),
                               this, SLOT(slot_SyncDevice()));
    m_DeviceMenu->addSeparator();
    m_DeviceMenu->addAction(QIcon(":/Menu/rc/Menu/new.png"), tr("&New"),
                               this, SLOT(slot_NewProject()));
    m_DeviceMenu->addAction(QIcon(":/Menu/rc/Menu/add.png"), tr("&Add"),
                               this, SLOT(slot_AddProject()));
    m_DeviceMenu->addSeparator();
    m_DeviceMenu->addAction(QIcon(":/Menu/rc/Menu/delete.png"), tr("&Delete"),
                               this, SLOT(slot_DeleteDevice()));
    m_DeviceMenu->addSeparator();
    m_DeviceMenu->addAction(QIcon(":/Menu/rc/Menu/grid.png"), tr("&Explorer"),
                               this, SLOT(slot_ExploreDevice()));
    m_DeviceMenu->addAction(QIcon(":/Menu/rc/Menu/toolView.png"), tr("&Options"),
                               this, SLOT(slot_DeviceOptions()));
    /*
        SetRunning
        ---------
        New
        Add
        ---------
        Open
        Close
        Delete
        ---------
        Regular
        Download
        Upload
        ---------
        Explorer
        Options
    */
    m_ProjectMenu = new QMenu;
    m_ProjectMenu->addAction(QIcon(":/Menu/rc/Menu/build.png"), tr("&Set as Running"),
                             this, SLOT(slot_SetRunningProject()));
    m_ProjectMenu->addSeparator();
    m_ProjectMenu->addAction(QIcon(":/Menu/rc/Menu/new.png"), tr("&New"),
                             this, SLOT(slot_NewFile()));
    m_ProjectMenu->addAction(QIcon(":/Menu/rc/Menu/add.png"), tr("&Add"),
                             this, SLOT(slot_AddFile()));
    m_ProjectMenu->addSeparator();
    m_ProjectMenu->addAction(QIcon(":/Menu/rc/Menu/open.png"), tr("&Open"),
                             this, SLOT(slot_OpenProject()));
    m_ProjectMenu->addAction(QIcon(":/Menu/rc/Menu/fail.png"), tr("&Close"),
                             this, SLOT(slot_CloseProject()));
    m_ProjectMenu->addAction(QIcon(":/Menu/rc/Menu/minus.png"), tr("&Delete"),
                             this, SLOT(slot_DeleteProject()));
    m_ProjectMenu->addSeparator();
    m_ProjectMenu->addAction(QIcon(":/Menu/rc/Menu/simulator.png"), tr("&Regular"),
                             this, SLOT(slot_RegularProject()));
    m_ProjectMenu->addAction(QIcon(":/Menu/rc/Menu/down.png"), tr("&Download"),
                          this, SLOT(slot_DownloadProject()));
    m_ProjectMenu->addAction(QIcon(":/Menu/rc/Menu/up.png"), tr("&Upload"),
                          this, SLOT(slot_UploadProject()));
    m_ProjectMenu->addSeparator();
    m_ProjectMenu->addAction(QIcon(":/Menu/rc/Menu/grid.png"), tr("&Explorer"),
                               this, SLOT(slot_ExploreProject()));
    m_ProjectMenu->addAction(QIcon(":/Menu/rc/Menu/toolView.png"), tr("&Options"),
                               this, SLOT(slot_ProjectOptions()));
    /*
        Open
        Close
        Delete
        ---------
        Explorer
    */
    m_FileMenu = new QMenu;
    m_FileMenu->addAction(QIcon(":/Menu/rc/Menu/open.png"), tr("&Open"),
                          this, SLOT(slot_OpenFile()));
    m_FileMenu->addAction(QIcon(":/Menu/rc/Menu/dir.png"), tr("&Close"),
                          this, SLOT(slot_CloseFile()));
    m_FileMenu->addAction(QIcon(":/Menu/rc/Menu/minus.png"), tr("&Delete"),
                          this, SLOT(slot_DeletFile()));
    m_FileMenu->addSeparator();
    m_FileMenu->addAction(QIcon(":/Menu/rc/Menu/grid.png"), tr("&Explorer"),
                          this, SLOT(slot_ExploreFile()));
}

void ProjectManager::InitScan()
{
    mToolUdp = new ToolUdp(this);
    if(!mToolUdp->OpenUdp(D_CMDPORT))
    {
        mToolUdp->deleteLater();
        mToolUdp = 0;
    }
    else
    {
        mHostNetDiscovery = new HostNetDiscovery(this);
        mHostNetDiscovery->SyncDev(mToolUdp);
        connect(mToolUdp, SIGNAL(readyRead()), mHostNetDiscovery, SLOT(ParseDataStream()));
        connect(mHostNetDiscovery, SIGNAL(sCommand(QString)), GlobalPara.m_OutputMsgList, SLOT(append(QString)));

        mScanTimer.setInterval(D_SCANIPFREQ);
        connect(&mScanTimer, SIGNAL(timeout()), this, SLOT(slot_NetScan()));
        mScanTimer.start();
    }

    mStorageWatcher = new DeviceWatcher;
    connect(mStorageWatcher, SIGNAL(sDeviceAdded(QString)), this, SLOT(slot_DeviceAdded(QString)));
    connect(mStorageWatcher, SIGNAL(sDeviceRemoved(QString)), this, SLOT(slot_DeviceRemoved(QString)));
}

void ProjectManager::ReleaseScan()
{
    if(mHostNetDiscovery !=0 )
    {
        mScanTimer.stop();
        mHostNetDiscovery->deleteLater();
        mHostNetDiscovery = 0;
    }
    if(mToolUdp !=0)
    {
        disconnect(mToolUdp, SIGNAL(readyRead()), 0, 0);
        mToolUdp->deleteLater();
        mToolUdp = 0;
    }
    if(mStorageWatcher)
    {
        disconnect(mStorageWatcher, SIGNAL(sDeviceAdded(QString)), 0, 0);
        disconnect(mStorageWatcher, SIGNAL(sDeviceRemoved(QString)), 0, 0);
        mStorageWatcher->quit();
        mStorageWatcher->deleteLater();
        mStorageWatcher = 0;
    }
}

void ProjectManager::slot_NetScan()
{
    if(mHostNetDiscovery == 0)
        return;
    //>@扫描节点
    QStringList netList = mHostNetDiscovery->ScanDevIP(1000);
    for(int i=0;i<netList.count();i++)
    {
        QString tmpIP = netList.at(i);
        if(tmpIP.isEmpty())
            continue;
        AddDevice(BaseDevice::TYPE_NET, tmpIP, QString(), S_ONLINE);
    }
}

void ProjectManager::slot_DeviceAdded(QString dev)
{
    QStringList tmpList = dev.split('>');
    if(tmpList.count()!=2)
    {
        IDE_TRACE_STR(dev);
        return;
    }
    QString tmpType = tmpList.at(0);
    if(tmpType.compare("storage", Qt::CaseInsensitive) == 0)  //>@U盘
    {
        AddDevice(BaseDevice::TYPE_STORAGE, tmpList.at(1), QString(), S_DEVSYNC);
    }
}

void ProjectManager::slot_DeviceRemoved(QString dev)
{
    QStringList tmpList = dev.split('>');
    if(tmpList.count()!=2)
    {
        IDE_TRACE_STR(dev);
        return;
    }
    QString tmpType = tmpList.at(0);
    if(tmpType.compare("storage", Qt::CaseInsensitive) == 0)  //>@U盘
    {
        DeviceTreeItem *tmpItem = GetDeviceFromAddr(tmpList.at(1));
        if(tmpItem)
            tmpItem->SetState(S_OFFLINE);
    }
}

void ProjectManager::slot_ItemDoubleClicked(QTreeWidgetItem *pItem, int column)
{
    if(!pItem)
        return;
    int tmpTreeType = pItem->type();
    if(tmpTreeType != FILETREEITEM_TYPE)
        return;
    FileTreeItem *tmpItem = (FileTreeItem*)pItem;
    if(tmpItem)
        tmpItem->Open();
}

void ProjectManager::slot_ItemPressed(QTreeWidgetItem *item, int column)
{
    if(QApplication::mouseButtons() == Qt::RightButton && item)
    {
        int tmpTreeType = item->type();
        m_MenuOnItem = item;//为什么这里返回的都是NULL？
        DeviceTreeItem *tmpItem = 0;
        if(tmpTreeType == FILETREEITEM_TYPE)
        {
            m_FileMenu->exec(QCursor::pos());
            tmpItem = (DeviceTreeItem *)(item->parent()->parent());
        }
        else if(tmpTreeType == PROJECTTREEITEM_TYPE)
        {
            m_ProjectMenu->exec(QCursor::pos());
            tmpItem = (DeviceTreeItem *)(item->parent());
        }
        else if(tmpTreeType == DEVICETREEITEM_TYPE)
        {
            m_DeviceMenu->exec(QCursor::pos());
            tmpItem = (DeviceTreeItem *)(item);
        }
        //>@更新DeviceToolbar的状态
        m_DevToolBar->slot_Connstate(tmpItem);
    }
}

void ProjectManager::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        if(!itemAt(event->pos()))
            m_BlankMenu->exec(QCursor::pos());
    }
    QTreeWidget::mousePressEvent(event);
}

DeviceTreeItem* ProjectManager::GetDevice(BaseDevice *pDev)
{
    if(pDev)
    {
        //>@对于有mDevAddr的设备只能比对地址,对于没有mDevAddr的设备需要比对bufferpath才可以
        DeviceTreeItem *tmpItem = 0;
        if(pDev->mDevAddr.isEmpty())
            tmpItem = GetDeviceFromBuffer(pDev->mBufferPath);
        else
            tmpItem = GetDeviceFromAddr(pDev->mDevAddr);
        return tmpItem;
    }
    return (DeviceTreeItem*)0;
}

DeviceTreeItem* ProjectManager::GetDeviceFromAddr(QString pDevAddr)
{
    if(!pDevAddr.isEmpty())
    {
        for(int i=0;i<topLevelItemCount();i++)
        {
            DeviceTreeItem *tmpItem = (DeviceTreeItem*)topLevelItem(i);
            if(tmpItem && tmpItem->mInterfaceDevice &&
               (tmpItem->mInterfaceDevice->mDevAddr.compare(pDevAddr, Qt::CaseInsensitive) == 0))
            {
                return tmpItem;
            }
        }
    }
    return (DeviceTreeItem*)0;
}

DeviceTreeItem* ProjectManager::GetDeviceFromBuffer(QString pDevBuffer)
{
    if(!pDevBuffer.isEmpty())
    {
        for(int i=0;i<topLevelItemCount();i++)
        {
            DeviceTreeItem *tmpItem = (DeviceTreeItem*)topLevelItem(i);
            if(tmpItem && tmpItem->mInterfaceDevice &&
               (tmpItem->mInterfaceDevice->mBufferPath.compare(pDevBuffer, Qt::CaseInsensitive) == 0))
            {
                return tmpItem;
            }
        }
    }
    return (DeviceTreeItem*)0;
}

DeviceTreeItem* ProjectManager::AddDevice(BaseDevice::DEVICETYPE pType, QString pDevName,
                                          QString pBufferPath, DEVICESTATE pState)
{
    if(pDevName.isEmpty())
        return (DeviceTreeItem*)0;
    //>@防止重复插入，如果检测到插入的设备已经存在则只更新缓冲以及状态
    DeviceTreeItem *tmpItem = GetDeviceFromAddr(pDevName);
    if(tmpItem)
    {
        if(!pBufferPath.isEmpty())
            tmpItem->SetBuffer(pBufferPath);
        tmpItem->SetState(pState);
        return tmpItem;
    }
    DeviceTreeItem *tmpTreeItem = new DeviceTreeItem(pType, pDevName, pBufferPath, pState);
    addTopLevelItem(tmpTreeItem);
    return tmpTreeItem;
}

DeviceTreeItem* ProjectManager::AddDevice(BaseDevice* pDev)
{
    if(!pDev)
        return (DeviceTreeItem*)0;
    //>@防止重复插入
    DeviceTreeItem *tmpItem = GetDevice(pDev);
    if(tmpItem)
        return tmpItem;
    DeviceTreeItem *tmpTreeItem = new DeviceTreeItem(pDev);
    addTopLevelItem(tmpTreeItem);
    return tmpTreeItem;
}

bool ProjectManager::DelDevice(DeviceTreeItem *&pTreeItem, bool pDelFile)
{
    if(pTreeItem == 0)
        return false;
    pTreeItem->mDelFileFlag = pDelFile;
    takeTopLevelItem(indexOfTopLevelItem(pTreeItem));
    delete pTreeItem;
    pTreeItem = 0;
    return true;
}

void ProjectManager::slot_Options()
{

}

void ProjectManager::slot_NewDevice()
{
    DevWizard newDevWizard;
    int ret = newDevWizard.exec();
    if(ret == QDialog::Accepted)
    {
        AddDevice(newDevWizard.GetDevice());
    }
}

void ProjectManager::slot_AddDevice()
{
    static QString sDir = "/";
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select Device Buffer"),
                                                    sDir,
                                                    QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;
    sDir = dir;
    DevWizard newDevWizard(dir);
    int ret = newDevWizard.exec();
    if(ret == QDialog::Accepted)
    {
        AddDevice(newDevWizard.GetDevice());
    }
}

void ProjectManager::slot_ClearDevice()
{
    //>@显示菜单时已经确定m_MenuOnItem是DeviceTreeItem类型的了
    DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem;
    if(tmpDevItem)
    {
        int ret = QMessageBox::question(0, tr("Question"),
                                       tr("Do you want to clear?"),
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
        if(ret == QMessageBox::Yes)
        {
            tmpDevItem->mDelFileFlag = true;
            tmpDevItem->Clear();
        }
    }
}

void ProjectManager::slot_RefreshDevice()
{
    DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem;
    if(tmpDevItem && tmpDevItem->mInterfaceDevice)
    {
        tmpDevItem->mInterfaceDevice->Scan();
    }
}

void ProjectManager::slot_SyncDevice()
{
    DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem;
    if(!(tmpDevItem && tmpDevItem->mInterfaceDevice && !tmpDevItem->mInterfaceDevice->mBufferPath.isEmpty()))
    {
        int ret = QMessageBox::question(0, tr("Question"),
                                       tr("Device is not set completely,\n"
                                          "Do you want to configure now?"),
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
        if(ret == QMessageBox::Yes)
            slot_DeviceOptions();
        return;
    }
    QAction *tmpAction = VAR_CAST<QAction *>(sender());
    if(tmpAction)
    {
        QString tmpText = tmpAction->text();
        if(tmpText.compare("Sync", Qt::CaseInsensitive) == 0)
        {
            if(tmpDevItem->mInterfaceDevice->Sync(1000))
                tmpAction->setText("Async");
        }
        else
        {
            if(tmpDevItem->mInterfaceDevice->Async(500))
                tmpAction->setText("Sync");
        }
    }
}

void ProjectManager::slot_DeleteDevice()
{
    DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem;
    if(tmpDevItem)
    {
        int ret = QMessageBox::question(0, tr("Question"),
                                       tr("Do you want to delete?"),
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
        if(ret == QMessageBox::Yes)
        {
            //>@如果当前删除的设备为DeviceToolbar中关联的设别，则更新DeviceToolbar的状态
            if(tmpDevItem == m_DevToolBar->mDevTreeItem)
                m_DevToolBar->slot_Connstate((DeviceTreeItem *)0);
            DelDevice(tmpDevItem, true);
        }
    }
}

void ProjectManager::slot_ExploreDevice()
{
    DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem;
    if(tmpDevItem && tmpDevItem->mInterfaceDevice)
    {
        QString tmpPath;
        switch(tmpDevItem->mInterfaceDevice->mDevType)
        {
            case BaseDevice::TYPE_NET:  //>@以FTP方式打开设备目录
            {
                tmpPath = QString("ftp://%1/THEME/").arg(tmpDevItem->mInterfaceDevice->mDevAddr);
                break;
            }
            case BaseDevice::TYPE_STORAGE:
            {
                tmpPath = QString("file:///%1/THEME/").arg(tmpDevItem->mInterfaceDevice->mDevAddr);
                break;
            }
            case BaseDevice::TYPE_COM:
            {
                QMessageBox::warning(0,  tr("Warn"),
                                         tr("Browsing feature of COM is not supported for the time being!"),
                                         QMessageBox::Yes);
                return;
            }
            default:
            {
                tmpPath = QString("file:///%1/").arg(tmpDevItem->mInterfaceDevice->mBufferPath);
                break;
            }
        }
        if(!QDesktopServices::openUrl(QUrl(tmpPath, QUrl::TolerantMode)))
        {
            QMessageBox::warning(0,  tr("Error"),
                                     tr("Open %1 error!").arg(tmpPath),
                                     QMessageBox::Yes);
        }
    }
}

void ProjectManager::slot_DeviceOptions()
{
    DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem;
    if(tmpDevItem)
    {
        DevWizard newDevWizard(tmpDevItem->mInterfaceDevice);
        newDevWizard.exec();
        if(tmpDevItem->mInterfaceDevice)
            tmpDevItem->mInterfaceDevice->Scan();
    }
}

void ProjectManager::slot_SetRunningProject()
{
    if(m_MenuOnItem)
    {
        DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem->parent();
        if(tmpDevItem && tmpDevItem->mInterfaceDevice)
        {
            if(tmpDevItem->mInterfaceDevice->mDevType == BaseDevice::TYPE_NET)
            {
                tmpDevItem->SetRunningProject((ProjectTreeItem *)m_MenuOnItem);
            }
        }
    }
}

void ProjectManager::slot_NewProject()
{
    DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem;
    if(tmpDevItem)
    {
        //>@查看是否已经具有缓冲路径
        if(!(tmpDevItem->mInterfaceDevice && !tmpDevItem->mInterfaceDevice->mBufferPath.isEmpty()))
        {
            int ret = QMessageBox::question(0, tr("Question"),
                                           tr("Buffer path is not set,\n"
                                              "Do you want to configure?"),
                                           QMessageBox::Yes | QMessageBox::No,
                                           QMessageBox::No);
            if(ret == QMessageBox::Yes)
            {
                slot_DeviceOptions();
            }
            return;
        }
        QDir tmpDir(tmpDevItem->mInterfaceDevice->mBufferPath);
        if(!tmpDir.exists())
            tmpDir.mkpath(tmpDevItem->mInterfaceDevice->mBufferPath);
        ProWizard newProWizard;
        newProWizard.SetMemoryDir(tmpDevItem->mInterfaceDevice->mBufferPath, true);
        int ret = newProWizard.exec();
        if(ret == QDialog::Accepted)
        {
            tmpDevItem->AddProject(newProWizard.GetManager());
        }
    }
}

void ProjectManager::slot_AddProject()
{
    DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem;
    if(tmpDevItem)
    {
        //>@查看是否已经具有缓冲路径
        if(!(tmpDevItem->mInterfaceDevice && !tmpDevItem->mInterfaceDevice->mBufferPath.isEmpty()))
        {
            int ret = QMessageBox::question(0, tr("Question"),
                                           tr("Buffer path is not set,\n"
                                              "Do you want to configure now?"),
                                           QMessageBox::Yes | QMessageBox::No,
                                           QMessageBox::No);
            if(ret == QMessageBox::Yes)
            {
                slot_DeviceOptions();
            }
            return;
        }
        QDir tmpDir(tmpDevItem->mInterfaceDevice->mBufferPath);
        if(!tmpDir.exists())
            tmpDir.mkpath(tmpDevItem->mInterfaceDevice->mBufferPath);

        static QString sSelectPath = "/";
        QString fileName = QFileDialog::getOpenFileName(this, tr("Select Project"),
                                                        sSelectPath,
                                                        tr("Project files (*.bstpro)"));
        if (fileName.isEmpty())
            return;
        sSelectPath = getFileDirectory(fileName);

        ProWizard newProWizard(fileName);
        int ret = newProWizard.exec();
        if(ret == QDialog::Accepted)
        {
            tmpDevItem->AddProject(newProWizard.GetManager());
        }
    }
}

void ProjectManager::slot_OpenProject()
{
    ProjectTreeItem *tmpProItem = (ProjectTreeItem*)m_MenuOnItem;
    if(tmpProItem)
    {
        tmpProItem->Open();
    }
}

void ProjectManager::slot_CloseProject()
{
    ProjectTreeItem *tmpProItem = (ProjectTreeItem*)m_MenuOnItem;
    if(tmpProItem)
    {
        tmpProItem->Close();
    }
}

void ProjectManager::slot_DeleteProject()
{
    if(m_MenuOnItem)
    {
        DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem->parent();
        if(tmpDevItem)
        {
            int ret = QMessageBox::question(0, tr("Question"),
                                           tr("Do you want to delete this project?"),
                                           QMessageBox::Yes | QMessageBox::No,
                                           QMessageBox::No);
            if(ret == QMessageBox::Yes)
            {
                int flag = QMessageBox::question(0, tr("Question"),
                                               tr("Do you want to delete all files?"),
                                               QMessageBox::Yes | QMessageBox::No,
                                               QMessageBox::No);
                if(flag == QMessageBox::Yes)
                    tmpDevItem->DelProject((ProjectTreeItem *&)m_MenuOnItem, true);
                else
                    tmpDevItem->DelProject((ProjectTreeItem *&)m_MenuOnItem, false);
            }
        }
    }
}

void ProjectManager::slot_RegularProject()
{
    if(m_MenuOnItem)
    {
        DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem->parent();
        if(!tmpDevItem)
            return;
        ProjectTreeItem *tmpProItem = (ProjectTreeItem*)m_MenuOnItem;
        if(tmpProItem && tmpProItem->mProManager && tmpProItem->mProManager->m_Valid)
        {
            //>@首先对主题进行一次规整操作
            FileTreeItem *tmpFileItem = tmpProItem->GetFileItem(TYPE_UI);
            if(!tmpFileItem)
            {
                QMessageBox::critical(0, tr("Error"),
                                       tr("Can't find this file,\n"
                                          "Please check it!"),
                                       QMessageBox::Yes);
                return;
            }
            uiPage *tmpUiPage = VAR_CAST<uiPage*>(tmpFileItem->mFileInfo->PageDialogPtr);
            if(!(tmpUiPage && tmpUiPage->m_GraphicsView->scene->m_UiManager))
            {
                int ret = QMessageBox::question(0, tr("Question"),
                                               tr("The theme isn't opened,\n"
                                                  "Do you want to open the page firstly?"),
                                               QMessageBox::Yes | QMessageBox::No,
                                               QMessageBox::No);
                if(ret == QMessageBox::No)
                    return;
                slot_ItemDoubleClicked(tmpFileItem, 0);
                while(1)
                {
                    tmpUiPage = VAR_CAST<uiPage*>(tmpFileItem->mFileInfo->PageDialogPtr);
                    if(tmpUiPage && tmpUiPage->m_GraphicsView->scene->m_InitializeOK)
                        break;
                    QtSleep(10);
                }
            }
            tmpUiPage->slot_Generate();
        }
    }
}

void ProjectManager::slot_DownloadProject()
{
    if(m_MenuOnItem)
    {
        DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem->parent();
        if(!(tmpDevItem && tmpDevItem->GetState() == S_DEVSYNC))
        {
            int ret = QMessageBox::question(0, tr("Question"),
                                           tr("Device is not sync,\n"
                                              "Do you want to sync now?"),
                                           QMessageBox::Yes | QMessageBox::No,
                                           QMessageBox::No);
            if(ret == QMessageBox::Yes)
                slot_SyncDevice();
            return;
        }
        QAction *tmpAction = VAR_CAST<QAction *>(sender());
        if(tmpAction)
        {
            QString tmpText = tmpAction->text();
            if(tmpText.compare("Download", Qt::CaseInsensitive) == 0)
            {
                if(tmpDevItem->mInterfaceDevice->mIsBusy)
                {
                    QMessageBox::warning(0, tr("Warn"),
                                           tr("Device is busy,\n"
                                              "Please wait a minute!"),
                                           QMessageBox::Yes);
                    return;
                }
                ProjectTreeItem *tmpProItem = (ProjectTreeItem*)m_MenuOnItem;
                if(!(tmpProItem && tmpProItem->mProManager && tmpProItem->mProManager->m_Valid))
                {
                    int ret = QMessageBox::question(0, tr("Question"),
                                                   tr("The project is closed,\n"
                                                      "Do you want to open now?"),
                                                   QMessageBox::Yes | QMessageBox::No,
                                                   QMessageBox::No);
                    if(ret == QMessageBox::Yes)
                        slot_OpenProject();
                    else
                        return;
                }
                tmpAction->setText("Downloading... ...");
                //>@开始下载操作
                QString tmpThemeName = GetDirectoryName(tmpProItem->mProManager->m_FileDir);
                if(!tmpDevItem->mInterfaceDevice->Download(tmpThemeName))
                {
                    QMessageBox::critical(0, tr("Error"),
                                           tr("Download theme error,\n"
                                              "Please check this!"),
                                           QMessageBox::Yes);
                }
                else
                {
                    QStringList tmpUiFiles = SearchFiles(tmpProItem->mProManager->m_FileDir, "ui");
                    if(!tmpUiFiles.isEmpty())
                        tmpProItem->AddFile(tmpUiFiles.first());
                    QStringList tmpMdpFiles = SearchFiles(tmpProItem->mProManager->m_FileDir, "mdp");
                    if(!tmpMdpFiles.isEmpty())
                        tmpProItem->AddFile(tmpMdpFiles.first());
                    QStringList tmpMapiFiles = SearchFiles(tmpProItem->mProManager->m_FileDir, "map");
                    if(!tmpMapiFiles.isEmpty())
                        tmpProItem->AddFile(tmpMapiFiles.first());
                }
                //>@下载结束
                tmpAction->setText("Download");
            }
            else
            {
                //>@终止传输
                tmpDevItem->mInterfaceDevice->Abort();
                tmpAction->setText("Download");
            }
        }
    }
}

void ProjectManager::slot_UploadProject()
{
    if(m_MenuOnItem)
    {
        DeviceTreeItem *tmpDevItem = (DeviceTreeItem*)m_MenuOnItem->parent();
        if(!(tmpDevItem && tmpDevItem->GetState() == S_DEVSYNC))
        {
            int ret = QMessageBox::question(0, tr("Question"),
                                           tr("Device is not sync,\n"
                                              "Do you want to sync now?"),
                                           QMessageBox::Yes | QMessageBox::No,
                                           QMessageBox::No);
            if(ret == QMessageBox::Yes)
                slot_SyncDevice();
            return;
        }
        QAction *tmpAction = VAR_CAST<QAction *>(sender());
        if(tmpAction)
        {
            QString tmpText = tmpAction->text();
            if(tmpText.compare("Upload", Qt::CaseInsensitive) == 0)
            {
                tmpAction->setText("Uploading... ...");
                ProjectTreeItem *tmpProItem = (ProjectTreeItem*)m_MenuOnItem;
                if(tmpProItem && tmpProItem->mProManager && tmpProItem->mProManager->m_Valid)
                {
                    QString tmpThemeName = GetDirectoryName(tmpProItem->mProManager->m_FileDir);
                    //>@首先对主题进行一次规整操作
                    QString tmpOutputThemeDir = QString("%1/%2/%3").arg(tmpProItem->mProManager->m_FileDir).arg(OUTPUTDIR).arg(tmpThemeName);
                    QDir tmpDir(tmpOutputThemeDir);
                    if(!tmpDir.exists())
                    {
                        int ret = QMessageBox::question(0, tr("Question"),
                                                       tr("Project is unregular,\n"
                                                          "Do you want to regular now?"),
                                                       QMessageBox::Yes | QMessageBox::No,
                                                       QMessageBox::No);
                        if(ret == QMessageBox::Yes)
                        {
                            slot_RegularProject();
                        }
                    }
                    //>@开始上传操作
                    if(tmpDevItem->mInterfaceDevice->Upload(tmpThemeName))
                    {
                        QMessageBox::critical(0, tr("Error"),
                                               tr("Upload theme error,\n"
                                                  "Please check this!"),
                                               QMessageBox::Yes);
                    }
                    else
                    {
                        QMessageBox::information(0, tr("Warn"),
                                                   tr("Upload theme success!"),
                                                   QMessageBox::Yes);
                    }
                }
                else
                {
                    QMessageBox::critical(0, tr("Error"),
                                           tr("The project is closed,\n"
                                              "Please open it firstly!"),
                                           QMessageBox::Yes);
                }
                //>@上传结束
                tmpAction->setText("Upload");
            }
            else
            {
                //>@终止传输
                tmpDevItem->mInterfaceDevice->Abort();
                tmpAction->setText("Upload");
            }
        }
    }
}

void ProjectManager::slot_ExploreProject()
{
    ProjectTreeItem *tmpProItem = (ProjectTreeItem*)m_MenuOnItem;
    if(tmpProItem && tmpProItem->mProManager)
    {
        QString tmpPath = QString("file:///%1/").arg(tmpProItem->mProManager->m_FileDir);
        if(!QDesktopServices::openUrl(QUrl(tmpPath, QUrl::TolerantMode)))
        {
            QMessageBox::warning(0,  tr("Error"),
                                     tr("Open %1 error!").arg(tmpPath),
                                     QMessageBox::Yes);
        }
    }
}

void ProjectManager::slot_ProjectOptions()
{
    ProjectTreeItem *tmpProItem = (ProjectTreeItem*)m_MenuOnItem;
    if(tmpProItem)
    {
        ProWizard newProWizard(tmpProItem->mProManager);
        newProWizard.exec();
    }
}

void ProjectManager::slot_NewFile()
{
    ProjectTreeItem *tmpProItem = (ProjectTreeItem*)m_MenuOnItem;
    if(tmpProItem)
    {
        if(!(tmpProItem->mProManager && tmpProItem->mProManager->m_Valid))
        {
            int ret = QMessageBox::question(0, tr("Question"),
                                           tr("Project is uninitial,\n"
                                              "Do you want to configure now?"),
                                           QMessageBox::Yes | QMessageBox::No,
                                           QMessageBox::No);
            if(ret == QMessageBox::Yes)
            {
                slot_ProjectOptions();
            }
            return;
        }
        newFileForm FileForm;
        int ret = FileForm.exec();
        if(ret == QDialog::Accepted)
        {
            FILE_TYPE tmpFileType = getFileType(&(FileForm.m_FileInfo));
            if(tmpFileType == TYPE_UI || tmpFileType == TYPE_MAP || tmpFileType == TYPE_MDP)
            {
                QString fileName = QFileDialog::getSaveFileName(0, tr("New File"),
                                                                tmpProItem->mProManager->m_FileDir,
                                                                tr("file (*%1)").arg(FileForm.m_FileInfo.ItemType));
                if (fileName.isEmpty())
                    return;
                tmpProItem->AddFile(fileName);
            }
        }
    }
}

void ProjectManager::slot_AddFile()
{
    ProjectTreeItem *tmpProItem = (ProjectTreeItem*)m_MenuOnItem;
    if(tmpProItem)
    {
        if(!(tmpProItem->mProManager && tmpProItem->mProManager->m_Valid))
        {
            int ret = QMessageBox::question(0, tr("Question"),
                                           tr("Project is uninitial,\n"
                                              "Do you want to configure now?"),
                                           QMessageBox::Yes | QMessageBox::No,
                                           QMessageBox::No);
            if(ret == QMessageBox::Yes)
            {
                slot_ProjectOptions();
            }
            return;
        }
        QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"),
                                                        tmpProItem->mProManager->m_FileDir,
                                                        tr("ui files (*.ui);;"
                                                           "map files (*.map);;"
                                                           "mdp files (*.mdp)"));
        if (fileName.isEmpty())
            return;
        tmpProItem->AddFile(fileName);
    }
}

void ProjectManager::slot_OpenFile()
{
    slot_ItemDoubleClicked(m_MenuOnItem, 0);
}

void ProjectManager::slot_CloseFile()
{
    if(!m_MenuOnItem)
        return;
    int tmpTreeType = m_MenuOnItem->type();
    if(tmpTreeType != FILETREEITEM_TYPE)
        return;
    FileTreeItem *tmpItem = (FileTreeItem*)m_MenuOnItem;
    if(tmpItem)
        tmpItem->Close();
}

void ProjectManager::slot_DeletFile()
{
    if(m_MenuOnItem)
    {
        ProjectTreeItem *tmpProItem = (ProjectTreeItem*)m_MenuOnItem->parent();
        if(tmpProItem)
        {
            int ret = QMessageBox::question(0, tr("Question"),
                                           tr("Do you want to delete this file?"),
                                           QMessageBox::Yes | QMessageBox::No,
                                           QMessageBox::No);
            if(ret == QMessageBox::Yes)
            {
                tmpProItem->DelFile((FileTreeItem *&)m_MenuOnItem, true);
            }
        }
    }
}

void ProjectManager::slot_ExploreFile()
{
    FileTreeItem *tmpFileItem = (FileTreeItem*)m_MenuOnItem;
    if(tmpFileItem && tmpFileItem->mFileInfo)
    {
        QString tmpPath = QString("file:///%1/").arg(tmpFileItem->mFileInfo->FileDir);
        if(!QDesktopServices::openUrl(QUrl(tmpPath, QUrl::TolerantMode)))
        {
            QMessageBox::warning(0,  tr("Error"),
                                     tr("Open %1 error!").arg(tmpPath),
                                     QMessageBox::Yes);
        }
    }
}

//>@同步当前Running工程中的文件，如果此文件还没有规整，则自动规整并同步
void ProjectManager::slot_StartSync()
{
    int tmpProgress = 0;
    int tmpDiff = 0;
    if(!m_DevToolBar->isConnected())
    {
        QMessageBox::critical(this, tr("Error"),
                                       tr("This device is disconnected, \n"
                                          "Please Check it!"),
                                       QMessageBox::Ok,
                                       QMessageBox::Ok);
        return;
    }
    if(m_DevToolBar->m_UpgradeState == DeviceToolBar::PAUSE)
    {
        m_DevToolBar->m_UpgradeState = DeviceToolBar::START;
        return;
    }
    //>@定位到当前running主题，并打开UI文件
    ProjectTreeItem *tmpProItem = m_DevToolBar->mDevTreeItem->GetRunningProject();
    if(!tmpProItem)
    {
        QMessageBox::warning(0,  tr("Warn"),
                                 tr("There is't running project yet! \n"
                                    "Please set running project firstly!"),
                                 QMessageBox::Yes);
        return;
    }
    if(!tmpProItem->mProManager)
    {
        return;
    }
    QString tmpRunningProjectDir = tmpProItem->mProManager->m_FileDir;
    if(!m_DevToolBar->mDevTreeItem->mInterfaceDevice)
    {
        return;
    }
    m_DevToolBar->m_UpgradeState = DeviceToolBar::START;
    if(m_DevToolBar->mDevTreeItem->mInterfaceDevice->mDevType == BaseDevice::TYPE_NET)
    {
        //>@通过网络同步
        NetDevice *tmpDev = (NetDevice *)m_DevToolBar->mDevTreeItem->mInterfaceDevice;
        QString tmpFSRCPath = tmpDev->mNetManager->m_DevInfoList.value("FSRC");
        //>@首先同步MDP文件
        tmpProgress = 10;
        GlobalPara.updateProgress(tr("Sync MDP Files!"), tmpProgress);
        QStringList tmpMDPList = SearchFiles(tmpRunningProjectDir, QString("mdp"), false);
        tmpDiff = 40.0 / (qreal)(tmpMDPList.count());
        for(int i=0;i<tmpMDPList.count();i++)
        {
            while(m_DevToolBar->m_UpgradeState == DeviceToolBar::PAUSE)
                QtSleep(100);
            if(m_DevToolBar->m_UpgradeState == DeviceToolBar::STOP)
            {
                GlobalPara.updateProgress(tr(""), 0);
                return;
            }
            QString tmpMdpFile = tmpMDPList.at(i);
            QString tmpDstMdpFile = tmpFSRCPath + D_BOOTDIR + getFileName(tmpMdpFile);
            if(tmpDev->mNetManager->SendFile("y", tmpMdpFile, tmpDstMdpFile, 3000) == false)
            {
                GlobalPara.updateProgress(tr("Sync %1 to %2 Failed!").
                                          arg(tmpMdpFile).
                                          arg(tmpDstMdpFile), tmpProgress + (i+1)*tmpDiff);
            }
            else
            {
                GlobalPara.updateProgress(tr("Sync %1 to %2 Success!").
                                          arg(tmpMdpFile).
                                          arg(tmpDstMdpFile), tmpProgress + (i+1)*tmpDiff);
            }
        }
        //>@其次同步ui文件
        tmpProgress = 50;
        GlobalPara.updateProgress(tr("Sync UI Files!"), tmpProgress);
        QStringList tmpUIList = SearchFiles(tmpRunningProjectDir+QString("Output/"), QString("zip"), false);
        tmpDiff = 40.0 / (qreal)(tmpUIList.count());

        tmpDev->mNetManager->Set("<Theme State=\"Stop\"/>");
        QtSleep(100);
        for(int i=0;i<tmpUIList.count();i++)
        {
            while(m_DevToolBar->m_UpgradeState == DeviceToolBar::PAUSE)
                QtSleep(100);
            if(m_DevToolBar->m_UpgradeState == DeviceToolBar::STOP)
            {
                GlobalPara.updateProgress(tr(""), 0);
                slot_StopSync();
                return;
            }
            QString tmpUIFile = tmpUIList.at(i);
            QString tmpThemeInfo = tmpDev->mNetManager->m_DevInfoList.value("ThemeManager");
            QStringList tmpThemeParas = tmpThemeInfo.split(',');
            if(tmpThemeParas.count() < 1)
                return;
            QString tmpDstUIFile = tmpThemeParas.at(0)+GetDirectoryName(tmpRunningProjectDir)+QString("/");
            if(tmpDev->mNetManager->UpLoadThemeZip(tmpUIFile, tmpDstUIFile, 600000) == false)
            {
                GlobalPara.updateProgress(tr("Sync %1 to %2 Failed!").
                                          arg(tmpUIFile).
                                          arg(tmpDstUIFile), tmpProgress + (i+1)*tmpDiff);
            }
            else
            {
                GlobalPara.updateProgress(tr("Sync %1 to %2 Success!").
                                          arg(tmpUIFile).
                                          arg(tmpDstUIFile), tmpProgress + (i+1)*tmpDiff);
            }
        }
        tmpProgress = 95;
        //>@删除output文件夹
        DelFolder(tmpRunningProjectDir+QString("Output/"));
        GlobalPara.updateProgress(tr("Sync Finised, Rebootting Current Device ..."), tmpProgress);
        tmpDev->mNetManager->SendCommand("killall BST_DISPLAY");
    }
    GlobalPara.updateProgress(tr(""), 0);
    m_DevToolBar->m_UpgradeState = DeviceToolBar::STOP;
}

void ProjectManager::slot_StopSync()
{
    m_DevToolBar->m_UpgradeState = DeviceToolBar::STOP;
}

void ProjectManager::slot_PauseSync()
{
    m_DevToolBar->m_UpgradeState = DeviceToolBar::PAUSE;
}


void ProjectManager::slot_OpenUiDesignerTool()
{
    if(!m_DevToolBar->isConnected())
        return;
    //>@定位到当前running主题，并打开UI文件
    ProjectTreeItem *tmpProItem = m_DevToolBar->mDevTreeItem->GetRunningProject();
    if(!tmpProItem)
    {
        QMessageBox::warning(0,  tr("Warn"),
                                 tr("There is't running project yet! \n"
                                    "Please set running project firstly!"),
                                 QMessageBox::Yes);
        return;
    }
    for(int i=0;i<tmpProItem->childCount();i++)
    {
        FileTreeItem *tmpItem = (FileTreeItem *)(tmpProItem->child(i));
        if(tmpItem == 0)
            continue;
        if(tmpItem->mFileInfo && tmpItem->mFileInfo->FileType == TYPE_UI)
        {
            slot_ItemDoubleClicked(tmpItem, 0);
            return;
        }
    }
}

void ProjectManager::slot_OpenMdpSetTool()
{
    if(!m_DevToolBar->isConnected())
        return;
    //>@定位到当前running主题，并打开UI文件
    ProjectTreeItem *tmpProItem = m_DevToolBar->mDevTreeItem->GetRunningProject();
    if(!tmpProItem)
    {
        QMessageBox::warning(0,  tr("Warn"),
                                 tr("There is't running project yet! \n"
                                    "Please set running project firstly!"),
                                 QMessageBox::Yes);
        return;
    }
    for(int i=0;i<tmpProItem->childCount();i++)
    {
        FileTreeItem *tmpItem = (FileTreeItem *)(tmpProItem->child(i));
        if(tmpItem == 0)
            continue;
        if(tmpItem->mFileInfo && tmpItem->mFileInfo->FileType == TYPE_MDP)
        {
            slot_ItemDoubleClicked(tmpItem, 0);
            return;
        }
    }
}

void ProjectManager::slot_OpenScheduleMapTool()
{
    if(!m_DevToolBar->isConnected())
        return;
    //>@定位到当前running主题，并打开UI文件
    ProjectTreeItem *tmpProItem = m_DevToolBar->mDevTreeItem->GetRunningProject();
    if(!tmpProItem)
    {
        QMessageBox::warning(0,  tr("Warn"),
                                 tr("There is't running project yet! \n"
                                    "Please set running project firstly!"),
                                 QMessageBox::Yes);
        return;
    }
    for(int i=0;i<tmpProItem->childCount();i++)
    {
        FileTreeItem *tmpItem = (FileTreeItem *)(tmpProItem->child(i));
        if(tmpItem == 0)
            continue;
        if(tmpItem->mFileInfo && tmpItem->mFileInfo->FileType == TYPE_MAP)
        {
            slot_ItemDoubleClicked(tmpItem, 0);
            return;
        }
    }
}

void ProjectManager::slot_OpenSoftwareTestTool()
{
    if(!m_DevToolBar->isConnected())
        return;
    //>@定位到当前running主题，并打开UI文件
    ProjectTreeItem *tmpProItem = m_DevToolBar->mDevTreeItem->GetRunningProject();
    if(!tmpProItem)
    {
        QMessageBox::warning(0,  tr("Warn"),
                                 tr("There is't running project yet! \n"
                                    "Please set running project firstly!"),
                                 QMessageBox::Yes);
        return;
    }
    for(int i=0;i<tmpProItem->childCount();i++)
    {
        FileTreeItem *tmpItem = (FileTreeItem *)(tmpProItem->child(i));
        if(tmpItem == 0)
            continue;
        if(tmpItem->mFileInfo && tmpItem->mFileInfo->FileType == TYPE_STC)
        {
            slot_ItemDoubleClicked(tmpItem, 0);
            return;
        }
    }
}

void ProjectManager::slot_OpenLogQueryTool()
{
//    NetInfo* tmpNet = deviceToolBar->mDevListFrom->GetCurNetInfo();
//    if(!(tmpNet && tmpNet->IsConnected()))
//        return;
//    QString tmpLogFile = QString("%1%2").arg(D_LOGDIR).arg(D_LOGFILE);
//    int tmpProgress = 10;
//    GlobalPara.updateProgress(tr("Sync LOG Files!"), tmpProgress);
//    quint64 life = 10000;
//    QString tmpFSRCPath = tmpNet->mNetManager->m_DevInfoList.value("FSRC");
//    tmpNet->mNetManager->GetFile("y", tmpFSRCPath + tmpLogFile,
//                                 deviceToolBar->m_NFCBufferPath + tmpLogFile, life);
//    int tmpDiff = 90.0 / 50.0;
//    for(int i=0;i<50;i++)
//    {
//        QString tmpSuffix = QString(".%1").arg(i);
//        QString tmpSrcLog = tmpFSRCPath + tmpLogFile + tmpSuffix;
//        QString tmpDstLog = deviceToolBar->m_NFCBufferPath + tmpLogFile + tmpSuffix;
//        GlobalPara.updateProgress(tr("Sync LOG : %1!").arg(tmpLogFile + tmpSuffix), tmpProgress+tmpDiff*i);
//        tmpNet->mNetManager->GetFile("y", tmpSrcLog, tmpDstLog, life);
//    }
//    GlobalPara.updateProgress(tr(""), 0);
//    ui->tabWidget->AddTabPage(deviceToolBar->m_NFCBufferPath + D_LOGDIR + QString("LogQuery"),
//                              getFileType(QString("LogQuery")), ":/Menu/rc/Menu/grid.png");
}
