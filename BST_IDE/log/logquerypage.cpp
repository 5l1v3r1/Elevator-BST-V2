#include "logquerypage.h"
#include "ui_logquerypage.h"

logQueryPage::logQueryPage(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::logQueryPage)
{
    ui->setupUi(this);
}

logQueryPage::logQueryPage(QString pLogDirPath, QWidget *parent) :
    TabPage(parent),
    ui(new Ui::logQueryPage)
{
    ui->setupUi(this);
    ui->LogDirPath->setDisabled(true);
    InitLogPage(pLogDirPath);
}

logQueryPage::~logQueryPage()
{
    delete ui;
}

void logQueryPage::on_LogDirPath_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"/",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;
    InitLogPage(dir);
}

bool logQueryPage::InitEnvir(QString pLogDirPath)
{
    QDir tmpDir(pLogDirPath);
    if(tmpDir.exists() == false)
        return false;
    if(pLogDirPath.endsWith('/') == false)
        pLogDirPath.append('/');
    m_LogDirPath = pLogDirPath;
    return true;
}

void logQueryPage::InitLogPage(QString pLogDirPath)
{
    if(InitEnvir(pLogDirPath) == false)
        return;
    ui->LogFilter->clear();
    ui->LogWidget->clear();

    QDir tmpDir(pLogDirPath);
    QStringList files = tmpDir.entryList(QStringList("*.log*"), QDir::Files | QDir::NoSymLinks);
    for(int i=0;i<files.count();i++)
    {
        QString tmpString = m_LogDirPath + files.at(i);
        QFile file(tmpString);
        if(file.exists())
        {
            ui->LogFilter->addItem(files.at(i));
        }
    }
    ui->LogFilter->setCurrentIndex(-1);
}

void logQueryPage::on_LogFilter_currentIndexChanged(const QString &arg1)
{
    ui->LogWidget->clear();
    ui->LogWidget->setRowCount(0);
    QFile file(m_LogDirPath + arg1);
    if(file.exists() == false)
        return;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream stream(&file);
    QString line;
    while (!stream.atEnd())
    {
        line = stream.readLine();
        if(line.isEmpty() == false && line.isNull() == false)
            LogFileParse(line);
    }
}

void logQueryPage::LogFileParse(QString line)
{
    QStringList tmploglist = SplitString(line," ");
    int count = ui->LogWidget->rowCount();
    ui->LogWidget->insertRow(count);
    QTableWidgetItem * DateTimeLabel = new QTableWidgetItem();
    QTableWidgetItem * LevelLabel = new QTableWidgetItem();
    QTableWidgetItem * LogLabel = new QTableWidgetItem();
    DateTimeLabel->setText("  " + tmploglist.at(0)+" "+tmploglist.at(1) + "  ");
    LevelLabel->setText("  " + tmploglist.at(4) + "  ");
    LogLabel->setText("  " + tmploglist.at(6));
    ui->LogWidget->setItem(count,0,DateTimeLabel);
    ui->LogWidget->setItem(count,1,LevelLabel);
    ui->LogWidget->setItem(count,2,LogLabel);

    if(LevelLabel->text().compare("  INFO  ") == 0)
    {
        ui->LogWidget->item(count,1)->setIcon(QIcon("rc/Info/TipIcon.ico"));
    }
    else if(LevelLabel->text().compare("  WARN  ") == 0)
    {
        ui->LogWidget->item(count,1)->setIcon(QIcon("rc/Info/WarnIcon.ico"));
    }
    else if(LevelLabel->text().compare("  ERROR  ") == 0)
    {
        ui->LogWidget->item(count,1)->setIcon(QIcon("rc/Menu/delete.png"));
    }
}

void logQueryPage::on_Log_Serach_clicked()
{
    QString StartTime = ui->Log_StartTime->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString EndTime = ui->Log_EndTime->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString Level;
    int StartTimeCompareResult,EndTimeCompareResult;
    for(int i=0;i<ui->LogWidget->rowCount();i++)
    {
        ui->LogWidget->showRow(i);
        if(ui->checkBox_StartTime->checkState() == Qt::Checked)
        {
            IDE_TRACE_STR("checkBox_StartTime");
            StartTimeCompareResult = ui->LogWidget->item(i,0)->text().compare("  "+StartTime+"  ");
            if(StartTimeCompareResult < 0)
            {
                ui->LogWidget->hideRow(i);
                continue;
            }
        }

        if(ui->checkBox_EndTime->checkState() == Qt::Checked)
        {
            IDE_TRACE_STR("checkBox_EndTime");
            EndTimeCompareResult = ui->LogWidget->item(i,0)->text().compare("  "+EndTime+"  ");
            if(EndTimeCompareResult > 0)
            {
                ui->LogWidget->hideRow(i);
                continue;
            }
        }

        if(ui->checkBox_Level->checkState() == Qt::Checked)
        {
            IDE_TRACE_STR("checkBox_Level");
            Level = ui->Log_Level->currentText();
            if(ui->LogWidget->item(i,1)->text().compare("  "+Level+"  ")!= 0)
            {
                ui->LogWidget->hideRow(i);
                continue;
            }
        }
    }
}
