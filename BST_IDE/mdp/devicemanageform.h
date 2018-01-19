#ifndef DEVICEMANAGEFORM_H
#define DEVICEMANAGEFORM_H

#include "tabpage.h"
#include "devicescene.h"
#include "treeitemclass.h"

namespace Ui {
class DeviceManageForm;
}

class DeviceManageForm : public TabPage
{
    Q_OBJECT
public:
    explicit DeviceManageForm(QString pMdpPath, QWidget *parent = 0);
    ~DeviceManageForm();
    
    void InitScene(QString pMdpPath);
    void UpdateZoomSlider(int value);

    void SyncTreeItem(FileTreeItem *pTreeItem)
    {
        m_TreeItem = pTreeItem;
    }

private slots:
    void on_ZoomInBtn_clicked();
    void on_ZoomSlider_valueChanged(int value);
    void on_ZoomOutBtn_clicked();
    void on_BtnUndo_clicked();
    void on_BtnRedo_clicked();
    void on_BtnRefresh_clicked();
    void on_BtnSave_clicked();
    void on_CollisionBtn_clicked(bool checked);

public:
    FileTreeItem            *m_TreeItem;    //>@在项目管理窗口中的TreeItem
    DeviceScene*             mDeviceScene;
    Ui::DeviceManageForm    *ui;
};

#endif // DEVICEMANAGEFORM_H
