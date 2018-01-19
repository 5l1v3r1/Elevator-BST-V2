#ifndef AXWIDGET_H
#define AXWIDGET_H

#include "define.h"
#include <QAxWidget>

class AxWidget : public QAxWidget
{
public:
    AxWidget(QWidget* parent = 0);
    ~AxWidget();

    void PlaySwf(QString pFile);
    void OpenWebPagInIE(QString pUrl);
    void OpenXls(QString pFile);
    void OpenDoc(QString pFile);
};

#endif // AXWIDGET_H
