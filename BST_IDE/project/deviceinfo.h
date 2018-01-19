#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include "playlabel.h"
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QMutex>

namespace Ui {
class DeviceInfo;
}

class DeviceToolBar;

class DeviceInfo : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceInfo(DeviceToolBar *pDevToolbar, QWidget *parent=0);
    ~DeviceInfo();

public:
    bool SetBufferPath(QString pPath);
    bool SetDevGrabScreenSrc(QString pPath) { mGrabScreenSrc =  pPath;  return true; }
    void StartGrab(QString pPath);
    void StopGrab();

protected:
    void mouseDoubleClickEvent(QMouseEvent *);

public slots:
    void slot_Print();
    void slot_Update();
    void on_AutoGrabScreenIntervel_valueChanged(int arg1);
    void on_MannualGrabScreenBtn_clicked(bool checked);
    void on_SelectLocationBtn_clicked();
    void on_OpenLocationBtn_clicked();

private:
    Ui::DeviceInfo     *ui;
    bool                mZoom;
    QRect               mRect;
    QString             mBufferPath;
    QString             mGrabScreenSrc;
    DeviceToolBar*      mDevToolbar;
    QTimer              mPrintTimer;
};

#endif // DEVICEINFO_H
