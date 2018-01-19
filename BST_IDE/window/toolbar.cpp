#include "toolbar.h"
#include "mainwindow.h"
#include "projectmanager.h"
#include "deviceclass.h"
#include "simulatorform.h"
#include "devicequeryform.h"
#include "deviceinfo.h"
#include "toolbox.h"
#include "propertytree.h"
#include "findfileform.h"

BaseToolBar::BaseToolBar(QWidget *parent) :
    QToolBar(parent)
{
    mMainWindow = VAR_CAST<MainWindow*>(parent);
}

BaseToolBar::~BaseToolBar()
{

}

DeviceToolBar:: DeviceToolBar(QWidget *parent) :
    BaseToolBar(parent)
{
    setAllowedAreas(Qt::LeftToolBarArea);
    setOrientation (Qt::Vertical);

    ProjectBar *tmpProjectBar = VAR_CAST<ProjectBar*>(GlobalPara.m_ProjectBar);
    mProjectManager = tmpProjectBar->mProjectManager;

    m_UpgradeState = STOP;
    mDevInfoForm = 0;
    mDevTreeItem = 0;
    CreateLeftFrame();
    InitAction();
}

void DeviceToolBar::CreateLeftFrame()
{
    //>@DEV INFO
    mDevInfoDock = new QDockWidget(tr("DeviceInfo"));
    mDevInfoDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    mMainWindow->addDockWidget(Qt::LeftDockWidgetArea, mDevInfoDock);
    //mDevInfoDock->setMinimumWidth(200);
    mDevInfoDock->setVisible(false);

    mDevInfoForm = new DeviceInfo(this);
    connect(this, SIGNAL(sUpdate()), mDevInfoForm, SLOT());
    mDevInfoDock->setWidget(mDevInfoForm);
}

void DeviceToolBar::InitAction()
{
    devInfoAct = mDevInfoDock->toggleViewAction();
    devInfoAct->setIcon(QIcon(":/Menu/rc/Menu/deviceconf.png"));
    devInfoAct->setStatusTip(tr("View Information ..."));
    devInfoAct->setCheckable(true);
    devInfoAct->setChecked(false);

    startAct = new QAction(QIcon(":/Menu/rc/Menu/build.png"), tr("&Start"), this);
    startAct->setStatusTip(tr("Start Executing ..."));
    connect(startAct, SIGNAL(triggered()), mProjectManager, SLOT(slot_StartSync()));

    pauseAct = new QAction(QIcon(":/Menu/rc/Menu/pause.png"), tr("&Pause"), this);
    pauseAct->setStatusTip(tr("Pause Executing ..."));
    connect(startAct, SIGNAL(triggered()), mProjectManager, SLOT(slot_PauseSync()));

    stopAct = new QAction(QIcon(":/Menu/rc/Menu/stop.png"), tr("&Stop"), this);
    stopAct->setStatusTip(tr("Stop Executing ..."));
    connect(startAct, SIGNAL(triggered()), mProjectManager, SLOT(slot_StopSync()));

    uiDesignerAct = new QAction(QIcon(":/Page/rc/Page/ui.png"), tr("UIDesigner"), this);                //>@UI设计工具
    uiDesignerAct->setStatusTip(tr("Help to designer a ui theme."));
    connect(uiDesignerAct, SIGNAL(triggered()), mProjectManager, SLOT(slot_OpenUiDesignerTool()));

    scheduleMapAct = new QAction(QIcon(":/Menu/rc/Menu/grid.png"), tr("ScheduleMap"), this);                //>@UI设计工具
    scheduleMapAct->setStatusTip(tr("Help to designer a ScheduleMap."));
    connect(scheduleMapAct, SIGNAL(triggered()), mProjectManager, SLOT(slot_OpenScheduleMapTool()));

    mdpSetAct = new QAction(QIcon(":/Page/rc/Page/mdp.png"), tr("MDPSet"), this);           //>@近场设置工具
    mdpSetAct->setStatusTip(tr("Help to modify/create a mdp file."));
    connect(mdpSetAct, SIGNAL(triggered()), mProjectManager, SLOT(slot_OpenMdpSetTool()));

    softwareTestAct = new QAction(QIcon(":/Page/rc/Page/stc.png"), tr("SoftwareTest"), this);           //>@软件测试工具
    softwareTestAct->setStatusTip(tr("Help to simulator device."));
    connect(softwareTestAct, SIGNAL(triggered()), mProjectManager, SLOT(slot_OpenSoftwareTestTool()));

    logQueryAct = new QAction(QIcon(":/Page/rc/Page/table.png"), tr("LogQuery"), this);           //>@用户管理工具
    logQueryAct->setStatusTip(tr("Help to parse device log file."));
    connect(logQueryAct, SIGNAL(triggered()), mProjectManager, SLOT(slot_OpenLogQueryTool()));

    addAction(uiDesignerAct);
    addSeparator();
    addAction(scheduleMapAct);
    addSeparator();
    addAction(mdpSetAct);
    addSeparator();
    addAction(softwareTestAct);
    addSeparator();
    addAction(logQueryAct);
    addSeparator();

    addWidget(new QLabel());
    addWidget(new QLabel());
    addAction(devInfoAct);
    addWidget(new QLabel());
    addWidget(new QLabel());

    addSeparator();
    addAction(startAct);
    addSeparator();
    addAction(pauseAct);
    addSeparator();
    addAction(stopAct);

    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIconSize (QSize(32, 32) );
    setMovable(false);
    setFloatable(false);

    slot_Disconnected();
}

