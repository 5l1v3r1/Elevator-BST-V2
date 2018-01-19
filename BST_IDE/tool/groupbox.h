#ifndef GROUPBOX_H
#define GROUPBOX_H

#include "label.h"
#include <QWidget>

class Groupbox : public QWidget
{
    Q_OBJECT
public:
    Groupbox(QWidget *parent = 0);
    Groupbox(QString pText, QWidget *parent = 0);

    void paintEvent(QPaintEvent *e);

public:
    QVBoxLayout *m_VLayout;
};

#endif // GROUPBOX_H
