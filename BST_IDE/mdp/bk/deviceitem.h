#ifndef DEVICEITEM_H
#define DEVICEITEM_H

#include "global.h"
#include <QtOpenGL>
#include <QGLWidget>
//#include <glut>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtOpenGL/qglshaderprogram.h>
#include <QVector>
#include <QPainter>
#include <QGraphicsWidget>

#define D_ParaRadius    75

class DeviceScene;
class DeviceItem : public QGraphicsWidget
{
    Q_OBJECT
//    Q_PROPERTY(QRectF geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
public:
    DeviceItem(DeviceScene* scene = 0, QGraphicsWidget *parent = 0);
    ~DeviceItem();
//>@BasicFun
    void setPos(const QPointF &pos)
    {
        QPointF pointF(pos.x() - rect().width()/2,
                     pos.y() - rect().height()/2);
        QGraphicsWidget::setPos(pointF);
    }
    inline void setPos(qreal x, qreal y)
    {
        setPos(QPointF(x, y));
    }
//    QRectF geometry() const{return QGraphicsWidget::geometry();}
    void setGeometry(const QRectF &rect)
    {
        QRectF redressRectF(rect.left()-rect.width()/2,
                            rect.top()-rect.height()/2,
                            rect.width(),
                            rect.height());
        QGraphicsWidget::setGeometry(redressRectF);
    }
    inline void setGeometry(qreal x, qreal y, qreal w, qreal h)
    {
        setGeometry(QRectF(x, y, w, h));
    }
    void setImg(QImage pImg){mImg = pImg;}
    void setFont(QFont pFont){mFont = pFont;}
    void SetColor(QColor pColor){mColor = pColor;}
//>@AdvanceFun
    DeviceScene*            mScene;
    QImage                  mImg;
    QFont                   mFont;
    QColor                  mColor;
private:
    QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void UpdateItem(QPainter *painter)
    {
        painter->save();
        painter->drawImage(rect(), mImg, mImg.rect());
        painter->restore();
    }
    void UpdateItemName(QPainter *painter)
    {
        QString name = data(0).toString();
        if(name.isEmpty()||
          (!name.compare("BottomImg"))||
          (!name.compare("BoardImg"))||
          (!name.compare("Cpu"))||
          (!name.compare("Mem"))||
          (!name.compare("Net")))
        return;
        painter->save();
        painter->setFont(mFont);
        painter->setPen(mColor);
//>@¼ÆËã×Ö·û¿í¶È
        QFontMetrics fm(mFont);
        int pixelsWide = fm.width(name);
        int pixelsHigh = fm.height();
        QRectF tragRectF(rect().left()-(pixelsWide-rect().width()+3)/2,
                        rect().top()+rect().height()+5,
                        pixelsWide+6,
                        pixelsHigh+3);
//        QRadialGradient gradient(50, 50, 50, 50, 50);
//        gradient.setColorAt(0, QColor::fromRgbF(0, 1, 0, 1));
//        gradient.setColorAt(1, QColor::fromRgbF(0.2, 0.2, 0, 0));
//        QBrush brush(gradient);
//        painter->setBrush(brush);
//        painter->drawRoundedRect(tragRectF, 5.0, 5.0);
        painter->drawText(tragRectF, Qt::AlignCenter, name);
        painter->restore();
    }
signals:
//    void geometryChanged();
};
#endif // DEVICEITEM_H
