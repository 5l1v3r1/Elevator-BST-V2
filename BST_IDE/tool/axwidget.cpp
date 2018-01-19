#include "axwidget.h"

AxWidget::AxWidget(QWidget* parent) :
    QAxWidget(parent)
{
}

AxWidget::~AxWidget()
{

}

void AxWidget::PlaySwf(QString pFile)
{
    //connect(this,SIGNAL(FSCommand(QString,QString)),this,SLOT(flashAction(QString,QString)));
    setControl(QString::fromUtf8("{d27cdb6e-ae6d-11cf-96b8-444553540000}"));
    dynamicCall("LoadMovie(long,string)", 0, pFile.toLatin1().data());
}

void AxWidget::OpenWebPagInIE(QString pUrl)
{
    setControl(QString::fromUtf8("{8856F961-340A-11D0-A96B-00C04FD705A2}")); //设置插件为IE
    dynamicCall("Navigate(const QString&)", pUrl.toLatin1().data()); //调用参数
}

void AxWidget::OpenXls(QString pFile)
{
    setProperty("Visible", true);
    setControl(QString("Excel.Application"));
    dynamicCall("Open (const QString&)", pFile);
}

void AxWidget::OpenDoc(QString pFile)
{
    setProperty("Visible", true);
    setControl(QString("Word.Document"));
    dynamicCall("Open (const QString&)", pFile);
}

