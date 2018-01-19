#ifndef WEBVIEW_H
#define WEBVIEW_H

#include "define.h"
#include <QWebView>
#include <QWebFrame>
#include <QtWebKit/QWebView>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkAccessManager>

class WebView : public QWebView
{
    Q_OBJECT
public:
    explicit WebView(QWidget *parent = 0);
    ~WebView()
    {
        if(netwkAcssMng)
            delete netwkAcssMng;
    }

    void SetUrl(QString pUrl);
    void ShowHtml(QString pHtmlFile);
    QVariant EvaluateJS(QString pJs);

public:
    QNetworkAccessManager *netwkAcssMng;
};

#endif // WEBVIEW_H
