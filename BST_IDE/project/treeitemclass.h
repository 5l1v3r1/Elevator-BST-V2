#ifndef TREEITEMCLASS_H
#define TREEITEMCLASS_H

#include "global.h"
#include <QTreeWidget>
#include "deviceclass.h"

#define FILETREEITEM_TYPE       1001
#define PROJECTTREEITEM_TYPE    1002
#define DEVICETREEITEM_TYPE     1003

#define ROLE_RUNNING            1010

class FileTreeItem : public QTreeWidgetItem
{
public:
    FileTreeItem(QTreeWidgetItem *parent, QString pFilePath, int type = FILETREEITEM_TYPE);
    ~FileTreeItem();

    void ChangeFile(QString pFilePath);
    xmlParse *GetFileParse();
    void SetDelFlags(bool flag) {mDelFileFlag = flag;}

    void SetActive(bool flag);
    bool IsActive();

    bool Open();
    bool Close();

    QString GetProjectPath();
public:
    PAGE_INFO       *mFileInfo;
    bool             mDelFileFlag;  //>@是否删除源文件
};

class ProjectTreeItem : public QTreeWidgetItem
{
public:
    ProjectTreeItem(QTreeWidgetItem *parent, QString pProFile, int type = PROJECTTREEITEM_TYPE);
    ProjectTreeItem(QTreeWidgetItem *parent, bstproManager *pManager, int type = PROJECTTREEITEM_TYPE);
    ~ProjectTreeItem();

    bool isOpen();
    bool Open();
    void Close();
    void Clear();

    void SetDelFlags(bool flag) {mDelFileFlag = flag;}

    QString CopyToHere(QString pFilePath);  //>@将文件以及相关资源拷贝到当前工程目录

    bool AddFile(QString pFilePath);
    bool DelFile(FileTreeItem *&pFileItem, bool pDelFile=false);
    void RemoveFile(FileTreeItem *&pFileItem, bool pDelFile);

    FileTreeItem *GetFileItem(FILE_TYPE pType);
    TabPage *GetPagePtr(FILE_TYPE pType);

    void SetActive(bool flag);  //>@用于指示当前工程是否打开了
    bool IsActive();

    void SetRunning(bool flag);
    bool IsRunning();

public:
    QString          mProPath;
    bstproManager   *mProManager;
    bool             mDelFileFlag;  //>@是否删除源文件
};

/*
    设备文本区域可显示三种数据：本地缓冲路径、设备地址、设备别名，优先级依次递增
    指定设备后，不能再更换
    可以通过拖拽方式移动/复制某个工程到其他设备中
*/

class DeviceTreeItem : public QTreeWidgetItem
{
public:
    DeviceTreeItem(BaseDevice::DEVICETYPE pType, QString pAddr, QString pBufferPath=QString(),
                   DEVICESTATE pState = S_OFFLINE, int type = DEVICETREEITEM_TYPE);
    DeviceTreeItem(BaseDevice *pDev, int type = DEVICETREEITEM_TYPE);
    ~DeviceTreeItem();

    void SetState(DEVICESTATE pState);
    DEVICESTATE GetState();

    void SetBuffer(QString pBufferPath);
    QString GetBuffer();

    void UpdateDeviceText();//>@

    QString CopyToHere(QString pBstproFile);

    //>@查找是否有同名工程。注意，此处仅仅根据工程名进行查找
    ProjectTreeItem* GetProject(bstproManager *pManager);
    ProjectTreeItem* GetProject(QString pBstproFile);

    bool AddProject(QString pBstproFile);
    bool AddProject(bstproManager *pManager);
    bool DelProject(ProjectTreeItem *&pTreeItem, bool pDelFile=false);
    void Clear();

    void SetRunningProject(ProjectTreeItem *pProItem);
    void SetRunningProject(QString pBstproFile);
    ProjectTreeItem *GetRunningProject();

public:
    BaseDevice      *mInterfaceDevice;
    bool             mDelFileFlag;  //>@是否删除源文件
};

inline BaseDevice* CreateDevice(BaseDevice::DEVICETYPE pType, QString pAddr, QString pBufferPath=QString())
{
    BaseDevice *tmpDevice = 0;
    switch(pType)
    {
        case BaseDevice::TYPE_NET:
        {
            tmpDevice = new NetDevice(pAddr, pBufferPath);
            break;
        }
        case BaseDevice::TYPE_STORAGE:
        {
            tmpDevice = new StorageDevice(pAddr, pBufferPath);
            break;
        }
        case BaseDevice::TYPE_COM:
        {
            tmpDevice = new ComDevice(pAddr, pBufferPath);
            break;
        }
        default:
        {
            tmpDevice = new BaseDevice(pAddr, pBufferPath);
            break;
        }
    }
    return tmpDevice;
}

#endif // TREEITEMCLASS_H
