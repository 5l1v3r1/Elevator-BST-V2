#include "modalwindow.h"
#include "treeitemclass.h"
#include "ui_prowizard.h"
#include "ui_devwizard.h"

ProWizard::ProWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::ProWizard)
{
    m_CloseFlag = true;
    InitUI();
    connect(ui->ProjPath, SIGNAL(textChanged(QString)), this, SLOT(slot_ProjPathChanged(QString)));
}

ProWizard::ProWizard(QString pFilePath, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::ProWizard)
{
    m_CloseFlag = true;
    InitUI();
    connect(ui->ProjPath, SIGNAL(textChanged(QString)), this, SLOT(slot_ProjPathChanged(QString)));
    ui->ProjPath->setText(pFilePath);
}

ProWizard::ProWizard(bstproManager *&pManager, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::ProWizard)
{
    InitUI();
    m_ProManager = pManager;
    if(m_ProManager)
    {
        ui->ProjPath->setText(m_ProManager->m_FileDir + m_ProManager->m_FileName);
        ui->Browser->setDisabled(true);
        InitWizard();
    }
    m_CloseFlag = false;
}

ProWizard::~ProWizard()
{
    delete ui;
}

void ProWizard::InitUI()
{
    ui->setupUi(this);
    setWindowTitle(tr("Welcome to use project wizard!"));
    setWindowIcon(QIcon(":/Page/rc/Page/bstpro.png"));
    setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette tmpPalette = palette();
    tmpPalette.setBrush(QPalette::Window,
                        QBrush(Qt::lightGray)/*QImage(":/Form/rc/Form/GlassFrame1.bmp").scaled(size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation))*/);
    setPalette(tmpPalette); // widget加上了背景图.

    m_ProManager = 0;
    m_MemoryDirLock = false;
}

void ProWizard::accept()
{
    SyncWizard();
    QDialog::accept();
}

void ProWizard::rejected()
{
    if(m_CloseFlag && m_ProManager)
    {
        m_ProManager->SavePro();
        m_ProManager->deleteLater();
        m_ProManager = 0;
    }
    QDialog::rejected();
}

void ProWizard::SetMemoryDir(QString pPath, bool pLock)
{
    m_MemoryDirLock = pLock;
    m_MemoryDir = pPath;
    if(!m_MemoryDir.endsWith("/"))
        m_MemoryDir.append("/");
}

void ProWizard::slot_ProjPathChanged(QString pProPath)
{
    if(pProPath.isEmpty())
        return;
    if(m_ProManager)
    {
        m_ProManager->deleteLater();
        m_ProManager = 0;
    }
    m_ProManager = new bstproManager(pProPath);
    if(!isFileExist(pProPath))
    {
        m_ProManager->NewPro();
    }
    else
    {
        m_ProManager->OpenPro();
    }
    InitWizard();
}

