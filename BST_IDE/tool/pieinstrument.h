#ifndef PIEINSTRUMENT_H
#define PIEINSTRUMENT_H

#include "global.h"

class PieInstrument : public QWidget
{
    Q_OBJECT

public:
    PieInstrument(QWidget *parent = 0);
    ~PieInstrument();

    void paintEvent(QPaintEvent *e);
};

#endif
