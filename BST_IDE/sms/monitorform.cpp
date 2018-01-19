#include "monitorform.h"
#include "ui_monitorform.h"

MonitorForm::MonitorForm(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::MonitorForm)
{
    ui->setupUi(this);

    AddNewMonitor(QString("127.0.0.1"));
    AddNewMonitor(QString("192.168.0.1"));
    AddNewMonitor(QString("192.168.0.2"));
}

MonitorForm::~MonitorForm()
{
    delete ui;
}

void MonitorForm::AddNewMonitor(QString pUrl)
{
    QStringList tmpList;
    tmpList << "???" << pUrl << "HTTP";
    QTreeWidgetItem *tmpItem = new QTreeWidgetItem(tmpList);
    if(pUrl.startsWith("http://") == false)
        pUrl.prepend(QString("http://"));
    if(pUrl.endsWith(":8090/monitor.rm") == false)
        pUrl.prepend(QString(":8090/monitor.rm"));
    tmpItem->setStatusTip(1, pUrl);
    ui->MonitorTree->addTopLevelItem(tmpItem);
}

bool MonitorForm::MdiChildHasCreated(QString pFilePath)
{
    QList<QMdiSubWindow *>  tmpList = ui->VideoMdiArea->subWindowList();
    for(int i=0;i<tmpList.count();i++)
    {
        StreamMediaLabel *child = VAR_CAST<StreamMediaLabel *>(tmpList.at(i)->widget());
        if(child)
        {
            QString tmpString = child->windowTitle();
            if(tmpString.compare(pFilePath, Qt::CaseInsensitive) == 0)
                return true;
        }
    }
    return false;
}

QMdiSubWindow *MonitorForm::createMdiChild(QString pFilePath)
{
    if (pFilePath.isEmpty())
    {
        QMessageBox::warning(this, tr("open error"),
                             tr("File path is null!"));
        return (QMdiSubWindow*)0;
    }
    //>@查看是否已经存在此子窗口
    if(MdiChildHasCreated(pFilePath))
    {
        QMessageBox::warning(this, tr("Warn"),
                             tr("%1 has opened!").arg(pFilePath));
        return (QMdiSubWindow*)0;
    }
    StreamMediaLabel *tmpLabel = new StreamMediaLabel;
    QMdiSubWindow* tmpSubWin = ui->VideoMdiArea->addSubWindow(tmpLabel);
    if(tmpSubWin == (QMdiSubWindow*)0)
    {
        QMessageBox::warning(this, tr("Eroor"),
                             tr("%1 open failed!").arg(pFilePath));
        return tmpSubWin;
    }
    tmpLabel->SetUrl(pFilePath, true);
    connect(tmpSubWin, SIGNAL(destroyed(QObject*)), this, SLOT(destroyed(QObject *)));
    return tmpSubWin;
}

void MonitorForm::on_MonitorTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(item)
    {
        QString tmpString = item->statusTip(1);
        QMdiSubWindow * tmpSubWin = createMdiChild(tmpString);
        if(tmpSubWin)
            tmpSubWin->show();
    }
}

void MonitorForm::destroyed(QObject *pObject)
{

}

void MonitorForm::on_BtnGrid_clicked()
{
    ui->VideoMdiArea->cascadeSubWindows();
}
