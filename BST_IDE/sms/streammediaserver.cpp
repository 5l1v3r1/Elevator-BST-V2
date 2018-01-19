#include "streammediaserver.h"
#include "ui_streammediaserver.h"

StreamMediaServer::StreamMediaServer(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::StreamMediaServer)
{
    ui->setupUi(this);

    m_Process = 0;
    ui->ServerConfEdit->setText(getFileDirectory(D_FFMPEG_PATH) + D_FFSERVERCONF);

    ServerClosed();

    m_AddToMediaList = new QAction(QIcon(":/Menu/rc/Menu/plus.png"), tr("&Add To MediaList"), this);
    m_AddToMediaList->setStatusTip(tr("Add To MediaList"));
    connect(m_AddToMediaList, SIGNAL(triggered()), this, SLOT(slot_AddToMediaList()));

    m_SetMediaUp = new QAction(QIcon(":/Menu/rc/Menu/up.png"), tr("Move &UP"), this);
    m_SetMediaUp->setStatusTip(tr("Move Media UP"));
    connect(m_SetMediaUp, SIGNAL(triggered()), this, SLOT(slot_SetMediaUp()));

    m_SetMediaDown = new QAction(QIcon(":/Menu/rc/Menu/down.png"), tr("Move &DOWN"), this);
    m_SetMediaDown->setStatusTip(tr("Move Media DOWN"));
    connect(m_SetMediaDown, SIGNAL(triggered()), this, SLOT(slot_SetMediaDown()));

    m_DelMedia = new QAction(QIcon(":/Menu/rc/Menu/minus.png"), tr("&Delete"), this);
    m_DelMedia->setStatusTip(tr("Delete Media"));
    connect(m_DelMedia, SIGNAL(triggered()), this, SLOT(slot_DelMedia()));

    m_PlayMedia = new QAction(QIcon(":/Menu/rc/Menu/build.png"), tr("&Play"), this);
    m_PlayMedia->setStatusTip(tr("Play Media"));
    connect(m_PlayMedia, SIGNAL(triggered()), this, SLOT(slot_PlayMedia()));

    m_DelUser = new QAction(QIcon(":/Menu/rc/Menu/minus.png"), tr("&Delete"), this);
    m_DelUser->setStatusTip(tr("Delete User"));
    connect(m_DelUser, SIGNAL(triggered()), this, SLOT(slot_DelUser()));
}

StreamMediaServer::~StreamMediaServer()
{
    SYSTEM_CMD::SetMonitor(false);
    SYSTEM_CMD::SetSMS(false);
    delete ui;
}

void StreamMediaServer::slot_AddToMediaList()
{
    QTreeWidgetItem *tmpTreeItem = ui->MediaTreeList->currentItem();
    if(tmpTreeItem == 0)
        return;
    QString tmpString = tmpTreeItem->statusTip(0);
    if(tmpString.isEmpty())
        return;
    QFileInfo tmpFileInfo(tmpString);
    if(tmpFileInfo.isDir())
        return;
    AddMedia(tmpString);
}

void StreamMediaServer::slot_SetMediaUp()
{
    int index = ui->MediaQueneList->currentIndex().row();
    if(index <= 0 || index >= m_MediaList.count())
        return;
    m_MediaList.move(index, index-1);
    UpdateMediaList();
}

void StreamMediaServer::slot_SetMediaDown()
{
    int index = ui->MediaQueneList->currentIndex().row();
    if(index < 0 || index >= m_MediaList.count()-1)
        return;
    m_MediaList.move(index, index+1);
    UpdateMediaList();
}

void StreamMediaServer::slot_DelMedia()
{
    int index = ui->MediaQueneList->currentIndex().row();
    DelMedia(index);
}

void StreamMediaServer::slot_PlayMedia()
{

}

void StreamMediaServer::slot_DelUser()
{

}

