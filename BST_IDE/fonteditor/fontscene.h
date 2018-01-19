#ifndef FONTSCENE_H
#define FONTSCENE_H

#include "global.h"
#include "imageitem.h"

class TEXTINFO
{
public:
    TEXTINFO()
    {
        mItem = 0;
    }
    QString     mName;
    TextItem *  mItem;
};

class FontScene : public QGraphicsScene
{
    Q_OBJECT

public:
    FontScene(QObject *parent = 0);
    ~FontScene();

    QFont font() const
        { return myFont; }
    QColor textColor() const
        { return myTextColor; }

    void setTextColor(const QColor &color);
    void setFont(const QFont &font);

    void CreateMenuAction();
    void DestoryMenuAction();

    void SetMode(ITEM_OPERATECODE Code) { myOperateCode = Code; }
    void SetType(IMAGEITEM_TYPE pType) { myItemType = pType; }

public slots:
    void editorLostFocus(TextItem *item);
    void RegularZValue();
    void SLOT_ComModify();
    void SLOT_ComDel();

signals:
    void itemSelected(QGraphicsItem *item);
    void mouseRelease();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

public:
    QMenu *                 m_ItemContexMenu;
    QAction                *m_ModifyCom, *m_DelCom;
    IMAGEITEM_TYPE          myItemType;
    ITEM_OPERATECODE        myOperateCode;
    QFont                   myFont;
    QColor                  myTextColor;

    QList<TEXTINFO*>        m_LayList;
    ImageFrame             *m_Frame;
    IMAGEITEMINFO*          m_SelectItemInfo;

    int                     m_LastLayNum;
};

#endif // IMAGESCENE_H
