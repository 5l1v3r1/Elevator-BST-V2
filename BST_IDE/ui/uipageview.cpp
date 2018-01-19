#include "uipageview.h"

uiPageView::uiPageView(QWidget *parent) :
    QGraphicsView(parent)
{
    scene = 0;
    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(QBrush(Qt::gray));
    //setBackgroundBrush(QPixmap(":/Page/rc/Page/background.png"));
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setAcceptDrops(true);
    setDragMode(QGraphicsView::RubberBandDrag);
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(true);

    scene = new DiagramScene(this);
    setScene(scene);
}

uiPageView::~uiPageView()
{
    if(scene)
    {
        scene->deleteLater();
        scene = 0;
    }
}

//>@缩放Scene到View的大小
//fitInView(this->rect(), Qt::KeepAspectRatio);

void uiPageView::paintEvent(QPaintEvent *event)
{
    static bool busy = false;
    if(!busy)
        busy = true;
    else
        return;
    QGraphicsView::paintEvent(event);
    busy = false;
}

void uiPageView::drawBackground(QPainter *painter, const QRectF &rect)
{
    static bool busy = false;
    if(!busy)
        busy = true;
    else
        return;
    QGraphicsView::drawBackground(painter, rect);
    if(scene)
    {
        int sceneWidth = scene->width();
        if(sceneWidth<=RectPulse*2)
            sceneWidth = 800+2*StartPosX;
        int sceneHeight = scene->height();
        if(sceneHeight<=RectPulse*2)
            sceneHeight = 600+2*StartPosX;
        int w = (int)(sceneWidth-RectPulse*2);
        int h = (int)(sceneHeight-RectPulse*2);
        painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
        painter->setBrush(Qt::white);
        painter->drawRect(QRect(0, 0, sceneWidth, sceneHeight));
        painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
        painter->drawRect(QRect(RectPulse, RectPulse, w, h));
    }
//    qreal tmp;
//    for(tmp = 0; tmp < scene->width()-LineLen; tmp+=LinePulse)
//    {
//        painter->drawLine(QPointF(tmp,0), QPointF(tmp,LineLen));
//        painter->drawLine(QPointF(tmp,scene->height()-LineLen), QPointF(tmp,scene->height()));
//    }
//    for(tmp = 0; tmp < scene->height()-LineLen; tmp+=LinePulse)
//    {
//        painter->drawLine(QPointF(0,tmp), QPointF(LineLen,tmp));
//        painter->drawLine(QPointF(scene->width()-LineLen,tmp), QPointF(scene->width(),tmp));
//    }
    busy = false;
}

