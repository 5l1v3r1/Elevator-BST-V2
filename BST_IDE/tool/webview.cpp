#include "webview.h"

WebView::WebView(QWidget *parent) :
    QWebView(parent)
{
    netwkAcssMng = 0;
    /*netwkAcssMng = new QNetworkAccessManager;
    QNetworkProxy proxy;
    proxy = QNetworkProxy::HttpProxy;
    proxy.setHostName(tr("10.200.0.1"));    //代理服务器
    proxy.setPort(8080);
    netwkAcssMng->setProxy(proxy);
    page()->setNetworkAccessManager(netwkAcssMng);
    */
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    //显示Flash的配置
    setAttribute(Qt::WA_DeleteOnClose);
    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    settings()->setAttribute(QWebSettings::PluginsEnabled, true);//重要
    settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    settings()->setAttribute(QWebSettings::JavaEnabled, true);
    //解决打不开页面中的二级链接问题
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
}

void WebView::SetUrl(QString pUrl)
{
    load(QUrl(pUrl));
    show();
}

void WebView::ShowHtml(QString pHtmlFile)
{
    QFile file(pHtmlFile);
    if(!file.open(QIODevice::ReadOnly))
    {
        IDE_TRACE();
        return;
    }
    QString content;
    QTextStream stream(&file);
    content = stream.readAll();
    setHtml(content);
    show();
}

QVariant WebView::EvaluateJS(QString pJs)
{
    QWebFrame *frame = page()->mainFrame();
    if(frame)
        return frame->evaluateJavaScript(pJs);
    return QVariant();
}
