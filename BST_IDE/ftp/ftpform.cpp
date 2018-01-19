#include "ftpform.h"
#include "ui_ftpform.h"

FtpForm::FtpForm(QString pFtpAddr, QWidget *parent)
    : TabPage(parent),
      ui(new Ui::Ftp),
      ftpobj(0)
{
    ui->setupUi(this);
    ENET_ADDR tmpAddr(pFtpAddr);
    if(tmpAddr.isValid())
    {
        IDE_TRACE_STR(pFtpAddr);
        ui->FtpAddress->insertItem(0, pFtpAddr);
        ui->FtpAddress->setCurrentIndex(0);
    }

    m_CurListItemFile.setFile("");

    ui->listView->setViewMode(QListView::ListMode);
    ui->listView->setGridSize(QSize(60,20));
    ui->listView->setEditTriggers(QListView::NoEditTriggers);
    LocalDirModel=new QDirModel(nameFilter,QDir::AllEntries|QDir::NoDotAndDotDot,QDir::Name|QDir::DirsFirst|QDir::Type);
    LocalDirModel->setReadOnly(false);
    ui->listView->setModel(LocalDirModel);
    ui->listView->setRootIndex(LocalDirModel->index("/"));
    connect(ui->listView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickListItem(QModelIndex)));
    connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(selectListItem(QModelIndex)));

    ui->Passwd->setEchoMode(QLineEdit::Password);   //password
    ui->UserName->setText("lds");
    ui->Passwd->setText("lds");

    ui->treeWidget->setAcceptDrops(true);
    setAcceptDrops(true);
    ui->treeWidget->setRootIsDecorated(false);
    ui->treeWidget->header()->hide();
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setColumnWidth(0,ui->treeWidget->width());
    connect(ui->treeWidget, SIGNAL(itemActivated(QTreeWidgetItem *, int)), this, SLOT(doubleClickTreeItem(QTreeWidgetItem *, int)));

    connect(ui->BtnConnect, SIGNAL(clicked()), this, SLOT(connectOrDisconnect()));
    connect(ui->BtnDownLoad, SIGNAL(clicked()), this, SLOT(DownLoad()));
    connect(ui->BtnUpLoad, SIGNAL(clicked()), this, SLOT(Upload()));

    ftpobj = 0;
    pos=0;
    Local_CurrentPath = "";
    pathList.append("/");

    Disconnected();
}

FtpForm::~FtpForm()
{
    ReleaseFTP();
    delete ui;
}

void FtpForm::InitFTP(QString pFtpAddr, quint16 pFtpPort, QString pUsrname, QString pPasswd, QString pDefaultPath)
{
    ReleaseFTP();

    ftpobj = new QFtp(this);
    connect(ftpobj, SIGNAL(stateChanged(int)), this, SLOT(FtpStateChanged(int)));
    connect(ftpobj, SIGNAL(commandFinished(int, bool)), this, SLOT(FtpCommandFinished(int, bool)));
    connect(ftpobj, SIGNAL(listInfo(const QUrlInfo &)), this, SLOT(addToFileList(const QUrlInfo &)));
    connect(ftpobj, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(updateDataTransferProgress(qint64, qint64)));

    QUrl url(pFtpAddr);
    if (!url.isValid() || url.scheme().toLower() != QLatin1String("ftp"))
    {
        if(ConnectToHost(pFtpAddr, pFtpPort, 100) && LoginFtp(pUsrname, pPasswd, 100))
        {
            Connected();
            ui->treeWidget->setFocus();
        }
        else
        {
            ReleaseFTP();
            Disconnected();
            return;
        }
    }
    else
    {
        if(ConnectToHost(url.host(), url.port(pFtpPort), 100) && LoginFtp(pUsrname, pPasswd, 100))
        {
            Connected();
            ui->treeWidget->setFocus();
        }
        else
        {
            ReleaseFTP();
            Disconnected();
            return;
        }
    }
    if(pDefaultPath.isEmpty())
    {
        GotoFtpPath("/", 100);
    }
    else
    {
        GotoFtpPath(pDefaultPath, 100);
    }
}