void ProWizard::on_Browser_clicked()
{
    QString tmpPath = QFileDialog::getSaveFileName(this, tr("Open Solution File"),
                                                   m_MemoryDir,
                                                   tr("project (*.bstpro)"));
    if (tmpPath.isEmpty())
        return;
    QString tmpFileDir = getFileDirectory(tmpPath);
    QString tmpFileName = getFileName(tmpPath);
    if(tmpFileDir.compare(m_MemoryDir) == 0)  //>@如果建立在缓冲区内，则自动根据工程名创建一个文件夹
    {
        QString tmpName = tmpFileName;  //>@防止tmpFileName被破坏
        QString tmpProName = tmpName.remove(bstproManager::BstProSuffix);
        QString tmpProPath = tmpFileDir + tmpProName + QString("/");
        CreatPath(tmpProPath);
        tmpPath = tmpProPath;
    }
    QString tmpBuffer = getParentDirectory(tmpPath);  //>@新工程目录的父文件夹路径
    if(!m_MemoryDirLock)
    {
        m_MemoryDir = tmpBuffer;
    }
    else if(m_MemoryDir.compare(tmpBuffer))  //>@新选择的路径不符合默认路径
    {
        QMessageBox::critical(this, tr("Error"),
                              tr("The Project Path is invalid,\n"
                                 "Please create project in %1!").arg(m_MemoryDir),
                              QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    QString tmpFilePath = tmpPath+tmpFileName;
    ui->ProjPath->setText(tmpFilePath);
}

void ProWizard::InitWizard()
{
    if(m_ProManager->m_Valid == false)
    {
        IDE_TRACE();
        return;
    }
    QString tmpString = m_ProManager->GetFile("ui");
    if(!tmpString.isEmpty())
        ui->UiName->setText(tmpString);
    else
        ui->UiName->setText(D_UINAME);
    tmpString = m_ProManager->GetFile("map");
    if(!tmpString.isEmpty())
        ui->MapName->setText(tmpString);
    else
        ui->MapName->setText(D_MAPNAME);
    tmpString = m_ProManager->GetFile("mdp");
    if(!tmpString.isEmpty())
        ui->MdpName->setText(tmpString);
    else
        ui->MdpName->setText(D_MDPNAME);
}

void ProWizard::SyncWizard()
{
    //>@ProjPath,修改工程的路径变量并初始化文件夹
    QString tmpString = ui->ProjPath->text();
    if(tmpString.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"),
                              tr("The Project Path is invalid,\n"
                                 "Please correct!"),
                              QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    QString tmpProDir = getFileDirectory(tmpString);
    //>@UI
    tmpString = ui->UiName->text();
    if(!tmpString.isEmpty())
    {
        if(!tmpString.endsWith(uiManager::UiSuffix))
            tmpString.append(uiManager::UiSuffix);
        //>@修改UI文件
        QString tmpUiPath = tmpProDir + tmpString;
        bool flag = false;
        uiManager *tmpUiManager = new uiManager(tmpUiPath);
        if(!isFileExist(tmpUiPath))
        {
            flag = tmpUiManager->NewUI();
        }
        else
        {
            flag = tmpUiManager->OpenUI();
        }
        if(!flag)
        {
            QMessageBox::critical(this, tr("Error"),
                                   tr("Can't create/open ui file,\n"
                                      "Please check Path:%1!").arg(tmpProDir),
                                   QMessageBox::Ok,
                                   QMessageBox::Ok);
        }
        else
        {
            //>@根据界面配置修改Desktop的大小
            QString tmpSizeW = ui->UiSizeW->text();
            QString tmpSizeH = ui->UiSizeH->text();
            if(!tmpSizeW.isEmpty() && !tmpSizeH.isEmpty())
            {
                QRect tmpComRect = QRect(0,0,tmpSizeW.toInt(),tmpSizeH.toInt());
                QDomElement tmpDesktopElement = tmpUiManager->m_ComElementGroup.value(COM_DESKTOP);
                if(tmpDesktopElement.isNull())
                {
                    tmpUiManager->AddComDom(COM_DESKTOP, tmpComRect, Horizontal);
                }
                else
                {
                    tmpUiManager->ModifyComRect(tmpDesktopElement, tmpComRect, Horizontal);
                }
            }
            m_ProManager->AddFile("ui", tmpString);
        }
        delete tmpUiManager;
    }
    else
    {
        m_ProManager->DelFile("ui");
    }
    //>@MAP
    tmpString = ui->MapName->text();
    if(!tmpString.isEmpty())
    {
        if(!tmpString.endsWith(mapManager::MapSuffix))
            tmpString.append(mapManager::MapSuffix);
        //>@修改MAP文件
        QString tmpMapPath = tmpProDir + tmpString;
        bool flag = false;
        mapManager *tmpMapManager = new mapManager(tmpMapPath);
        if(!isFileExist(tmpMapPath))
        {
            flag = tmpMapManager->NewMAP();
        }
        else
        {
            flag = tmpMapManager->OpenMAP();
        }
        if(!flag)
        {
            QMessageBox::critical(this, tr("Error"),
                                   tr("Can't create/open map file,\n"
                                      "Please check Path:%1!").arg(tmpProDir),
                                   QMessageBox::Ok,
                                   QMessageBox::Ok);
        }
        else
        {
            m_ProManager->AddFile("map", tmpString);
        }
        delete tmpMapManager;
    }
    else
    {
        m_ProManager->DelFile("map");
    }
    //>@MDP页
    tmpString = ui->MdpName->text();
    if(!tmpString.isEmpty())
    {
        if(!tmpString.endsWith(mdpManager::MdpSuffix))
            tmpString.append(mdpManager::MdpSuffix);
        QString tmpMdpPath = tmpProDir + tmpString;
        bool flag = false;
        mdpManager *tmpMdpManager = new mdpManager(tmpMdpPath);
        if(isFileExist(tmpMdpPath) == false)
            flag = tmpMdpManager->NewMDP();
        else
            flag = tmpMdpManager->OpenMDP();
        if(!flag)
        {
            QMessageBox::critical(this, tr("Error"),
                                   tr("Can't create/open mdp file,\n"
                                      "Please check Path:%1!").arg(tmpProDir),
                                   QMessageBox::Ok,
                                   QMessageBox::Ok);
        }
        else
        {
            tmpMdpManager->SetNickName(ui->NickName->text());
            tmpMdpManager->SetPassword(ui->Password->text());
            m_ProManager->AddFile("mdp", tmpString);
        }
        tmpMdpManager->m_IsChanged = true;
        delete tmpMdpManager;
    }
    else
    {
        m_ProManager->DelFile("mdp");
    }
    m_ProManager->SavePro();
}

DevWizard::DevWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::DevWizard)
{
    m_Dev = 0;
    m_CloseFlag = true;
    InitUI();
}

DevWizard::DevWizard(QString pPath, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::DevWizard)
{
    m_Dev = 0;
    m_CloseFlag = true;
    InitUI();
    ui->DevBufferPath->setText(pPath);
}

DevWizard::DevWizard(BaseDevice *&pDev, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::DevWizard)
{
    m_Dev = pDev;
    InitUI();
    //>@设备创建好后，不可以修改设备类型，但可以修改地址以及缓冲路径
    if(m_Dev)
    {
        ui->DevType->setCurrentIndex((int)(m_Dev->mDevType));
        ui->DevType->setDisabled(true);

        ui->DevAddr->setText(m_Dev->mDevAddr);
        ui->DevBufferPath->setText(m_Dev->mBufferPath);
        m_CloseFlag = false;
    }
    else
    {
        m_CloseFlag = true;
    }
}

DevWizard::~DevWizard()
{
    delete ui;
}

void DevWizard::InitUI()
{
    ui->setupUi(this);
    setWindowTitle(tr("Welcome to use device wizard!"));
    setWindowIcon(QIcon(":/Menu/rc/Menu/device.png"));

    setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette tmpPalette = this->palette();
    tmpPalette.setBrush(QPalette::Window, QBrush(Qt::lightGray));
    setPalette(tmpPalette); // widget加上了背景图.
}

void DevWizard::SyncUI()
{
    QString tmpDevAddr = ui->DevAddr->text();
    QString tmpBufferPath = ui->DevBufferPath->text();  //>?中文编码问题没有解决
    if(!m_Dev)
    {
        BaseDevice::DEVICETYPE tmpType = (BaseDevice::DEVICETYPE)(ui->DevType->currentIndex());
        m_Dev = CreateDevice(tmpType, tmpDevAddr, tmpBufferPath);
    }
    else
    {
        m_Dev->ChangeDevAddr(tmpDevAddr);
        m_Dev->ChangeBuffer(tmpBufferPath);
    }
}

void DevWizard::accept()
{
    SyncUI();
    QDialog::accept();
}

void DevWizard::rejected()
{
    if(m_CloseFlag && m_Dev)
    {
        m_Dev->deleteLater();
        m_Dev = 0;
    }
    QDialog::rejected();
}


void DevWizard::on_Browser_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select Directory"),
                                                    m_MemoryDir,
                                                    QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;
    m_MemoryDir = dir;
    ui->DevBufferPath->setText(dir);
}
