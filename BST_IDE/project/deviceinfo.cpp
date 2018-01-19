#include "deviceinfo.h"
#include "toolbar.h"
#include "treeitemclass.h"
#include "ui_deviceinfo.h"

DeviceInfo::DeviceInfo(DeviceToolBar *pDevToolbar, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceInfo)
{
    ui->setupUi(this);
    ui->label->SetEvnRcver(this);
    mZoom = false;
    mDevToolbar = pDevToolbar;
}

DeviceInfo::~DeviceInfo()
{
    StopGrab();
    delete ui;
}

bool DeviceInfo::SetBufferPath(QString pPath)
{
    if(pPath.isEmpty())
        return false;
    if(pPath.endsWith('/') == false)
        pPath.append('/');
    QDir tmpDir(pPath);
    tmpDir.mkpath(pPath);
    ui->SavePathEdit->setText(pPath);
    mBufferPath = pPath;
    return true;
}

void DeviceInfo::StartGrab(QString pPath)
{
    if(SetBufferPath(pPath) == false)
        return;
    on_AutoGrabScreenIntervel_valueChanged(ui->AutoGrabScreenIntervel->value());
}

void DeviceInfo::StopGrab()
{
    mPrintTimer.stop();
    disconnect(&mPrintTimer,SIGNAL(timeout()),0,0);
    QImage tmpImage(":/Page/rc/Page/VideoScreen.png");
    if(mZoom)
        ui->label->setPixmap(QPixmap::fromImage(tmpImage.scaled(ui->label->size())));
    else
        ui->label->setPixmap(QPixmap::fromImage(tmpImage.scaled(mRect.width(), mRect.height())));
}

void DeviceInfo::mouseDoubleClickEvent(QMouseEvent *)
{
    if(!mZoom)
    {
        mZoom = true;
        mRect = ui->label->geometry();
        ui->label->setParent(0);
        ui->label->setGeometry(0,
                               0,
                               QApplication::desktop()->size().width()-2*0,
                               QApplication::desktop()->size().height()-2*0);
        ui->label->show();
    }
    else
    {
        mZoom = false;
        ui->label->setParent(ui->groupBoxView);
        ui->label->setGeometry(mRect);
        ui->verticalLayout->insertWidget(0, ui->label);
        ui->label->setScaledContents(true);
    }
}

void DeviceInfo::slot_Update()
{
    if(mDevToolbar && mDevToolbar->isConnected())
    {
        if(!isEnabled())
            setEnabled(true);
    }
    else
    {
        if(isEnabled())
            setDisabled(true);
    }
}

void DeviceInfo::slot_Print()
{
    if(!isEnabled())
        return;
    if(!ui->DevScreenBtn->isChecked())
        return;
    if(!isActiveWindow())
    {
        IDE_TRACE();
        return;
    }
    if(!(mDevToolbar && mDevToolbar->isConnected()))
    {
        IDE_TRACE();
        return;
    }
    NetDevice *tmpNetDev = VAR_CAST<NetDevice*>(mDevToolbar->mDevTreeItem->mInterfaceDevice);
    if(!tmpNetDev)  //>@之前已经判断过是否建立连接了，此处不需要再次判断了
    {
        IDE_TRACE();
        return;
    }
    int life = ui->AutoGrabScreenIntervel->value();
    QString tmpPath = mBufferPath + D_GrabScreenName;
    if(tmpNetDev->mNetManager->GetDesktop(mGrabScreenSrc, tmpPath, life))
    {
        QImage tmpImage(tmpPath);
        if(tmpImage.isNull())
        {
            IDE_TRACE();
            return;
        }
        if(ui->DevInfoBtn->isChecked())
        {
            QPainter p(&tmpImage);
            p.setRenderHint(QPainter::Antialiasing, true);
            QFont font("Arial", 11);
            p.setFont(font);
            p.setPen(QPen(Qt::red, 20, Qt::SolidLine, Qt::SquareCap));
            qreal left = 10;
            qreal top =  20;
            qreal width = ui->label->size().width()-20;
            QFontMetrics tmpMetrics(font);
            qreal height = tmpMetrics.height()+5;
            QStringList tmpList = tmpNetDev->mNetManager->m_DevInfoList.keys();
            for(int i=0;i<tmpList.count();i++)
            {
                QString tmpTag = tmpList.at(i);
                QString tmpContent = tmpNetDev->mNetManager->m_DevInfoList.value(tmpTag);
                p.drawText(QRectF(left, top + height*i, width, height),
                           Qt::AlignJustify,
                           QString("%1:%2").arg(tmpTag).arg(tmpContent));
            }
            p.end();
        }
        if(mZoom)
            ui->label->setPixmap(QPixmap::fromImage(tmpImage.scaled(ui->label->size())));
        else
            ui->label->setPixmap(QPixmap::fromImage(tmpImage.scaled(mRect.width(), mRect.height())));
    }
}

void DeviceInfo::on_AutoGrabScreenIntervel_valueChanged(int arg1)
{
    if(arg1 < 0)
        return;
    mPrintTimer.stop();
    mPrintTimer.start(arg1);
    connect(&mPrintTimer,SIGNAL(timeout()),this,SLOT(slot_Print()));
}

void DeviceInfo::on_MannualGrabScreenBtn_clicked(bool checked)
{
    if(checked)
        StartGrab(ui->SavePathEdit->text());
    else
        StopGrab();
}

void DeviceInfo::on_SelectLocationBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Directory"),
                                                    "/",
                                                    QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;
    ui->SavePathEdit->setText(dir);
}

void DeviceInfo::on_OpenLocationBtn_clicked()
{
    QString tmpPath = QString("file:///%1").arg(ui->SavePathEdit->text());
    QDesktopServices::openUrl(QUrl(tmpPath, QUrl::TolerantMode));
}
