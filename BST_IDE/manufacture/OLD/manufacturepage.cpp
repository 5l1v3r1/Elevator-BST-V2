#include "manufacturepage.h"
#include "ui_manufacturepage.h"

ManufacturePage::ManufacturePage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManufacturePage)
{
    ui->setupUi(this);

    m_SqlQuery = 0;

    SFConnectEstablished = false;
    ui->DeviceOperateLog->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui->DeviceOperateLog->horizontalHeader()->setStretchLastSection(true);  //列宽度自适应
    ui->DeviceOperateLog->verticalHeader()->hide();

    ui->ConnectTypeTabWidget->setCurrentIndex(0);
    ui->LocalIP->setText(GetLocalIP());
    ui->IPAddr->setText("10.200.19.16");
    ui->IPPort->setText(QString("%1").arg(D_SQLPORT));

    DeviceTableList << QString("设备信息")
                    << QString("设备操作记录")
                    << QString("设备反馈记录")
                    << QString("设备返修记录");
}

ManufacturePage::~ManufacturePage()
{
    if(m_SqlQuery != 0)
    {
        m_SqlQuery->deleteLater();
        m_SqlQuery = 0;
    }
    delete ui;
}

void ManufacturePage::on_ServerConnectBtn_clicked()
{
    QString tmpString = ui->ServerConnectBtn->text();
    if(tmpString == "Connect")
    {
        QString tmpIP = ui->IPAddr->text();
        ENET_ADDR tmpAddr(tmpIP);
        if(tmpAddr.isValid() == false)
        {
            QMessageBox::warning(this, tr("Error"),
                                       tr("IP is invalid, please correct!"),
                                 QMessageBox::Yes,
                                 QMessageBox::Yes);
            return;
        }

        QString tmpPortString = ui->IPPort->text();
        if(tmpPortString.isEmpty())
        {
            QMessageBox::warning(this, tr("Error"),
                                       tr("IPPort is invalid, please correct!"),
                                 QMessageBox::Yes,
                                 QMessageBox::Yes);
            return;
        }
        int tmpPort = tmpPortString.toInt();

        if(m_SqlQuery != 0)
        {
            m_SqlQuery->deleteLater();
            m_SqlQuery = 0;
        }
        m_SqlQuery = new SqlQuery(this);
        connect(m_SqlQuery, SIGNAL(sConnState(CONN_STATE)), this, SLOT(slot_SQLConnectState(CONN_STATE)));
        connect(m_SqlQuery, SIGNAL(sProgress(quint8)), this, SLOT(slot_TransFileProgress(quint8)));
        if(m_SqlQuery->Connect(tmpIP, tmpPort))
        {
            SqlConnected();
        }
    }
    else
    {
        SqlDisconnected();
    }
}

void ManufacturePage::slot_TransFileProgress(quint8 pValue)
{
    if(pValue == 100)
    {
        ui->BtnDownloadStart->setEnabled(true);
        ui->BtnDownloadPause->setEnabled(false);
        ui->BtnDownloadStop->setEnabled(false);
        GlobalPara.updateProgress(QString(""), 0);//进度条
    }
    else
    {
        GlobalPara.updateProgress(QString("Trans File ..."), pValue);//进度条
    }
}

void ManufacturePage::slot_SQLConnectState(CONN_STATE pState)
{
    switch(pState)
    {
        case Conn_SFDisconn:
        {
            SqlDisconnected();
            break;
        }
        case Conn_SFConn:
        {
            break;
        }
        default:
            break;
    }
}

