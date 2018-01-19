#ifndef GraphicsFLOOR_H
#define GraphicsFLOOR_H

#include "graphicsbase.h"

typedef enum{
    PTN_F_DISCREATE = 0,    //>@图片分散
    PTN_F_INTEGRATE         //>@图片集成
} FLOOR_PATTERN;

class GraphicsFloor : public GraphicsBase
{
    Q_OBJECT
public:
    GraphicsFloor(QGraphicsItem *parent, QRectF pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    virtual bool PaintEffect(QPainter *p);
    void updateEffect(FLOOR_INFO pFloorInfo, AREA_OPERATE pOperate);
    QPixmap GenerateFloorImage(FLOOR_INFO pFloorInfo);
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    void UpdateCom(int pFrame) { GraphicsBase::UpdateCom(pFrame); }
public:
    FLOOR_PATTERN       m_FloorStyle;

    //>@如果是分散图像模式，则会用到以下内容。
    //>@将百十个位绘制在一张图片上，将楼层图片宽度分为10等份，0~9个等分区域
    QRectF              m_FloorNumRect[3];  //>@Hundred:2   Ten:1  Single:0
    FLOOR_INFO          m_FloorNumInfo;
    qreal               m_FloorRectDiff;

    //>@如果是文本模式，则会用到以下内容。
    int                 m_FloorTextSize;
};

class DesignFloor : public GraphicsFloor
{
    Q_OBJECT
public:
    DesignFloor(QGraphicsItem *parent, QRectF pRect);

    QSize GetRcSize(QString pRcName);
    QList<QAction*> GetActionList();

    bool PaintEffect(QPainter *p)
    {
        GraphicsFloor::PaintEffect(p);
        //>@绘制边框
        QRectF tmpRect = geometry();
        qreal Width = tmpRect.width();
        qreal Height = tmpRect.height();
        p->translate(Width / 2, Height / 2);
        p->setPen(Qt::DashDotDotLine);
        p->setPen(QColor(10,10,255));
        p->drawRect(QRect(-Width/2, -Height/2, Width, Height));
        p->drawRect(QRect(-Width/2+1, -Height/2+1, Width-2, Height-2));
        return true;
    }

public slots:
    void ExecAction(QAction *pAction);
};

#endif // EFFECTFLOOR_H
