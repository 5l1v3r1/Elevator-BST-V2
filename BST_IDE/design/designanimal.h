#ifndef DESIGNANIMAL_H
#define DESIGNANIMAL_H

#include "designbase.h"

class DesignAnimal : public DesignBase
{
    Q_OBJECT
public:
    explicit DesignAnimal(QGraphicsItem *parent, QRectF pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter *p);

    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();
    void ExecAction(QAction *pAction);

    void updateEffect(AnimalStage pStage, AREA_OPERATE pOperate);
    void updateEffect(QPointF pDstPos, AREA_OPERATE pOperate);

    QLineF GetTrackLine(QPointF pPoint);
    qreal  GetTrackDiffAngle(QPointF pPoint);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
private:
    AnimalType      m_AnimalType;

    AnimalStage     m_AnimalStage;
    QLineF          m_CurTrackLine, m_NextTrackLine;
    qreal           m_CurAngle, m_DiffAngle;        //>@‘À∂ØΩ«∂»≤Ó
};

#endif // DESIGNANIMAL_H
