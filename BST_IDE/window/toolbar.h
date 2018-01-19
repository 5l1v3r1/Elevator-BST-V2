#ifndef UI_CLASS_H
#define UI_CLASS_H

#include "global.h"
#include <QToolBar>
#include <QLabel>
#include <QUdpSocket>

class MainWindow;
class TabPageManager;
class ProjectManager;
class DeviceTreeItem;

class DeviceInfo;
class SimulatorForm;
class ToolBox;
class DeviceQueryForm;
class UiPropertyWidget;
class CFindFileForm;

class PropertyTree;

class BaseToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit BaseToolBar(QWidget *parent=0);
    ~BaseToolBar();

public:
    MainWindow          *mMainWindow;
};

class DeviceToolBar : public BaseToolBar
{
    Q_OBJECT
public:
    enum UPGRADESTATE { START, STOP, PAUSE };
    explicit DeviceToolBar(QWidget *parent=0);
    ~DeviceToolBar();

    void CreateLeftFrame();
    void InitAction();
    QMenu *GenerateMenu();
    bool isConnected();

signals:
    void sUpdate();

public slots:
    void slot_Connstate(DeviceTreeItem *pDevItem);
    void slot_Connected();
    void slot_Disconnected();

public:
    UPGRADESTATE        m_UpgradeState;
    QAction             *startAct,          //>@设备仿真开始
                        *pauseAct,
                        *stopAct,
                        *devInfoAct;

    QAction             *uiDesignerAct,     //>@UI设计工具
                        *scheduleMapAct,    //>@map编辑器
                        *mdpSetAct,         //>@MDP设置工具
                        *softwareTestAct,   //>@软件测试工具
                        *logQueryAct;       //>@日志查询工具

    QDockWidget         *mDevInfoDock;
    DeviceInfo*          mDevInfoForm;

    QMenu               *deviceToolMenu;

    DeviceTreeItem      *mDevTreeItem;
    ProjectManager      *mProjectManager;
};

class ProjectBar : public BaseToolBar
{
    Q_OBJECT
public:
    explicit ProjectBar(QWidget *parent=0);
    ~ProjectBar();

    void CreateFrame();
    void InitAction();
    QMenu *GenerateMenu();

public slots:
    void slot_OpenBar(bool flag);
    void slot_UpdateBar();

public:
    QAction             *mOpenDockAct,
                        *mTagViewAct,
                        *mNetViewAct,
                        *mPropertyViewAct,
                        *mToolViewAct,
                        *mSimulatorViewAct;

    QDockWidget         *mProjectViewDock,
                        *mTagViewDock,
                        *mNetViewDock,
                        *mPropertyViewDock,
                        *mToolViewDock,
                        *mSimulatorViewDock;

    ProjectManager      *mProjectManager;
    SimulatorForm       *mSimulatorView;
    PropertyTree        *mPropertyWidget;
    ToolBox             *mToolTreeView;            //>@工具箱

    QMenu               *mProjectMenu;
};

class SQLToolBar : public BaseToolBar
{
    Q_OBJECT
public:
    explicit SQLToolBar(QWidget *parent=0);
    ~SQLToolBar();

    void CreateFrame();
    void InitAction();
    QMenu *GenerateMenu();

public slots:
    void slot_Connstate(bool pState, QString pIP);
    void slot_Connected();
    void slot_Disconnected();

public:
    QAction             *openDockAct;

    QDockWidget         *mDevQueryViewDock;
    DeviceQueryForm*     mDevQueryFrom;

    QMenu               *sqlToolMenu;
};

typedef enum{
    ModeNetworkManager=0,               //>@网络管理员
    ModeManufacturingTechnician,        //>@生产技术员
    ModeSoftwareTester,                 //>@软件测试员
    ModeAftersalesMaintenanceMember,    //>@售后维修员
    ModeCustomer,                       //>@客户
    ModeDefault
}USERMODE;

class DockViewToolBar : public BaseToolBar
{
    Q_OBJECT
public:
    explicit DockViewToolBar(QWidget *parent=0);
    ~DockViewToolBar();

