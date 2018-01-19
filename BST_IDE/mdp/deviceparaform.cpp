#include "deviceparaform.h"
#include "ui_deviceparaform.h"

#ifdef IDE

#endif

DeviceParaForm::DeviceParaForm(DeviceItem *pItem, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceParaForm)
{
    ui->setupUi(this);

    m_Item = pItem;

    if(m_Item)
    {
        if(m_Item->mDevType == DEV_PHYSICAL)
        {
            if(m_Item->mPHDev)
            {
                setWindowTitle(m_Item->mPHDev->mDevName);
                setWindowIcon(QIcon(QPixmap::fromImage(m_Item->mImg)));
                if(m_Item->mPHDev && m_Item->mPHDev->m_SettingGroup)
                {
                    ui->BasicStackedWidget->addWidget(m_Item->mPHDev->m_SettingGroup);
                    ui->BasicStackedWidget->setFixedHeight(m_Item->mPHDev->m_SettingGroup->height());
                }
                InitAdvancedTabWidget();
            }
        }
        else if(m_Item->mDevType == DEV_LOGIC)
        {
            if(m_Item->mLGDev)
            {
                setWindowTitle(m_Item->mLGDev->mDevName);
                setWindowIcon(QIcon(QPixmap::fromImage(m_Item->mImg)));
                if(m_Item->mLGDev->m_SettingGroup)
                {
                    m_Item->mLGDev->SetPHDeviceEditble(true);
                    connect(m_Item->mLGDev, SIGNAL(sPHDeviceChanged()), this, SLOT(UpdateAdvancedTabWidget()));
                    ui->BasicStackedWidget->addWidget(m_Item->mLGDev->m_SettingGroup);
                    ui->BasicStackedWidget->setFixedHeight(m_Item->mLGDev->m_SettingGroup->height());
                }
                InitAdvancedTabWidget();
            }
        }
        on_BtnAdvanced_clicked(false);
    }
    else
    {
        on_buttonBox_rejected();
    }
}

DeviceParaForm::~DeviceParaForm()
{
    if(m_Item)
    {
        if(m_Item->mDevType == DEV_PHYSICAL)
        {
            if(m_Item->mPHDev && m_Item->mPHDev->m_SettingGroup)
            {
                ui->BasicStackedWidget->removeWidget(m_Item->mPHDev->m_SettingGroup);
                m_Item->mPHDev->m_SettingGroup->setParent(0);  //>@如果不改变其父窗口，则在此设置框注销时同时注销这些子配置Groupbox
            }
            ReleaseAdvancedTabWidget();
        }
        else if(m_Item->mDevType == DEV_LOGIC)
        {
            if(m_Item->mLGDev && m_Item->mLGDev->m_SettingGroup)
            {
                ui->BasicStackedWidget->removeWidget(m_Item->mLGDev->m_SettingGroup);
                m_Item->mLGDev->m_SettingGroup->setParent(0);
            }
            ReleaseAdvancedTabWidget();
        }
    }
    delete ui;
}

void DeviceParaForm::InitAdvancedTabWidget()
{
    if(m_Item->mDevType == DEV_PHYSICAL)
    {
        if(m_Item->mPHDev)
        {
            QList<LG_Type> tmpList = m_Item->mPHDev->mLGDevices.keys();
            for(int i=0;i<tmpList.count();i++)
            {
                LogicDevice* tmpLGDev = m_Item->mPHDev->mLGDevices.value(tmpList.at(i));
                if(tmpLGDev && tmpLGDev->m_SettingGroup)
                {
                    //>@在LG为Advance功能选项卡时禁用修改Device的功能。
                    tmpLGDev->SetPHDeviceEditble(false);
                    ui->AdvancedTabWidget->addTab(tmpLGDev->m_SettingGroup, tmpLGDev->mDevName);
                }
            }
        }
    }
    else if(m_Item->mDevType == DEV_LOGIC)
    {
        if(m_Item->mLGDev)
        {
            QList<PH_Type> tmpList = m_Item->mLGDev->mPHDevices.keys();
            for(int i=0;i<tmpList.count();i++)
            {
                PhysicalDevice* tmpPHDev = m_Item->mLGDev->mPHDevices.value(tmpList.at(i));
                if(tmpPHDev && tmpPHDev->m_SettingGroup)
                {
                    ui->AdvancedTabWidget->addTab(tmpPHDev->m_SettingGroup, tmpPHDev->mDevName);
                }
            }
        }
    }
}

void DeviceParaForm::ReleaseAdvancedTabWidget()
{

    if(m_Item->mDevType == DEV_PHYSICAL)
    {
        ui->AdvancedTabWidget->clear();
        QList<LG_Type> tmpList = m_Item->mPHDev->mLGDevices.keys();
        for(int i=0;i<tmpList.count();i++)
        {
            LogicDevice* tmpLGDev = m_Item->mPHDev->mLGDevices.value(tmpList.at(i));
            if(tmpLGDev && tmpLGDev->m_SettingGroup)
                tmpLGDev->m_SettingGroup->setParent(0);
        }
    }
    else if(m_Item->mDevType == DEV_LOGIC)
    {
        ui->AdvancedTabWidget->clear();
        QList<PH_Type> tmpList = m_Item->mLGDev->mPHDevices.keys();
        for(int i=0;i<tmpList.count();i++)
        {
            PhysicalDevice* tmpPHDev = m_Item->mLGDev->mPHDevices.value(tmpList.at(i));
            if(tmpPHDev && tmpPHDev->m_SettingGroup)
                tmpPHDev->m_SettingGroup->setParent(0);
        }
    }
}

void DeviceParaForm::UpdateAdvancedTabWidget()
{
    ReleaseAdvancedTabWidget();
    InitAdvancedTabWidget();
}

void DeviceParaForm::on_BtnAdvanced_clicked(bool checked)
{
    if(checked)
    {
        ui->AdvancedTabWidget->setHidden(false);
        ui->BasicStackedWidget->setHidden(false);
        ui->HideSpacer->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
        //setFixedHeight(ui->AdvancedTabWidget->height() + ui->BasicStackedWidget->height() + 70);
    }
    else
    {
        ui->AdvancedTabWidget->setHidden(true);
        ui->BasicStackedWidget->setHidden(false);
        ui->HideSpacer->changeSize(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
        //setFixedHeight(ui->BasicStackedWidget->height() + 70);
    }
}

void DeviceParaForm::on_buttonBox_accepted()
{
    QDialog::accept();
}

void DeviceParaForm::on_buttonBox_rejected()
{
    QDialog::reject();
}