void ManufacturePage::SqlConnected()
{
    m_CurBootloaderItem = 0;
    m_CurKernelItem = 0;
    m_CurFileSystemItem = 0;
    m_CurApplicationItem = 0;

    ui->DeviceTableCombox->clear();
    ui->DeviceTableCombox->addItems(DeviceTableList);
    ui->DeviceTableCombox->setCurrentIndex(0);

    ui->groupBox_2->setEnabled(true);
    ui->groupBox_3->setEnabled(true);
    ui->groupBox_4->setEnabled(true);
    ui->groupBox_5->setEnabled(true);
    ui->groupBox_6->setEnabled(true);
    ui->ServerConnectBtn->setText("Disconnect");
    ui->IPAddr->setEnabled(false);
    ui->IPPort->setEnabled(false);

    GlobalPara.updateProgress(QString("Initializing Bootloader!"),50);//进度条
    InitBootloaderList();
    GlobalPara.updateProgress(QString("Initializing Kernel!"),60);//进度条
    InitKernelList();
    GlobalPara.updateProgress(QString("Initializing FileSystem!"),70);//进度条
    InitFileSystemList();
    GlobalPara.updateProgress(QString("Initializing Application!"),80);//进度条
    InitApplicationList();

    GlobalPara.updateProgress(QString("Initializing Device Information!"),90);//进度条
    InitDeviceInfo(ui->BarcodeEdit->text());

    GlobalPara.updateProgress(QString("Connected!"),0);//进度条
    ui->BarcodeEdit->setFocus();
}

void ManufacturePage::SqlDisconnected()
{
    if(m_SqlQuery != 0)
    {
        m_SqlQuery->deleteLater();
        m_SqlQuery = 0;
    }

    ui->groupBox_2->setEnabled(false);
    ui->groupBox_3->setEnabled(false);
    ui->groupBox_4->setEnabled(false);
    ui->groupBox_5->setEnabled(false);
    ui->groupBox_6->setEnabled(false);
    ui->ServerConnectBtn->setText("Connect");
    ui->IPAddr->setEnabled(true);
    ui->IPPort->setEnabled(true);
}

void ManufacturePage::on_DeviceConnectBtn_clicked()
{

}

void ManufacturePage::on_BtnBarcodeSearch_clicked()
{
    on_DeviceTableCombox_currentIndexChanged(0);
    ui->BarcodeEdit->selectAll();
}

void ManufacturePage::on_BarcodeEdit_editingFinished()
{
    on_BtnBarcodeSearch_clicked();
}

void ManufacturePage::on_BtnBufferPathSelect_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"/",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;
    ui->BufferPathEdit->setText(dir);
}

