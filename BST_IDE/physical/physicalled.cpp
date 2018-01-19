#include "physicalled.h"

PhysicalLED::PhysicalLED(QObject *parent) :
    PhysicalDevice(parent)
{
}

PhysicalLED::~PhysicalLED()
{
}

void PhysicalLED::slot_SetLED(int id, bool light)
{
    SYSTEM_CMD::SetLED(id, light);
}
