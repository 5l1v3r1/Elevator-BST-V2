#ifndef RPOJECTMANAGER_H
#define RPOJECTMANAGER_H

#include "global.h"
#include "upitem.h"
#include "treeitemclass.h"
#include "newfileform.h"
#include "modalwindow.h"
#include "toolbar.h"

//>@与DeviceToolBar绑定
class DeviceToolBar;

class ProjectManager : public UpQTreeWidget
{
    Q_OBJECT
public:
    ProjectManager(QWidget *parent = 0);
    ~ProjectManager();

    void CreateMenu();
    void CreateDeviceTool();

    void InitScan();
    void ReleaseScan();

    DeviceTreeItem* GetDevice(BaseDevice *pDev);
    DeviceTreeItem* GetDeviceFromAddr(QString pDevAddr);
    DeviceTreeItem* GetDeviceFromBuffer(QString pDevBuffer);

    DeviceTreeItem* AddDevice(BaseDevice::DEVICETYPE pType, QString pDevName,
                              QString pBufferPath=QString(), DEVICESTATE pState=S_OFFLINE);
    DeviceTreeItem* AddDevice(BaseDevice* pDev);
    bool DelDevice(DeviceTreeItem *&pTreeItem, bool pDelFile=false);

    void mousePressEvent(QMouseEvent *event);

public slots:
    void slot_ItemDoubleClicked(QTreeWidgetItem *item, int column);
    void slot_ItemPressed(QTreeWidgetItem *pItem, int column);

    void slot_DeviceAdded(QString dev);
    void slot_DeviceRemoved(QString dev);
    void slot_NetScan();   //>@扫描网络设备

    void slot_Options();   //>@整体属性

    void slot_NewDevice();  //>@添加一个本地的设备
    void slot_AddDevice();
    void slot_ClearDevice();
    void slot_RefreshDevice();
    void slot_SyncDevice();       //>@进入同步模式，此时，对此设备中的活动工程做任何操作都会立即同步到设备中
    void slot_DeleteDevice();
    void slot_ExploreDevice();    //>@
    void slot_DeviceOptions();    //>@

    void slot_SetRunningProject();
    void slot_NewProject();
    void slot_AddProject();
    void slot_OpenProject();
    void slot_CloseProject();
    void slot_DeleteProject();
    void slot_RegularProject();
    void slot_DownloadProject();
    void slot_UploadProject();    //>@上传某个主题
    void slot_ExploreProject();
    void slot_ProjectOptions();

    void slot_NewFile();
    void slot_AddFile();
    void slot_OpenFile();
    void slot_CloseFile();
    void slot_DeletFile();
    void slot_ExploreFile();

    void slot_StartSync();
    void slot_StopSync();
    void slot_PauseSync();

    void slot_OpenUiDesignerTool();
    void slot_OpenMdpSetTool();
    void slot_OpenScheduleMapTool();
    void slot_OpenSoftwareTestTool();
    void slot_OpenLogQueryTool();

public:
    QMenu*                 m_BlankMenu;
    QMenu*                 m_DeviceMenu;
    QMenu*                 m_ProjectMenu;
    QMenu*                 m_FileMenu;

    DeviceToolBar         *m_DevToolBar;

    QTreeWidgetItem*       m_MenuOnItem;

    //>@Arp扫描
    ToolUdp*               mToolUdp;
    HostNetDiscovery      *mHostNetDiscovery;
    QTimer                 mScanTimer;

    DeviceWatcher         *mStorageWatcher;
};




#endif // RPOJECTMANAGER_H