DeviceToolBar::~DeviceToolBar()
{
    startAct->deleteLater();
    pauseAct->deleteLater();
    stopAct->deleteLater();
    uiDesignerAct->deleteLater();
    scheduleMapAct->deleteLater();
    mdpSetAct->deleteLater();
    softwareTestAct->deleteLater();
    logQueryAct->deleteLater();
    devInfoAct->deleteLater();
    deviceToolMenu->deleteLater();
}

QMenu *DeviceToolBar::GenerateMenu()
{
    deviceToolMenu = new QMenu(tr("&Device"));
    deviceToolMenu->addSeparator();
    deviceToolMenu->addAction(startAct);
    deviceToolMenu->addAction(pauseAct);
    deviceToolMenu->addAction(stopAct);
    deviceToolMenu->addSeparator();
    deviceToolMenu->addAction(devInfoAct);
    deviceToolMenu->addAction(uiDesignerAct);
    deviceToolMenu->addAction(scheduleMapAct);
    deviceToolMenu->addAction(mdpSetAct);
    deviceToolMenu->addAction(softwareTestAct);
    deviceToolMenu->addAction(logQueryAct);
    return deviceToolMenu;
}

bool DeviceToolBar::isConnected()
{
    ProjectBar *tmpProBar = VAR_CAST<ProjectBar *>(GlobalPara.m_ProjectBar);
    if(tmpProBar && tmpProBar->mProjectManager && mDevTreeItem &&
       mDevTreeItem->mInterfaceDevice &&
       mDevTreeItem->mInterfaceDevice->isSync())
        return true;
    return false;
}

void DeviceToolBar::slot_Connstate(DeviceTreeItem *pDevItem)
{
    if(mDevTreeItem != pDevItem)
    {
        mDevTreeItem = pDevItem;
    }
    if(isConnected())
    {
        //>@设置抓取屏幕图的临时路径
        if(pDevItem->mInterfaceDevice->mDevType == BaseDevice::TYPE_NET)
        {
            NetDevice *tmpDev = VAR_CAST<NetDevice*>(pDevItem->mInterfaceDevice);
            if(tmpDev && tmpDev->mNetManager)
            {
                QString tmpThemeInfo = tmpDev->mNetManager->m_DevInfoList.value("ThemeManager");
                QStringList tmpThemeParas = tmpThemeInfo.split(',');
                if(tmpThemeParas.count())
                {
                    mDevInfoForm->SetDevGrabScreenSrc(tmpThemeParas.at(0)+D_GrabScreenName);
                }
            }
        }
        mDevInfoForm->SetBufferPath(pDevItem->mInterfaceDevice->mBufferPath);

        slot_Connected();
    }
    else
    {
        slot_Disconnected();
    }
    emit sUpdate();
}

void DeviceToolBar::slot_Connected()
{
    startAct->setEnabled(true);
    pauseAct->setEnabled(true);
    stopAct->setEnabled(true);
    uiDesignerAct->setEnabled(true);
    scheduleMapAct->setEnabled(true);
    mdpSetAct->setEnabled(true);
    softwareTestAct->setEnabled(true);
    logQueryAct->setEnabled(true);
}

void DeviceToolBar::slot_Disconnected()
{
    startAct->setDisabled(true);
    pauseAct->setDisabled(true);
    stopAct->setDisabled(true);
    uiDesignerAct->setDisabled(true);
    scheduleMapAct->setDisabled(true);
    mdpSetAct->setDisabled(true);
    softwareTestAct->setDisabled(true);
    logQueryAct->setDisabled(true);
}

ProjectBar::ProjectBar(QWidget *parent) :
    BaseToolBar(parent)
{
    setAllowedAreas(Qt::LeftToolBarArea);
    setOrientation (Qt::Vertical);
    CreateFrame();
    InitAction();
    slot_OpenBar(true);
    slot_UpdateBar();
}

ProjectBar::~ProjectBar()
{
    mOpenDockAct->deleteLater();
    mProjectManager->deleteLater();
    mProjectViewDock->deleteLater();

    mTagViewAct->deleteLater();
    mNetViewAct->deleteLater();

    mProjectMenu->deleteLater();
}

void ProjectBar::CreateFrame()
{
    mProjectViewDock = new QDockWidget(tr("Device Manager"));
    mProjectViewDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    mProjectManager = new ProjectManager;
    mProjectViewDock->setWidget(mProjectManager);

    mTagViewDock = new QDockWidget(tr("Tag View"));
    mTagViewDock->setAllowedAreas(Qt::LeftDockWidgetArea);

    mNetViewDock = new QDockWidget(tr("Net View"));
    mNetViewDock->setAllowedAreas(Qt::LeftDockWidgetArea);

    mPropertyViewDock = new QDockWidget(tr("Property"));
    mPropertyViewDock->setAllowedAreas(Qt::RightDockWidgetArea);
    mPropertyWidget = new PropertyTree;
    mPropertyViewDock->setWidget(mPropertyWidget);

    mToolViewDock = new QDockWidget(tr("Tool"));
    mToolViewDock->setAllowedAreas(Qt::RightDockWidgetArea);
    mToolTreeView = new ToolBox;
    mToolViewDock->setWidget(mToolTreeView);

    mSimulatorViewDock = new QDockWidget(tr("Simulator"));
    mSimulatorViewDock->setAllowedAreas(Qt::RightDockWidgetArea);
    mSimulatorView = new SimulatorForm;
    mSimulatorViewDock->setWidget(mSimulatorView);

    mMainWindow->addDockWidget(Qt::LeftDockWidgetArea, mProjectViewDock);
    mMainWindow->addDockWidget(Qt::LeftDockWidgetArea, mTagViewDock);
    mMainWindow->addDockWidget(Qt::LeftDockWidgetArea, mNetViewDock);
    mMainWindow->addDockWidget(Qt::RightDockWidgetArea, mPropertyViewDock);
    mMainWindow->addDockWidget(Qt::RightDockWidgetArea, mToolViewDock);
    mMainWindow->addDockWidget(Qt::RightDockWidgetArea, mSimulatorViewDock);

    TabPageManager *tmpPageManager = VAR_CAST<TabPageManager*>(GlobalPara.m_TabPageManager);
    if(tmpPageManager)
        connect(tmpPageManager, SIGNAL(sTabPageChanged()), this, SLOT(slot_UpdateBar()));
}

