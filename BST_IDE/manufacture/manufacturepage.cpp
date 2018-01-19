#include "manufacturepage.h"
#include "ui_manufacturepage.h"
#include "toolbar.h".h"
#include "devicequeryform.h"

ManufacturePage::ManufacturePage(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::ManufacturePage)
{
    ui->setupUi(this);
    DeviceDisconnected();
    m_DfuStopFlag = false;
    m_UsbDfu = 0;
    m_RadioGroup = new QButtonGroup(this);
    m_RadioGroup->addButton(ui->RadioUSB, 0);
    m_CurRadioIndex = 0;
    m_RadioGroup->addButton(ui->RadioTF, 1);
    connect(m_RadioGroup, SIGNAL(buttonClicked(int)), this, SLOT(slot_BootModeChanged(int)));
    SQLToolBar *tmpSqlToolBar = VAR_CAST<SQLToolBar *>(GlobalPara.m_SqlToolBar);
    if(tmpSqlToolBar && tmpSqlToolBar->mDevQueryFrom)
    {
        connect(tmpSqlToolBar->mDevQueryFrom, SIGNAL(sConnstate(bool,QString)), this, SLOT(slot_SQLConnState(bool,QString)));
    }
}

ManufacturePage::~ManufacturePage()
{
    ReleaseDFU();
    delete ui;
}

void ManufacturePage::InitDFU()
{
    ui->BtnScan->setIcon(QIcon(":/Menu/rc/Menu/find.png"));
    ui->BtnScan->setText(tr("Scanning ..."));
    QtSleep(200);  //>@解决插入USB设备后过快点击Scan按钮导致USB驱动死锁问题。
    if(m_UsbDfu == 0)
    {
        m_UsbDfu = new UsbDFU(GUID_CLASS_I82930_BULK, this);
        connect(m_UsbDfu, SIGNAL(sConnState(bool)), this, SLOT(slot_ConnState(bool)), Qt::AutoConnection);
        connect(m_UsbDfu, SIGNAL(sProgress(QString,int)), this, SLOT(slot_TransProgress(QString,int)));
    }
}

void ManufacturePage::ReleaseDFU()
{
    if(m_UsbDfu)
    {
        disconnect(m_UsbDfu, SIGNAL(sConnState(bool)), this, SLOT(slot_ConnState(CONN_STATE)));
        disconnect(m_UsbDfu, SIGNAL(sProgress(QString,int)), this, SLOT(slot_TransProgress(QString,int)));
        m_UsbDfu->deleteLater();
        m_UsbDfu = 0;
    }
    DeviceDisconnected();
}

void ManufacturePage::DeviceConnected()
{
    ui->BtnScan->setIcon(QIcon(":/Menu/rc/Menu/deviceonline.png"));
    ui->BtnScan->setText(tr("Disconnect"));

    ui->BtnScan->setChecked(true);
    ui->BtnScan->setEnabled(true);
    ui->BootSelectGroupBox->setEnabled(true);
    ui->RunGroupBox->setEnabled(true);
    ui->OperateGroupbox->setEnabled(true);
    if(ui->RadioUSB->isChecked())
        ui->RcGroupBox->setEnabled(true);
    else
        ui->RcGroupBox->setDisabled(true);
    //>@初始化数据库中的镜像文件
    InitDataBaseList();
}

void ManufacturePage::slot_SQLConnState(bool pSuccess, QString pIP)
{
    if(pSuccess)
        InitDataBaseList();
    else
        ReleaseDataBaseList();
}

void ManufacturePage::InitDataBaseList()
{
    if(ui->BootloaderGroupBox->isChecked())
        ui->BootloaderList->InitDataBaseList(QString("资源管理"),QString("资源版本,存储位置"),QString("资源类型='Bootloader'"));
    if(ui->KernelGroupBox->isChecked())
        ui->KernelList->InitDataBaseList(QString("资源管理"),QString("资源版本,存储位置"),QString("资源类型='Kernel'"));
    if(ui->FilesystemGroupBox->isChecked())
        ui->FileSystemList->InitDataBaseList(QString("资源管理"),QString("资源版本,存储位置"),QString("资源类型='FileSystem'"));
    if(ui->ApplicationGroupBox->isChecked())
        ui->ApplicationList->InitDataBaseList(QString("资源管理"),QString("资源版本,存储位置"),QString("资源类型='Application'"));
}

void ManufacturePage::ReleaseDataBaseList()
{
    ui->BootloaderList->ReleaseDataBaseList();
    ui->KernelList->ReleaseDataBaseList();
    ui->FileSystemList->ReleaseDataBaseList();
    ui->ApplicationList->ReleaseDataBaseList();
}

void ManufacturePage::DeviceDisconnected()
{
    ui->BtnScan->setIcon(QIcon(":/Menu/rc/Menu/deviceoffline.png"));
    ui->BtnScan->setText(tr("Connect"));

    ui->BtnScan->setChecked(false);
    ui->BtnScan->setEnabled(true);
    ui->BootSelectGroupBox->setEnabled(true);
    ui->RunGroupBox->setDisabled(true);
    ui->OperateGroupbox->setDisabled(true);
    ui->RcGroupBox->setDisabled(true);

    ReleaseDataBaseList();
}

