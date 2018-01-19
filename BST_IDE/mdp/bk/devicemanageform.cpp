#include "devicemanageform.h"
#include "ui_devicemanageform.h"

DeviceManageForm::DeviceManageForm(QString pMdpPath, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceManageForm),
    mDeviceScene(0)
{
    ui->setupUi(this);
    InitView(pMdpPath);
    InitConnect();
}

DeviceManageForm::~DeviceManageForm()
{
    ReleaseConnect();
    if(mDeviceScene)
    {
        mDeviceScene->deleteLater();
        mDeviceScene = 0;
    }
    delete ui;
}

void DeviceManageForm::InitView(QString pMdpPath)
{
    ui->DeviceView->resize(800,800);
    ui->DeviceView->setBackgroundBrush(QBrush(QColor(15,15,18,255)));
    ui->DeviceView->setAutoFillBackground(true);
    ui->DeviceView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    QRect viewRect = ui->DeviceView->rect();
    int adjustW = 20*viewRect.width()/(viewRect.width()+viewRect.height());
    int adjustH = 20*viewRect.height()/(viewRect.width()+viewRect.height());
    mDeviceScene = new DeviceScene(pMdpPath,
                                   QRectF(-viewRect.width()/2+adjustW,
                                          -viewRect.height()/2+adjustH,
                                          viewRect.width()-2*adjustW,
                                          viewRect.height()-2*adjustH),
                                   this);
    ui->DeviceView->setScene(mDeviceScene);
    QStringList keyList = GlobalPara.m_ConfigInfoGroup.keys();
    for(int i=0;i<keyList.count();i++)
    {
        QString key = keyList.at(i);
        MDPConfigInfo *configInfo = GlobalPara.m_ConfigInfoGroup.value(key);
        if((!key.compare("BottomImg", Qt::CaseInsensitive))||
           (!key.compare("BoardImg", Qt::CaseInsensitive))||
           (!key.compare("Cpu", Qt::CaseInsensitive))||
           (!key.compare("Mem", Qt::CaseInsensitive))||
           (!key.compare("Net", Qt::CaseInsensitive)))
            continue;
        QListWidgetItem* item = new QListWidgetItem;
        item->setText(key);
        item->setIcon(QIcon(configInfo->mRc));
        ui->PHNodeList->addItem(item);
    }
    for(int i=0;i<ui->PHNodeList->count();i++)
    {
        QListWidgetItem* item = ui->PHNodeList->item(i);
        for(int j=0;j<mDeviceScene->mDeviceParaForm->mNodeList.count();j++)
        {
            QString name = mDeviceScene->mDeviceParaForm->mNodeList.at(j);
            if(!item->text().compare(name))
                item->setHidden(true);
        }
    }
}

void DeviceManageForm::InitConnect()
{
    connect(ui->PHNodeList, SIGNAL(sDragEnter(QDragEnterEvent*)), this, SLOT(slot_PHNodeListDragEnter(QDragEnterEvent*)));
    connect(ui->PHNodeList, SIGNAL(sDrop(QDropEvent*)), this, SLOT(slot_PHNodeListDrop(QDropEvent*)));
    connect(ui->PHNodeList, SIGNAL(sStartDrag(Qt::DropActions)), this, SLOT(slot_PHNodeListStartDrag(Qt::DropActions)));
    connect(ui->DeviceView, SIGNAL(sDropEvent(QDropEvent*)), this, SLOT(slot_DeviceViewDrop(QDropEvent*)));
    connect(mDeviceScene, SIGNAL(sDelNodes(QStringList)), this, SLOT(slot_PHNodeListShow(QStringList)));
    connect(this, SIGNAL(sDelNode(QString)), mDeviceScene->mDeviceParaForm, SLOT(slot_DelNode(QString)));
    connect(this, SIGNAL(sAddNode(QString)), mDeviceScene->mDeviceParaForm, SLOT(slot_AddNode(QString)));
}

void DeviceManageForm::ReleaseConnect()
{
    disconnect(ui->PHNodeList, SIGNAL(sDragEnter(QDragEnterEvent*)), 0, 0);
    disconnect(ui->PHNodeList, SIGNAL(sDrop(QDropEvent*)), 0, 0);
    disconnect(ui->PHNodeList, SIGNAL(sStartDrag(Qt::DropActions)), 0, 0);
    disconnect(ui->DeviceView, SIGNAL(sDropEvent(QDropEvent*)), 0, 0);
    disconnect(mDeviceScene, SIGNAL(sDelNodes(QStringList)), 0, 0);
    disconnect(this, SIGNAL(sDelNode(QString)), 0, 0);
    disconnect(this, SIGNAL(sAddNode(QString)), 0, 0);
}

void DeviceManageForm::slot_PHNodeListDragEnter(QDragEnterEvent* event)
{
//    IDE_DEBUG("PHNodeListDragEnter");
    event->accept();
}

void DeviceManageForm::slot_PHNodeListDrop(QDropEvent* event)
{
    IDE_DEBUG("PHNodeListDragDrop");
}

void DeviceManageForm::slot_PHNodeListShow(QStringList pNameList)
{
    IDE_DEBUG("PHNodeListShow");
    for(int i=0;i<ui->PHNodeList->count();i++)
    {
        QListWidgetItem* item = ui->PHNodeList->item(i);
        for(int j=0;j<pNameList.count();j++)
        {
            QString name = pNameList.at(j);
            if(!item->text().compare(name))
            {
                item->setHidden(false);
                emit sDelNode(name);
            }
        }
    }
}

void DeviceManageForm::slot_PHNodeListStartDrag(Qt::DropActions)
{
//    IDE_DEBUG("PHNodeListDragStartDrag");
    QListWidgetItem* item = ui->PHNodeList->currentItem();

    QPixmap pixmap = item->icon().pixmap(100,120);
    QString eventInfo = item->text();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream<<eventInfo;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("PHNode", itemData);

    QDrag *drag = new QDrag(ui->PHNodeList);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    drag->setMimeData(mimeData);
    drag->exec(Qt::MoveAction);
}
//>@*********************************************
void DeviceManageForm::slot_DeviceViewDrop(QDropEvent* event)
{
    ui->DeviceView->DownDropEvent(event);
    QListWidgetItem* item = ui->PHNodeList->currentItem();
    item->setHidden(true);
    emit sAddNode(item->text());
}