    //创建边栏操作要在创建Action操作之前，否则View菜单项中的菜单项无法使用。
    void CreateFrame();
    void InitAction();
    QMenu *GenerateMenu();
    QComboBox *GenerateUserModeCombox();

public slots:
    void InitDockView(USERMODE pMode);
    void UpdateDockView(int pMode);

public:
    QAction         *helpViewAct,
                    *outputViewAct,
                    *findViewAct;
    QDockWidget     *helpViewDock,
                    *outputViewDock,
                    *findViewDock;
    CFindFileForm   *mFindForm;
    QMenu           *viewMenu;
    QComboBox       *userModeComBox;
};

class FileToolBar : public BaseToolBar
{
    Q_OBJECT
public:
    explicit FileToolBar(QWidget *parent=0);
    ~FileToolBar();
    QMenu *GenerateMenu();
public:
    QAction             *newAct,
                        *openAct,
                        *addAct,        //附加已存在文件
                        *saveAct,
                        *saveAsAct,
                        *saveSolutionAct,   //保存整个工程
                        *closeAct,          //关闭文件
                        *closeSolutionAct,  //关闭项目
                        *importAct,
                        *exportAct,
                        *printAct,
                        *quitAct;
    QMenu               *fileMenu;
    ProjectManager      *mProjectManager;
};

class EditToolBar : public BaseToolBar
{
    Q_OBJECT
public:
    explicit EditToolBar(QWidget *parent=0);
    ~EditToolBar();
    QMenu *GenerateMenu();
public:
    QAction             *undoAct,
                        *redoAct,
                        *cutAct,
                        *copyAct,
                        *pasteAct,
                        *selectAllAct,
                        *findAct,
                        *replaceAct,
                        *findPreviousAct,
                        *findNextAct,
                        *gotoAct;
    QMenu               *editMenu;
    ProjectManager      *mProjectManager;
};

class TextToolBar : public BaseToolBar
{
    Q_OBJECT
public:
    explicit TextToolBar(QWidget *parent=0);
    ~TextToolBar();
    QMenu *GenerateMenu();
public:
    QAction             *textBoldAct,
                        *textUnderlineAct,
                        *textItalicAct,
                        *textFontAct,
                        *alignLeftAct,
                        *alignCenterAct,
                        *alignRightAct,
                        *alignJustifyAct;
    QMenu               *textMenu;
    ProjectManager      *mProjectManager;
};

class HelpToolBar : public BaseToolBar
{
    Q_OBJECT
public:
    explicit HelpToolBar(QWidget *parent=0);
    ~HelpToolBar();
    QMenu *GenerateMenu();
public:
    QAction         *startPageAct,
                    *contextAct,
                    *indexAct,
                    *technicalSupportAct,
                    *reportBugAct,
                    *checkUpdateAct,
                    *aboutIDEAct,
                    *aboutBSTAct;
    QMenu           *helpMenu;
};

class ExtendToolBar : public BaseToolBar
{
    Q_OBJECT
public:
    explicit ExtendToolBar(QWidget *parent=0);
    ~ExtendToolBar();
    QMenu *GenerateMenu();
public:
    QAction *imageEditorAct;
    QAction *videoEditorAct;
    QAction *audioEditorAct;
    QAction *fontEditorAct;
    QAction *manufactureToolAct;
    QAction *upgradeToolAct;
    QAction *networkManagerToolAct;
    QAction *asciiCodecToolAct;
    QAction *faceDetectToolAct;
    QAction *rcManagerAct;      //>@资源管理工具，USB从机功能
    QAction *ftpToolAct;        //>@FTP功能
    QAction *smsToolAct;        //>@流媒体服务器
    QAction *sqlserverToolAct;  //>@SQL服务器
    QAction *monitorToolAct;
    QAction *chatFormAct;
    QAction *mapsFormAct;

    QAction *addNewToolAct;

    QMenu       *extendMenu;
};


#endif // UI_CLASS_H
