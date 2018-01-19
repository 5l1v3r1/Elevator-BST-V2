#ifndef UIPAGEVIEW_H
#define UIPAGEVIEW_H

#include <QGraphicsView>
#include "diagramitem.h"
#include "diagramscene.h"

class uiPageView : public QGraphicsView
{
public:
    uiPageView(QWidget *parent);
    ~uiPageView();

    void paintEvent(QPaintEvent *event);
    virtual void drawBackground(QPainter *painter, const QRectF &rect);

public:
    DiagramScene *scene;
};

#endif // UIPAGEVIEW_H
