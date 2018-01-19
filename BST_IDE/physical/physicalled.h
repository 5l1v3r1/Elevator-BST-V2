#ifndef PHYSICALLED_H
#define PHYSICALLED_H

#include "physicalbase.h"

class PhysicalLED : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalLED(QObject *parent = 0);
    ~PhysicalLED();

public slots:
    void slot_SetLED(int id, bool light);
};

#endif // PHYSICALLED_H
