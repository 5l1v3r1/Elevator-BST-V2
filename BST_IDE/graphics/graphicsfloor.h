#ifndef GraphicsFLOOR_H
#define GraphicsFLOOR_H

#include "graphicsoperate.h"

#ifdef IDE
    #define ComFloor  DesignFloor
#elif DISPLAY
    #define ComFloor  GraphicsFloor
#endif

class GraphicsFloor : public GraphicsOperate
{
    Q_OBJECT
public:
    typedef enum{
        PTN_F_DISCREATE = 0,    //>@图片分散
        PTN_F_INTEGRATE         //>@图片集成
    } FLOOR_PATTERN;

    GraphicsFloor(QGraphicsItem *parent);
    bool InitSubCommonPara(AREA_STYLE* pAreaStyle, QString pLabel, QString pContent);
    bool Start();
    void updateEffect(FLOOR_INFO pFloorInfo, AREA_OPERATE pOperate);

    QPixmap GenerateFloorImage(FLOOR_INFO pFloorInfo);
    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }
public slots:
    virtual void ExecAction(QAction *pAction) {}
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    void UpdateCom(int pFrame) { GraphicsComponent::UpdateCom(pFrame); }
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
    DesignFloor(QGraphicsItem *parent);

    QSize GetRcSize(QString pRcName);
    QList<QAction*> GetActionList();
    bool Start()
    {
        if(GraphicsOperate::Start() == false)
            return false;
        if(m_FloorStyle == PTN_F_DISCREATE)
        {
            m_FloorRectDiff = size().width() / 9.0;
        }
        m_FloorNumInfo = 0xff000000;
        updateEffect(m_FloorNumInfo, OPERATE_CHANGE);  //>@隐藏楼层
        return true;
    }

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