void ProjectBar::InitAction()
{
    mOpenDockAct = new QAction(QIcon(":/Menu/rc/Menu/list.png"), tr("&Open..."), this);
    mOpenDockAct->setShortcuts(QKeySequence::Open);
    mOpenDockAct->setStatusTip(tr("Show Project Manager View"));
    connect(mOpenDockAct, SIGNAL(toggled(bool)), this, SLOT(slot_OpenBar(bool)));
    mOpenDockAct->setCheckable(true);
    mOpenDockAct->setChecked(true);

    addAction(mOpenDockAct);

    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIconSize (QSize(32, 32) );
    setMovable(false);
    setFloatable(false);
}

void ProjectBar::slot_OpenBar(bool flag)
{
    if(flag)
    {
        mProjectViewDock->setVisible(true);
        mTagViewDock->setVisible(true);
        mNetViewDock->setVisible(true);

        mMainWindow->tabifyDockWidget(mProjectViewDock, mTagViewDock);
        mMainWindow->tabifyDockWidget(mTagViewDock, mNetViewDock);
        mMainWindow->tabifyDockWidget(mTagViewDock, mProjectViewDock);
        mProjectViewDock->raise();
    }
    else
    {
        mProjectViewDock->setVisible(false);
        mTagViewDock->setVisible(false);
        mNetViewDock->setVisible(false);
    }
}

void ProjectBar::slot_UpdateBar()
{
    TabPageManager *tmpTabManager = VAR_CAST<TabPageManager *>(sender());
    if(tmpTabManager)
    {
        PAGE_INFO *tmpPage = tmpTabManager->getActivePage();
        if(tmpPage)
        {
            if(tmpPage->FileType == TYPE_UI)
            {
                mPropertyViewDock->setVisible(true);
                mToolViewDock->setVisible(true);
                mSimulatorViewDock->setVisible(true);
                mToolTreeView->InitToolBox(TYPE_UI);
                mMainWindow->tabifyDockWidget(mPropertyViewDock, mToolViewDock);
                mMainWindow->tabifyDockWidget(mToolViewDock, mSimulatorViewDock);
                mPropertyViewDock->raise();
                return;
            }
            else if(tmpPage->FileType == TYPE_MDP)
            {
                mPropertyViewDock->setVisible(true);
                mToolViewDock->setVisible(true);
                mSimulatorViewDock->setVisible(false);
                mToolTreeView->InitToolBox(TYPE_MDP);
                mMainWindow->tabifyDockWidget(mPropertyViewDock, mToolViewDock);
                mPropertyViewDock->raise();
                return;
            }
        }
    }
    mPropertyViewDock->setVisible(false);
    mToolViewDock->setVisible(false);
    mSimulatorViewDock->setVisible(false);
}

QMenu *ProjectBar::GenerateMenu()
{
    mProjectMenu = new QMenu(tr("Project"));
    mProjectMenu->addAction(mOpenDockAct);
    return mProjectMenu;
}

SQLToolBar::SQLToolBar(QWidget *parent) :
    BaseToolBar(parent)
{
    setAllowedAreas(Qt::RightToolBarArea);
    setOrientation (Qt::Vertical);
    CreateFrame();
    InitAction();
}

SQLToolBar::~SQLToolBar()
{
    openDockAct->deleteLater();
    mDevQueryFrom->deleteLater();
    mDevQueryViewDock->deleteLater();
}

void SQLToolBar::CreateFrame()
{
    mDevQueryViewDock = new QDockWidget(tr("SQL Server"));
    mDevQueryViewDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    mMainWindow->addDockWidget(Qt::RightDockWidgetArea, mDevQueryViewDock);

    mDevQueryFrom = new DeviceQueryForm;
    connect(mDevQueryFrom, SIGNAL(sConnstate(bool, QString)), this, SLOT(slot_Connstate(bool, QString)));
    mDevQueryViewDock->setWidget(mDevQueryFrom);
    mDevQueryViewDock->setVisible(false);
}

void SQLToolBar::InitAction()
{
    openDockAct = mDevQueryViewDock->toggleViewAction();
    openDockAct->setIcon(QIcon(":/Menu/rc/Menu/notpass.png"));
    openDockAct->setStatusTip(tr("deviceQuery View"));
    openDockAct->setCheckable(true);
    openDockAct->setChecked(false);

    addAction(openDockAct);

    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIconSize (QSize(32, 32) );
    setMovable(false);
    setFloatable(false);

    slot_Disconnected();
}

QMenu *SQLToolBar::GenerateMenu()
{
    sqlToolMenu = new QMenu(tr("&SQL"));
    sqlToolMenu->addAction(openDockAct);
    return sqlToolMenu;
}

void SQLToolBar::slot_Connstate(bool pState, QString pIP)
{
    if(pState)
        slot_Connected();
    else
        slot_Disconnected();
}

void SQLToolBar::slot_Connected()
{
    openDockAct->setIcon(QIcon(":/Menu/rc/Menu/pass.png"));
}

