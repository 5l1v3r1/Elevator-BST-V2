#include "analysisform.h"
#include "ui_analysisform.h"

AnalysisForm::AnalysisForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnalysisForm)
{
    m_UartPort = 0;
    m_SocketPort = 0;
    m_Interface = 0;
    ui->setupUi(this);
    //>@初始化ProtocolTypeCombo
    QStringList tmpTypeList = GlobalPara.m_ProtocolInterfaceGroup.keys();
    for(int i=0;i<tmpTypeList.count();i++)
    {
        QDomElement tmpElement = GlobalPara.m_ProtocolInterfaceGroup.value(tmpTypeList.at(i));
        if(tmpElement.isNull())
            continue;
        ui->ProtocolTypeCombo->addItem(QIcon(tmpElement.attribute("img")), tmpElement.tagName());
        ui->ProtocolTypeCombo->setCurrentIndex(-1);
    }
}

AnalysisForm::AnalysisForm(QString pProtocol, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnalysisForm)
{
    m_UartPort = 0;
    m_SocketPort = 0;
    m_Interface = 0;
    ui->setupUi(this);
    //>@获取QDomelement
    QDomElement tmpElement = GlobalPara.m_ProtocolInterfaceGroup.value(pProtocol);
    if(AddParseUI(pProtocol, tmpElement))
    {
        ui->ProtocolTypeCombo->addItem(QIcon(tmpElement.attribute("img")), tmpElement.tagName());
        ui->ProtocolTypeCombo->setCurrentIndex(0);
        ui->ProtocolTypeCombo->setDisabled(true);
    }
    else
    {
        QStringList tmpTypeList = GlobalPara.m_ProtocolInterfaceGroup.keys();
        for(int i=0;i<tmpTypeList.count();i++)
        {
            QDomElement tmpElement = GlobalPara.m_ProtocolInterfaceGroup.value(tmpTypeList.at(i));
            if(tmpElement.isNull())
                continue;
            ui->ProtocolTypeCombo->addItem(QIcon(tmpElement.attribute("img")), tmpElement.tagName());
            ui->ProtocolTypeCombo->setCurrentIndex(-1);
        }
    }
}

AnalysisForm::~AnalysisForm()
{
    if(m_UartPort)
        m_UartPort->deleteLater();
    if(m_SocketPort)
        m_SocketPort->deleteLater();
    delete ui;
}

InterfaceBase* AnalysisForm::GetInterface()
{
    return VAR_CAST<InterfaceBase*>(ui->ParseTab->currentWidget());
}

HostNetManager* AnalysisForm::GetNetManager()
{
    return (HostNetManager*)0;
}

bool AnalysisForm::AddParseUI(QString pProtocol, QDomElement pElement)
{
    if(pProtocol.isEmpty())
        return false;
    if(pElement.isNull())
        return false;
    if(pProtocol.compare("3GPP", Qt::CaseInsensitive) == 0)
    {
        Interface3GPP *tmpParseUI = new Interface3GPP;
        tmpParseUI->InitAnalysis(pElement);
        ui->ParseTab->addTab(tmpParseUI, QIcon(pElement.attribute("img")), pProtocol);
    }
    return true;
}

bool AnalysisForm::DelParseUI(QString pProtocol)
{
    if(ui->ParseTab->count())
    {
        for(int i=0;i<ui->ParseTab->count();i++)
        {
            if(ui->ParseTab->tabText(i).compare(pProtocol, Qt::CaseInsensitive) == 0)
            {
                ui->ParseTab->removeTab(i);
                return true;
            }
        }
    }
    return false;
}

void AnalysisForm::ClearLog()
{
    ui->HistoryTable->clear();
    ui->ErrorTable->clear();
}

bool AnalysisForm::SaveLog()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存接口数据记录"), tr("接口数据记录"),
                                                    tr("日志(*.log);;All File(*.*)"));
    if(!fileName.isEmpty())
    {
        logManager *tmpLogManager = new logManager(fileName);
        tmpLogManager->NewLog();
        if(tmpLogManager->GetItemElement("/Log/History"))
        {
            for(int i=0;i<ui->HistoryTable->rowCount();i++)
            {
                QTableWidgetItem *tmpItem = ui->HistoryTable->item(i, 0);
                if(tmpItem)
                    tmpLogManager->CreateItemElement(tmpLogManager->itemElement, QString("%1").arg(i), tmpItem->text());
            }
        }
        if(tmpLogManager->GetItemElement("/Log/Error"))
        {
            for(int i=0;i<ui->ErrorTable->rowCount();i++)
            {
                QTableWidgetItem *tmpItem1 = ui->ErrorTable->item(i, 0);
                QTableWidgetItem *tmpItem2 = ui->ErrorTable->item(i, 1);
                if(tmpItem1 && tmpItem2)
                    tmpLogManager->CreateItemElement(tmpLogManager->itemElement, tmpItem1->text(), tmpItem2->text());
            }
        }
        tmpLogManager->SaveLog();
        delete tmpLogManager;

        ClearLog();
        return true;
    }
    return false;
}

