/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <math.h>
#include "fontitem.h"

ImageItem::ImageItem(IMAGEITEM_TYPE pType, QGraphicsItem *parent) :
    QGraphicsWidget(parent, Qt::FramelessWindowHint)
{
    setData(ITEMTYPE, pType);
    if(pType != ITEM_BASE)
        setFlags(ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

ImageItem::ImageItem(QString pFileName, IMAGEITEM_TYPE pType, QGraphicsItem *parent) :
    QGraphicsWidget(parent, Qt::FramelessWindowHint)
{
    setData(ITEMTYPE, pType);
    if(pType != ITEM_BASE)
        setFlags(ItemIsMovable);
    SetPixmap(pFileName);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

ImageItem::ImageItem(QPixmap pPixmap, IMAGEITEM_TYPE pType, QGraphicsItem *parent) :
    QGraphicsWidget(parent, Qt::FramelessWindowHint)
{
    setData(ITEMTYPE, pType);
    if(pType != ITEM_BASE)
        setFlags(ItemIsMovable);
    SetPixmap(pPixmap);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

QRectF ImageItem::boundingRect() const
{
    QRectF tmpRect = this->geometry().normalized();
    return QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized();
}

QPainterPath ImageItem::shape() const
{
    QPainterPath path;
    QRectF tmpRect = this->geometry().normalized();
    path.addRect(QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized());
    return path;
}

void ImageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(m_Pixmap.isNull() == false)
        painter->drawPixmap(rect().toRect(), m_Pixmap);
}

bool ImageItem::Scale()
{

}

bool ImageItem::Rotate()
{

}

bool ImageItem::MoveTo()
{

}

void ImageItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(type() == ITEM_BASE)
        return;
    QApplication::setOverrideCursor(Qt::SizeAllCursor);
    QGraphicsItem::mousePressEvent(event);
}

void ImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF MousePos = event->scenePos();
    emit ReleaseSingnal(MousePos);
    QApplication::restoreOverrideCursor();
    QGraphicsItem::mouseReleaseEvent(event);
}

TextItem::TextItem(QGraphicsItem *parent)
    : QGraphicsTextItem(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setData(ITEMTYPE, ITEM_TEXT);
}

QVariant TextItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        emit selectedChange(this);
    return value;
}

void TextItem::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}

void TextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction)
        setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