void SQLToolBar::slot_Disconnected()
{
    openDockAct->setIcon(QIcon(":/Menu/rc/Menu/notpass.png"));
}

DockViewToolBar::DockViewToolBar(QWidget *parent) :
    BaseToolBar(parent)
{
    setAllowedAreas(Qt::TopToolBarArea);
    setOrientation (Qt::Horizontal);
    CreateFrame();
    InitAction();
    InitDockView(ModeDefault);
}

DockViewToolBar::~DockViewToolBar()
{
    helpViewAct->deleteLater();
    outputViewAct->deleteLater();
    findViewAct->deleteLater();

    helpViewDock->deleteLater();
    outputViewDock->deleteLater();
    findViewDock->deleteLater();

    viewMenu->deleteLater();
}

void DockViewToolBar::InitAction()
{
    helpViewAct = helpViewDock->toggleViewAction();
    helpViewAct->setIcon(QIcon(":/Menu/rc/Menu/help.png"));
    helpViewAct->setStatusTip(tr("Help View"));
    helpViewAct->setCheckable(true);

    outputViewAct = outputViewDock->toggleViewAction();
    outputViewAct->setIcon(QIcon(":/Menu/rc/Menu/outputView.png"));
    outputViewAct->setStatusTip(tr("Output View"));
    outputViewAct->setCheckable(true);

    findViewAct = findViewDock->toggleViewAction();
    findViewAct->setIcon(QIcon(":/Menu/rc/Menu/search.png"));
    findViewAct->setStatusTip(tr("Find View"));
    findViewAct->setCheckable(true);

    addAction(helpViewAct);
    addSeparator();
    addAction(outputViewAct);
    addAction(findViewAct);

    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIconSize (QSize(32, 32) );
    setMovable(false);
    setFloatable(false);
}

void DockViewToolBar::CreateFrame()
{
    helpViewDock = new QDockWidget(tr("Help"));
    helpViewDock->setAllowedAreas(Qt::RightDockWidgetArea);
    mMainWindow->addDockWidget(Qt::RightDockWidgetArea, helpViewDock);

    findViewDock = new QDockWidget(tr("Find/Replace"));
    findViewDock->setAllowedAreas(Qt::RightDockWidgetArea);
    mMainWindow->addDockWidget(Qt::RightDockWidgetArea, findViewDock);
    mFindForm = new CFindFileForm;
    findViewDock->setWidget(mFindForm);

    mMainWindow->tabifyDockWidget(findViewDock, helpViewDock);

    outputViewDock = new QDockWidget(tr("Output"));
    outputViewDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    mMainWindow->addDockWidget(Qt::BottomDockWidgetArea, outputViewDock);
    //>@
    GlobalPara.m_OutputMsgList = new QTextEdit;
    GlobalPara.m_OutputMsgList->append(QString(">------ Welcome to use BST IDE! ------"));
    outputViewDock->setWidget(GlobalPara.m_OutputMsgList);
}

void DockViewToolBar::UpdateDockView(int pMode)
{
    InitDockView((USERMODE)pMode);
}

void DockViewToolBar::InitDockView(USERMODE pMode)
{
    switch(pMode)
    {
        case ModeNetworkManager:
        {
            helpViewDock->setVisible(true);
            outputViewDock->setVisible(false);
            findViewDock->setVisible(false);
            break;
        }
        case ModeManufacturingTechnician:
        {
            helpViewDock->setVisible(true);
            outputViewDock->setVisible(true);
            findViewDock->setVisible(false);
            break;
        }
        case ModeSoftwareTester:
        {
            helpViewDock->setVisible(false);
            outputViewDock->setVisible(false);
            findViewDock->setVisible(false);
            break;
        }
        case ModeAftersalesMaintenanceMember:
        {
            helpViewDock->setVisible(true);
            outputViewDock->setVisible(true);
            findViewDock->setVisible(false);
            break;
        }
        case ModeCustomer:
        {
            helpViewDock->setVisible(true);
            outputViewDock->setVisible(false);
            findViewDock->setVisible(true);
            break;
        }
        default:
        {
            helpViewDock->setVisible(false);
            outputViewDock->setVisible(false);
            findViewDock->setVisible(false);
            break;
        }
    }
}

QMenu *DockViewToolBar::GenerateMenu()
{
    viewMenu = new QMenu(tr("&View"));
    viewMenu->addAction(helpViewAct);
    viewMenu->addSeparator();
    viewMenu->addAction(outputViewAct);
    viewMenu->addAction(findViewAct);
    return viewMenu;
}

QComboBox *DockViewToolBar::GenerateUserModeCombox()
{
    userModeComBox = new QComboBox;
    userModeComBox->setMinimumHeight(30);
    userModeComBox->setMinimumWidth(70);
    QStringList tmpList;
    tmpList << tr("Manager")
            << tr("Technician")
            << tr("Tester")
            << tr("Maintenancer")
            << tr("Customer")
            << tr("Default");
    userModeComBox->clear();
    userModeComBox->addItems(tmpList);
    connect(userModeComBox, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDockView(int)));
    userModeComBox->setCurrentIndex(5);
    return userModeComBox;
}