void ManufacturePage::slot_BootModeChanged(int pIndex)
{
    if(m_CurRadioIndex != pIndex)
    {
        m_CurRadioIndex = pIndex;
        DeviceDisconnected();
        switch(pIndex)
        {
        case 0:
            ui->RadioUSB->setChecked(true);
            ui->RadioTF->setChecked(false);
            break;
        case 1:
            ui->RadioUSB->setChecked(false);
            ui->RadioTF->setChecked(true);
            break;
        default:
            break;
        }
    }
}

void ManufacturePage::slot_TransProgress(QString pMsg,int pValue)
{
    if(pValue >= 100 || pValue <= 0)
    {
        ui->BtnStart->setEnabled(true);
        ui->BtnPause->setEnabled(false);
        ui->BtnStop->setEnabled(false);
        GlobalPara.updateProgress(QString(""), 0);//进度条
    }
    else
    {
        GlobalPara.updateProgress(pMsg, pValue);//进度条
    }
}

void ManufacturePage::on_BtnBufferPathSelect_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"/",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;
    ui->BufferPathEdit->setText(dir);
}

void ManufacturePage::DownLoadRun()
{
    ui->BtnStart->setEnabled(false);
    ui->BtnPause->setEnabled(true);
    ui->BtnStop->setEnabled(true);
}

void ManufacturePage::DownLoadPause()
{

}

void ManufacturePage::DownLoadStop()
{
    ui->BtnStart->setEnabled(true);
    ui->BtnPause->setEnabled(false);
    ui->BtnStop->setEnabled(false);
}

void ManufacturePage::on_BtnStart_clicked()
{
    //>@判断缓冲目录是否已经设置
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

    m_DfuStopFlag = false;
    DownLoadRun();
    for(int i=0;i<4;i++)
    {
        EXEC_CMD tmpCMD;
        //>@开始获取文件
        int progress = 20*(i+1);
        RcTree *tmpTree;
        bool tmpChecked = false;
        switch(i)
        {
        case 0:
            tmpCMD = CMD_UBOOTUPGRADE;
            tmpTree = ui->BootloaderList;
            tmpChecked = ui->BootloaderGroupBox->isChecked();
            break;
        case 1:
            tmpCMD = CMD_KERNELUPGRADE;
            tmpTree = ui->KernelList;
            tmpChecked = ui->KernelGroupBox->isChecked();
            break;
        case 2:
            tmpCMD = CMD_FILESYSTEMUPGRADE;
            tmpTree = ui->FileSystemList;
            tmpChecked = ui->FilesystemGroupBox->isChecked();
            break;
        case 3:
            tmpCMD = CMD_APPLICATIONUPGRADE;
            tmpTree = ui->ApplicationList;
            tmpChecked = ui->ApplicationGroupBox->isChecked();
            break;
        default:
            continue;
        }
        if(!tmpChecked)
            continue;
        GlobalPara.updateProgress(QString("Parpare %1 ...").arg(tmpDstPath),progress);//进度条
        QString tmpFilePath;
        if(tmpTree->PrepareActiveFile(tmpDstPath, tmpFilePath))
        {
            //>@往设备下载缓冲区的文件
            GlobalPara.updateProgress(QString("Downloading %1 ...").arg(tmpFilePath),progress+10);//进度条
            if(m_UsbDfu)
                m_UsbDfu->DownloadFile(tmpFilePath, tmpCMD);
        }
        GlobalPara.updateProgress(QString("Parpare %1 Fail!").arg(tmpDstPath),progress+20);//进度条
    }
    //>@下载完毕
    while(1)
    {
        if(m_UsbDfu->isDownloaded())
            break;
        QtSleep(100);
    }
    DownLoadStop();
    GlobalPara.updateProgress(tr(""), 0);
}

void ManufacturePage::on_BtnPause_clicked()
{
    m_DfuStopFlag = true;
    DownLoadStop();
}

void ManufacturePage::on_BtnStop_clicked()
{
    m_DfuStopFlag = true;
    DownLoadStop();
}

void ManufacturePage::on_BtnScan_clicked(bool checked)
{
    if(checked)
    {
        InitDFU();
    }
    else
    {
        ReleaseDFU();
    }
}

void ManufacturePage::slot_ConnState(bool pState)
{
    if(pState == true)
        DeviceConnected();
    else
        ReleaseDFU();
}

void ManufacturePage::OnSending()
{
    ui->OperateGroupbox->setDisabled(true);
}

void ManufacturePage::OffSending()
{
    ui->OperateGroupbox->setEnabled(true);
}

void ManufacturePage::on_BtnSendCmd_clicked()
{
    QString tmpString = ui->SendTextEdit->document()->toPlainText();
    IDE_TRACE_STR(tmpString);
    if(m_UsbDfu)
    {
        OnSending();
        //>@先发送数据
        m_UsbDfu->SendData(tmpString);
        IDE_TRACE();
        //>@再发送命令
        m_UsbDfu->SendCMD(CMD_UBOOTSTD);
        IDE_TRACE();
        OffSending();
    }
}
