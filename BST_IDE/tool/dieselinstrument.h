#ifndef DIESELINSTRUMENT_H
#define DIESELINSTRUMENT_H

#include "globalfun.h"
#include <QPainter>

class DieselInstrument : public QWidget
{
    Q_OBJECT
public:
    explicit DieselInstrument(QWidget *parent = 0);
    
public slots:
    void setValue(int newvalue);
    void init();

public:
    void paintEvent(QPaintEvent *event);
    int mValue;
};

#endif  //SPEEDINSTRUMENT_H