void ManufacturePage::on_BtnDownloadStart_clicked()
{
    ui->BtnDownloadStart->setEnabled(false);
    ui->BtnDownloadPause->setEnabled(true);
    ui->BtnDownloadStop->setEnabled(true);

    connect(this,SIGNAL(TransFinishedInfo()),this,SLOT(TransFinished()));
    QString tmpDstPath = ui->BufferPathEdit->text();
    if(tmpDstPath.isEmpty())
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("Buffer path is empty, please correct!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    if(tmpDstPath.endsWith("/") == false)
        tmpDstPath.append("/");
    QDir tmpDir(tmpDstPath);
    if(tmpDir.exists() == false)
        tmpDir.mkpath(tmpDstPath);

    GlobalPara.updateProgress(QString("Downloading..."),10);//进度条

    QString tmpDatabaseFile, tmpBufferFile;
    for(int i=0;i<4;i++)
    {
        bool tmpContinueFlag = false;
        switch(i)
        {
            case 0:
            {
                //>@判断缓冲目录中是否已存在此镜像文件
                if(m_CurBootloaderItem == 0)
                {
                    tmpContinueFlag = true;
                    break;
                }
                tmpDatabaseFile = m_CurBootloaderItem->text(1);
                IDE_TRACE_STR(tmpDatabaseFile);
                tmpBufferFile = tmpDstPath + getFileName(tmpDatabaseFile);
                if(isFileExist(tmpBufferFile))
                {
                    int ret = QMessageBox::warning(this, tr("Warnning"),
                                                         tr("The current selected Bootloader file has exist in buffer direction,\ndo you want to overwrite!"),
                                                         QMessageBox::Yes | QMessageBox::No,
                                                         QMessageBox::No);
                    if(ret == QMessageBox::Yes)
                        delFile(tmpBufferFile);
                    else
                        tmpContinueFlag = true;
                }
                break;
            }
            case 1:
            {
                if(m_CurKernelItem == 0)
                {
                    tmpContinueFlag = true;
                    break;
                }
                tmpDatabaseFile = m_CurKernelItem->text(1);
                IDE_TRACE_STR(tmpDatabaseFile);
                tmpBufferFile = tmpDstPath + getFileName(tmpDatabaseFile);
                if(isFileExist(tmpBufferFile))
                {
                    int ret = QMessageBox::warning(this, tr("Warnning"),
                                                         tr("The current selected Kernel file has exist in buffer direction,\ndo you want to overwrite!"),
                                                         QMessageBox::Yes | QMessageBox::No,
                                                         QMessageBox::No);
                    if(ret == QMessageBox::Yes)
                        delFile(tmpBufferFile);
                    else
                        tmpContinueFlag = true;
                }
                break;
            }
            case 2:
            {
                if(m_CurFileSystemItem == 0)
                {
                    tmpContinueFlag = true;
                    break;
                }
                tmpDatabaseFile = m_CurFileSystemItem->text(1);
                IDE_TRACE_STR(tmpDatabaseFile);
                tmpBufferFile = tmpDstPath + getFileName(tmpDatabaseFile);
                if(isFileExist(tmpBufferFile))
                {
                    int ret = QMessageBox::warning(this, tr("Warnning"),
                                                         tr("The current selected FileSystem file has exist in buffer direction,\ndo you want to overwrite!"),
                                                         QMessageBox::Yes | QMessageBox::No,
                                                         QMessageBox::No);
                    if(ret == QMessageBox::Yes)
                        delFile(tmpBufferFile);
                    else
                        tmpContinueFlag = true;
                }
                break;
            }
            case 3:
            {
                if(m_CurApplicationItem == 0)
                {
                    tmpContinueFlag = true;
                    break;
                }
                tmpDatabaseFile = m_CurApplicationItem->text(1);
                IDE_TRACE_STR(tmpDatabaseFile);
                tmpBufferFile = tmpDstPath + getFileName(tmpDatabaseFile);
                if(isFileExist(tmpBufferFile))
                {
                    int ret = QMessageBox::warning(this, tr("Warnning"),
                                                         tr("The current selected Application file has exist in buffer direction,\ndo you want to overwrite!"),
                                                         QMessageBox::Yes | QMessageBox::No,
                                                         QMessageBox::No);
                    if(ret == QMessageBox::Yes)
                        delFile(tmpBufferFile);
                    else
                        tmpContinueFlag = true;
                }
                break;
            }
        }

        QFile file(tmpDatabaseFile);
        if(!file.exists())
        {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("%1 Is Not Exists.")
                                 .arg(tmpDatabaseFile));
            ui->BtnDownloadStart->setEnabled(true);
            ui->BtnDownloadPause->setEnabled(false);
            ui->BtnDownloadStop->setEnabled(false);
            return;
        }
        if(tmpContinueFlag)
            continue;
        IDE_TRACE();
        m_SqlQuery->m_HostApp->GetFile("Y", tmpDatabaseFile, tmpBufferFile, 10000);
        IDE_TRACE();
        GlobalPara.updateProgress(tr("Downloading %1 ...").arg(tmpDatabaseFile), 10 + 20*i);//进度条
    }
    GlobalPara.updateProgress(tr("").arg(tmpDatabaseFile), 100);
}

void ManufacturePage::on_BtnDownloadPause_clicked()
{
    ui->BtnDownloadStart->setEnabled(true);
    ui->BtnDownloadPause->setEnabled(false);
    ui->BtnDownloadStop->setEnabled(true);
}

void ManufacturePage::on_BtnDownloadStop_clicked()
{
    ui->BtnDownloadStart->setEnabled(true);
    ui->BtnDownloadPause->setEnabled(false);
    ui->BtnDownloadStop->setEnabled(false);
}

void ManufacturePage::DelTreeAllItem(QTreeWidgetItem *parentItem)
{
    QList<QTreeWidgetItem*> ItemList = parentItem->takeChildren();
    QTreeWidgetItem* tmpItem;
    for(int i=0;i<ItemList.count();i++)
    {
        tmpItem = ItemList.at(i);
        parentItem->removeChild(tmpItem);
    }
}

