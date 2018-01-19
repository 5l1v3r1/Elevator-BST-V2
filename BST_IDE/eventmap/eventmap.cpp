#include "eventmap.h"

ActionInfo::ActionInfo()
{
    mDesktop = INVALIDRC;
    mArrow = mFloor = mFunction = INVALIDRC;
    mVideo = mAudio = mImage = mText = INVALIDRC;
    mLogo = mNamePlate = mWeather = INVALIDRC;
}

ActionInfo::ActionInfo(QString pActions)
{
    mDesktop = INVALIDRC;
    mArrow = mFloor = mFunction = INVALIDRC;
    mVideo = mAudio = mImage = mText = INVALIDRC;
    mLogo = mNamePlate = mWeather = INVALIDRC;
    MapToAction(pActions);
}

bool ActionInfo::MapToAction(QString pActions)
{
    QStringList actionList = pActions.split(';', QString::SkipEmptyParts);
    if(actionList.isEmpty())
        return false;
    for(int i=0;i<actionList.count();i++)
    {
        QStringList tmpList = actionList.at(i).split('-', QString::SkipEmptyParts);
        if(tmpList.count() < 2)
            continue;
        COM_TYPE tmpComType = getComponetType(tmpList.at(0));
        switch(tmpComType)
        {
            case COM_DESKTOP:
                mDesktop = GetRcNum(tmpList.at(1));
                break;
            case COM_FLOOR:
                if(tmpList.count() != 4)
                    return false;
                mFloor = ParseFloorString(tmpList.at(1),tmpList.at(2),tmpList.at(3));
                break;
            case COM_ARROW:
                mArrow = GetRcNum(tmpList.at(1));
                break;
            case COM_FUNCTION:
                mFunction = GetRcNum(tmpList.at(1));
                break;
            case COM_VIDEO:
                mVideo = GetRcNum(tmpList.at(1));
                break;
            case COM_AUDIO:
                mAudio = GetRcNum(tmpList.at(1));
                break;
            case COM_IMAGE:
                mImage = GetRcNum(tmpList.at(1));
                break;
            case COM_TEXT:
                mText = GetRcNum(tmpList.at(1));
                break;
            case COM_LOGO:
                mLogo = GetRcNum(tmpList.at(1));
                break;
            case COM_NAMEPLATE:
                mNamePlate = GetRcNum(tmpList.at(1));
                break;
            case COM_WEATHER:
                mWeather = GetRcNum(tmpList.at(1));
                break;
            default:
                continue;
        }
    }
    return true;
}

bool ActionInfo::IsNull()
{
    if( (mArrow == INVALIDRC) && (mFloor == INVALIDRC) && (mFunction == INVALIDRC) &&
        (mDesktop == INVALIDRC) &&
        (mVideo == INVALIDRC) && (mAudio == INVALIDRC) && (mImage == INVALIDRC) && (mText == INVALIDRC) &&
        (mLogo == INVALIDRC) && (mNamePlate == INVALIDRC) && (mWeather == INVALIDRC))
        return true;
    return false;
}

EventMap::EventMap(QObject *parent) :
    QObject(parent)
{
    mMutex = new QMutex(QMutex::Recursive);
    mMapHash.clear();
    mMapManager = 0;
}

EventMap::~EventMap()
{
    CloseMap();
    if(mMutex)
        delete mMutex;
    mMapHash.clear();
}

bool EventMap::OpenMap(QString pPath)
{
    CloseMap();
    mMapManager = new mapManager(pPath);
    if(!mMapManager->OpenMAP())
    {
        mMapManager->m_IsChanged = false;
        delete mMapManager;
        mMapManager = 0;
        return false;
    }
    return true;
}

void EventMap::CloseMap()
{
    if(mMapManager)
    {
        mMapManager->m_IsChanged = false;
        delete mMapManager;
        mMapManager = 0;
    }
}

bool EventMap::Initialize(QString pPath)
{
    if(!OpenMap(pPath))
        return false;

    mMapHash.clear();
    //>@初始化UI哈希表
    QStringList labelList = mMapManager->m_CfgparaElementGroup.keys();
    for(int i=0;i<labelList.count();i++)
    {
        QString eventLabel = labelList.at(i);
        MAP_EVENT eventType = GetMapEventType(getFileName(eventLabel));
        if(eventType == EVENT_INVALID)
            continue;
        QDomElement parElement = mMapManager->m_CfgparaElementGroup.value(eventLabel);
        if(parElement.isNull())
            continue;
        QDomNodeList nodeList = parElement.childNodes();  //>@某个类型事件中的子项
        for(int j=0;j<nodeList.count();j++)
        {
            QDomElement element = nodeList.at(j).toElement();
            if(element.isNull())
                continue;
            //>@MapToEvent
            quint32 tmpEventKey = MapToEvent(eventType, element.tagName());
            if(!tmpEventKey)
                continue;
            //>@MapToAction
            ActionInfo tmpActionInfo(element.text());
            if(tmpActionInfo.IsNull())
                continue;
            //>@AddToHash
            mMapHash.insert(tmpEventKey, tmpActionInfo);
        }
    }
    return true;
}

