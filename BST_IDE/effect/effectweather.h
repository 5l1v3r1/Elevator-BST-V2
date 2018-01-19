#ifndef EFFECTWEATHER_H
#define EFFECTWEATHER_H

#include "effectbase.h"

typedef enum{
    WEATHER_PIC0=0,
    WEATHER_PIC1,
    WEATHER_PIC2,
    WEATHER_PIC3,
    WEATHER_PIC4,
    WEATHER_PIC5,
    WEATHER_PIC6,
    WEATHER_PIC7,
    WEATHER_PIC8,
    WEATHER_PIC9
} WEATHER_STATE;

class EffectWeather : public EffectBase
{
    Q_OBJECT
public:
    EffectWeather(QWidget *parent, QRect pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(WEATHER_STATE pState, AREA_OPERATE pOperate);
private:
    WEATHER_STATE     m_WeatherState;
};

#endif // EFFECTWEATHER_H
