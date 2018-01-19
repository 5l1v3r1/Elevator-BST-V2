#ifndef LABEL_H
#define LABEL_H

#include "toolbase.h"
#include <QLabel>

class Label : public QLabel
{
    Q_OBJECT
public:
    Label(QString text, QWidget *parent = 0);
};

#endif // LABEL_H
