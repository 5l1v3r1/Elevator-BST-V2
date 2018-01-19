#include "ftp.h"
#include "ui_ftpform.h"

FtpForm::FtpForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::Ftp), ftpobj(0)
{
    ui->setupUi(this);

    ui->listView->setViewMode(QListView::ListMode);
    ui->listView->setGridSize(QSize(60,20));
    ui->listView->setEditTriggers(QListView::NoEditTriggers);
    connect(ui->listView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(changeIndex(QModelIndex)));
    dirModel1=new QDirModel(nameFilter,QDir::AllEntries|QDir::NoDotAndDotDot,QDir::Name|QDir::DirsFirst|QDir::Type);
    dirModel1->setReadOnly(false);
    ui->listView->setModel(dirModel1);
    ui->listView->setRootIndex(dirModel1->index("/"));
    connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(isSelected(QModelIndex)));

    ui->Passwd->setEchoMode(QLineEdit::Password);   //password
    ui->UserName->setText("lds");
    ui->Passwd->setText("lds");

    ui->treeWidget->setAcceptDrops(true);
    this->setAcceptDrops(true);

    ui->treeWidget->setEnabled(false);
    ui->treeWidget->setRootIsDecorated(false);
    ui->treeWidget->header()->hide();
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setColumnWidth(0,ui->treeWidget->width());
    connect(ui->treeWidget, SIGNAL(itemActivated(QTreeWidgetItem *, int)), this, SLOT(processItem(QTreeWidgetItem *, int)));
    connect(ui->treeWidget,SIGNAL(clicked(QModelIndex)),this,SLOT(FTPSelected(QModelIndex)));

    ui->BtnConnect->setDefault(false);
    ui->BtnDownLoad->setEnabled(false);
    ui->BtnUpLoad->setEnabled(false);
    progressDialog = new QProgressDialog(this);

    connect(ui->checkBox, SIGNAL(clicked()), this, SLOT(enableCheckBox()));
    connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
    connect(ui->BtnConnect, SIGNAL(clicked()), this, SLOT(connectOrDisconnect()));
    connect(ui->BtnDownLoad, SIGNAL(clicked()), this, SLOT(DownLoad()));
    connect(ui->BtnUpLoad, SIGNAL(clicked()), this, SLOT(UpLoad()));

    pos=0;
    Local_CurrentPath = "";
    DownLoadFilePath = "";
    UpLoadFile = "";
    UploadFilePath = "";
    FtpUpLoadPath = "";
    pathList.append("/");
    setWindowTitle(tr("FTP"));
}

FtpForm::~FtpForm()
{
    delete ui;
}

void FtpForm::FTPSelected(QModelIndex index)
{
    IDE_TRACE();

    QString SelectItem = ui->treeWidget->currentItem()->text(0);
    if(isDirectory.value(SelectItem))
    {
        FtpUpLoadPath = SrcRootpath;
        if(!FtpUpLoadPath.endsWith("/"))
        {
            FtpUpLoadPath.append("/");
        }
        FtpUpLoadPath = FtpUpLoadPath + SelectItem;
        IDE_TRACE_STR(FtpUpLoadPath);
    }
//    else
//    {

//    }
}

void FtpForm::changeIndex(QModelIndex index)
{
    QFileInfo *info=new QFileInfo(dirModel1->filePath(index));
    if(info->isDir())
    {
        pathList.append(dirModel1->filePath(index));
        pos++;
        Local_CurrentPath = dirModel1->filePath(index);
        ui->listView->setRootIndex(index);
    }
    DstDir = dirModel1->filePath(index);
    IDE_TRACE_STR(DstDir);
}