ImageFrame::ImageFrame(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItemGroup(parent, scene)
{
    setCursor(Qt::ArrowCursor);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    QApplication::setStartDragDistance(5);

    setData(ITEMTYPE, ITEM_FRAME);

    GlobalPara.m_ActiveImageItemInfo = 0;

    m_SelectFlag = false;
    m_DragPosChangeFlag = false;
    m_SavedComOperate = DRAGNONE;

    DiagramFrameItem *tmpLeftFrameItem = new DiagramFrameItem(this);
    Frames.insert(COM_LEFTFRAME, tmpLeftFrameItem);
    DiagramFrameItem *tmpTopFrameItem = new DiagramFrameItem(this);
    Frames.insert(COM_TOPFRAME, tmpTopFrameItem);
    DiagramFrameItem *tmpRightFrameItem = new DiagramFrameItem(this);
    Frames.insert(COM_RIGHTFRAME, tmpRightFrameItem);
    DiagramFrameItem *tmpBottomFrameItem = new DiagramFrameItem(this);
    Frames.insert(COM_BOTTOMFRAME, tmpBottomFrameItem);

    m_ChildParentDiff = QPointF(0,0);
}

void ImageFrame::setColor(const QColor &color)
{
    DiagramFrameItem *tmpFrameItem = Frames.value(COM_LEFTFRAME);
    tmpFrameItem->setColor(color);
    tmpFrameItem = Frames.value(COM_TOPFRAME);
    tmpFrameItem->setColor(color);
    tmpFrameItem = Frames.value(COM_RIGHTFRAME);
    tmpFrameItem->setColor(color);
    tmpFrameItem = Frames.value(COM_BOTTOMFRAME);
    tmpFrameItem->setColor(color);
}

void ImageFrame::OffsetPoint(QGraphicsItem* ItemPar,QGraphicsItem* ItemCom)
{
    if(ItemCom == 0)
        return;
    if(ItemPar == 0)
    {
        m_ChildParentDiff = m_ComRect.topLeft();
        return;
    }
    m_ParRect = ItemPar->sceneBoundingRect();
    m_ComRect = ItemCom->sceneBoundingRect();
    m_ChildParentDiff = m_ComRect.topLeft() - m_ParRect.topLeft();
}

void ImageFrame::DragComPos(QPointF Pos)
{
    if(m_Com==0)
        return;
    if(m_Par==0)
        return;
    QRectF ComItemRect = m_Com->sceneBoundingRect();
    qreal  ComW = ComItemRect.width();
    qreal  ComH = ComItemRect.height();

    QRectF ParItemRect = m_Par->sceneBoundingRect();
    qreal  ParW = ParItemRect.width();
    qreal  ParH = ParItemRect.height();
    int OverFlag = 0;
    QRectF rect =  mapRectToItem(m_Par,0,0,100,100).normalized();
    qreal Dx = rect.left();
    qreal Dy = rect.top();
    if(Dx<=0)
    {
       OverFlag=1;
       Dx = 0;
    }
    if(Dy<=0)
    {
       OverFlag=1;
       Dy = 0;
    }
    if(Dx>=ParW-ComW)
    {
        OverFlag=1;
        Dx = ParW-ComW;
    }
    if(Dy>=ParH-ComH)
    {
        OverFlag=1;
        Dy = ParH-ComH;
    }
    if(OverFlag==1)
    {
        m_Com->setPos(Dx,Dy);
        GlobalPara.m_ActiveImageItemInfo->mRect.setTopLeft(QPointF(Dx,Dy));
    }
    //>@更新m_Com坐标
    GlobalPara.m_ActiveImageItemInfo->mRect.setSize(m_Com->sceneBoundingRect().size());
    m_DragPosChangeFlag = true;
    emit updateControl(this);
    disconnect((ImageItem *)m_Com, SIGNAL(ReleaseSingnal(QPointF)), this, SLOT(DragComPos(QPointF)));
}


void ImageFrame::InitFrame(IMAGEITEMINFO* pItemInfo)
{
    if(pItemInfo == 0)
    {
        IDE_TRACE();
        return;
    }
    m_Com = (ImageItem*)(pItemInfo->mItem);
    if(pItemInfo->mParItem)
    {
        m_Par = (ImageItem*)(pItemInfo->mParItem);
    }
    else
    {
        m_Par = 0;
    }
    OffsetPoint(m_Par, m_Com);
    m_FramePolygon.clear();
    QRectF tmpRect = m_Com->sceneBoundingRect();
    if(tmpRect.isEmpty() || tmpRect.isValid()==false)
    {
        IDE_TRACE();
        return;
    }
    QPointF topLeft = QPointF(0,0);
    QPointF topRight = QPointF(tmpRect.width(),0);
    QPointF bottomRight = QPointF(tmpRect.width(),tmpRect.height());
    QPointF bottomLeft = QPointF(0,tmpRect.height());
    m_FramePolygon << topLeft
                   << topRight
                   << bottomRight
                   << bottomLeft;
    DrawFrame();
}

void ImageFrame::DrawFrame()
{
    if(m_FramePolygon.count() != 4)
    {
        IDE_TRACE_INT(m_FramePolygon.count());
        return;
    }
    DiagramFrameItem *tmpFrameItem = Frames.value(COM_LEFTFRAME);
    tmpFrameItem->updatePosition(m_FramePolygon, 3, 0);
    tmpFrameItem = Frames.value(COM_TOPFRAME);
    tmpFrameItem->updatePosition(m_FramePolygon, 0, 1);
    tmpFrameItem = Frames.value(COM_RIGHTFRAME);
    tmpFrameItem->updatePosition(m_FramePolygon, 1, 2);
    tmpFrameItem = Frames.value(COM_BOTTOMFRAME);
    tmpFrameItem->updatePosition(m_FramePolygon, 2, 3);
}

COM_OPERTATE ImageFrame::GetCursorShape(QPointF Point)
{
    if(m_SelectFlag)
    {
        if(m_FramePolygon.count() != 4)
        {
            IDE_TRACE_INT(m_FramePolygon.count());
            return DRAGNONE;
        }
        QPointF tmpPoint0 = (QPointF)(m_FramePolygon.at(0));
        QPointF tmpPoint2 = (QPointF)(m_FramePolygon.at(2));

        int il = tmpPoint0.x();
        int it = tmpPoint0.y();
        int ir = tmpPoint2.x();
        int ib = tmpPoint2.y();
        if ((Point.x()<=il+FRAMEVALIDWIDTH)&&(Point.x()>=il-FRAMEVALIDWIDTH)&&
                 (Point.y()<it+FRAMEVALIDWIDTH)&&(Point.y()>=it-FRAMEVALIDWIDTH))	//左上角缩放
        {
             QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
            return DRAGLEFTTOP;
        }
        else if ((Point.x()<=ir+FRAMEVALIDWIDTH)&&(Point.x()>ir-FRAMEVALIDWIDTH)&&
                 (Point.y()<it+FRAMEVALIDWIDTH)&&(Point.y()>=it-FRAMEVALIDWIDTH))	//右上角缩放
        {
             QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
            return DRAGRIGHTTOP;
        }
        else if ((Point.x()<=ir+FRAMEVALIDWIDTH)&&(Point.x()>ir-FRAMEVALIDWIDTH)&&
                 (Point.y()<=ib+FRAMEVALIDWIDTH)&&(Point.y()>ib-FRAMEVALIDWIDTH))	//右下角缩放
        {
             QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
            return DRAGRIGHTBOTTOM;
        }
        else if ((Point.x()<il+FRAMEVALIDWIDTH)&&(Point.x()>=il-FRAMEVALIDWIDTH)&&
                 (Point.y()<=ib+FRAMEVALIDWIDTH)&&(Point.y()>ib-FRAMEVALIDWIDTH))	//左下角缩放
        {
             QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
            return DRAGLEFTBOTTOM;
        }
        else if((Point.x()<il+FRAMEVALIDWIDTH)&&(Point.x()>=il-FRAMEVALIDWIDTH)&&
                (Point.y()<ib-FRAMEVALIDWIDTH)&&(Point.y()>it+FRAMEVALIDWIDTH))		//左缩放
        {
              QApplication::setOverrideCursor(Qt::SizeHorCursor);
            return DRAGLEFT;
        }
        else if((Point.x()<ir-FRAMEVALIDWIDTH)&&(Point.x()>il+FRAMEVALIDWIDTH)&&
                (Point.y()<it+FRAMEVALIDWIDTH)&&(Point.y()>it-FRAMEVALIDWIDTH))		//上缩放
        {
             QApplication::setOverrideCursor(Qt::SizeVerCursor);
            return DRAGTOP;
        }
        else if ((Point.x()<=ir+FRAMEVALIDWIDTH)&&(Point.x()>ir-FRAMEVALIDWIDTH)&&
                 (Point.y()>it+FRAMEVALIDWIDTH)&&(Point.y()<ib-FRAMEVALIDWIDTH))	//右缩放
        {
             QApplication::setOverrideCursor(Qt::SizeHorCursor);
            return DRAGRIGHT;
        }
        else if ((Point.x()<ir-FRAMEVALIDWIDTH)&&(Point.x()>il+FRAMEVALIDWIDTH)&&
                 (Point.y()<=ib+FRAMEVALIDWIDTH)&&(Point.y()>ib-FRAMEVALIDWIDTH))	//下缩放
        {
             QApplication::setOverrideCursor(Qt::SizeVerCursor);
            return DRAGBOTTOM;
        }
    }
    return DRAGNONE;
}

void ImageFrame::UpdateFrame(QGraphicsSceneMouseEvent *event)
{
    if(m_SelectFlag)
    {
        if(event == 0)
            return;
        QPointF Point = event->pos();
        QPointF ScenePoint = event->scenePos();
        if(Point.isNull() || ScenePoint.isNull())
            return;
        if(m_FramePolygon.count() != 4)
        {
            IDE_TRACE_INT(m_FramePolygon.count());
            return;
        }
        QRectF parrect = m_Par->sceneBoundingRect();
        QRectF comrect = m_Com->sceneBoundingRect();
        QPointF lefttop = m_FramePolygon.at(0);
        QPointF righttop = m_FramePolygon.at(1);
        QPointF rightbottom = m_FramePolygon.at(2);
        QPointF leftbottom = m_FramePolygon.at(3);
        m_FramePolygon.clear();
        switch(m_SavedComOperate)
        {
            case DRAGLEFT:                  //左缩放
            {
                m_LeftLimitMin = parrect.left();
                m_LeftLimitMax = comrect.right();
                if(ScenePoint.x() >= m_LeftLimitMin && ScenePoint.x() <= m_LeftLimitMax)
                {
                    lefttop.setX(Point.x());
                    leftbottom.setX(Point.x());
                }
                else if(ScenePoint.x() < m_LeftLimitMin)
                {
                    QRectF rect =  mapRectFromScene(m_LeftLimitMin,0,100,100).normalized();
                    if(rect.isEmpty())
                        break;
                    lefttop.setX(rect.left());
                    leftbottom.setX(rect.left());
                }
                else if(ScenePoint.x() > m_LeftLimitMax)
                {
                    QRectF rect =  mapRectFromScene(m_LeftLimitMax,0,100,100).normalized();
                    if(rect.isEmpty())
                        break;
                    lefttop.setX(rect.left());
                    leftbottom.setX(rect.left());
                }
                break;
            }
            case DRAGTOP:                   //上缩放
            {
                m_TopLimitMin = parrect.top();
                m_TopLimitMax = comrect.bottom();
                if(ScenePoint.y() >= m_TopLimitMin && ScenePoint.y() <= m_TopLimitMax)
                {
                    lefttop.setY(Point.y());
                    righttop.setY(Point.y());
                }
                else if(ScenePoint.y() < m_TopLimitMin)
                {
                    QRectF rect =  mapRectFromScene(0,m_TopLimitMin,100,100).normalized();
                    if(rect.isEmpty())
                        break;
                    lefttop.setY(rect.top());
                    righttop.setY(rect.top());
                }
                else if(ScenePoint.y() > m_TopLimitMax)
                {
                    QRectF rect =  mapRectFromScene(0,m_TopLimitMax,100,100).normalized();
                    if(rect.isEmpty())
                        break;
                    lefttop.setY(rect.top());
                    righttop.setY(rect.top());
                }
                break;
            }
            case DRAGRIGHT:                 //右缩放
            {
                m_RightLimitMin = comrect.left();
                m_RightLimitMax = parrect.right();
                if(rightbottom.isNull() || righttop.isNull())
                {
                    IDE_TRACE();
                    break;
                }
                if(ScenePoint.x() >= m_RightLimitMin && ScenePoint.x() <= m_RightLimitMax)
                {
                    righttop.setX(Point.x());
                    rightbottom.setX(Point.x());
                }
                else if(ScenePoint.x() < m_RightLimitMin)
                {
                    QRectF rect =  mapRectFromScene(m_RightLimitMin,0,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    righttop.setX(rect.left());
                    rightbottom.setX(rect.left());
                }
                else if(ScenePoint.x() > m_RightLimitMax)
                {
                    QRectF rect =  mapRectFromScene(m_RightLimitMax,0,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    righttop.setX(rect.left());
                    rightbottom.setX(rect.left());
                }
                break;
            }
            case DRAGBOTTOM:                //下缩放
            {
                m_BottomLimitMin = comrect.top();
                m_BottomLimitMax = parrect.bottom();
                if(rightbottom.isNull() || leftbottom.isNull())
                {
                    IDE_TRACE();
                    break;
                }
                if(ScenePoint.y() >= m_BottomLimitMin && ScenePoint.y() <= m_BottomLimitMax)
                {
                    rightbottom.setY(Point.y());
                    leftbottom.setY(Point.y());
                }
                else if(ScenePoint.y() < m_BottomLimitMin)
                {
                    QRectF rect =  mapRectFromScene(0,m_BottomLimitMin,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    rightbottom.setY(rect.top());
                    leftbottom.setY(rect.top());
                }
                else if(ScenePoint.y() > m_BottomLimitMax)
                {
                    QRectF rect =  mapRectFromScene(0,m_BottomLimitMax,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    rightbottom.setY(rect.top());
                    leftbottom.setY(rect.top());
                }
                break;
            }
            case DRAGLEFTTOP:               //左上角缩放
            {
                m_LeftLimitMin = parrect.left();
                m_LeftLimitMax = comrect.right();
                m_TopLimitMin = parrect.top();
                m_TopLimitMax = comrect.bottom();
                if((ScenePoint.x() >= m_LeftLimitMin && ScenePoint.x() <= m_LeftLimitMax ))
                {
                    lefttop.setX(Point.x());
                    leftbottom.setX(Point.x());
                }
                else if(ScenePoint.x() < m_LeftLimitMin)
                {
                    QRectF rect =  mapRectFromScene(m_LeftLimitMin,0,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    lefttop.setX(rect.left());
                    leftbottom.setX(rect.left());
                }
                else if(ScenePoint.x() > m_LeftLimitMax)
                {
                    QRectF rect =  mapRectFromScene(m_LeftLimitMax,0,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    lefttop.setX(rect.left());
                    leftbottom.setX(rect.left());
                }
                if(ScenePoint.y() >= m_TopLimitMin && ScenePoint.y() <= m_TopLimitMax)
                {
                    lefttop.setY(Point.y());
                    righttop.setY(Point.y());
                }
                else if(ScenePoint.y() < m_TopLimitMin)
                {
                    QRectF rect =  mapRectFromScene(0,m_TopLimitMin,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    lefttop.setY(rect.top());
                    righttop.setY(rect.top());
                }
                else if(ScenePoint.y() > m_TopLimitMax)
                {
                    QRectF rect =  mapRectFromScene(0,m_TopLimitMax,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    lefttop.setY(rect.top());
                    righttop.setY(rect.top());
                }
                break;
            }
            case DRAGRIGHTTOP:          //右上角缩放
            {
                m_TopLimitMin = parrect.top();
                m_TopLimitMax = comrect.bottom();
                m_RightLimitMin = comrect.left();
                m_RightLimitMax = parrect.right();
                if(ScenePoint.x() >= m_RightLimitMin && ScenePoint.x() <= m_RightLimitMax)
                {
                    righttop.setX(Point.x());
                    rightbottom.setX(Point.x());
                }
                else if(ScenePoint.x() < m_RightLimitMin)
                {
                    QRectF rect =  mapRectFromScene(m_RightLimitMin,0,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    righttop.setX(rect.left());
                    rightbottom.setX(rect.left());
                }
                else if(ScenePoint.x() > m_RightLimitMax)
                {
                    QRectF rect =  mapRectFromScene(m_RightLimitMax,0,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    righttop.setX(rect.left());
                    rightbottom.setX(rect.left());
                }
                if(ScenePoint.y() >= m_TopLimitMin && ScenePoint.y() <= m_TopLimitMax)
                {
                    lefttop.setY(Point.y());
                    righttop.setY(Point.y());
                }
                else if(ScenePoint.y() < m_TopLimitMin)
                {
                    QRectF rect =  mapRectFromScene(0,m_TopLimitMin,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    lefttop.setY(rect.top());
                    righttop.setY(rect.top());
                }
                else if(ScenePoint.y() > m_TopLimitMax)
                {
                    QRectF rect =  mapRectFromScene(0,m_TopLimitMax,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    lefttop.setY(rect.top());
                    righttop.setY(rect.top());
                }
                break;
            }
            case DRAGRIGHTBOTTOM:       //右下角缩放
            {
                m_RightLimitMin = comrect.left();
                m_RightLimitMax = parrect.right();
                m_BottomLimitMin = comrect.top();
                m_BottomLimitMax = parrect.bottom();
                if(leftbottom.isNull() || rightbottom.isNull() || righttop.isNull())
                {
                    IDE_TRACE();
                    break;
                }
                if(ScenePoint.x() >= m_RightLimitMin && ScenePoint.x() <= m_RightLimitMax)
                {
                    righttop.setX(Point.x());
                    rightbottom.setX(Point.x());
                }
                else if(ScenePoint.x() < m_RightLimitMin)
                {
                    QRectF rect =  mapRectFromScene(m_RightLimitMin,0,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    righttop.setX(rect.left());
                    rightbottom.setX(rect.left());
                }
                else if(ScenePoint.x() > m_RightLimitMax)
                {
                    QRectF rect =  mapRectFromScene(m_RightLimitMax,0,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    righttop.setX(rect.left());
                    rightbottom.setX(rect.left());
                }
                if(ScenePoint.y() >= m_BottomLimitMin && ScenePoint.y() <= m_BottomLimitMax)
                {
                    rightbottom.setY(Point.y());
                    leftbottom.setY(Point.y());
                }
                else if(ScenePoint.y() < m_BottomLimitMin)
                {
                    QRectF rect =  mapRectFromScene(0,m_BottomLimitMin,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    rightbottom.setY(rect.top());
                    leftbottom.setY(rect.top());
                }
                else if(ScenePoint.y() > m_BottomLimitMax)
                {
                    QRectF rect =  mapRectFromScene(0,m_BottomLimitMax,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    rightbottom.setY(rect.top());
                    leftbottom.setY(rect.top());
                }
                break;
            }
            case DRAGLEFTBOTTOM:        //左下角缩放
            {
                m_LeftLimitMin = parrect.left();
                m_LeftLimitMax = comrect.right();
                m_BottomLimitMin = comrect.top();
                m_BottomLimitMax = parrect.bottom();
                if(leftbottom.isNull() || rightbottom.isNull() || lefttop.isNull())
                {
                    IDE_TRACE();
                    break;
                }
                if((ScenePoint.x() >= m_LeftLimitMin && ScenePoint.x() <= m_LeftLimitMax ))
                {
                    lefttop.setX(Point.x());
                    leftbottom.setX(Point.x());
                }
                else if(ScenePoint.x() < m_LeftLimitMin)
                {
                    QRectF rect =  mapRectFromScene(m_LeftLimitMin,0,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    lefttop.setX(rect.left());
                    leftbottom.setX(rect.left());
                }
                else if(ScenePoint.x() > m_LeftLimitMax)
                {
                    QRectF rect =  mapRectFromScene(m_LeftLimitMax,0,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    lefttop.setX(rect.left());
                    leftbottom.setX(rect.left());
                }
                if(ScenePoint.y() >= m_BottomLimitMin && ScenePoint.y() <= m_BottomLimitMax)
                {
                    rightbottom.setY(Point.y());
                    leftbottom.setY(Point.y());
                }
                else if(ScenePoint.y() < m_BottomLimitMin)
                {
                    QRectF rect =  mapRectFromScene(0,m_BottomLimitMin,100,100).normalized();
                    if(rect.isEmpty())
                    {
                        IDE_TRACE();
                        break;
                    }
                    rightbottom.setY(rect.top());
                    leftbottom.setY(rect.top());
                }
                else if(ScenePoint.y() > m_BottomLimitMax)
                {
                    QRectF rect =  mapRectFromScene(0,m_BottomLimitMax,100,100).normalized();
                    rightbottom.setY(rect.top());
                    leftbottom.setY(rect.top());
                }
                break;
            }
            case DRAGNONE:
            {
                m_FramePolygon << lefttop
                               << righttop
                               << rightbottom
                               << leftbottom;
                return;
            }
        }
        m_FramePolygon << lefttop
                       << righttop
                       << rightbottom
                       << leftbottom;
        DrawFrame();
        m_SavedPoint = Point;
    }
}

void ImageFrame::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    OffsetPoint(m_Par, m_Com);
    m_SelectFlag = true;
    m_SavedPoint = event->pos();
    m_SavedComOperate = GetCursorShape(event->pos());  //>@设置了OverideCursor
}

void ImageFrame::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    UpdateFrame(event);
}

void ImageFrame::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_SelectFlag = false;
    UpdateFrame(event);
    IDE_TRACE();
    emit updateControl(this);
    QApplication::restoreOverrideCursor();              //>@回复OverideCursor
}
