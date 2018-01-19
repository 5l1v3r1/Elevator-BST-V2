#include "middletoptabinit.h"

MiddleTopTabInit::MiddleTopTabInit(QWidget *parent) :
    TabPage(parent),
    ui(new Ui::MiddleTopTabInit)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    netwkAcssMng = 0;
    InitUI();
}

MiddleTopTabInit::~MiddleTopTabInit()
{
    if(netwkAcssMng)
        delete netwkAcssMng;
    delete ui;
}

void MiddleTopTabInit::InitUI()
{
    ui->widget->Initial(ANALOG);

    //>@初始化视频
    int index = GlobalPara.m_SoftwareDir.lastIndexOf('/');
    if(index < GlobalPara.m_SoftwareDir.length()-1)
        GlobalPara.m_SoftwareDir.append('/');
    QStringList tmpFindFiles = findFilesInFloder(GlobalPara.m_SoftwareDir, VIDEOPATH, true);
    if(tmpFindFiles.count())
    {
        QString VideoPath = tmpFindFiles.at(0);
        ui->VideoAera->SetUrl(VideoPath, true);
    }

//    ui->VideoAera->setVolume(50);
//    ui->VideoAera->setFile(VideoPath);
//    ui->VideoAera->ShowControlBar(true);
//    ui->VideoAera->setSmallScreen(true);
//    ui->VideoAera->setScale(0); //>@0:适应高度，1：适应宽度
//    ui->VideoAera->show();

/*    netwkAcssMng = new QNetworkAccessManager;
    QNetworkProxy proxy;
    proxy = QNetworkProxy::HttpProxy;
    proxy.setHostName(tr("10.200.0.1"));    //代理服务器
    proxy.setPort(8080);
    netwkAcssMng->setProxy(proxy);
    */
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    //显示Flash的配置
    ui->webView->setAttribute(Qt::WA_DeleteOnClose);
    ui->webView->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    ui->webView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);//重要
    //解决打不开页面中的二级链接问题
    ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    //ui->webView->page()->setNetworkAccessManager(netwkAcssMng);
    tmpFindFiles = findFilesInFloder(GlobalPara.m_SoftwareDir, WEBPATH, true);
    if(tmpFindFiles.count())
    {
        QString WebPath = tmpFindFiles.at(0);
        ui->webView->load(QUrl::fromLocalFile(WebPath));
        ui->webView->show();
    }
}

void MiddleTopTabInit::on_webView_linkClicked(const QUrl &arg1)
{
    ui->webView->load(arg1);
}

void MiddleTopTabInit::on_webView_loadProgress(int progress)
{
    if(GlobalPara.progressStatus)
    {
        GlobalPara.progressStatus->show();
        GlobalPara.progressStatus->setValue(progress);
        if(progress >= 100)
            GlobalPara.progressStatus->hide();
    }
}