void FtpForm::isSelected(QModelIndex index)
{
    QFileInfo *info=new QFileInfo(dirModel1->filePath(index));
    if(info->isDir())
    {
        DownLoadFilePath = dirModel1->filePath(index);
//        IDE_TRACE_STR(DownLoadFilePath);
        for(int i=DownLoadFilePath.length()-1;i>=0;i--)
        {
            if(i ==DownLoadFilePath.length()-1)
                continue;
            if(DownLoadFilePath.at(i) == '/')
            {
                DownLoadFilePath = DownLoadFilePath.mid(0,i);
                if(!DownLoadFilePath.compare(""))
                    DownLoadFilePath = "/";
                break;
            }
        }
        QDir tmpdir(dirModel1->filePath(index));
        DstRootPath = tmpdir.absolutePath();
//        IDE_TRACE_STR(DstRootPath);
        UploadFilePath = dirModel1->filePath(index);
        UpLoadFile = "";
//        IDE_TRACE_STR(UploadFilePath);
//        IDE_TRACE_STR(DownLoadFilePath);
//        DstDir = tmpdir.absolutePath();
//        DstDir = DstDir.left(DstDir.lastIndexOf("/"));
//        if(!DstDir.compare(""))
//        {
//            DstDir = "/";
//        }
//        IDE_TRACE_STR(DstDir);
    }
    else if(info->isFile())
    {
        UpLoadFile = dirModel1->filePath(index);
        UploadFilePath = "";
        DstRootPath = "";
    }
}

void FtpForm::connectOrDisconnect()
{
    if(!ui->checkBox->isChecked())
    {
        if(ui->UserName->text().isEmpty()||ui->Passwd->text().isEmpty())
         {
            QMessageBox::information(this, tr("FTP"),tr("请检查ftp账号信息，用户名和密码都不允许为空。"));
            return ;
        }
    }

    if (ftpobj)
    {
        ftpobj->abort();
        ftpobj->deleteLater();
        ftpobj = 0;

        ui->UserName->setEnabled(true);
        ui->Passwd->setEnabled(true);
        ui->checkBox->setEnabled(true);
        ui->treeWidget->setEnabled(false);
        ui->BtnDownLoad->setEnabled(false);
        ui->BtnUpLoad->setEnabled(false);
        ui->BtnConnect->setText(tr("Connect"));
        ui->comboBox->setEditable(true);
        ui->checkBox->setChecked(false);
        return;
    }
    else
    {
        pathList.append("/");
        ui->BtnConnect->setText(tr("DisConnect"));
        ui->comboBox->setEditable(false);
        ui->UserName->setEnabled(false);
        ui->Passwd->setEnabled(false);
        ui->checkBox->setEnabled(false);
        ui->treeWidget->setEnabled(true);
        ui->BtnDownLoad->setEnabled(true);
        ui->BtnUpLoad->setEnabled(true);

        ftpobj = new QFtp(this);

        connect(ftpobj, SIGNAL(commandFinished(int, bool)),
                this, SLOT(ftpCommandFinished(int, bool)));
        connect(ftpobj, SIGNAL(listInfo(const QUrlInfo &)),
                this, SLOT(addToList(const QUrlInfo &)));
        connect(ftpobj, SIGNAL(dataTransferProgress(qint64, qint64)),
                this, SLOT(updateDataTransferProgress(qint64, qint64)));

        ui->treeWidget->clear();
        currentPath.clear();
        isDirectory.clear();

        QUrl url(ui->comboBox->currentText());
        if (!url.isValid() || url.scheme().toLower() != QLatin1String("ftp"))
        {
            ftpobj->connectToHost(ui->comboBox->currentText(), 21);
            if(ui->checkBox->isChecked())
                ftpobj->login();
            else
                ftpobj->login(ui->UserName->text(),ui->Passwd->text());
            SrcDir = QString("/home/FTP"); //dir first showed
        }
        else
        {
            ftpobj->connectToHost(url.host(), url.port(21));

            if(ui->checkBox->isChecked())
                ftpobj->login();
            else
                ftpobj->login(ui->UserName->text(),ui->Passwd->text());

            if (!url.path().isEmpty())
            {
                ftpobj->cd(url.path());
                if(url.path().startsWith("/"))
                    SrcDir = url.path();
                else
                    return;
            }
        }

        SrcRootpath = QString("/home/FTP");
        DstRootPath = QString("/");
    }
}

