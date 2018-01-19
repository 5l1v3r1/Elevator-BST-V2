#ifndef MODALWINDOW_H
#define MODALWINDOW_H

#include <QWizard>
#include "global.h"
#include "deviceclass.h"

namespace Ui {
class ProWizard;
}

class ProWizard : public QWizard
{
    Q_OBJECT
public:
    ProWizard(QWidget *parent=0);                           //>@用于新建一个工程
    ProWizard(QString pFilePath, QWidget *parent=0);        //>@用于添加一个已经存在的工程
    ProWizard(bstproManager *&pManager, QWidget *parent=0);  //>@用于打开一个已经打开的工程
    ~ProWizard();

    void InitUI();
    void InitWizard();
    void SyncWizard();  //>@将UI的的修改操作同步到对应的文件以及文件夹的操作中

    void accept();
    void rejected();

    //>@将目录选择的记忆地址锁定
    void SetMemoryDir(QString pPath, bool pLock=false);

    bstproManager* GetManager() {return m_ProManager;}

private slots:
    void on_Browser_clicked();
    void slot_ProjPathChanged(QString pProPath);

public:
    Ui::ProWizard       *ui;
    bool                 m_MemoryDirLock;
    QString              m_MemoryDir;
    bstproManager       *m_ProManager;
    bool                 m_CloseFlag;       //>@用于指示当点击取消按钮时，已经创建的bstproManager是否删除。
};

namespace Ui {
class DevWizard;
}

class DevWizard : public QWizard
{
    Q_OBJECT
public:
    DevWizard(QWidget *parent=0);                           //>@用于新建一个设备
    DevWizard(QString pBufferPath, QWidget *parent=0);      //>@用于添加一个已经存在的设备
    DevWizard(BaseDevice *&pDev, QWidget *parent=0);
    ~DevWizard();

    void InitUI();
    void SyncUI();  //>@将界面修改同步到BaseDevice中。

    void accept();
    void rejected();

    BaseDevice* GetDevice() {return m_Dev;}

private slots:
    void on_Browser_clicked();
    void slot_DevBufferPathChanged(QString pBufferPath) {}

public:
    Ui::DevWizard       *ui;
    QString              m_MemoryDir;
    BaseDevice          *m_Dev;
    bool                 m_CloseFlag;
};

#endif // MODALWINDOW_H
