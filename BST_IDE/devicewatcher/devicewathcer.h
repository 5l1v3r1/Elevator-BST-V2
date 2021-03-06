#ifndef DEVICEWATHCER_H
#define DEVICEWATHCER_H

#include "globalfun.h"
#include "qdevicewatcher.h"
#include "qdevicechangeevent.h"

class DeviceWatcher : public QThread
{
    Q_OBJECT
public:
    DeviceWatcher(QObject *parent = 0);

signals:
    void sDeviceAdded(const QString& dev);
    void sDeviceRemoved(const QString& dev);
    void sDeviceChanged(const QString& dev);

private:
    QDeviceWatcher *watcher;
};


#endif // DEVICEWATHCER_H