FileToolBar::FileToolBar(QWidget *parent) :
    BaseToolBar(parent)
{
    ProjectBar *tmpProjectBar = VAR_CAST<ProjectBar*>(GlobalPara.m_ProjectBar);
    mProjectManager = tmpProjectBar->mProjectManager;

    newAct = new QAction(QIcon(":/Menu/rc/Menu/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), mProjectManager, SLOT(newFile()));

    openAct = new QAction(QIcon(":/Menu/rc/Menu/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), mProjectManager, SLOT(open()));

    addAct = new QAction(tr("&Add"), this);
    addAct->setShortcuts(QKeySequence::UnknownKey);
    addAct->setStatusTip(tr("Add a new file"));
    connect(addAct, SIGNAL(triggered()), mProjectManager, SLOT(add()));

    saveAct = new QAction(QIcon(":/Menu/rc/Menu/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), mProjectManager, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), mProjectManager, SLOT(saveAs()));

    saveSolutionAct = new QAction(QIcon(":/Menu/rc/Menu/saveall.png"), tr("Save Solution"), this);
    saveSolutionAct->setStatusTip(tr("Save all of solution"));
    connect(saveSolutionAct, SIGNAL(triggered()), mProjectManager, SLOT(saveSolution()));

    closeAct = new QAction(tr("Close"), this);
    closeAct->setShortcuts(QKeySequence::UnknownKey);
    closeAct->setStatusTip(tr("Close current file"));
    connect(closeAct, SIGNAL(triggered()), mProjectManager, SLOT(close()));

    closeSolutionAct = new QAction(tr("Close Solution"), this);
    closeSolutionAct->setShortcuts(QKeySequence::UnknownKey);
    closeSolutionAct->setStatusTip(tr("Close current Solution"));
    connect(closeSolutionAct, SIGNAL(triggered()), mProjectManager, SLOT(closeSolution()));

    importAct = new QAction(tr("&Import"), this);
    importAct->setShortcuts(QKeySequence::UnknownKey);
    importAct->setStatusTip(tr("Import current solution"));
    connect(importAct, SIGNAL(triggered()), mProjectManager, SLOT(importProject()));

    exportAct = new QAction(tr("&Export"), this);
    exportAct->setShortcuts(QKeySequence::UnknownKey);
    exportAct->setStatusTip(tr("Export current solution"));
    connect(exportAct, SIGNAL(triggered()), mProjectManager, SLOT(exportProject()));

    printAct = new QAction(QIcon(":/Menu/rc/Menu/print.png"), tr("&Print"), this);
    printAct->setShortcuts(QKeySequence::Print);
    printAct->setStatusTip(tr("Print current document"));
    connect(printAct, SIGNAL(triggered()), mProjectManager, SLOT(print()));

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));
    connect(quitAct, SIGNAL(triggered()), mProjectManager, SLOT(quit()));

    addAction(newAct);
    addAction(openAct);
    addAction(saveAct);
    addAction(saveSolutionAct);
    addSeparator();
    addAction(printAct);
    setObjectName(tr("File"));
}

FileToolBar::~FileToolBar()
{
    newAct->deleteLater();
    openAct->deleteLater();
    addAct->deleteLater();        //附加已存在文件
    saveAct->deleteLater();
    saveAsAct->deleteLater();
    saveSolutionAct->deleteLater();   //保存整个工程
    closeAct->deleteLater();          //关闭文件
    closeSolutionAct->deleteLater();  //关闭项目
    importAct->deleteLater();
    exportAct->deleteLater();
    printAct->deleteLater();
    quitAct->deleteLater();

    fileMenu->deleteLater();
}

QMenu *FileToolBar::GenerateMenu()
{
    fileMenu = new QMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(saveSolutionAct);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAct);
    fileMenu->addAction(closeSolutionAct);
    fileMenu->addSeparator();
    fileMenu->addAction(importAct);
    fileMenu->addAction(exportAct);
    fileMenu->addSeparator();
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);
    return fileMenu;
}

EditToolBar::EditToolBar(QWidget *parent) :
    BaseToolBar(parent)
{
    ProjectBar *tmpProjectBar = VAR_CAST<ProjectBar*>(GlobalPara.m_ProjectBar);
    mProjectManager = tmpProjectBar->mProjectManager;

    undoAct = new QAction(QIcon(":/Menu/rc/Menu/undo.png"), tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Revoked the last operation"));
    connect(undoAct, SIGNAL(triggered()), mProjectManager, SLOT(undo()));

    redoAct = new QAction(QIcon(":/Menu/rc/Menu/redo.png"), tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo the last operation"));
    connect(redoAct, SIGNAL(triggered()), mProjectManager, SLOT(redo()));

    cutAct = new QAction(QIcon(":/Menu/rc/Menu/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(cutAct, SIGNAL(triggered()), mProjectManager, SLOT(cut()));

    copyAct = new QAction(QIcon(":/Menu/rc/Menu/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    connect(copyAct, SIGNAL(triggered()), mProjectManager, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/Menu/rc/Menu/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    connect(pasteAct, SIGNAL(triggered()), mProjectManager, SLOT(paste()));

    selectAllAct = new QAction(tr("Select &All"), this);
    selectAllAct->setShortcuts(QKeySequence::SelectAll);
    selectAllAct->setStatusTip(tr("Select all contexts"));
    connect(selectAllAct, SIGNAL(triggered()), mProjectManager, SLOT(selectAll()));

    findAct = new QAction(QIcon(":/Menu/rc/Menu/search.png"), tr("&Find"), this);
    findAct->setShortcuts(QKeySequence::Find);
    findAct->setStatusTip(tr("Find the Keyword"));
    connect(findAct, SIGNAL(triggered()), mProjectManager, SLOT(find()));

    replaceAct = new QAction(QIcon(":/Menu/rc/Menu/replace.png"), tr("&Replace"), this);
    replaceAct->setShortcuts(QKeySequence::Replace);
    replaceAct->setStatusTip(tr("Replace keyword with new words"));
    connect(replaceAct, SIGNAL(triggered()), mProjectManager, SLOT(replace()));

    findNextAct = new QAction(QIcon(":/Menu/rc/Menu/next.png"), tr("Find &Next"), this);
    findNextAct->setShortcuts(QKeySequence::FindNext);
    findNextAct->setStatusTip(tr("Find next keyword"));
    connect(findNextAct, SIGNAL(triggered()), mProjectManager, SLOT(findNext()));

    findPreviousAct = new QAction(QIcon(":/Menu/rc/Menu/previous.png"), tr("Find &Previous"), this);
    findPreviousAct->setShortcuts(QKeySequence::FindPrevious);
    findPreviousAct->setStatusTip(tr("Find previous keyword"));
    connect(findPreviousAct, SIGNAL(triggered()), mProjectManager, SLOT(findPrevious()));

    gotoAct = new QAction(tr("&GoTo"), this);
    gotoAct->setStatusTip(tr("Go to the specific Line"));
    connect(gotoAct, SIGNAL(triggered()), mProjectManager, SLOT(gotoSomeWhere()));

    addAction(undoAct);
    addAction(redoAct);
    addSeparator();
    addAction(cutAct);
    addAction(copyAct);
    addAction(pasteAct);
    addSeparator();
    addAction(findAct);
    addAction(replaceAct);
    addAction(findPreviousAct);
    addAction(findNextAct);
    setObjectName(tr("Edit"));
}

EditToolBar::~EditToolBar()
{
    undoAct->deleteLater();
    redoAct->deleteLater();
    cutAct->deleteLater();
    copyAct->deleteLater();
    pasteAct->deleteLater();
    selectAllAct->deleteLater();
    findAct->deleteLater();
    replaceAct->deleteLater();
    findPreviousAct->deleteLater();
    findNextAct->deleteLater();
    gotoAct->deleteLater();

    editMenu->deleteLater();
}

QMenu *EditToolBar::GenerateMenu()
{
    editMenu = new QMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();
    editMenu->addAction(selectAllAct);
    editMenu->addSeparator();
    editMenu->addAction(findAct);
    editMenu->addAction(replaceAct);
    editMenu->addAction(findNextAct);
    editMenu->addAction(findPreviousAct);
    editMenu->addSeparator();
    editMenu->addAction(gotoAct);
    return editMenu;
}

TextToolBar::TextToolBar(QWidget *parent) :
    BaseToolBar(parent)
{
    ProjectBar *tmpProjectBar = VAR_CAST<ProjectBar*>(GlobalPara.m_ProjectBar);
    mProjectManager = tmpProjectBar->mProjectManager;

    textBoldAct = new QAction(QIcon(":/Menu/rc/Menu/textbold.png"), tr("&Bold"), this);
    textBoldAct->setShortcuts(QKeySequence::Bold);
    textBoldAct->setStatusTip(tr("Set the current selection's contents bold"));
    connect(textBoldAct, SIGNAL(triggered()), mProjectManager, SLOT(textBold()));

    textUnderlineAct = new QAction(QIcon(":/Menu/rc/Menu/textunder.png"), tr("&Underline"), this);
    textUnderlineAct->setStatusTip(tr("Set the current selection's contents Underline"));
    connect(textUnderlineAct, SIGNAL(triggered()), mProjectManager, SLOT(textUnderline()));

    textItalicAct = new QAction(QIcon(":/Menu/rc/Menu/textitalic.png"), tr("&Italic"), this);
    textItalicAct->setShortcuts(QKeySequence::Italic);
    textItalicAct->setStatusTip(tr("Set the current selection's contents Italic"));
    connect(textItalicAct, SIGNAL(triggered()), mProjectManager, SLOT(textItalic()));

    textFontAct = new QAction(QIcon(":/Menu/rc/Menu/font.png"), tr("&Font"), this);
    textFontAct->setStatusTip(tr("Set Font of the current selection's contents"));
    connect(textFontAct, SIGNAL(triggered()), this, SLOT(textFont()));

    alignLeftAct = new QAction(QIcon(":/Menu/rc/Menu/textleft.png"), tr("Align&Left"), this);
    alignLeftAct->setStatusTip(tr("Set the current selection's contents align Left"));
    connect(alignLeftAct, SIGNAL(triggered()), mProjectManager, SLOT(alignLeft()));

    alignCenterAct = new QAction(QIcon(":/Menu/rc/Menu/textcenter.png"), tr("Align&Center"), this);
    alignCenterAct->setStatusTip(tr("Set the current selection's contents align Center"));
    connect(alignCenterAct, SIGNAL(triggered()), mProjectManager, SLOT(alignCenter()));

    alignRightAct = new QAction(QIcon(":/Menu/rc/Menu/textright.png"), tr("Align&Right"), this);
    alignRightAct->setStatusTip(tr("Set the current selection's contents align Right"));
    connect(alignRightAct, SIGNAL(triggered()), mProjectManager, SLOT(alignRight()));

    alignJustifyAct = new QAction(QIcon(":/Menu/rc/Menu/textjustify.png"), tr("Align&Justify"), this);
    alignJustifyAct->setStatusTip(tr("Set the current selection's contents align Justify"));
    connect(alignJustifyAct, SIGNAL(triggered()), mProjectManager, SLOT(alignJustify()));

    addAction(textBoldAct);
    addAction(textUnderlineAct);
    addAction(textItalicAct);
    addAction(textFontAct);
    addSeparator();
    addAction(alignLeftAct);
    addAction(alignCenterAct);
    addAction(alignRightAct);
    addAction(alignJustifyAct);
    setObjectName(tr("Text"));
}

TextToolBar::~TextToolBar()
{
    textBoldAct->deleteLater();
    textUnderlineAct->deleteLater();
    textItalicAct->deleteLater();
    textFontAct->deleteLater();
    alignLeftAct->deleteLater();
    alignCenterAct->deleteLater();
    alignRightAct->deleteLater();
    alignJustifyAct->deleteLater();

    textMenu->deleteLater();
}

QMenu *TextToolBar::GenerateMenu()
{
    textMenu = new QMenu(tr("&Text"));
    textMenu->addAction(textBoldAct);
    textMenu->addAction(textUnderlineAct);
    textMenu->addAction(textItalicAct);
    textMenu->addAction(textFontAct);
    textMenu->addSeparator();
    textMenu->addAction(alignLeftAct);
    textMenu->addAction(alignCenterAct);
    textMenu->addAction(alignRightAct);
    textMenu->addAction(alignJustifyAct);
    return textMenu;
}

HelpToolBar::HelpToolBar(QWidget *parent) :
    BaseToolBar(parent)
{
    startPageAct = new QAction(QIcon(":/Menu/rc/Menu/app.png"), tr("StartPage"), this);                //>@UI设计工具
    startPageAct->setStatusTip(tr("StartPage."));
    connect(startPageAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenStartPage()));

    contextAct = new QAction(QIcon(":/Menu/rc/Menu/help.png"), tr("Context"), this);
    contextAct->setStatusTip(tr("Context"));
    connect(contextAct, SIGNAL(triggered()), mMainWindow, SLOT(context()));

    indexAct = new QAction(QIcon(":/Menu/rc/Menu/search.png"), tr("Index"), this);
    indexAct->setStatusTip(tr("Index"));
    connect(indexAct, SIGNAL(triggered()), mMainWindow, SLOT(index()));

    technicalSupportAct = new QAction(tr("Technical Support"), this);
    technicalSupportAct->setStatusTip(tr("Technical Support"));
    connect(technicalSupportAct, SIGNAL(triggered()), mMainWindow, SLOT(technicalSupport()));

    reportBugAct = new QAction(tr("Report Bug"), this);
    reportBugAct->setStatusTip(tr("Report Bug"));
    connect(reportBugAct, SIGNAL(triggered()), mMainWindow, SLOT(reportBug()));

    checkUpdateAct = new QAction(tr("Check Update"), this);
    checkUpdateAct->setStatusTip(tr("Check Update"));
    connect(checkUpdateAct, SIGNAL(triggered()), mMainWindow, SLOT(checkUpdate()));

    aboutIDEAct = new QAction(tr("AboutIDE"), this);
    aboutIDEAct->setShortcuts(QKeySequence::WhatsThis);
    aboutIDEAct->setStatusTip(tr("Show the Infomation of IDE"));
    connect(aboutIDEAct, SIGNAL(triggered()), mMainWindow, SLOT(aboutIDE()));

    aboutBSTAct = new QAction(tr("AboutBST"), mMainWindow);
    aboutBSTAct->setShortcuts(QKeySequence::WhatsThis);
    aboutBSTAct->setStatusTip(tr("Show the Infomation of BST"));
    connect(aboutBSTAct, SIGNAL(triggered()), mMainWindow, SLOT(aboutBST()));

    addAction(startPageAct);
    addAction(contextAct);
    addAction(indexAct);
}

HelpToolBar::~HelpToolBar()
{
    startPageAct->deleteLater();
    contextAct->deleteLater();
    indexAct->deleteLater();
    technicalSupportAct->deleteLater();
    reportBugAct->deleteLater();
    checkUpdateAct->deleteLater();
    aboutIDEAct->deleteLater();
    aboutBSTAct->deleteLater();

    helpMenu->deleteLater();
}

QMenu *HelpToolBar::GenerateMenu()
{
    helpMenu = new QMenu(tr("&Help"),mMainWindow);
    helpMenu->addAction(startPageAct);
    helpMenu->addSeparator();
    helpMenu->addAction(contextAct);
    helpMenu->addAction(indexAct);
    helpMenu->addAction(technicalSupportAct);
    helpMenu->addAction(reportBugAct);
    helpMenu->addAction(checkUpdateAct);
    helpMenu->addAction(aboutIDEAct);
    helpMenu->addAction(aboutBSTAct);
    return helpMenu;
}

ExtendToolBar::ExtendToolBar(QWidget *parent) :
    BaseToolBar(parent)
{
    imageEditorAct = new QAction(QIcon(":/Page/rc/Page/jpg.png"), tr("Image Editor Tool"), this);
    imageEditorAct->setStatusTip(tr("Image Editor"));
    connect(imageEditorAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenImageEditor()));

    videoEditorAct = new QAction(QIcon(":/Page/rc/Page/mp4.png"), tr("Video Editor Tool"), this);
    videoEditorAct->setStatusTip(tr("Video Editor"));
    connect(videoEditorAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenVideoEditor()));

    audioEditorAct = new QAction(QIcon(":/Page/rc/Page/mp3.png"), tr("Audio Editor Tool"), this);
    audioEditorAct->setStatusTip(tr("Audio Editor"));
    connect(audioEditorAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenAudioEditor()));

    fontEditorAct = new QAction(QIcon(":/Page/rc/Page/txt.png"), tr("Text Editor Tool"), this);
    fontEditorAct->setStatusTip(tr("Text Editor"));
    connect(fontEditorAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenFontEditor()));

    manufactureToolAct = new QAction(QIcon(":/Page/rc/Page/manufacture.png"), tr("Manufacture Tool"), this);
    manufactureToolAct->setStatusTip(tr("Manufacture"));
    connect(manufactureToolAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenManufactureTool()));

    upgradeToolAct = new QAction(QIcon(":/Menu/rc/Menu/undo.png"), tr("Upgrade Tool"), this);
    upgradeToolAct->setStatusTip(tr("Upgrade"));
    connect(upgradeToolAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenUpgradeTool()));

    networkManagerToolAct = new QAction(QIcon(":/Network/rc/Network/wlan.svg"), tr("NetworkManager Tool"), this);
    networkManagerToolAct->setStatusTip(tr("NetworkManager"));
    connect(networkManagerToolAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenNetworkManagerTool()));

    asciiCodecToolAct = new QAction(QIcon(":/Menu/rc/Menu/font.png"), tr("Ascii Codec Tool"), this);
    asciiCodecToolAct->setStatusTip(tr("Text Codec Tool"));
    connect(asciiCodecToolAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenAsciiCodecTool()));

    faceDetectToolAct = new QAction(QIcon(":/Menu/rc/Menu/face.png"), tr("Face Detect"), this);                  //>@资源管理工具
    faceDetectToolAct->setStatusTip(tr("Detect faces."));
    connect(faceDetectToolAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenFaceDetectTool()));

    rcManagerAct = new QAction(QIcon(":/Page/rc/Page/rc.png"), tr("RcManager"), this);                  //>@资源管理工具
    rcManagerAct->setStatusTip(tr("Edit device's resource."));
    connect(rcManagerAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenRcManagerTool()));

    ftpToolAct = new QAction(QIcon(":/Menu/rc/Menu/ftp.png"), tr("FTP Tool"), this);
    ftpToolAct->setStatusTip(tr("FTP"));
    connect(ftpToolAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenFtpTool()));

    smsToolAct = new QAction(QIcon(":/Menu/rc/Menu/media.png"), tr("StreamMedia Server"), this);
    smsToolAct->setStatusTip(tr("StreamMedia Server"));
    connect(smsToolAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenSmsForm()));

    sqlserverToolAct = new QAction(QIcon(":/Menu/rc/Menu/database.png"), tr("SQL Server"), this);
    sqlserverToolAct->setStatusTip(tr("SQL Server"));
    connect(sqlserverToolAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenSQLServerForm()));

    monitorToolAct = new QAction(QIcon(":/Menu/rc/Menu/monitor.png"), tr("Monitor Tool"), this);
    monitorToolAct->setStatusTip(tr("Monitor Tool"));
    connect(monitorToolAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenMonitorForm()));

    chatFormAct = new QAction(QIcon(":/Menu/rc/Menu/chat.png"), tr("Chat Tool"), this);
    chatFormAct->setStatusTip(tr("Chat Message"));
    connect(chatFormAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenChatForm()));

    mapsFormAct = new QAction(QIcon(":/Menu/rc/Menu/map.png"), tr("Maps Tool"), this);
    mapsFormAct->setStatusTip(tr("Maps"));
    connect(mapsFormAct, SIGNAL(triggered()), mMainWindow, SLOT(OpenMapsForm()));

    addNewToolAct = new QAction(QIcon(":/Menu/rc/Menu/plus.png"), tr("Add New Tool"), this);
    addNewToolAct->setStatusTip(tr("Add a new tool in ToolMenu"));
    connect(addNewToolAct, SIGNAL(triggered()), mMainWindow, SLOT(addNewTool()));

    addAction(imageEditorAct);
    addAction(videoEditorAct);
    addAction(audioEditorAct);
    addAction(fontEditorAct);
    addAction(manufactureToolAct);
    addAction(upgradeToolAct);
    addAction(networkManagerToolAct);
    addAction(asciiCodecToolAct);
    addAction(faceDetectToolAct);
    addAction(rcManagerAct);
    addAction(ftpToolAct);
    addAction(smsToolAct);
    addAction(sqlserverToolAct);
    addAction(monitorToolAct);
    addAction(chatFormAct);
    addAction(mapsFormAct);
    addAction(addNewToolAct);

    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIconSize (QSize(32, 32) );
    setMovable(false);
    setFloatable(false);
}

