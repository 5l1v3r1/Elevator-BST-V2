#ifndef NODECAN_H
#define NODECAN_H

#include "physicalbase.h"
#include "physicalserial.h"

class PhysicalCAN : public PhysicalDevice
{
    Q_OBJECT
public:
    PhysicalCAN(QObject *parent);

    bool open(OpenMode mode);
    void close();
    bool ConvertProtocol(QByteArray pByteArray);

public slots:
    void Monitor();
    QByteArray readAll();

public:
    PhysicalSerial          *m_CanData;
    QByteArray              m_CanByteArray;
    quint8                          m_Old_Check;
    quint8                          m_Old_basicfun;

};

#endif // NODECAN_H