//>@返回父item
QTreeWidgetItem *ManufacturePage::DelTreeItem(QTreeWidgetItem *pItem)
{
    QTreeWidgetItem* tmpParentItem = pItem->parent();
    if(tmpParentItem == 0)
        return 0;
    tmpParentItem->removeChild(pItem);
    return tmpParentItem;
}

//>@返回添加的Item
QTreeWidgetItem *ManufacturePage::AddTreeItem(QTreeWidgetItem *parentItem, QStringList contentList, bool Editable)
{
    static int i = 0;
    QTreeWidgetItem *pNewItem = new QTreeWidgetItem(parentItem, contentList);
    pNewItem->setTextAlignment(0, Qt::AlignLeft || Qt::AlignVCenter);
    pNewItem->setTextAlignment(1, Qt::AlignLeft || Qt::AlignVCenter);
    pNewItem->setBackgroundColor(0, (i%2)?QColor(254,235,208):QColor(255,250,179));
    pNewItem->setBackgroundColor(1, (i%2)?QColor(254,235,208):QColor(255,250,179));
    pNewItem->setFont(0, QFont());
    if(Editable)
        pNewItem->setFlags(Qt::ItemIsEditable |Qt::ItemIsEnabled);
    parentItem->addChild(pNewItem);
    i++;
    return pNewItem;
}

void ManufacturePage::InitBootloaderList()
{
    QTreeWidgetItem *tmpTreeItem = ui->BootloaderList->topLevelItem(0);
    DelTreeAllItem(tmpTreeItem);
    QList<QVariant> tmpRev = m_SqlQuery->QueryDatas(QString("资源管理"),
                                                    QString("资源版本,存储位置"),
                                                    QString("资源类型='Bootloader'"));
    for(int i=0; i<tmpRev.count(); i++)
    {
        QVariant tmpVariant = tmpRev.at(i);
        QStringList tmpList = tmpVariant.toStringList();
        AddTreeItem(tmpTreeItem, tmpList, false);
    }
    ui->BootloaderList->expandAll();
}

void ManufacturePage::InitKernelList()
{
    QTreeWidgetItem *tmpTreeItem = ui->KernelList->topLevelItem(0);
    DelTreeAllItem(tmpTreeItem);
    QList<QVariant> tmpRev = m_SqlQuery->QueryDatas(QString("资源管理"),
                                                    QString("资源版本,存储位置"),
                                                    QString("资源类型='Kernel'"));
    for(int i=0; i<tmpRev.count(); i++)
    {
        QVariant tmpVariant = tmpRev.at(i);
        QStringList tmpList = tmpVariant.toStringList();
        AddTreeItem(tmpTreeItem, tmpList, false);
    }
    ui->KernelList->expandAll();
}

void ManufacturePage::InitFileSystemList()
{
    QTreeWidgetItem *tmpTreeItem = ui->FileSystemList->topLevelItem(0);
    DelTreeAllItem(tmpTreeItem);
    QList<QVariant> tmpRev = m_SqlQuery->QueryDatas(QString("资源管理"),
                                                    QString("资源版本,存储位置"),
                                                    QString("资源类型='FileSystem'"));
    for(int i=0; i<tmpRev.count(); i++)
    {
        QVariant tmpVariant = tmpRev.at(i);
        QStringList tmpList = tmpVariant.toStringList();
        AddTreeItem(tmpTreeItem, tmpList, false);
    }
    ui->FileSystemList->expandAll();
}

void ManufacturePage::InitApplicationList()
{
    QTreeWidgetItem *tmpTreeItem = ui->ApplicationList->topLevelItem(0);
    DelTreeAllItem(tmpTreeItem);
    QList<QVariant> tmpRev = m_SqlQuery->QueryDatas(QString("资源管理"),
                                                    QString("资源版本,存储位置"),
                                                    QString("资源类型='Application'"));
    for(int i=0; i<tmpRev.count(); i++)
    {
        QVariant tmpVariant = tmpRev.at(i);
        QStringList tmpList = tmpVariant.toStringList();
        AddTreeItem(tmpTreeItem, tmpList, false);
    }
    ui->ApplicationList->expandAll();
}

