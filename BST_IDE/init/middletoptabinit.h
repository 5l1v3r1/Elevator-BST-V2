#ifndef MIDDLETOPTABINIT_H
#define MIDDLETOPTABINIT_H

#include "tabpage.h"
#include <QtWebKit/QWebView>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkAccessManager>
#include <QSettings>
#include <QTextCodec>
#include "ui_middletoptabinit.h"

#define VIDEOPATH  QString("Introduce.avi")
#define WEBPATH  QString("shbst.html")

namespace Ui {
class MiddleTopTabInit;
}

class MiddleTopTabInit : public TabPage
{
    Q_OBJECT
    
public:
    explicit MiddleTopTabInit(QWidget *parent = 0);
    ~MiddleTopTabInit();
    void InitUI();

private slots:
    void on_webView_linkClicked(const QUrl &arg1);
    void on_webView_loadProgress(int progress);

public:
    QNetworkAccessManager *netwkAcssMng;
    Ui::MiddleTopTabInit *ui;
};

#endif // MIDDLETOPTABINIT_H