quint32 EventMap::MapToEvent(MAP_EVENT pEventType, QString pTitle)
{
    QStringList tmpList = pTitle.split('-', QString::SkipEmptyParts);
    if(tmpList.count() < 2)
        return EVENT_INVALID;
    quint32 tmpValue = EVENT_INVALID;
    switch(pEventType)
    {
        case EVENT_FLOOR:
            if(tmpList.count() != 4)
                return false;
            tmpValue = ParseFloorString(tmpList.at(1),tmpList.at(2),tmpList.at(3)) + (pEventType<<28);
            break;
        case EVENT_ARROW:
        case EVENT_FUNCTION:
        case EVENT_AUDIO:
            tmpValue = GetRcNum(tmpList.at(1)) + (pEventType<<28);
            break;
        case EVENT_TIMER:
        {
            if(tmpList.count() != 2)
                return false;
            int timeCnt = 0;
            if(!StrToInt(timeCnt, 10, tmpList.at(1)))
                return false;
            tmpValue = (quint32)(startTimer(timeCnt)) + (pEventType<<28);
            break;
        }
        case EVENT_ALARM:
            if(tmpList.count() != 6)
                return false;
            tmpValue = ParseTimeString(tmpList.at(1),
                                       tmpList.at(2),
                                       tmpList.at(3),
                                       tmpList.at(4),
                                       tmpList.at(5)) + (pEventType<<28);
            break;
        default:
            break;
    }
    return tmpValue;
}

void EventMap::EmitActions(ActionInfo pAction)
{
    if(pAction.mArrow != INVALIDRC)
        emit sArrowAction(OPERATE_CHANGE, QVariant(pAction.mArrow));
    if(pAction.mFloor != INVALIDRC)
        emit sFloorAction(OPERATE_CHANGE, QVariant(pAction.mFloor));
    if(pAction.mFunction != INVALIDRC)
        emit sFunctionAction(OPERATE_CHANGE, QVariant(pAction.mFunction));
    if(pAction.mDesktop != INVALIDRC)
        emit sDesktopAction(OPERATE_CHANGE, QVariant(pAction.mDesktop));
    if(pAction.mVideo != INVALIDRC)
        emit sVideoAction(OPERATE_CHANGE, QVariant(pAction.mVideo));
    if(pAction.mAudio != INVALIDRC)
        emit sAudioAction(OPERATE_CHANGE, QVariant(pAction.mAudio));
    if(pAction.mImage != INVALIDRC)
        emit sImageAction(OPERATE_CHANGE, QVariant(pAction.mImage));
    if(pAction.mText != INVALIDRC)
        emit sTextAction(OPERATE_CHANGE, QVariant(pAction.mText));
    if(pAction.mLogo != INVALIDRC)
        emit sLogoAction(OPERATE_CHANGE, QVariant(pAction.mLogo));
    if(pAction.mNamePlate != INVALIDRC)
        emit sNamePlateAction(OPERATE_CHANGE, QVariant(pAction.mLogo));
    if(pAction.mWeather != INVALIDRC)
        emit sWeatherAction(OPERATE_CHANGE, QVariant(pAction.mLogo));
}

void EventMap::timerEvent(QTimerEvent *event)
{
    quint32 timeID = (quint32)(event->timerId()) + (EVENT_TIMER<<28);
    ActionInfo actionInfo = mMapHash.value(timeID);
    if(!actionInfo.IsNull())
        EmitActions(actionInfo);
}

void EventMap::slot_AlarmEvent(QDateTime pTime)
{
    emit sClockAction(OPERATE_CHANGE, QVariant(pTime));
    int year = pTime.date().year();
    int month = pTime.date().month();
    int day = pTime.date().day();
    int hour = pTime.time().hour();
    int minute = pTime.time().minute();
    quint32 tmpEvent[5];
    tmpEvent[0] = ParseTimeString(year, month, day, hour, minute);
    tmpEvent[1] = ParseTimeString(0x3f, month, day, hour, minute);
    tmpEvent[2] = ParseTimeString(0x3f, 0x0f, day, hour, minute);
    tmpEvent[3] = ParseTimeString(0x3f, 0x0f, 0x1f, hour, minute);
    tmpEvent[4] = ParseTimeString(0x3f, 0x0f, 0x1f, 0x1f, minute);
    for(int i=0;i<5;i++)
    {
        ActionInfo actionInfo = mMapHash.value(tmpEvent[i]+(EVENT_ALARM<<28));
        if(!actionInfo.IsNull())
            EmitActions(actionInfo);
    }
}

void EventMap::slot_FloorEvent(quint32 pKey)
{
    //IDE_TRACE_INT(pKey);
    ActionInfo actionInfo = mMapHash.value(pKey+(EVENT_FLOOR<<28));
    if(actionInfo.IsNull())
    {
        //IDE_TRACE_INT(pKey);
        emit sFloorAction(OPERATE_CHANGE, QVariant(pKey));
    }
    else
        EmitActions(actionInfo);
}

void EventMap::slot_ArrowEvent(ARROWSTATE pKey)
{
    //IDE_TRACE_INT(pKey);
    ActionInfo actionInfo = mMapHash.value(pKey+(EVENT_ARROW<<28));
    if(actionInfo.IsNull())
    {
        emit sArrowAction(OPERATE_CHANGE, QVariant((quint32)pKey));
    }
    else
        EmitActions(actionInfo);
}

void EventMap::slot_FunctionEvent(quint32 pKey)
{
    //IDE_TRACE_INT(pKey);
    ActionInfo actionInfo = mMapHash.value(pKey+(EVENT_FUNCTION<<28));
    if(actionInfo.IsNull())
    {
        //IDE_TRACE();
        emit sFunctionAction(OPERATE_CHANGE, QVariant(pKey));
    }
    else
    {
        //IDE_TRACE();
        EmitActions(actionInfo);
    }
}

void EventMap::slot_AudioEvent(quint32 pKey)
{
    ActionInfo actionInfo = mMapHash.value(pKey+(EVENT_AUDIO<<28));
    if(actionInfo.IsNull())
        emit sFunctionAction(OPERATE_CHANGE, QVariant(pKey));
    else
        EmitActions(actionInfo);
}