void FtpForm::DownLoad()
{
    QString SelectedItem = ui->treeWidget->currentItem()->text(0);
    if(isDirectory.value(SelectedItem))
    {
        //TODU  get two absolutely path below
        SrcDir = SrcRootpath;
        if(!SrcDir.endsWith("/"))
        {
            SrcDir.append("/");
        }
        SrcDir = SrcDir + ui->treeWidget->currentItem()->text(0);
//        DstDir = DstRootPath;
//        DstDir = DstDir.left(DstDir.lastIndexOf("/"));
        if(!DstDir.endsWith("/"))
        {
            DstDir.append("/");
        }
        DstDir = DstDir + ui->treeWidget->currentItem()->text(0);
//        DstRootPath = QString("/home/ftp/finally");
//        SrcRootpath = QString("/home/lds/desktop/finally");
        IDE_TRACE_STR(SrcDir);
        IDE_TRACE_STR(DstDir);

        CopyDirectory(SrcDir , DstDir);
    }
    else
    {
        downloadFile();
    }
}

void FtpForm::downloadFile()
{
    QString fileNam = ui->treeWidget->currentItem()->text(0);
    QString fileName;

    if(DownLoadFilePath.isEmpty())
    {
        fileName = QFileDialog::getExistingDirectory(this, tr("保存到。。。"),
                                                             ".",
                                                             QFileDialog::ShowDirsOnly
                                                             | QFileDialog::DontResolveSymlinks);
        if(fileName.isNull())
            return;

        if(!fileName.endsWith("/"))
        {
            fileName.append("/");
        }
        fileName += fileNam;
        if (QFile::exists(fileName))
        {
            int answer = QMessageBox::question(this,tr("FTP"),
                                               tr("同名文件 %1已经存在，需要覆盖吗?").arg(fileName),
                                               QMessageBox::Yes|QMessageBox::No);
            if(answer == QMessageBox::Yes)
            {
                QDir dir(fileName);
                dir.rmdir(fileName);
            }
            if(answer == QMessageBox::No)
                return;
        }
    }
    else
    {
        if(!DownLoadFilePath.endsWith("/"))
        {
            DownLoadFilePath.append("/");
        }
        fileName = DownLoadFilePath + fileNam;
    }

    QFile *file = new QFile(fileName);
    if (!file->open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("FTP"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(fileName).arg(file->errorString()));
        delete file;
        return;
    }

    ftpobj->get(ui->treeWidget->currentItem()->text(0), file);
    progressDialog->setLabelText(tr("Downloading %1...").arg(fileName));
    progressDialog->exec();
    file->deleteLater();
}

void FtpForm::downloadFile(QString SrcFileName,QString DstFile )
{
    IDE_TRACE_STR(SrcFileName);
    IDE_TRACE_STR(DstFile);
    QFile *file = new QFile(DstFile);
    IDE_TRACE();
    if (!file->open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("FTP"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(SrcFileName).arg(file->errorString()));
        delete file;
        return;
    }
    ftpobj->get(SrcFileName, file);
    progressDialog->setLabelText(tr("Downloading %1...").arg(SrcFileName));
    progressDialog->exec();
    QtSleep(10);
    file->deleteLater();
}

void Ftp::UpLoad()
{
    if(UpLoadFile.isEmpty() && !UploadFilePath.isEmpty())
    {
        //UpLoad dir
//        QString SrcDir = "/home/lds/desktop/finally";
//        QString DstDir = "/home/ftp/UpLoad/finally";
        SrcDir = DstRootPath;
        DstDir = SrcRootpath;
//        IDE_TRACE_STR(SrcRootpath);
//        IDE_TRACE_STR(DstRootPath);
        if(!DstDir.endsWith("/"))
        {
            DstDir.append("/");
        }
        DstDir = DstDir + SrcDir.mid(SrcDir.lastIndexOf("/")+1);
//        QString TmpDir = dirModel1->filePath(ui->listView->currentIndex());
//        QString tmpFilename = TmpDir.mid(TmpDir.lastIndexOf("/")+1);

//        SrcDir =  dirModel1->filePath(ui->listView->currentIndex());
//        if(!DstDir.endsWith("/"))
//        {
//            DstDir.append("/");
//        }
//        DstDir = DstDir + tmpFilename;
        IDE_TRACE_STR(SrcDir);
        IDE_TRACE_STR(DstDir);
        QString tmpDstDir = DstDir.mid(DstDir.lastIndexOf("/")+1);
        ftpobj->mkdir(tmpDstDir);
        ftpobj->cd(tmpDstDir);
        UpLoadDirectory(SrcDir,DstDir);
    }
    else if(!UpLoadFile.isEmpty() && UploadFilePath.isEmpty())
    {
        uploadLocalFile();
    }
}

