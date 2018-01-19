#include "devicemanageform.h"
#include "ui_devicemanageform.h"

DeviceManageForm::DeviceManageForm(QString pMdpPath, QWidget *parent) :
    TabPage(parent),
    ui(new Ui::DeviceManageForm),
    mDeviceScene(0)
{
    ui->setupUi(this);
    ui->DeviceOperateView->setBackgroundBrush(QBrush(QColor(15,15,18,255)));
    ui->DeviceOperateView->setAutoFillBackground(true);
    ui->DeviceOperateView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    m_TreeItem = 0;
    InitScene(pMdpPath);
}

DeviceManageForm::~DeviceManageForm()
{
    if(m_TreeItem)
    {
        m_TreeItem->mFileInfo->PageDialogPtr = 0;
    }
    if(mDeviceScene)
    {
        mDeviceScene->deleteLater();
        mDeviceScene = 0;
    }
    delete ui;

    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void DeviceManageForm::InitScene(QString pMdpPath)
{
    mDeviceScene = new DeviceScene(pMdpPath,this);
    ui->DeviceOperateView->setScene(mDeviceScene);

//    double newScale;
//    if(ui->DeviceOperateView->height() > ui->DeviceOperateView->width())
//        newScale = (double)(ui->DeviceOperateView->width()+1) / D_SCENESIZE;
//    else
//        newScale = (double)(ui->DeviceOperateView->height()+1) / D_SCENESIZE;
//    QMatrix oldMatrix = ui->DeviceOperateView->matrix();
//    ui->DeviceOperateView->resetMatrix();
//    ui->DeviceOperateView->translate(oldMatrix.dx(), oldMatrix.dy());
//    ui->DeviceOperateView->scale(newScale, newScale);
//    UpdateZoomSlider(int value);
}


void DeviceManageForm::on_ZoomInBtn_clicked()
{
    int pos = ui->ZoomSlider->value();
    if(pos<100)
        ui->ZoomSlider->setValue(pos+1);
    else
        ui->ZoomSlider->setValue(99);
}

void DeviceManageForm::on_ZoomSlider_valueChanged(int value)
{
    if(!ui->DeviceOperateView || value < 10 || value >= 100)
        return;
    double newScale = (double)(value+1) * 2.0 / 100.0;
    QMatrix oldMatrix = ui->DeviceOperateView->matrix();
    ui->DeviceOperateView->resetMatrix();
    ui->DeviceOperateView->translate(oldMatrix.dx(), oldMatrix.dy());
    ui->DeviceOperateView->scale(newScale, newScale);
}

void DeviceManageForm::UpdateZoomSlider(int value)
{
    ui->ZoomSlider->setValue(value);
}

void DeviceManageForm::on_ZoomOutBtn_clicked()
{
    int pos = ui->ZoomSlider->value();
    if(pos>=10)
        ui->ZoomSlider->setValue(pos-1);
    else
        ui->ZoomSlider->setValue(10);
}

void DeviceManageForm::on_BtnUndo_clicked()
{

}

void DeviceManageForm::on_BtnRedo_clicked()
{

}

void DeviceManageForm::on_BtnRefresh_clicked()
{

}

void DeviceManageForm::on_BtnSave_clicked()
{
    mDeviceScene->SaveBootMdp();
}

void DeviceManageForm::on_CollisionBtn_clicked(bool checked)
{
    if(checked)
    {
        //ui->DeviceOperateView->scene->m_timer->start();
    }
    else
    {
        //ui->DeviceOperateView->scene->m_timer->stop();
    }
}