void ManufacturePage::InitDeviceInfo(QString pBarCode)
{
    QString tmpCondition;
    if(pBarCode.isEmpty())
        tmpCondition = QString("");
    else
        tmpCondition = QString("where 设备号='%1'").arg(pBarCode);
//    Qt::CheckState usercheck = ui->UserCheckbox->checkState();
//    Qt::CheckState timecheck = ui->TimeCheckbox->checkState();
    QString tmpQueryString = QString("select %1 from %2 %3")
                             .arg("*")
                             .arg(ui->DeviceTableCombox->currentText())
                             .arg(tmpCondition);
    if(m_SqlQuery->ExecSqlQuery(tmpQueryString))
    {
        IDE_TRACE();
        ui->DeviceOperateLog->setModel(m_SqlQuery->m_SqlQueryModel);
    }
    else
    {
        ui->DeviceOperateLog->setModel(new QSqlQueryModel);
        QMessageBox::warning(this, tr("Error"),
                             m_SqlQuery->m_LastErrorString,
                             QMessageBox::Ok,
                             QMessageBox::Ok);
    }
}

void ManufacturePage::on_DeviceTableCombox_currentIndexChanged(int index)
{
    QString tmpBarCode = ui->BarcodeEdit->text();
    InitDeviceInfo(tmpBarCode);
}

