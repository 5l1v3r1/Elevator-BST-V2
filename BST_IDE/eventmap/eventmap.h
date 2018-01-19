#ifndef EVENTMAP_H
#define EVENTMAP_H

#include "global.h"

class ActionInfo
{
public:
    ActionInfo();
    ActionInfo(QString pActions);

public:
    bool MapToAction(QString pActions);
    bool IsNull();

public:
    quint32         mDesktop;
    quint32         mArrow, mFloor, mFunction;
    quint32         mVideo, mAudio, mImage, mText;
    quint32         mLogo, mNamePlate, mWeather;
};

class EventMap : public QObject
{
    Q_OBJECT
public:
    explicit EventMap(QObject *parent = 0);
    ~EventMap();

    bool Initialize(QString pPath);

    bool OpenMap(QString pPath);
    void CloseMap();

    void EmitActions(ActionInfo pAction);

    //>@-----------------------
    //>@ 31 ~28
    //>@  类型(16个)
    //>@-----------------------
    quint32 MapToEvent(MAP_EVENT pEventType, QString pTitle);

signals:
    void sDesktopAction(AREA_OPERATE, QVariant);
    void sArrowAction(AREA_OPERATE, QVariant);
    void sFloorAction(AREA_OPERATE, QVariant);
    void sFunctionAction(AREA_OPERATE, QVariant);
    void sVideoAction(AREA_OPERATE, QVariant);
    void sAudioAction(AREA_OPERATE, QVariant);
    void sImageAction(AREA_OPERATE, QVariant);
    void sTextAction(AREA_OPERATE, QVariant);
    void sLogoAction(AREA_OPERATE, QVariant);
    void sWeatherAction(AREA_OPERATE, QVariant);
    void sNamePlateAction(AREA_OPERATE, QVariant);
    void sClockAction(AREA_OPERATE, QVariant);

protected:
    void timerEvent(QTimerEvent *event); //>@循环时间事件

public slots:
    void slot_AlarmEvent(QDateTime pTime);  //>@时钟事件
    void slot_ArrowEvent(ARROWSTATE pState);
    void slot_FloorEvent(quint32 pNum);
    void slot_FunctionEvent(quint32 pNum);
    void slot_AudioEvent(quint32 pNum);

public:
    mapManager                 *mMapManager;
    QMutex                     *mMutex;
    QHash<quint32, ActionInfo>  mMapHash;
};

#endif // EVENTMAP
