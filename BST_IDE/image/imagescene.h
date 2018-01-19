#ifndef IMAGESCENE_H
#define IMAGESCENE_H

#include "global.h"
#include "imageitem.h"
#include "imageeffectform.h"
#include <QMenu>

class ImageScene : public QGraphicsScene
{
    Q_OBJECT

public:
    ImageScene(QObject *parent = 0);
    ~ImageScene();

    QFont font() const
        { return myFont; }
    QColor textColor() const
        { return myTextColor; }
    QColor itemColor() const
        { return myItemColor; }
    QColor lineColor() const
        { return myLineColor; }

    void setLineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setItemColor(const QColor &color);
    void setFont(const QFont &font);

    bool ItemOperate(IMAGEITEMINFO *&ItemInfo, ITEM_OPERATECODE Code);
    void deleteFrame();
    void createFrame();
    void CreateMenuAction();
    void DestoryMenuAction();

    void UpdateScene(QPixmap pPic);
    bool AddBackground(QPixmap pPic);
    bool DelBackground();

    void SetMode(ITEM_OPERATECODE Code) { myOperateCode = Code; }
    void SetType(IMAGEITEM_TYPE pType) { myItemType = pType; }

public slots:
    void editorLostFocus(TextItem *item);
    void RegularZValue();
    void updateControl(ImageFrame *pFrame);
    void updateFrame(IMAGEITEMINFO* ComInfo);

signals:
    void itemSelected(QGraphicsItem *item);
    void mouseRelease();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

public:
    IMAGEITEM_TYPE          myItemType;
    ITEM_OPERATECODE        myOperateCode;
    bool                    leftButtonDown;
    QPointF                 startPoint;
    QFont                   myFont;
    TextItem *              textItem;
    QColor                  myTextColor;
    QColor                  myItemColor;
    QColor                  myLineColor;

    QMenu *                 m_ItemContexMenu;

    QList<IMAGEITEMINFO*>   m_LayList;
    ImageFrame             *m_Frame;
    IMAGEITEMINFO*          m_SelectItemInfo;

    QAction                *m_ModifyCom, *m_DelCom;

    int                     m_LastLayNum;
};

#endif // IMAGESCENE_H