void ManufacturePage::on_BtnBootloaderAdd_clicked()
{
    QTreeWidgetItem *tmpTreeItem = ui->BootloaderList->topLevelItem(1);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Bootloader Image File"),
                                         QDir::currentPath(),
                                         tr("Bootloader files (*.bin);;"
                                            "All files (*.*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.exists())
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The select file is not exist,\nplease select again!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    QStringList tmpList;
    tmpList.append(getFileName(fileName));
    tmpList.append(fileName);
    AddTreeItem(tmpTreeItem, tmpList, false);
    ui->BootloaderList->expandAll();
}

void ManufacturePage::on_BtnBootloaderDel_clicked()
{
    if(m_CurBootloaderItem == 0)
        return;
    QTreeWidgetItem* tmpParentItem = m_CurBootloaderItem->parent();
    if(tmpParentItem == 0)  //>@当前选中的是顶级项，此时将右下边的状态机恢复到默认即可
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The select item is topItem,\nplease select again!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    else if(tmpParentItem->text(0).compare(tr("DataBase")) == 0)
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The select item is Database item,\nplease select again!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    else
    {
        DelTreeItem(m_CurBootloaderItem);
    }
}

void ManufacturePage::on_BtnBootloaderRefresh_clicked()
{

}

void ManufacturePage::on_BootloaderList_itemSelectionChanged()
{
    QList<QTreeWidgetItem*> tmpList = ui->BootloaderList->selectedItems();
    if(tmpList.count() <= 0)
        return;
    m_CurBootloaderItem = tmpList.at(0);
    ui->BootloaderSelectLabel->setText(m_CurBootloaderItem->text(0));
}

void ManufacturePage::on_BtnKernelAdd_clicked()
{
    QTreeWidgetItem *tmpTreeItem = ui->KernelList->topLevelItem(1);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Kernel Image File"),
                                         QDir::currentPath(),
                                         tr("Kernel files (*.bin);;"
                                            "All files (*.*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.exists())
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The select file is not exist,\nplease select again!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    QStringList tmpList;
    tmpList.append(getFileName(fileName));
    tmpList.append(fileName);
    AddTreeItem(tmpTreeItem, tmpList, false);
    ui->KernelList->expandAll();
}

void ManufacturePage::on_BtnKernelDel_clicked()
{
    if(m_CurKernelItem == 0)
        return;
    QTreeWidgetItem* tmpParentItem = m_CurKernelItem->parent();
    if(tmpParentItem == 0)  //>@当前选中的是顶级项，此时将右下边的状态机恢复到默认即可
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The select item is topItem,\nplease select again!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    else if(tmpParentItem->text(0).compare(tr("DataBase")) == 0)
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The select item is Database item,\nplease select again!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    else
    {
        DelTreeItem(m_CurKernelItem);
    }
}

void ManufacturePage::on_BtnKernelRefresh_clicked()
{

}

void ManufacturePage::on_KernelList_itemSelectionChanged()
{
    QList<QTreeWidgetItem*> tmpList = ui->KernelList->selectedItems();
    if(tmpList.count() <= 0)
        return;
    m_CurKernelItem = tmpList.at(0);
    ui->KernelSelectLabel->setText(m_CurKernelItem->text(0));
}

void ManufacturePage::on_BtnFsAdd_clicked()
{
    QTreeWidgetItem *tmpTreeItem = ui->FileSystemList->topLevelItem(1);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open FileSystem Image File"),
                                         QDir::currentPath(),
                                         tr("FileSystem files (*.bin);;"
                                            "All files (*.*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.exists())
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The select file is not exist,\nplease select again!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    QStringList tmpList;
    tmpList.append(getFileName(fileName));
    tmpList.append(fileName);
    AddTreeItem(tmpTreeItem, tmpList, false);
    ui->FileSystemList->expandAll();
}

void ManufacturePage::on_BtnFsDel_clicked()
{
    if(m_CurFileSystemItem == 0)
        return;
    QTreeWidgetItem* tmpParentItem = m_CurFileSystemItem->parent();
    if(tmpParentItem == 0)  //>@当前选中的是顶级项，此时将右下边的状态机恢复到默认即可
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The select item is topItem,\nplease select again!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    else if(tmpParentItem->text(0).compare(tr("DataBase")) == 0)
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The select item is Database item,\nplease select again!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    else
    {
        DelTreeItem(m_CurFileSystemItem);
    }
}

void ManufacturePage::on_BtnFsRefresh_clicked()
{

}

void ManufacturePage::on_FileSystemList_itemSelectionChanged()
{
    QList<QTreeWidgetItem*> tmpList = ui->FileSystemList->selectedItems();
    if(tmpList.count() <= 0)
        return;
    m_CurFileSystemItem = tmpList.at(0);
    ui->FileSystemSelectLabel->setText(m_CurFileSystemItem->text(0));
}

void ManufacturePage::on_BtnAppAdd_clicked()
{
    QTreeWidgetItem *tmpTreeItem = ui->ApplicationList->topLevelItem(1);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Application Image File"),
                                         QDir::currentPath(),
                                         tr("Application files (*.bin);;"
                                            "All files (*.*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.exists())
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The select file is not exist,\nplease select again!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    QStringList tmpList;
    tmpList.append(getFileName(fileName));
    tmpList.append(fileName);
    AddTreeItem(tmpTreeItem, tmpList, false);
    ui->ApplicationList->expandAll();
}

void ManufacturePage::on_BtnAppDel_clicked()
{
    if(m_CurApplicationItem == 0)
        return;
    QTreeWidgetItem* tmpParentItem = m_CurApplicationItem->parent();
    if(tmpParentItem == 0)  //>@当前选中的是顶级项，此时将右下边的状态机恢复到默认即可
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The select item is topItem,\nplease select again!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    else if(tmpParentItem->text(0).compare(tr("DataBase")) == 0)
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("The select item is Database item,\nplease select again!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }
    else
    {
        DelTreeItem(m_CurApplicationItem);
    }
}

void ManufacturePage::on_BtnAppRefresh_clicked()
{

}

void ManufacturePage::on_ApplicationList_itemSelectionChanged()
{
    QList<QTreeWidgetItem*> tmpList = ui->ApplicationList->selectedItems();
    if(tmpList.count() <= 0)
        return;
    m_CurApplicationItem = tmpList.at(0);
    ui->ApplicationSelectLabel->setText(m_CurApplicationItem->text(0));
}
