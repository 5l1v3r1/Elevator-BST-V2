#ifndef EFFECTFLOOR_H
#define EFFECTFLOOR_H

#include "effectbase.h"

class EffectFloor : public EffectBase
{
    Q_OBJECT
public:
    EffectFloor(QWidget *parent, QRect pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(FLOOR_INFO pFloorInfo, AREA_OPERATE pOperate);
    quint8 GetFloorRcText(quint8 pFloorRcIndex);
    int  GetFloorRcIndex(quint8 pFloorNum);
    QPixmap GenerateFloorImage(FLOOR_INFO pFloorInfo);
private:
    //>@将百十个位绘制在一张图片上，将楼层图片宽度分为10等份，0~9个等分区域
    QRect               m_FloorNumRect[3];  //>@Hundred:2   Ten:1  Single:0
    FLOOR_INFO          m_FloorNumInfo;

    qreal               m_FloorRectDiff;
    int                 m_FloorTextSize;

    FLOOR_STYLE         m_FloorStyle;
};

#endif // EFFECTFLOOR_H