ExtendToolBar::~ExtendToolBar()
{
    imageEditorAct->deleteLater();
    videoEditorAct->deleteLater();
    audioEditorAct->deleteLater();
    fontEditorAct->deleteLater();
    faceDetectToolAct->deleteLater();
    rcManagerAct->deleteLater();
    ftpToolAct->deleteLater();
    manufactureToolAct->deleteLater();
    upgradeToolAct->deleteLater();
    networkManagerToolAct->deleteLater();
    addNewToolAct->deleteLater();
    chatFormAct->deleteLater();
    mapsFormAct->deleteLater();
    asciiCodecToolAct->deleteLater();

    extendMenu->deleteLater();
}

QMenu *ExtendToolBar::GenerateMenu()
{
    extendMenu = new QMenu(tr("&Extend"),mMainWindow);
    extendMenu->addAction(imageEditorAct);
    extendMenu->addAction(videoEditorAct);
    extendMenu->addAction(audioEditorAct);
    extendMenu->addAction(fontEditorAct);
    extendMenu->addAction(manufactureToolAct);
    extendMenu->addAction(upgradeToolAct);
    extendMenu->addAction(networkManagerToolAct);
    extendMenu->addAction(asciiCodecToolAct);
    extendMenu->addAction(faceDetectToolAct);
    extendMenu->addAction(rcManagerAct);
    extendMenu->addAction(ftpToolAct);
    extendMenu->addAction(smsToolAct);
    extendMenu->addAction(sqlserverToolAct);
    extendMenu->addAction(monitorToolAct);
    extendMenu->addAction(chatFormAct);
    extendMenu->addAction(mapsFormAct);
    extendMenu->addAction(addNewToolAct);
    return extendMenu;
}
