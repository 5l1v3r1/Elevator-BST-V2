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

#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include "global.h"
#include <QGraphicsPixmapItem>
#include <QList>
#include "diagramframe.h"

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsItem;
class QGraphicsScene;
class QTextEdit;
class QGraphicsSceneMouseEvent;
class QMenu;
class QGraphicsSceneContextMenuEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QPolygonF;
QT_END_NAMESPACE

#define ITEMZVALUEBASE     1000
#define ITEMTYPE            0

class ImageItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    ImageItem(IMAGEITEM_TYPE pType = ITEM_LAY, QGraphicsItem *parent = 0);
    ImageItem(QString pFileName, IMAGEITEM_TYPE pType = ITEM_LAY, QGraphicsItem *parent = 0);
    ImageItem(QPixmap pPixmap, IMAGEITEM_TYPE pType = ITEM_LAY, QGraphicsItem *parent = 0);

    int type() const
        { return data(ITEMTYPE).toInt(); }
    QRectF boundingRect() const;
    QPainterPath shape() const;

    bool SetPixmap(QString pFileName)
    {
        if(m_Pixmap.load(pFileName))
        {
            resize(QSizeF(m_Pixmap.size()));
            return true;
        }
        return false;
    }
    bool SetPixmap(QPixmap pPixmap)
    {
        m_Pixmap = pPixmap;
        if(m_Pixmap.isNull())
        {
            resize(QSizeF(m_Pixmap.size()));
            return true;
        }
        return false;
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    bool Scale();
    bool Rotate();
    bool MoveTo();

signals:
    void ReleaseSingnal(QPointF Pos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
    QPixmap         m_Pixmap;
};

class TextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    TextItem(QGraphicsItem *parent = 0);

    int type() const
        { return data(ITEMTYPE).toInt(); }

signals:
    void lostFocus(TextItem *item);
    void selectedChange(QGraphicsItem *item);
    void mouseRelease(QPointF Pos);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void focusOutEvent(QFocusEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
};

class ImageFrame : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT
public:
    explicit ImageFrame(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    qreal m_LeftLimitMin,m_LeftLimitMax;
    qreal m_TopLimitMin,m_TopLimitMax;
    qreal m_RightLimitMin,m_RightLimitMax;
    qreal m_BottomLimitMin,m_BottomLimitMax;

    int type() const
        { return data(ITEMTYPE).toInt(); }
    void setColor(const QColor &color);

    void InitFrame(IMAGEITEMINFO* pItemInfo);
    void DrawFrame();
    COM_OPERTATE GetCursorShape(QPointF Point);
    void UpdateFrame(QGraphicsSceneMouseEvent *event);
    void OffsetPoint(QGraphicsItem* ItemPar,QGraphicsItem* ItemCom);
public slots:
    void DragComPos(QPointF Pos);
signals:
    void updateControl(ImageFrame *);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
public:
    QHash<COM_TYPE, DiagramFrameItem *>     Frames;//>@插入的顺序是左边->上边->右边->下边
    bool                                    m_SelectFlag;
    QPolygonF                               m_FramePolygon;     //>@插入的顺序是左上角坐标->右上角->右下角->左下角
    QRectF                                  m_ComRect, m_ParRect;
    ImageItem                              *m_Com;
    ImageItem                              *m_Par;
    QPointF                                 m_ChildParentDiff;  //>@子父控件的偏移
    QPointF                                 m_SavedPoint;
    COM_OPERTATE                            m_SavedComOperate;
    bool                                    m_DragPosChangeFlag;//>@控件平移标志位
};

class IMAGEITEMINFO
{
public:
    IMAGEITEMINFO()
    {
        mSelectFlag = false;
        mParItem = 0;
        mItem = 0;
        mType = ITEM_INVALD;
        mRect = QRectF();
    }
    IMAGEITEMINFO(QString pName)
    {
        mSelectFlag = false;
        mParItem = 0;
        mItem = 0;
        mName = pName;
        mType = ITEM_INVALD;
        mRect = QRectF();
    }
    IMAGEITEMINFO(QString pName, IMAGEITEM_TYPE pType, QRectF pRect)
    {
        mSelectFlag = false;
        mParItem = 0;
        mItem = 0;
        mName = pName;
        mType = pType;
        mRect = pRect;
    }
    IMAGEITEMINFO(QString pName, QGraphicsItem *pParItem, IMAGEITEM_TYPE pType, QRectF pRect)
    {
        mSelectFlag = false;
        mParItem = pParItem;
        mItem = 0;
        mName = pName;
        mType = pType;
        mRect = pRect;
    }
    ~IMAGEITEMINFO()
    {
        mSelectFlag = false;
        if(mItem)
            delete mItem;
    }
public:
    bool SetPixmap(QString pFileName)
    {
        if(mItem && (mItem->type() == ITEM_BASE || mItem->type() == ITEM_LAY))
        {
            ImageItem *tmpItem = (ImageItem*)mItem;
            if(tmpItem)
            {
                tmpItem->SetPixmap(pFileName);
                return true;
            }
        }
        return false;
    }
    bool SetPixmap(QPixmap pPixmap)
    {
        if(mItem && (mItem->type() == ITEM_BASE || mItem->type() == ITEM_LAY))
        {
            ImageItem *tmpItem = (ImageItem*)mItem;
            if(tmpItem)
            {
                tmpItem->SetPixmap(pPixmap);
                return true;
            }
        }
        return false;
    }
    bool SetText(QString pContent)
    {
        if(mItem && mItem->type() == ITEM_TEXT)
        {
            TextItem *tmpItem = (TextItem*)(mItem);
            if(tmpItem)
            {
                tmpItem->setPlainText(pContent);
                return true;
            }
        }
        return false;
    }
    IMAGEITEM_TYPE type()
    {
        if(mItem)
            return (IMAGEITEM_TYPE)(mItem->type());
        return ITEM_INVALD;
    }

public:
    IMAGEITEM_TYPE  mType;
    QRectF          mRect;
    QString         mName;
    QGraphicsItem  *mParItem, *mItem;
    bool            mSelectFlag;
public:
    bool operator==(const IMAGEITEMINFO &s)
    {
        if(mName.compare(s.mName, Qt::CaseInsensitive))
            return false;
        if(mItem != s.mItem)
            return false;
        return true;
    }
    bool operator!=(const IMAGEITEMINFO &s)  { return !operator==(s); }
};

#endif //IMAGEITEM_H
