#ifndef DEVICEMANAGEFORM_H
#define DEVICEMANAGEFORM_H

#include "devicescene.h"
#include <QWidget>
#include <QImage>

namespace Ui {
class DeviceManageForm;
}

class DeviceManageForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit DeviceManageForm(QString pMdpPath = QString(), QWidget *parent = 0);
    ~DeviceManageForm();
    
    void InitView(QString pMdpPath);
    void InitConnect();
    void ReleaseConnect();

    DeviceScene*        mDeviceScene;
private:
    Ui::DeviceManageForm *ui;
signals:
    void sDelNode(QString);
    void sAddNode(QString);
private slots:
    void slot_PHNodeListDragEnter(QDragEnterEvent* event);
    void slot_PHNodeListDrop(QDropEvent* event);
    void slot_PHNodeListShow(QStringList pNameList);
    void slot_PHNodeListStartDrag(Qt::DropActions);
    void slot_DeviceViewDrop(QDropEvent* event);
};

#endif // DEVICEMANAGEFORM_H
