#ifndef DIAGRAMCOM_H
#define DIAGRAMCOM_H

#include "global.h"

class ComGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit ComGraphicsScene(QObject *parent, QRectF pRect);
public:
    QRectF      m_Rect;
};

class ComGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ComGraphicsView(QWidget *parent, QRect pRect);

    bool PrintSence(QString PicName)
    {
        if(m_Scene == 0 || PicName.isEmpty())
            return false;
        QPixmap pixmap;
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        m_Scene->render(&painter);
        painter.end();
        pixmap.save(PicName);
        return true;
    }
public:
    ComGraphicsScene *m_Scene;
};

class PreviewGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PreviewGraphicsView(QWidget *parent);

    void InitScene(QRect pRect)
    {
        QRectF tmpRect = QRectF(0, 0, pRect.width(), pRect.height());
        if(m_Scene)
        {
            m_Scene->m_Rect = tmpRect;
            m_Scene->setSceneRect(m_Scene->m_Rect);
        }
        else
        {
            m_Scene = new ComGraphicsScene(0, tmpRect);
            setScene(m_Scene);
        }
    }

    void resizeEvent(QResizeEvent *event)
    {
        fitInView(sceneRect(), Qt::KeepAspectRatio);
        sceneRect().setSize(size());
    }

public:
    ComGraphicsScene *m_Scene;
};

class ComBackground : public QWidget
{
    Q_OBJECT
public:
    explicit ComBackground(QWidget *parent, QRect pRect);
    void paintEvent(QPaintEvent *);
    void InitEffectRc(EFFECT_INFO pEffect);
    void SetImage(QString pFilePath)
    {
        m_EffectValid =  false;

        QImage tmpImage;
        if(tmpImage.load(pFilePath) == false)
            return;
        m_Pixmap = QPixmap::fromImage(tmpImage.scaled(QSize(width(), height()), Qt::IgnoreAspectRatio));
        m_EffectValid = true;
        update();
    }
    void SetImage(QPixmap pPixmap)
    {
        m_EffectValid =  false;
        if(pPixmap.isNull())
            return;
        m_Pixmap = pPixmap;
        m_EffectValid = true;
        update();
    }

private:
    QPixmap     m_Pixmap;
    bool        m_EffectValid;
};

#endif // DIAGRAMCOM_H
