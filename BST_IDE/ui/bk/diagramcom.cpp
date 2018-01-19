#include "diagramcom.h"

ComGraphicsScene::ComGraphicsScene(QObject *parent, QRectF pRect) :
    QGraphicsScene(parent)
{
    m_Rect = QRectF(0, 0, pRect.width(), pRect.height());
    setSceneRect(m_Rect);
}

ComGraphicsView::ComGraphicsView(QWidget *parent, QRect pRect):
    QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setGeometry(pRect);

    m_Scene = new ComGraphicsScene(0, pRect);
    setScene(m_Scene);

    setRenderHint(QPainter::Antialiasing);
    //setBackgroundBrush(QPixmap("d:\\bg.png"));
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setDragMode(QGraphicsView::RubberBandDrag);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(true);
#ifdef OPENGL
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

PreviewGraphicsView::PreviewGraphicsView(QWidget *parent):
    QGraphicsView(parent)
{
    m_Scene = 0;
    //m_PixmapItem = 0;

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setRenderHint(QPainter::Antialiasing);
    //setBackgroundBrush(QPixmap("d:\\bg.png"));
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setDragMode(QGraphicsView::ScrollHandDrag);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(true);
#ifndef LINUX
    //setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#endif
}


ComBackground::ComBackground(QWidget *parent, QRect pRect) :
    QWidget(parent)
{
    setGeometry(pRect);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    m_EffectValid = false;
}

void ComBackground::paintEvent(QPaintEvent *event)
{
    if(m_EffectValid == false)
        return;
    QPainter p(this);
    if(m_Pixmap.isNull() == false)
        p.drawPixmap(rect(),m_Pixmap);
}

void ComBackground::InitEffectRc(EFFECT_INFO pEffect)
{
    m_EffectValid = false;
    int count = pEffect.RcFiles.count();
    for(int i=0;i<count;i++)
    {
        RESOURCE_INFO tmpRc = pEffect.RcFiles.at(i);
        if(tmpRc.RcName == QString("PicBg"))
        {
            QPixmap tmpPix = QPixmap(tmpRc.RcFile);
            if(tmpPix.isNull())
            {
                tmpPix = QPixmap(tmpRc.RcFile);
                if(tmpPix.isNull())
                {
                    break;
                }
            }
            m_Pixmap = tmpPix.scaled(this->width(), this->height(),
                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            m_EffectValid = true;
            break;
        }
    }
}