void FtpForm::uploadLocalFile()
{
    QString fileName;
    if(UpLoadFile.isEmpty())
    {
        fileName = QFileDialog::getOpenFileName(this, tr("????????"),
                                                 ".",
                                                 "*");//to be upload
        IDE_TRACE_STR("UpLoadFile is Empty");
    }
    else
    {
        fileName = UpLoadFile;
        IDE_TRACE_STR(UpLoadFile);
    }

    if(fileName.isNull())
        return;

    if (!QFile::exists(fileName))
    {
         return;
    }

    QString DstFile =  fileName.mid(fileName.lastIndexOf("/")+1);
    UpLoadLocalFile(fileName,DstFile);
    /*
    file_upload = new QFile(fileName);
    if (!file_upload->open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("FTP"),
                                 tr("Unable to open the file %1: %2.")
                                 .arg(fileName).arg(file_upload->errorString()));
        return;
    }
    IDE_TRACE_STR(fileName);
    IDE_TRACE_STR(DstFile);
    ftpobj->put(file_upload,DstFile);
    progressDialog->setLabelText(tr("Uploading %1...").arg(fileName));
    progressDialog->exec();

    ui->treeWidget->clear();
    ftpobj->list();
    */
}

void FtpForm::UpLoadLocalFile(QString fileName,QString DstPath)
{
    IDE_TRACE_STR(fileName);
    IDE_TRACE_STR(DstPath);

    file_upload = new QFile(fileName);
    if (!file_upload->open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("FTP"),
                                 tr("Unable to open the file %1: %2.")
                                 .arg(fileName).arg(file_upload->errorString()));
        return;
    }
    ftpobj->put(file_upload,DstPath);
    progressDialog->setLabelText(tr("Uploading %1...").arg(fileName));
    progressDialog->exec();

    ui->treeWidget->clear();
    ftpobj->list();
}

void Ftp::cancelDownload()
{
    ftpobj->abort();
}

void FtpForm::ftpCommandFinished(int, bool error)
{
    if (ftpobj->currentCommand() == QFtp::ConnectToHost)
    {
        if (error)
        {
            QMessageBox::information(this, tr("FTP"),
                                     tr("Unable to connect to the FTP server "
                                        "at %1. Please check that the host "
                                        "name is correct.")
                                     .arg(ui->comboBox->currentText()));
            connectOrDisconnect();
            return;
        }
        ui->treeWidget->setFocus();
        ui->BtnConnect->setEnabled(true);
        return;
    }
    if (ftpobj->currentCommand() == QFtp::Login)
        ftpobj->list();
    if (ftpobj->currentCommand() == QFtp::Get)
    {
        progressDialog->hide();
    }
    else if (ftpobj->currentCommand() == QFtp::List)
    {
        if (isDirectory.isEmpty())
        {
            ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList() << tr("<empty>")));
            ui->treeWidget->setEnabled(false);
        }
    }
}

void FtpForm::addToList(const QUrlInfo &urlInfo)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, urlInfo.name());

    QPixmap pixmap(urlInfo.isDir() ? ":/images/dir.png" : ":/images/file.png");
    item->setIcon(0, pixmap);

    isDirectory[urlInfo.name()] = urlInfo.isDir();
    CurrentDirList.append(urlInfo.name());
//    IDE_TRACE_INT(CurrentDirList.count());
//    IDE_TRACE_STR(urlInfo.name());
    ui->treeWidget->addTopLevelItem(item);
    if (!ui->treeWidget->currentItem())
    {
        ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(0));
        ui->treeWidget->setEnabled(true);
    }
}