bool AnalysisForm::LoadLog()
{
    static QString sSelectPath = "/";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Log"),
                                                     sSelectPath,
                                                     tr("Log (*.log)"));
    if(!fileName.isEmpty())
    {
        sSelectPath = getFileDirectory(fileName);
        ClearLog();

        logManager *tmpLogManager = new logManager(fileName);
        tmpLogManager->OpenLog();
        if(tmpLogManager->GetItemElement("/Log/History"))
        {
            QDomNodeList tmpList = tmpLogManager->itemElement.childNodes();
            for(int i=0;i<tmpList.count();i++)
            {
                QDomElement tmpElement = tmpList.at(i).toElement();
                QTableWidgetItem *tmpItem = new QTableWidgetItem(tmpElement.text());
                if(tmpItem)
                    ui->HistoryTable->setItem(i, 0, tmpItem);
            }
        }
        if(tmpLogManager->GetItemElement("/Log/Error"))
        {
            QDomNodeList tmpList = tmpLogManager->itemElement.childNodes();
            for(int i=0;i<tmpList.count();i++)
            {
                QDomElement tmpElement = tmpList.at(i).toElement();
                QTableWidgetItem *tmpItem1 = new QTableWidgetItem(tmpElement.tagName());
                QTableWidgetItem *tmpItem2 = new QTableWidgetItem(tmpElement.text());
                if(tmpItem1)
                    ui->HistoryTable->setItem(i, 0, tmpItem1);
                if(tmpItem2)
                    ui->HistoryTable->setItem(i, 1, tmpItem2);
            }
        }
        delete tmpLogManager;
        return true;
    }
    return false;
}

void AnalysisForm::on_InterfaceTypeCombo_currentIndexChanged(const QString &arg1)
{
    if(arg1.compare("UART", Qt::CaseInsensitive)==0)
        ui->InterfaceParaStack->setCurrentIndex(0);
    else if(arg1.compare("Socket", Qt::CaseInsensitive)==0)
        ui->InterfaceParaStack->setCurrentIndex(1);
}

void AnalysisForm::on_BtnConnect_clicked(bool checked)
{
    if(checked)
    {
        ui->BtnConnect->setIcon(QIcon(":/Menu/rc/Menu/pass.png"));
        //>@禁止编辑类型Combo以及配置项
        ui->InterfaceTypeCombo->setDisabled(true);
        ui->InterfaceParaStack->setDisabled(true);
        //>@根据类型获取接口句柄
        QString tmpType = ui->InterfaceTypeCombo->currentText();
        if(tmpType.compare("UART", Qt::CaseInsensitive) == 0)
        {
            if(m_UartPort)
            {
                disconnect(m_UartPort, SIGNAL(readyRead()), 0, 0);
                m_UartPort->deleteLater();
            }
            m_UartPort = new SerialPort(ui->PortCombo->currentText(), (BaudRateType)(ui->BuadCombo->currentText().toInt()));
            m_Interface = GetInterface();
            if(!(m_Interface && m_UartPort->open(QIODevice::ReadWrite)))
            {
                on_BtnConnect_clicked(false);
                return;
            }
            connect(m_UartPort, SIGNAL(readyRead()), this, SLOT(slot_ReadUartFrame()));
        }
        else if(tmpType.compare("Socket", Qt::CaseInsensitive) == 0)
        {
            disconnect(m_SocketPort, SIGNAL(sParseFrame(QString)), 0, 0);
            m_SocketPort = GetNetManager();
            m_Interface = GetInterface();
            if(!(m_SocketPort && m_Interface))
            {
                on_BtnConnect_clicked(false);
                return;
            }
            connect(m_SocketPort, SIGNAL(sParseFrame(QByteArray)), m_Interface, SLOT(ParseFrame(QByteArray)));
        }
    }
    else
    {
        ui->BtnConnect->setIcon(QIcon(":/Menu/rc/Menu/notpass.png"));
        //>@使能编辑类型Combo以及配置项
        ui->InterfaceTypeCombo->setDisabled(false);
        ui->InterfaceParaStack->setDisabled(false);
    }
}

void AnalysisForm::slot_ReadUartFrame()
{
    if(!(m_SocketPort && m_Interface))
        return;
    QByteArray tmpFrame = m_SocketPort->readAll();
    //>@去掉帧的头尾以及校验位，然后调用Interface的ParseFrame
    QByteArray tmpContent = tmpFrame.mid(1, tmpFrame.count()-2);
    quint32 len = tmpContent.count();
    if(len != D_IBFRAMELEN-3)
    {
        IDE_TRACE_INT(len);
        return;
    }
    m_Interface->ParseFrame(tmpContent);
}

void AnalysisForm::on_ProtocolTypeCombo_currentIndexChanged(const QString &arg1)
{
    if(ui->ParseTab->count())
    {
        //>@查看是否已经存在此页面了
        for(int i=0;i<ui->ParseTab->count();i++)
        {
            if(ui->ParseTab->tabText(i).compare(arg1, Qt::CaseInsensitive) == 0)
            {
                ui->ParseTab->setCurrentIndex(i);
                ClearLog();
                return;
            }
        }
        //>@如果当前已经存在的页面中没有需要的页面，则提示保存历史记录
        int ret = QMessageBox::question(this, tr("Question"),
                                      tr("Do you want to save Log?"),
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::Yes);
        if(ret == QMessageBox::Yes)
        {
            if(!SaveLog())
                ClearLog();
        }
    }
    //>@获取QDomelement
    QDomElement tmpElement = GlobalPara.m_ProtocolInterfaceGroup.value(arg1);
    AddParseUI(arg1, tmpElement);
}

void AnalysisForm::on_ParseTab_currentChanged(int index)
{
    ui->ProtocolTypeCombo->setCurrentIndex(index);
}

void AnalysisForm::on_BtnOpen_clicked()
{
    LoadLog();
}

void AnalysisForm::on_BtnSearch_clicked()
{
    if(ui->EditSerach->text().isEmpty())
        return;
}

void AnalysisForm::on_BtnRefresh_clicked()
{
    ui->HistoryTable->scrollToTop();
}

void AnalysisForm::on_BtnSave_clicked()
{
    SaveLog();
}

void AnalysisForm::on_BtnClear_clicked()
{
    ClearLog();
}

