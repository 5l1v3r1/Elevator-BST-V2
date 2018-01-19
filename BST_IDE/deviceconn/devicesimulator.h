#ifndef DEVICESIMULATOR_H
#define DEVICESIMULATOR_H

#include <QWidget>

namespace Ui {
class DeviceSimulator;
}

class DeviceSimulator : public QWidget
{
    Q_OBJECT
    
public:
    explicit DeviceSimulator(QWidget *parent = 0);
    ~DeviceSimulator();
    
private:
    Ui::DeviceSimulator *ui;
};

#endif // DEVICESIMULATOR_H