void FtpForm::processItem(QTreeWidgetItem *item, int column)
{
    QString name = item->text(0);
    if (isDirectory.value(name))
    {
        ui->treeWidget->clear();
        isDirectory.clear();
        if(currentPath.isEmpty())
        {
             currentPath = name;
        }
        else
        {
            currentPath += "/" + name;
        }
        if(!SrcRootpath.endsWith("/"))
        {
            SrcRootpath.append("/");
        }
        SrcRootpath = SrcRootpath + name;
        IDE_TRACE_STR(SrcRootpath);
        ftpobj->cd(name);
        ftpobj->list();
        return;
    }
    IDE_TRACE();
}

void FtpForm::updateDataTransferProgress(qint64 readBytes,qint64 totalBytes)
{
    progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(readBytes);
}

void FtpForm::enableCheckBox()
{

    if (ui->checkBox->isChecked()) {
        ui->UserName->setEnabled(false);
        ui->Passwd->setEnabled(false);
    } else {
        ui->UserName->setEnabled(true);
        ui->Passwd->setEnabled(true);
    }
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
        ui->listView->setRootIndex(dirModel1->index("/"));
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
    ui->listView->setRootIndex(dirModel1->index(string));
    DstDir = Local_CurrentPath;
    IDE_TRACE_STR(DstDir);
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
    ui->listView->setRootIndex(dirModel1->index(string));
    DstDir = Local_CurrentPath;
    IDE_TRACE_STR(DstDir);
}

void FtpForm::on_Local_Upwards_clicked()
{
    if(Local_CurrentPath.endsWith(":/"))
    {
        Local_CurrentPath = "/";
        ui->listView->setRootIndex(dirModel1->index("/"));
        return;
    }
    QFileInfo *info=new QFileInfo(Local_CurrentPath);
    QDir dir=info->dir();
    QString string=dir.path();
    Local_CurrentPath = string;
    ui->listView->setRootIndex(dirModel1->index(string));
    DstDir = Local_CurrentPath;
    IDE_TRACE_STR(DstDir);
}

void FtpForm::on_Upwards_clicked()
{
    if(!ui->BtnConnect->text().compare("DisConnect"))
    {
        ui->treeWidget->clear();
        isDirectory.clear();
        if(currentPath.contains("/"))
        {
            currentPath = currentPath.left(currentPath.lastIndexOf('/'));
        }
        else
        {
            currentPath = "";
        }
        if (currentPath.isEmpty())
        {
            ftpobj->cd("/home/FTP");//ftp???,?????
        }
        else
        {
            ftpobj->cd("/home/FTP/"+currentPath);
        }
        ftpobj->list();
        if(SrcRootpath.endsWith("/"))
        {
            SrcRootpath = SrcRootpath.left(SrcRootpath.lastIndexOf("/"));
        }
        SrcRootpath = SrcRootpath.left(SrcRootpath.lastIndexOf("/"));
    }
}

void FtpForm::QtSleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1);
}

void FtpForm::CopyDirectory(QString SrcDir,QString DstDir) // Absolutely Path  : 1.SrcDir:dir on ftp server  2.DstDir:dir on localhost
{
    IDE_TRACE_STR(SrcDir);
    IDE_TRACE_STR(DstDir);
    CurrentDirList.clear();
    ftpobj->cd(SrcDir);
    ftpobj->list(SrcDir);
    QtSleep(50);

    QDir tmpDir(DstDir);
    if(!tmpDir.exists())
    {
        tmpDir.mkpath(DstDir);
    }
    IDE_TRACE_STR(DstDir);

    QStringList DirInfo = CurrentDirList;
    for(int i=0;i<DirInfo.count();i++)
    {
        IDE_TRACE_STR(DirInfo.at(i));
        if(isDirectory.value(DirInfo.at(i)))
        {
            if(!SrcDir.endsWith("/"))
            {
                SrcDir.append("/");
            }
            SrcDir = SrcDir + DirInfo.at(i);

            if(!DstDir.endsWith("/"))
            {
                DstDir.append("/");
            }
            DstDir = DstDir + DirInfo.at(i);

//            CopyDirectory(SrcDir,DstDir) ;

            // up to parent dir
            if(SrcDir.endsWith("/"))
            {
                SrcDir = SrcDir.left(SrcDir.lastIndexOf("/"));
            }
            if(DstDir.endsWith("/"))
            {
                DstDir = DstDir.left(DstDir.lastIndexOf("/"));
            }
            SrcDir = SrcDir.left(SrcDir.lastIndexOf("/"));
            DstDir = DstDir.left(DstDir.lastIndexOf("/"));

        }
        else if(!isDirectory.value(DirInfo.at(i)))
        {
            if(!DstDir.endsWith("/"))
                DstDir.append("/");

            QString SrcFile = DirInfo.at(i);
            QString DstFile = DstDir + DirInfo.at(i);
            IDE_TRACE_STR(SrcFile);
            IDE_TRACE_STR(DstFile);
            downloadFile(SrcFile,DstFile);   //still have question
        }
    }
}