void StreamMediaServer::on_MediaType_currentIndexChanged(const QString &arg1)
{
    if(arg1.compare("Camera", Qt::CaseInsensitive) == 0)
    {
        DelChildTreeItem(ui->MediaTreeList->topLevelItem(0));
        ui->MeidaListGroupBox->setDisabled(true);
        //>@遍历所有的采集设备
#ifdef WINDOWS
        AddTreeItem(ui->MediaTreeList->topLevelItem(0),"0", QIcon(":/Menu/rc/Menu/monitor.png"));
        AddTreeItem(ui->MediaTreeList->topLevelItem(0),"1", QIcon(":/Menu/rc/Menu/monitor.png"));
        AddTreeItem(ui->MediaTreeList->topLevelItem(0),"2", QIcon(":/Menu/rc/Menu/monitor.png"));
        AddTreeItem(ui->MediaTreeList->topLevelItem(0),"3", QIcon(":/Menu/rc/Menu/monitor.png"));
#else
        AddTreeItem(ui->MediaTreeList->topLevelItem(0),"/dev/video0", QIcon(":/Menu/rc/Menu/monitor.png"));
        AddTreeItem(ui->MediaTreeList->topLevelItem(0),"/dev/video1", QIcon(":/Menu/rc/Menu/monitor.png"));
        AddTreeItem(ui->MediaTreeList->topLevelItem(0),"/dev/video2", QIcon(":/Menu/rc/Menu/monitor.png"));
        AddTreeItem(ui->MediaTreeList->topLevelItem(0),"/dev/video3", QIcon(":/Menu/rc/Menu/monitor.png"));
#endif
        ui->MediaTreeList->expandAll();
        ui->MeidaListGroupBox->setDisabled(false);
    }
    else if(ui->MediaPath->text().isEmpty() == false)
    {
        MediaTreeListUpdate(ui->MediaPath->text(), arg1);
    }
}

//>@
void StreamMediaServer::on_MediaPath_textEdited(const QString &arg1)
{
    MediaTreeListUpdate(ui->MediaPath->text(), ui->MediaType->currentText());
}

void StreamMediaServer::on_MediaPathSelect_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/",
                                                    QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;
    ui->MediaPath->setText(dir);
    MediaTreeListUpdate(ui->MediaPath->text(), ui->MediaType->currentText());
}

void StreamMediaServer::DelChildTreeItem(QTreeWidgetItem *parentItem)
{
    QList<QTreeWidgetItem*> ItemList = parentItem->takeChildren();
    QTreeWidgetItem* tmpItem;
    for(int i=0;i<ItemList.count();i++)
    {
        tmpItem = ItemList.at(i);
        if(tmpItem == 0)
            continue;
        DelChildTreeItem(tmpItem);
        delete tmpItem;
    }
}

//>@?item
QTreeWidgetItem *StreamMediaServer::DelTreeItem(QTreeWidgetItem *pItem)
{
    if(pItem == 0)
        return (QTreeWidgetItem *)0;
    QTreeWidgetItem* tmpParentItem  = pItem->parent();
    if(tmpParentItem == 0)
        return 0;
    tmpParentItem->removeChild(pItem);
    delete pItem;
    return tmpParentItem;
}

//>@?Item
QTreeWidgetItem *StreamMediaServer::AddTreeItem(QTreeWidgetItem *parentItem, QString content, QIcon pIcon, bool Editable)
{
    if(parentItem == 0)
        return (QTreeWidgetItem *)0;
    QTreeWidgetItem *pNewItem = new QTreeWidgetItem(parentItem, QStringList(getFileName(content)));
    pNewItem->setStatusTip(0, content);
    pNewItem->setTextAlignment(0, Qt::AlignLeft || Qt::AlignVCenter);
    //pNewItem->setBackgroundColor(0, (i%2)?QColor(254,235,208):QColor(255,250,179));
    pNewItem->setFont(0, QFont());
    pNewItem->setIcon(0, pIcon);
    if(Editable)
        pNewItem->setFlags(Qt::ItemIsEditable |Qt::ItemIsEnabled);
    parentItem->addChild(pNewItem);
    return pNewItem;
}

void StreamMediaServer::ScanDirToMediaList(QTreeWidgetItem *parentItem, QString pPath, QStringList pSuffixList)
{
    if(parentItem == 0)
        return;
    if(!pPath.endsWith("/"))
        pPath.append("/");
    QDir tmpDir(pPath);
    if(!tmpDir.exists())
        return;
    QStringList folders = tmpDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < folders.size(); ++i)
    {
        QString tmpSubPath = QString("%1%2").arg(pPath).arg(folders[i]);
        QTreeWidgetItem *tmpSubDirTreeItem = AddTreeItem(parentItem, tmpSubPath, QIcon(":/Menu/rc/Menu/dir.png"));
        ScanDirToMediaList(tmpSubDirTreeItem, tmpSubPath, pSuffixList);
        QtSleep(1);
    }
    QStringList files = findFiles(pPath, "*", "", Qt::CaseInsensitive);
    for(int i = 0; i < files.size(); ++i)
    {
        QFileInfo tmpFileInfo(files[i]);
        QString tmpSuffix = tmpFileInfo.suffix();
        for(int j=0;j<pSuffixList.count();j++)
        {
            if(tmpSuffix.compare(pSuffixList[j], Qt::CaseInsensitive) == 0)
            {
                AddTreeItem(parentItem, files[i], QIcon(QString(":/Page/rc/Page/%1.png").arg(tmpSuffix.toLower())));
                break;
            }
        }
    }
}

