#ifndef DEVICESCENE_H
#define DEVICESCENE_H

#include "upitem.h"
#include "math.h"
#include "deviceitem.h"
#include <QGraphicsScene>
#include <QState>
#include <QStateMachine>

/*
    画布为1000x1000， 中心点为500,500，小圆半径225，大圆半径350
    显示模式，物理->逻辑图

    Physical->DeviceItem
    Logic->DeviceItem
    DeviceItem->Physical/Logic
    当删除一个DeviceItem时，如果其代表一个物理节点，则需要对应删除此物理节点以及此物理节点驱动的逻辑节点中的此物理节点的指针。
*/

class DeviceScene : public QGraphicsScene
{
    Q_OBJECT
public:
    DeviceScene(QString pMdpPath, QObject *parent = 0);
    ~DeviceScene();

    PhysicalDevice* CreateDevItem(PH_Type pType, QDomElement pElement);
    LogicDevice* CreateDevItem(LG_Type pType, QDomElement pElement);

    DeviceItem* AddDevice(PH_Type pType, QPointF pPos, LogicDevice* pLGDev, QDomElement pElement);
    void DelDevice(PH_Type pType);
    DeviceItem* AddDevice(LG_Type pType, QPointF pPos, QDomElement pElement);
    void DelDevice(LG_Type pType);
    void DelDevice(DeviceItem* pDevItem);

    bool OpenBootMdp(QString pPath);
    void CloseBootMdp();
    bool SaveBootMdp();

    void Connect(PhysicalDevice *pPHDev, LogicDevice *pLGDev);

    void SetFoucs(DeviceItem *pItem);
    void ClearFocus();

public slots:
    void Initilize();

protected:
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

public:
    QHash<PH_Type, PhysicalDevice*> m_PHDevList;
    QHash<LG_Type, LogicDevice*>    m_LGDevList;

    DeviceItem                     *m_CurDeviceItem;
    QGraphicsPixmapItem            *m_BackgroundItem;

    mdpManager*                     mMdpManager;
    QString                         mMDPPath;
};

inline void DeviceScene::SetFoucs(DeviceItem *pItem)
{
    if(m_CurDeviceItem == pItem)
        return;
    ClearFocus();
    m_CurDeviceItem = pItem;
    if(m_CurDeviceItem)
        SetColor(m_CurDeviceItem, D_SelectColor);
}

inline void DeviceScene::ClearFocus()
{
    if(m_CurDeviceItem)
    {
        ClearColor(m_CurDeviceItem);
        m_CurDeviceItem = 0;
    }
}

class DeviceView : public QGraphicsView
{
    Q_OBJECT
public:
    DeviceView(QWidget *parent = 0);

    void resizeEvent(QResizeEvent *event);

    void paintEvent(QPaintEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);
};


#endif // DEVICESCENE_H