void FtpForm::UpLoadDirectory(QString SrcDir,QString DstDir)// Absolutely Path  : 1.SrcDir:dir on localhost  2.DstDir:dir on ftp server
{
    IDE_TRACE_STR(SrcDir);
    IDE_TRACE_STR(DstDir);

    QDir tmpdir(SrcDir);
    if(!tmpdir.exists())
    {
        IDE_TRACE_STR("Dir Is Not Exist");
        return;
    }

    QFileInfoList CurDirList = tmpdir.entryInfoList();
    for(int i=0;i<CurDirList.count();i++)
    {
        if(!CurDirList.at(i).fileName().compare("."))
            continue;
        if(!CurDirList.at(i).fileName().compare(".."))
            continue;
        if(CurDirList.at(i).isDir())
        {
            QString absoluteFilePath = CurDirList.at(i).absoluteFilePath();
            if(!SrcDir.endsWith("/"))
            {
                SrcDir.append("/");
            }
            SrcDir = SrcDir + absoluteFilePath.mid(absoluteFilePath.lastIndexOf("/")+1);
            if(!DstDir.endsWith("/"))
            {
                DstDir.append("/");
            }
            DstDir = DstDir + absoluteFilePath.mid(absoluteFilePath.lastIndexOf("/")+1);
            QDir tmpFTPDir(DstDir);
            if(!tmpFTPDir.exists())
            {
                tmpFTPDir.mkpath(DstDir);
            }
            ftpobj->cd(DstDir);

            IDE_TRACE_STR(SrcDir);
            IDE_TRACE_STR(DstDir);
            UpLoadDirectory(SrcDir , DstDir);

            // up to parent floder
            if(SrcDir.endsWith("/"))
                SrcDir = SrcDir.left(SrcDir.lastIndexOf("/"));
            SrcDir = SrcDir.left(SrcDir.lastIndexOf("/"));
            if(DstDir.endsWith("/"))
                DstDir = DstDir.left(DstDir.lastIndexOf("/"));
            DstDir = DstDir.left(DstDir.lastIndexOf("/"));
        }
        else if(CurDirList.at(i).isFile())
        {
            //TUDO   Upload File
            QString absoluteFilePath = CurDirList.at(i).absoluteFilePath();
            if(!DstDir.endsWith("/"))
            {
             DstDir.append("/");
           }
            QString DstFile = DstDir + absoluteFilePath.mid(absoluteFilePath.lastIndexOf("/")+1);
            //QString DstFile = absoluteFilePath.mid(absoluteFilePath.lastIndexOf("/")+1);
            IDE_TRACE_STR(absoluteFilePath);
            IDE_TRACE_STR(DstFile);


            IDE_TRACE_STR(DstFile);
            QString tmpfilename = DstFile.mid(DstFile.lastIndexOf("/")+1);
            IDE_TRACE_STR(tmpfilename);
            QString tmpstring = DstFile.left(DstFile.lastIndexOf("/"));
            IDE_TRACE_STR(tmpstring);
            QString ParaentDir = tmpstring.mid(tmpstring.lastIndexOf("/")+1);
            IDE_TRACE_STR(ParaentDir);
            if(!ParaentDir.endsWith("/"))
            {
                ParaentDir.append("/");
            }
            DstFile = ParaentDir + tmpfilename;

            UpLoadLocalFile(absoluteFilePath,DstFile);
        }
    }
}