void StreamMediaServer::MediaTreeListUpdate(QString pPath, QString pFileType)
{
    DelChildTreeItem(ui->MediaTreeList->topLevelItem(0));
    ui->MeidaListGroupBox->setDisabled(true);
    QStringList tmpSuffixList;
    if(pFileType.compare("All", Qt::CaseInsensitive) == 0)
        tmpSuffixList << "asf" << "flv" << "rm" << "wav" << "mp3" << "png" << "jpg" << "bmp" << "gif";
    else if(pFileType.compare("Video", Qt::CaseInsensitive) == 0)
        tmpSuffixList << "asf" << "flv" << "rm";
    else if(pFileType.compare("Audio", Qt::CaseInsensitive) == 0)
        tmpSuffixList << "wav" << "mp3";
    else if(pFileType.compare("Image", Qt::CaseInsensitive) == 0)
        tmpSuffixList << "png" << "jpg" << "bmp" << "gif";
    ScanDirToMediaList(ui->MediaTreeList->topLevelItem(0), pPath, tmpSuffixList);
    ui->MediaTreeList->expandAll();
    ui->MeidaListGroupBox->setDisabled(false);
}

void StreamMediaServer::UpdateMediaList()
{
    ui->MediaQueneList->clear();
    for(int i=0;i<m_MediaList.count();i++)
    {
        MEDIAINFO tmpMediaInfo = m_MediaList.at(i);
        QListWidgetItem *tmpListItem = new QListWidgetItem(getFileName(tmpMediaInfo.mPath));
        tmpListItem->setStatusTip(tmpMediaInfo.mPath);
        ui->MediaQueneList->addItem(tmpListItem);
    }
}

void StreamMediaServer::AddMedia(QString pMediaPath)
{
    for(int i=0;i<m_MediaList.count();i++)
    {
        MEDIAINFO tmpMediaInfo = m_MediaList.at(i);
        if(pMediaPath.compare(tmpMediaInfo.mPath, Qt::CaseInsensitive) == 0)
            return;
    }
    MEDIAINFO tmpMediaInfo;
    tmpMediaInfo.mPath = pMediaPath;
    tmpMediaInfo.mIsPlayed = false;
    m_MediaList.append(tmpMediaInfo);
    UpdateMediaList();
}

void StreamMediaServer::DelMedia(QString pMediaPath)
{
    for(int i=0;i<m_MediaList.count();i++)
    {
        MEDIAINFO tmpMediaInfo = m_MediaList.at(i);
        if(pMediaPath.compare(tmpMediaInfo.mPath, Qt::CaseInsensitive) == 0)
        {
            DelMedia(i);
            return;
        }
    }
}

void StreamMediaServer::DelMedia(int pUserID)
{
    if(pUserID < 0 || pUserID >= m_MediaList.count())
        return;
    m_MediaList.removeAt(pUserID);
    UpdateMediaList();
}

void StreamMediaServer::on_MediaTreeList_itemPressed(QTreeWidgetItem *item, int column)
{
    if (QApplication::mouseButtons() == Qt::RightButton)
    {
        QMenu menu(tr("&Menu"),ui->MediaTreeList);
        menu.addAction(m_AddToMediaList);
        menu.exec(QCursor::pos());
    }
}

void StreamMediaServer::on_MediaQueneList_itemPressed(QListWidgetItem *item)
{
    if (QApplication::mouseButtons() == Qt::RightButton)
    {
        QMenu menu(tr("&Menu"),ui->MediaQueneList);
        menu.addAction(m_PlayMedia);
        menu.addAction(m_SetMediaUp);
        menu.addAction(m_SetMediaDown);
        menu.addAction(m_DelMedia);
        menu.exec(QCursor::pos());
    }
}

