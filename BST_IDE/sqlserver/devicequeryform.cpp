#include "devicequeryform.h"
#include "ui_devicequeryform.h"
#include "global.h"

DeviceQueryForm::DeviceQueryForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceQueryForm)
{
    ui->setupUi(this);
    m_SqlQuery = 0;
    ui->DeviceOperateLog->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui->DeviceOperateLog->horizontalHeader()->setStretchLastSection(true);  //列宽度自适应
    ui->DeviceOperateLog->verticalHeader()->hide();

    ui->ConnectTypeTabWidget->setCurrentIndex(0);
    ui->ServerIPAddr->setText(GetLocalIP());
    ui->ServerPort->setText(QString("%1").arg(D_NMPORT));
    ui->DataBaseName->addItem(QIcon(":/Menu/rc/Menu/grid.png"), QString("BST_IDE"));
    ui->UserName->setText("sa");

    SqlDisconnected();
}

DeviceQueryForm::~DeviceQueryForm()
{
    if(m_SqlQuery != 0)
    {
        m_SqlQuery->deleteLater();
        m_SqlQuery = 0;
    }
    delete ui;
}

void DeviceQueryForm::on_ServerConnectBtn_clicked()
{
    QString tmpString = ui->ServerConnectBtn->text();
    if(tmpString == "Connect")
    {
        QString tmpIP = ui->ServerIPAddr->text();
        ENET_ADDR tmpIpAddr(tmpIP);
        if(tmpIpAddr.isValid() == false)
        {
            QMessageBox::warning(this, tr("Error"),
                                       tr("Server IP is invalid, please correct!"),
                                 QMessageBox::Yes,
                                 QMessageBox::Yes);
            return;
        }

        QString tmpPortString = ui->ServerPort->text();
        if(tmpPortString.isEmpty())
        {
            QMessageBox::warning(this, tr("Error"),
                                       tr("Server Port is invalid, please correct!"),
                                 QMessageBox::Yes,
                                 QMessageBox::Yes);
            return;
        }
        int tmpPort = tmpPortString.toInt();

        if(m_SqlQuery != 0)
        {
            m_SqlQuery->deleteLater();
            m_SqlQuery = 0;
        }
        m_SqlQuery = new SqlQuery(this);
        if(m_SqlQuery->Connect(tmpIpAddr.toQString(),
                               tmpIpAddr.toQString(),
                               tmpPort,
                               ui->DataBaseName->currentText(),
                               ui->UserName->text(),
                               ui->Passwd->text()))
        {
            SqlConnected();
        }
    }
    else
    {
        if(m_SqlQuery != 0)
        {
            m_SqlQuery->deleteLater();
            m_SqlQuery = 0;
        }
        SqlDisconnected();
    }
}

void DeviceQueryForm::on_BtnBarcodeSearch_clicked()
{
    on_DeviceTableCombox_currentIndexChanged(0);
    ui->BarcodeEdit->selectAll();
}

void DeviceQueryForm::on_BarcodeEdit_editingFinished()
{
    on_BtnBarcodeSearch_clicked();
}

void DeviceQueryForm::SqlConnected()
{
    emit sConnstate(true, QString());

    ui->TableGroupbox->setEnabled(true);

    ui->ServerIPAddr->setDisabled(true);
    ui->ServerPort->setDisabled(true);
    ui->DataBaseName->setDisabled(true);
    ui->UserName->setDisabled(true);
    ui->Passwd->setDisabled(true);

    QStringList tmpTableList;
    tmpTableList << QString("设备信息")
                 << QString("设备操作记录")
                 << QString("设备反馈记录")
                 << QString("设备返修记录");
    ui->DeviceTableCombox->clear();
    ui->DeviceTableCombox->addItems(tmpTableList);
    ui->DeviceTableCombox->setCurrentIndex(0);
    ui->TableGroupbox->setEnabled(true);
    ui->ServerConnectBtn->setText("Disconnect");
    InitDeviceInfo(ui->BarcodeEdit->text());
    ui->BarcodeEdit->setFocus();
    GlobalPara.updateProgress(QString("SQL Connected!"),0);//进度条
}

void DeviceQueryForm::SqlDisconnected()
{
    emit sConnstate(false, QString());

    ui->TableGroupbox->setDisabled(true);
    ui->ServerIPAddr->setEnabled(true);
    ui->ServerPort->setEnabled(true);
    ui->DataBaseName->setEnabled(true);
    ui->UserName->setEnabled(true);
    ui->Passwd->setEnabled(true);

    if(m_SqlQuery != 0)
    {
        m_SqlQuery->deleteLater();
        m_SqlQuery = 0;
    }
    ui->TableGroupbox->setEnabled(false);
    ui->ServerConnectBtn->setText("Connect");
}


void DeviceQueryForm::InitDeviceInfo(QString pBarCode)
{
    QString tmpCondition;
    if(pBarCode.isEmpty())
        tmpCondition = QString("");
    else
        tmpCondition = QString("where 设备号='%1'").arg(pBarCode);
//    Qt::CheckState usercheck = ui->UserCheckbox->checkState();
//    Qt::CheckState timecheck = ui->TimeCheckbox->checkState();
    QString tmpQueryString = QString("select %1 from %2 %3")
                             .arg("*")
                             .arg(ui->DeviceTableCombox->currentText())
                             .arg(tmpCondition);
    if(m_SqlQuery->ExecSqlQuery(tmpQueryString))
    {
        IDE_TRACE();
        ui->DeviceOperateLog->setModel(m_SqlQuery->m_SqlQueryModel);
    }
    else
    {
        ui->DeviceOperateLog->setModel(new QSqlQueryModel);
        QMessageBox::warning(this, tr("Error"),
                             m_SqlQuery->m_LastErrorString,
                             QMessageBox::Ok,
                             QMessageBox::Ok);
    }
}

void DeviceQueryForm::on_DeviceTableCombox_currentIndexChanged(int index)
{
    QString tmpBarCode = ui->BarcodeEdit->text();
    InitDeviceInfo(tmpBarCode);
}
