#ifndef USBPORT_H
#define USBPORT_H

#include "global.h"

class UsbPort : public QObject
{
public:
    UsbPort(QObject *parent=0);
    ~UsbPort();
};


#endif // USBPORT_H
