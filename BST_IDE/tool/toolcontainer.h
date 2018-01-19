#ifndef TOOLCONTAINER_H
#define TOOLCONTAINER_H

#include "global.h"
#include <QKeyEvent>
#include <QWidget>

class ToolContainer : public QWidget
{
    Q_OBJECT
public:
    explicit ToolContainer(QWidget *parent = 0);
    
    void keyPressEvent(QKeyEvent *event);

    bool eventFilter(QObject *target, QEvent *event);
signals:
    
public slots:
    
};

#endif // TOOLCONTAINER_H