void FtpForm::FtpStateChanged(int pState)
{
    switch(pState)
    {
        case QFtp::Unconnected:
        {
//            ReleaseFTP();
//            Disconnected();
            break;
        }
        case QFtp::Closing:
        {
            ReleaseFTP();
            Disconnected();
            break;
        }
    }
}

void FtpForm::ReleaseFTP()
{
    if (ftpobj)
    {
        disconnect(ftpobj, SIGNAL(stateChanged(int)), 0, 0);
        disconnect(ftpobj, SIGNAL(commandFinished(int, bool)), 0, 0);
        disconnect(ftpobj, SIGNAL(listInfo(const QUrlInfo &)), 0, 0);
        disconnect(ftpobj, SIGNAL(dataTransferProgress(qint64, qint64)), 0, 0);
        ftpobj->abort();
        ftpobj->close();
        ftpobj->deleteLater();
        ftpobj = 0;
    }
}

void FtpForm::Connected()
{
    ui->BtnConnect->setText("Disconnect");
    ui->BtnConnect->setIcon(QIcon(":/Menu/rc/Menu/pass.png"));

    ui->FtpAddress->setEnabled(false);
    ui->FtpPort->setEnabled(false);
    ui->AuthenticationGroup->setEnabled(false);

    ui->Upwards->setEnabled(true);
    ui->RemoteCatalog->setEnabled(true);
    ui->treeWidget->setEnabled(true);
    ui->BtnDownLoad->setEnabled(true);
    ui->BtnUpLoad->setEnabled(true);

    ui->BtnAbort->setEnabled(true);
    ui->BtnRefresh->setEnabled(true);
}

void FtpForm::Disconnected()
{
    ui->BtnConnect->setText("Connect");
    ui->BtnConnect->setIcon(QIcon(":/Menu/rc/Menu/notpass.png"));

    ui->FtpAddress->setEnabled(true);
    ui->FtpPort->setEnabled(true);
    ui->AuthenticationGroup->setEnabled(true);

    ui->Upwards->setEnabled(false);
    ui->RemoteCatalog->setEnabled(false);
    ui->treeWidget->setEnabled(false);
    ui->BtnDownLoad->setEnabled(false);
    ui->BtnUpLoad->setEnabled(false);

    ui->BtnAbort->setEnabled(false);
    ui->BtnRefresh->setEnabled(false);
}

void FtpForm::connectOrDisconnect()
{
    //>@连接
    if(ui->BtnConnect->text().compare("Connect", Qt::CaseInsensitive) == 0)
    {
        if(ui->AuthenticationGroup->isChecked())
        {
            if(ui->UserName->text().isEmpty()||ui->Passwd->text().isEmpty())
             {
                QMessageBox::information(this,
                                         tr("FTP"),
                                         tr("请检查ftp账号信息，用户名和密码都不允许为空。"),
                                         QMessageBox::Ok);
                return ;
            }
        }
        if(ui->AuthenticationGroup->isChecked())
        {
            IDE_TRACE();
            InitFTP(ui->FtpAddress->currentText(), ui->FtpPort->value(), ui->UserName->text(), ui->Passwd->text());
        }
        else
        {
            IDE_TRACE();
            InitFTP(ui->FtpAddress->currentText(), ui->FtpPort->value());
        }
    }
    else
    {
        m_CurDirFileList.clear();
        ui->treeWidget->clear();
        ReleaseFTP();
        Disconnected();
    }
}

QTreeWidgetItem *FtpForm::CreateTreeItem(QUrlInfo urlInfo)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, urlInfo.name());
    item->setIcon(0, QIcon(urlInfo.isDir() ? (":/Menu/rc/Menu/dir.png"):(":/Menu/rc/Menu/new.png")));
    if(urlInfo.isDir())
        item->setWhatsThis(0,"dir");
    else if(urlInfo.isFile())
        item->setWhatsThis(0,"file");
    return item;
}

