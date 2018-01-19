#ifndef DESIGNREGION_H
#define DESIGNREGION_H

#include "designbase.h"

class DesignRegion : public DesignBase
{
    Q_OBJECT
public:
    DesignRegion(QGraphicsItem *parent, QRectF pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter *p);
    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();
    void ExecAction(QAction *pAction);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(int pState, AREA_OPERATE pOperate);
private:
    int            m_RegionState;
};

class DesignDesktop : public DesignRegion
{
    Q_OBJECT
public:
    DesignDesktop(QGraphicsItem *parent, QRectF pRect);
};

class DesignButtonOperate : public DesignRegion
{
    Q_OBJECT
public:
    DesignButtonOperate(QGraphicsItem *parent, QRectF pRect);
};

class DesignFuncInfo : public DesignRegion
{
    Q_OBJECT
public:
    DesignFuncInfo(QGraphicsItem *parent, QRectF pRect);
};

class DesignIntegratedInfo : public DesignRegion
{
    Q_OBJECT
public:
    DesignIntegratedInfo(QGraphicsItem *parent, QRectF pRect);
};

class DesignMultimedia : public DesignRegion
{
    Q_OBJECT
public:
    DesignMultimedia(QGraphicsItem *parent, QRectF pRect);
};

class DesignTextBulletin : public DesignRegion
{
    Q_OBJECT
public:
    DesignTextBulletin(QGraphicsItem *parent, QRectF pRect);
};


class DesignFunction : public DesignRegion
{
    Q_OBJECT
public:
    DesignFunction(QGraphicsItem *parent, QRectF pRect);
};

class DesignImage : public DesignRegion
{
    Q_OBJECT
public:
    DesignImage(QGraphicsItem *parent, QRectF pRect);
};

class DesignLogo : public DesignRegion
{
    Q_OBJECT
public:
    DesignLogo(QGraphicsItem *parent, QRectF pRect);
};

class DesignWeather : public DesignRegion
{
    Q_OBJECT
public:
    DesignWeather(QGraphicsItem *parent, QRectF pRect);
};

class DesignNamePlate : public DesignRegion
{
    Q_OBJECT
public:
    DesignNamePlate(QGraphicsItem *parent, QRectF pRect);
};


#endif // DesignREGION_H