void StreamMediaServer::ServerOpened()
{
    ui->MediaQueneGroupBox->setEnabled(true);
    ui->BtnOpenServer->setIcon(QIcon(":/Menu/rc/Menu/pass.png"));
    ui->BtnOpenServer->setText("CloseServer");
}

void StreamMediaServer::ServerClosed()
{
    ui->MediaQueneGroupBox->setDisabled(true);
    ui->BtnOpenServer->setIcon(QIcon(":/Menu/rc/Menu/notpass.png"));
    ui->BtnOpenServer->setText("OpenServer");
}

void StreamMediaServer::slot_OutputCommand()
{
    QString cmdoutput;
    cmdoutput.append(m_Process->readAllStandardOutput());
    cmdoutput.append(m_Process->readAllStandardError());
    GlobalPara.outputMessage(cmdoutput);
}

void StreamMediaServer::on_BtnOpenServer_clicked()
{
    QString tmpString = ui->BtnOpenServer->text();
    if(tmpString.compare("OpenServer",Qt::CaseInsensitive) == 0)
    {
        //>@开启服务器
        QString tmpConfFile = ui->ServerConfEdit->text();
        QFile tmpFile(tmpConfFile);
        if(tmpFile.exists() == false)
        {
            QMessageBox::warning(this, tr("Error"),
                                 tr("The ffserver configure file is invalid!"));
            return;
        }

        SYSTEM_CMD::SetMonitor(false);
        SYSTEM_CMD::SetSMS(false);
        QtSleep(1);
        if(SYSTEM_CMD::SetSMS(true, tmpConfFile) == false)
        {
            QMessageBox::warning(this, tr("Error"), tr("Open Server Failed!"));
            SYSTEM_CMD::SetSMS(false);
            return;
        }
        ServerOpened();
        QtSleep(1);
        UpdateMediaSource();
    }
    else if(tmpString.compare("CloseServer",Qt::CaseInsensitive) == 0)
    {
        //>@关闭服务器
        SYSTEM_CMD::SetMonitor(false);
        SYSTEM_CMD::SetSMS(false);
        ServerClosed();
    }
}

void StreamMediaServer::UpdateMediaSource()
{
    if(m_MediaList.count() <= 0)
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("Has no meida for streaming, please check it!"));
        return;
    }
    MEDIAINFO tmpMediaInfo = m_MediaList.first();
    SYSTEM_CMD::SetMonitor(false);
    if(SYSTEM_CMD::SetMonitor(true, tmpMediaInfo.mPath, 10, RESOLUTION(640, 480)) == false)
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("Capture %1 Failed!").arg(tmpMediaInfo.mPath));
        SYSTEM_CMD::SetMonitor(false);
    }
}

void StreamMediaServer::on_BtnStart_clicked()
{
    //>@播放当前流媒体视频
    unsigned long id = (unsigned long)ui->label->winId();
#ifdef WINDOWS
    QString cmd = QString("start /b %1 http://127.0.0.1:8090/buffer.rm -wid %2").arg(D_MPLAYER_PATH).arg(id);
#else
    QString cmd = QString("%1 http://127.0.0.1:8090/test.rm -nocache -slave -wid %2 &").arg(D_MPLAYER_PATH).arg(id);
#endif
    system(cmd.toLatin1().data());
    IDE_TRACE_STR(cmd);
}

void StreamMediaServer::on_BtnStop_clicked()
{
    //>@关闭当前流媒体视频
    SYSTEM_CMD::KillProcess(D_MPLAYER_PATH);
}

void StreamMediaServer::on_ServerConfSelect_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Server configure file"),
                                                    QDir::currentPath(),
                                                    tr("conf files (*.conf);;"
                                                       "All files (*.*)"));
    if (fileName.isEmpty())
        return;
    ui->ServerConfEdit->setText(fileName);
}

void StreamMediaServer::on_MediaQueneList_doubleClicked(const QModelIndex &index)
{
    MEDIAINFO tmpMediaInfo = m_MediaList.at(index.row());
    SYSTEM_CMD::SetMonitor(false);
    if(SYSTEM_CMD::SetMonitor(true, tmpMediaInfo.mPath, 10, RESOLUTION(640, 480)) == false)
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("Capture %1 Failed!").arg(tmpMediaInfo.mPath));
        SYSTEM_CMD::SetMonitor(false);
    }
}