void FtpForm::UpdateFtpTree()
{
    ui->treeWidget->clear();
    if(m_CurDirFileList.count() <= 0)
    {
        ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList() << tr("<empty>")));
        return;
    }
    IDE_TRACE_INT(m_CurDirFileList.count());
    for(int i=0;i<m_CurDirFileList.count();i++)
    {
        QUrlInfo tmpUrl = m_CurDirFileList.at(i);
        QTreeWidgetItem *tmpItem = CreateTreeItem(tmpUrl);
        ui->treeWidget->addTopLevelItem(tmpItem);
    }
    if (!ui->treeWidget->currentItem())
    {
        ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(0));
        ui->treeWidget->setEnabled(true);
    }
}

void FtpForm::addToFileList(const QUrlInfo &urlInfo)
{
    //>@不枚举所有的链接文件（快捷方式）
    if(urlInfo.isSymLink())
        return;
    m_CurDirFileList.append(urlInfo);
}

void FtpForm::doubleClickListItem(QModelIndex index)
{
    QFileInfo *info=new QFileInfo(LocalDirModel->filePath(index));
    if(info->isDir())
    {
        pathList.append(LocalDirModel->filePath(index));
        pos++;
        ui->LocalCatalog->setText(LocalDirModel->filePath(index));
        ui->listView->setRootIndex(index);
        Local_CurrentPath = LocalDirModel->filePath(index);
    }
}

void FtpForm::selectListItem(QModelIndex index)
{
    m_CurListItemFile.setFile(LocalDirModel->filePath(index));
}

bool FtpForm::ConnectToHost(QString pFtpAddr, quint16 pFtpPort, qint32 pDieTime)
{
    if(ftpobj == 0)
        return false;
    ftpobj->abort();
    ftpobj->clearPendingCommands();
    ftpobj->connectToHost(pFtpAddr, pFtpPort);
    while(ftpobj->state() != QFtp::Connected)
    {
        IDE_TRACE();
        if(pDieTime-- <= 0)
        {
            int ret = QMessageBox::question(this, tr("FTP"),
                                               tr("Unable to connect to the FTP server "
                                                  "at %1. Do you want to abort?")
                                               .arg(ui->FtpAddress->currentText()),
                                               QMessageBox::Yes | QMessageBox::No,
                                               QMessageBox::Yes);
            if(ret == QMessageBox::Yes)
            {
                return false;
            }
        }
        QtSleep(1);
    }
    return true;
}

bool FtpForm::LoginFtp(QString pUsrname, QString pPasswd, qint32 pDieTime)
{
    if(ftpobj == 0)
        return false;
    IDE_TRACE();
    ftpobj->abort();
    IDE_TRACE();
    ftpobj->clearPendingCommands();
    IDE_TRACE();
    if(pUsrname.isNull() || pPasswd.isNull())
        ftpobj->login();
    else
        ftpobj->login(pUsrname, pPasswd);
    IDE_TRACE();
    while(ftpobj->state() != QFtp::LoggedIn)
    {
        IDE_TRACE();
        if(pDieTime-- <= 0)
        {
            int ret = QMessageBox::question(this, tr("FTP"),
                                               tr("Unable to Login the FTP server "
                                                  "at %1. Do you want to abort?")
                                               .arg(ui->FtpAddress->currentText()),
                                               QMessageBox::Yes | QMessageBox::No,
                                               QMessageBox::Yes);
            if(ret == QMessageBox::Yes)
            {
                return false;
            }
        }
        QtSleep(1);
    }
    return true;
}

bool FtpForm::GotoFtpPath(QString pPath, qint32 pDieTime)
{
    if(ftpobj == 0)
        return false;
    ftpobj->abort();
    ftpobj->clearPendingCommands();
    m_CdCommandFinised = false;
    ftpobj->cd(pPath);
    while(!m_CdCommandFinised)
    {
        if(pDieTime-- <= 0)
        {
            int ret = QMessageBox::question(this, QString("Warn"),
                                 QString("GotoPath Timeout, Do you want to abort?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
            if(ret == QMessageBox::Yes)
            {
                ftpobj->cd(ui->RemoteCatalog->text());
                return false;
            }
        }
        QtSleep(1);
    }
    if(ftpobj->error() != QFtp::NoError)
    {
        ftpobj->cd(ui->RemoteCatalog->text());
        return false;
    }
    m_CurDirFileList.clear();
    ftpobj->abort();
    ftpobj->clearPendingCommands();
    m_ListCommandFinised = false;
    ftpobj->list();
    while(!m_ListCommandFinised)
    {
        if(pDieTime-- <= 0)
        {
            int ret = QMessageBox::question(this, QString("Warn"),
                                 QString("FTP Timeout, Do you want to abort?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
            if(ret == QMessageBox::Yes)
            {
                ftpobj->cd(ui->RemoteCatalog->text());
                return false;
            }
        }
        QtSleep(1);
    }
    if(ftpobj->error() != QFtp::NoError)
    {
        ftpobj->cd(ui->RemoteCatalog->text());
        return false;
    }
    UpdateFtpTree();
    ui->RemoteCatalog->setText(pPath);
    return true;
}

bool FtpForm::ListFtpPath(QString pPath, qint32 pDieTime)
{
    if(ftpobj == 0)
        return false;
    ftpobj->abort();
    ftpobj->clearPendingCommands();
    m_CurDirFileList.clear();
    m_ListCommandFinised = false;
    ftpobj->list(pPath);
    while(!m_ListCommandFinised)
    {
        if(pDieTime-- <= 0)
        {
            int ret = QMessageBox::question(this, QString("Warn"),
                                 QString("FTP Timeout, Do you want to abort?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
            if(ret == QMessageBox::Yes)
            {
                return false;
            }
        }
        QtSleep(1);
    }
    if(ftpobj->error() != QFtp::NoError)
    {
        return false;
    }
    return true;
}

bool FtpForm::MakeFtpPath(QString pPath, qint32 pDieTime)
{
    if(ftpobj == 0)
        return false;
    ftpobj->abort();
    ftpobj->clearPendingCommands();
    m_MkdirCommandFinised = false;
    ftpobj->mkdir(pPath);
    while(!m_MkdirCommandFinised)
    {
        if(pDieTime-- <= 0)
        {
            int ret = QMessageBox::question(this, QString("Warn"),
                                 QString("FTP Timeout, Do you want to abort?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
            if(ret == QMessageBox::Yes)
            {
                return false;
            }
        }
        QtSleep(1);
    }
    if(ftpobj->error() != QFtp::NoError)
    {
        return false;
    }
    return true;
}

bool FtpForm::GetFtpFile(QString pSrcPath, QFile &pDstFile, qint32 pDieTime)
{
    if(ftpobj == 0)
        return false;
    ftpobj->abort();
    ftpobj->clearPendingCommands();
    m_GetCommandFinised = false;
    IDE_TRACE_STR(pSrcPath);
    IDE_TRACE_STR(pDstFile.fileName());
    ftpobj->get(pSrcPath, &pDstFile);
    while(!m_GetCommandFinised)
    {
        if(pDieTime-- <= 0)
        {
            int ret = QMessageBox::question(this, QString("Warn"),
                                 QString("FTP Timeout, Cancle this DownLoad?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
            if(ret == QMessageBox::Yes)
            {
                return false;
            }
        }
        QtSleep(1);
    }
    if(ftpobj->error() != QFtp::NoError)
    {
        return false;
    }
    return true;
}

bool FtpForm::PutFtpFile(QFile &pSrcFile, QString pDstPath, qint32 pDieTime)
{
    if(ftpobj == 0)
        return false;
    ftpobj->abort();
    ftpobj->clearPendingCommands();
    m_PutCommandFinised = false;
    ftpobj->put(&pSrcFile, pDstPath);
    //ftpobj->put(&pSrcFile, pDstPath ,QFtp::Ascii);
    while(!m_PutCommandFinised)
    {
        if(pDieTime-- <= 0)
        {
            int ret = QMessageBox::question(this, QString("Warn"),
                                 QString("FTP Timeout, Do you want to abort?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes);
            if(ret == QMessageBox::Yes)
            {
                return false;
            }
        }
        QtSleep(1);
    }
    if(ftpobj->error() != QFtp::NoError)
    {
        return false;
    }
    return true;
}

void FtpForm::doubleClickTreeItem(QTreeWidgetItem *item, int column)
{
    if(item == 0)
        return;
    QString name = item->text(0);
    if (!item->whatsThis(0).compare("dir"))
    {
        ui->treeWidget->clear();
        QString tmpString = ui->RemoteCatalog->text();
        if(tmpString.endsWith('/') == false)
            tmpString.append('/');
        if(GotoFtpPath(tmpString + name, 100))
        {
            IDE_TRACE();
            ui->RemoteCatalog->setText(tmpString + name);
        }
    }
}

void FtpForm::DownLoad()
{
    QTreeWidgetItem *tmpItem = ui->treeWidget->currentItem();
    if(tmpItem == 0)
        return;
    QString tmpFileName = tmpItem->text(0);
    if(tmpFileName.isEmpty())
        return;
    QString tmpSrcPath, tmpDstPath;
    tmpDstPath = ui->LocalCatalog->text();
    if(tmpDstPath.endsWith('/') == false)
        tmpDstPath.append('/');
    tmpSrcPath = ui->RemoteCatalog->text();
    if(tmpSrcPath.endsWith('/') == false)
        tmpSrcPath.append('/');
    tmpSrcPath += tmpFileName;
    if(!tmpItem->whatsThis(0).compare("dir"))
    {
        IDE_TRACE_STR(tmpSrcPath);
        IDE_TRACE_STR(tmpDstPath);
        DownloadDirectory(tmpSrcPath, tmpDstPath, 100);
    }
    else
    {
        IDE_TRACE_STR(tmpSrcPath);
        IDE_TRACE_STR(tmpDstPath);
        DownloadFile(tmpSrcPath, tmpDstPath, 100);
    }
    RefreshList();
}

bool FtpForm::DownloadFile(QString pSrcPath, QString pDstPath, qint32 pDieTime)
{
    QString tmpFileName = getFileName(pSrcPath);
    if(pDstPath.endsWith('/') == false)
        pDstPath.append('/');
    IDE_TRACE_STR(pSrcPath);
    IDE_TRACE_STR(pDstPath);
    QString tmpDstFile = pDstPath + tmpFileName;
    QFile tmpFile(tmpDstFile);
    IDE_TRACE_STR(tmpDstFile);
    if(tmpFile.exists())
    {
        QFileInfo tmpFileInfo(tmpDstFile);
        if(tmpFileInfo.isFile())
        {
            int answer = QMessageBox::question(this,tr("Warn"),
                                               tr("%1 has existed，\r\nDo you want to overlap?").arg(tmpFileName),
                                               QMessageBox::Yes|QMessageBox::No);
            if(answer == QMessageBox::Yes)
            {
                tmpFile.remove();
            }
            else if(answer == QMessageBox::No)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    if (!tmpFile.open(QFile::WriteOnly))
    {
        QMessageBox::information(this, tr("FTP"),
                                 tr("%1: %2.")
                                 .arg(tmpFileName).arg(tmpFile.errorString()),
                                 QMessageBox::Yes);
        return false;
    }
    GlobalPara.updateProgress(tr("Downloading %1...").arg(tmpFileName), 10);
    IDE_TRACE_STR(pSrcPath);
    IDE_TRACE_STR(tmpDstFile);
    bool flag = GetFtpFile(pSrcPath, tmpFile, pDieTime);
    IDE_TRACE();
    if(flag)
    {
        GlobalPara.updateProgress(tr("Download %1 success!").arg(tmpFileName), 90);
    }
    else
    {
        GlobalPara.updateProgress(tr("Download %1 fail!").arg(tmpFileName), 90);
    }
    IDE_TRACE();
    tmpFile.close();
    GlobalPara.updateProgress(0);
    return flag;
}

bool FtpForm::DownloadDirectory(QString pSrcPath, QString pDstPath, qint32 pDieTime)
{
    if(pDstPath.endsWith('/') == false)
        pDstPath.append('/');
    if(pSrcPath.endsWith('/') == false)
        pSrcPath.append('/');
    QString tmpDirName = GetDirectoryName(pSrcPath);
    if(tmpDirName.isEmpty())
    {
        IDE_TRACE_STR(pSrcPath);
        return false;
    }
    QString tmpDstPath = pDstPath + tmpDirName;
    QDir tmpDir(tmpDstPath);
    if(!tmpDir.exists())
        tmpDir.mkpath(tmpDstPath);

    IDE_TRACE_STR(pSrcPath);
    IDE_TRACE_STR(tmpDstPath);

    ListFtpPath(pSrcPath, 100);
    QList<QUrlInfo> tmpUrlInfoList = m_CurDirFileList;
    for(int i=0;i<tmpUrlInfoList.count();i++)
    {
        QUrlInfo tmpUrlInfo = tmpUrlInfoList.at(i);
        QString tmpSrcDir = pSrcPath + tmpUrlInfo.name();
        IDE_TRACE_STR(tmpSrcDir);
        IDE_TRACE_STR(tmpDstPath);
        if(tmpUrlInfo.isDir())
        {
            if(!DownloadDirectory(tmpSrcDir, tmpDstPath, pDieTime))
            {
                int answer = QMessageBox::question(this,tr("Warn"),
                                                   tr("Download %1 to %2 fail,\r\nDo you want to exit?").arg(tmpSrcDir)
                                                                                                       .arg(tmpDstPath),
                                                   QMessageBox::Yes|QMessageBox::No,
                                                   QMessageBox::Yes);
                if(answer == QMessageBox::Yes)
                {
                    return false;
                }
            }
        }
        else
        {
            IDE_TRACE_STR(tmpSrcDir);
            IDE_TRACE_STR(tmpDstPath);
            IDE_TRACE_INT(pDieTime);
            if(!DownloadFile(tmpSrcDir, tmpDstPath, pDieTime))
            {
                int answer = QMessageBox::question(this,tr("Warn"),
                                                   tr("Download %1 to %2 fail,\r\nDo you want to exit?").arg(tmpSrcDir)
                                                                                                       .arg(tmpDstPath),
                                                   QMessageBox::Yes|QMessageBox::No,
                                                   QMessageBox::Yes);
                if(answer == QMessageBox::Yes)
                {
                    return false;
                }
            }
        }
        QtSleep(10);
    }
    return true;
}

void FtpForm::Upload()
{
    QString tmpDstPath = ui->RemoteCatalog->text();
    if(tmpDstPath.endsWith('/') == false)
        tmpDstPath.append('/');
    QString tmpSrcPath = m_CurListItemFile.filePath();
    if(m_CurListItemFile.isDir())
    {
        if(!tmpSrcPath.endsWith("/"))
            tmpSrcPath.append("/");
        UploadDirectory(tmpSrcPath, tmpDstPath, 100);
    }
    else if(m_CurListItemFile.isFile())
    {
        if(!tmpDstPath.endsWith("/"))
            tmpDstPath.append("/");
        tmpDstPath = tmpDstPath + tmpSrcPath.mid(tmpSrcPath.lastIndexOf("/")+1);

        IDE_TRACE_STR(tmpSrcPath);
        IDE_TRACE_STR(tmpDstPath);
        UploadFile(tmpSrcPath, tmpDstPath, 100);
    }
    RefreshTree();
}

bool FtpForm::UploadDirectory(QString pSrcPath, QString pDstPath, qint32 pDieTime)
{
    IDE_TRACE_STR(pSrcPath);
    IDE_TRACE_STR(pDstPath);

    QDir tmpdir(pSrcPath);
    if(!tmpdir.exists())
    {
        IDE_TRACE_STR(pSrcPath);
        return false;
    }
    if(pDstPath.endsWith('/') == false)
        pDstPath.append('/');
    QString tmpSrcDirName = GetDirectoryName(pSrcPath);
    if(tmpSrcDirName.isEmpty())
    {
        IDE_TRACE_STR(pSrcPath);
        return false;
    }
    pDstPath.append(tmpSrcDirName);
    if(MakeFtpPath(pDstPath, 100) == false)
    {
        IDE_TRACE_STR(pDstPath);
        return false;
    }
    if(!pSrcPath.endsWith("/"))
        pSrcPath.append("/");
    if(pDstPath.endsWith('/') == false)
        pDstPath.append('/');
    QFileInfoList tmpCurDirList = tmpdir.entryInfoList();
    for(int i=0;i<tmpCurDirList.count();i++)
    {
        QFileInfo tmpFileInfo = tmpCurDirList.at(i);
        if(!tmpFileInfo.fileName().compare("."))
            continue;
        if(!tmpFileInfo.fileName().compare(".."))
            continue;
        QString tmpFilePath = tmpFileInfo.absoluteFilePath();
        QString tmpSrcPath = tmpFilePath;
        if(tmpFileInfo.isDir())
        {
            QString tmpDstPath = pDstPath + GetDirectoryName(tmpFilePath);
            if(!UploadDirectory(tmpSrcPath, tmpDstPath, pDieTime))
            {
                int answer = QMessageBox::question(this,tr("Warn"),
                                                   tr("Upload %1 to %2 fail,\r\nDo you want to exit?").arg(tmpSrcPath)
                                                                                                       .arg(tmpDstPath),
                                                   QMessageBox::Yes|QMessageBox::No,
                                                   QMessageBox::Yes);
                if(answer == QMessageBox::Yes)
                {
                    return false;
                }
            }
        }
        else if(tmpFileInfo.isFile())
        {
            QString tmpDstPath = pDstPath + getFileName(tmpFilePath);
            if(!UploadFile(tmpSrcPath, tmpDstPath, pDieTime))
            {
                int answer = QMessageBox::question(this,tr("Warn"),
                                                   tr("Upload %1 to %2 fail,\r\nDo you want to exit?").arg(tmpSrcPath)
                                                                                                       .arg(tmpDstPath),
                                                   QMessageBox::Yes|QMessageBox::No,
                                                   QMessageBox::Yes);
                if(answer == QMessageBox::Yes)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool FtpForm::UploadFile(QString pSrcPath, QString pDstPath, qint32 pDieTime)
{
    IDE_TRACE_STR(pSrcPath);
    IDE_TRACE_STR(pDstPath);

    QString tmpFileName = getFileName(pSrcPath);
    QFile tmpFile(pSrcPath);
    if(tmpFile.exists() == false)
    {
        IDE_TRACE_STR(pSrcPath);
        return false;
    }
    if (!tmpFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("FTP"),
                                 tr("%1: %2.")
                                 .arg(tmpFileName).arg(tmpFile.errorString()));
        return false;
    }
    GlobalPara.updateProgress(tr("Uploading %1...").arg(tmpFileName), 10);
    bool flag = PutFtpFile(tmpFile, pDstPath, pDieTime);
    if(flag)
    {
        GlobalPara.updateProgress(tr("Upload %1 success!").arg(tmpFileName), 90);
    }
    else
    {
        GlobalPara.updateProgress(tr("Upload %1 fail!").arg(tmpFileName), 90);
    }
    tmpFile.close();
    GlobalPara.updateProgress(0);
    return flag;
}

void FtpForm::CancelFTP()
{
    ftpobj->abort();
    m_CancelFTP = true;
}

void FtpForm::FtpCommandFinished(int commandId, bool error)
{
    if(ftpobj == 0)
        return;
    if (ftpobj->currentCommand() == QFtp::Get && !error)
    {
        m_GetCommandFinised = true;
    }
    else if (ftpobj->currentCommand() == QFtp::Put && !error)
    {
        m_PutCommandFinised = true;
    }
    else if (ftpobj->currentCommand() == QFtp::Mkdir && !error)
    {
        m_MkdirCommandFinised = true;
    }
    else if (ftpobj->currentCommand() == QFtp::List && !error)
    {
        m_ListCommandFinised = true;
    }
    else if (ftpobj->currentCommand() == QFtp::Cd && !error)
    {
        m_CdCommandFinised = true;
    }
}

void FtpForm::updateDataTransferProgress(qint64 readBytes,qint64 totalBytes)
{
    GlobalPara.updateProgress((qreal)readBytes / (qreal)totalBytes);
}

void FtpForm::on_Local_Back_clicked()
{
    if(pathList.count()<=0)
        return ;
    if(!Local_CurrentPath.compare("/"))
        return;
    if(Local_CurrentPath.endsWith(":/"))
    {
        Local_CurrentPath = "/";
        ui->listView->setRootIndex(LocalDirModel->index("/"));
        return;
    }

    QString string;
    if(pos >= 2)
    {
        --pos;
        string = this->pathList.at(pos-1);
    }
    else
    {
        string = this->pathList.at(0);
    }
    Local_CurrentPath = string;
    ui->listView->setRootIndex(LocalDirModel->index(string));
    ui->LocalCatalog->setText(string);
}

void FtpForm::on_Local_Next_clicked()
{
    if(pos>pathList.count()-1&&pathList.count()>=0)
    {
        return;
    }
    QString string;
    if(pos >= 1)
    {
        ++pos;
        string=this->pathList.at(pos-1);
    }
    else
    {
        string=this->pathList.at(0);
    }
    Local_CurrentPath = string;
    ui->listView->setRootIndex(LocalDirModel->index(string));
    ui->LocalCatalog->setText(string);
}

void FtpForm::on_Local_Upwards_clicked()
{
    if(Local_CurrentPath.endsWith(":/"))
    {
        Local_CurrentPath = "/";
        ui->listView->setRootIndex(LocalDirModel->index("/"));
        return;
    }
    QFileInfo *info=new QFileInfo(Local_CurrentPath);
    QDir dir=info->dir();
    QString string=dir.path();
    Local_CurrentPath = string;
    ui->listView->setRootIndex(LocalDirModel->index(string));
    ui->LocalCatalog->setText(string);
}

void FtpForm::on_Upwards_clicked()
{
    ui->treeWidget->clear();
    m_CurDirFileList.clear();
    QString RemoteCatalog = ui->RemoteCatalog->text();
    QString tmpPath = RemoteCatalog.left(RemoteCatalog.lastIndexOf("/"));
    IDE_TRACE_INT(RemoteCatalog.lastIndexOf("/"));
    IDE_TRACE_STR(tmpPath);
    if(!tmpPath.compare(""))
        tmpPath = "/";
    ui->RemoteCatalog->setText(tmpPath);
    ftpobj->cd(tmpPath);
    m_ListCommandFinised = false;
    ftpobj->list();
    while(!m_ListCommandFinised)
        QtSleep(1);
    IDE_TRACE();
    UpdateFtpTree();

}

void FtpForm::RefreshList()
{
    QModelIndex tmpCurIndex = LocalDirModel->index(ui->LocalCatalog->text());
    if(tmpCurIndex.isValid() == false)
    {
        LocalDirModel->refresh();
    }
    else
    {
        LocalDirModel->refresh(tmpCurIndex.parent());
    }
}

void FtpForm::RefreshTree()
{
    IDE_TRACE();
    Disconnected();
    IDE_TRACE();
    if(ui->AuthenticationGroup->isChecked())
        InitFTP(ui->FtpAddress->currentText(), ui->FtpPort->value(), ui->UserName->text(), ui->Passwd->text(), ui->RemoteCatalog->text());
    else
        InitFTP(ui->FtpAddress->currentText(), ui->FtpPort->value(), QString(), QString(), ui->RemoteCatalog->text());
    IDE_TRACE();
}

void FtpForm::on_BtnRefresh_clicked()
{
    RefreshList();
    RefreshTree();
}

void FtpForm::on_BtnAbort_clicked()
{
    if(ftpobj == 0)
        return;
    ftpobj->abort();
    ftpobj->clearPendingCommands();
}

void FtpForm::on_LocalFileAdd_clicked()
{

    RefreshList();
}

void FtpForm::on_LocalFileDel_clicked()
{

}

void FtpForm::on_RemoteFileAdd_clicked()
{

}

void FtpForm::on_RemoteFileDel_clicked()
{

}
