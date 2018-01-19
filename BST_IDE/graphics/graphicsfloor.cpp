#include "graphicsfloor.h"

GraphicsFloor::GraphicsFloor(QGraphicsItem *parent) :
    GraphicsOperate(parent)
{
    m_FloorStyle = PTN_F_INTEGRATE;
}

bool GraphicsFloor::InitSubCommonPara(AREA_STYLE* pAreaStyle, QString pLabel, QString pContent)
{
    if(pLabel.compare(PARA_PATTERN, Qt::CaseInsensitive) == 0)
    {
        if(pContent == QString("Discrete"))
            m_FloorStyle = PTN_F_DISCREATE;
        else if(pContent == QString("Integrate"))
            m_FloorStyle = PTN_F_INTEGRATE;
    }
    return true;
}

bool GraphicsFloor::Start()
{
    if(GraphicsOperate::Start() == false)
        return false;
    if(m_FloorStyle == PTN_F_DISCREATE)
    {
        m_FloorRectDiff = size().width() / 9.0;
    }
    m_PixmapPointer.mLastPointer.fill(Qt::transparent);
    m_PixmapPointer.mCurPointer.fill(Qt::transparent);
    m_FloorNumInfo = 0xffffffff;
    updateEffect(m_FloorNumInfo, OPERATE_NONE);  //>@隐藏楼层
    return true;
}

QPixmap GraphicsFloor::GenerateFloorImage(FLOOR_INFO pFloorInfo)
{
    QPixmap tmpPixmap;
    //IDE_TRACE();
    if(m_RcStyle->mRcType.compare("Image") == 0)
    {
         //IDE_TRACE();
        if(m_FloorStyle == PTN_F_DISCREATE)
        {
            //>@准备资源
            QRectF tmpRect = rect();
            QImage tmpImage(size().width(), size().height(), QImage::Format_ARGB32_Premultiplied);
            if(tmpImage.isNull())
                return false;
            tmpImage.fill(Qt::transparent); //>@创建透明图层
            QPainter painter(&tmpImage);
            DrawPixmap(&painter, GetPixmap(BGRC));
            //>@绘制文字
            quint8 tmpHundredRcIndex = pFloorInfo.mHundredBits;
            quint8 tmpTenRcIndex = pFloorInfo.mTenBits;
            quint8 tmpSingleRcIndex = pFloorInfo.mSingleBits;
            m_FloorNumRect[Hundred] = QRect(tmpRect.left(), tmpRect.top(), m_FloorRectDiff*3, tmpRect.height());
            m_FloorNumRect[Ten] = QRect(tmpRect.left()+m_FloorRectDiff*3, tmpRect.top(), m_FloorRectDiff*3, tmpRect.height());
            m_FloorNumRect[Single] = QRect(tmpRect.left()+m_FloorRectDiff*6, tmpRect.top(), m_FloorRectDiff*3, tmpRect.height());
            DrawPixmap(&painter, GetPixmap(tmpHundredRcIndex, m_FloorNumRect[Hundred].size()), m_FloorNumRect[Hundred]);
            DrawPixmap(&painter, GetPixmap(tmpTenRcIndex, m_FloorNumRect[Ten].size()), m_FloorNumRect[Ten]);
            DrawPixmap(&painter, GetPixmap(tmpSingleRcIndex, m_FloorNumRect[Single].size()), m_FloorNumRect[Single]);
            painter.end();
            tmpPixmap = QPixmap::fromImage(tmpImage);
        }
        else if(m_FloorStyle == PTN_F_INTEGRATE)
        {
             //IDE_TRACE();
            tmpPixmap =LoadPixmap(m_EffectPath.value(pFloorInfo.toInteget()), size());
            //tmpPixmap = GetPixmap(pFloorInfo.toInteget(), size());
        }
    }
    else if(m_RcStyle->mRcType.compare("Text") == 0)
    {
        //>@准备资源
        QRectF tmpRect = rect();
        QImage tmpImage(size().width(), size().height(), QImage::Format_ARGB32_Premultiplied);
        if(tmpImage.isNull())
            return false;
        tmpImage.fill(Qt::transparent); //>@创建透明图层
        QPainter painter(&tmpImage);
        DrawPixmap(&painter, GetPixmap(BGRC));
        //>@绘制文字
        quint8 tmpHundredRcIndex = pFloorInfo.mHundredBits;
        quint8 tmpTenRcIndex = pFloorInfo.mTenBits;
        quint8 tmpSingleRcIndex = pFloorInfo.mSingleBits;
        char tmpFloorText[4] = {'\0'};
        tmpFloorText[0] = GetFloorRcText(tmpHundredRcIndex);
        tmpFloorText[1] = GetFloorRcText(tmpTenRcIndex);
        tmpFloorText[2] = GetFloorRcText(tmpSingleRcIndex);
        WriteText(&painter, QString(tmpFloorText), tmpRect.toRect(), QString("Arial"), Qt::blue, m_FloorTextSize);
        painter.end();
        tmpPixmap = QPixmap::fromImage(tmpImage);
    }
    return tmpPixmap;
}


void GraphicsFloor::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    GraphicsComponent::updateEffect(pOperate, pPara);
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            //>@分解坐标，如果点击上半区域，则向上滚动
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < size().height()/3)
            {
                FLOOR_INFO tmpFloorInfo(3,2,1);
                updateEffect(tmpFloorInfo, OPERATE_RUN);
            }
            else if(tmpPoint.y() > size().height()/3 && tmpPoint.y() < size().height()*2/3)
            {
                FLOOR_INFO tmpFloorInfo(6,5,4);
                updateEffect(tmpFloorInfo, OPERATE_RUN);
            }
            else if(tmpPoint.y() > size().height()*2/3)
            {
                FLOOR_INFO tmpFloorInfo(9,8,7);
                updateEffect(tmpFloorInfo, OPERATE_RUN);
            }
            break;
        }
        case OPERATE_CHANGE:
        {
            int index = pPara.toInt();
            FLOOR_INFO tmpFloorInfo(index);
            //>@如果遇到NEXTRC，则默认播放下一张
            if(index == NEXTRC)
            {
                if(m_FloorNumInfo.mSingleBits>=9)
                {
                    tmpFloorInfo.mSingleBits = 0;
                    if(m_FloorNumInfo.mTenBits>=9)
                    {
                        tmpFloorInfo.mTenBits = 0;
                        if(m_FloorNumInfo.mHundredBits>=9)
                        {
                            tmpFloorInfo.mHundredBits = 0;
                            tmpFloorInfo.mTenBits = 0;
                            tmpFloorInfo.mSingleBits = 0;
                        }
                        else
                            tmpFloorInfo.mHundredBits = m_FloorNumInfo.mHundredBits + 1;
                     }
                     else
                        tmpFloorInfo.mTenBits = m_FloorNumInfo.mTenBits +1;
                }
                else
                    tmpFloorInfo.mSingleBits = m_FloorNumInfo.mSingleBits + 1;
            }
            updateEffect(tmpFloorInfo, OPERATE_HALT);
            break;
        }
        default:
            break;
    }
}

void GraphicsFloor::updateEffect(FLOOR_INFO pFloorInfo, AREA_OPERATE pOperate)
{
    bool tmpFloorNumChange = false;
    bool tmpOperateChange = false;
    //>@比较楼层是否改变
    if(pFloorInfo.mHundredBits != m_FloorNumInfo.mHundredBits)
    {
        tmpFloorNumChange = true;
    }
    else if(pFloorInfo.mTenBits != m_FloorNumInfo.mTenBits)
    {
        tmpFloorNumChange = true;
    }
    else if(pFloorInfo.mSingleBits != m_FloorNumInfo.mSingleBits)
    {
        tmpFloorNumChange = true;
    }
    //>@比较操作是否改变
    if(pOperate != m_OperateInfo[STEP1].mOperate)
        tmpOperateChange = true;
    if(tmpFloorNumChange == false && tmpOperateChange == false)
        return;
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    //>@准备资源
    if(tmpFloorNumChange)
    {
        m_OperateInfo[STEP0].mValid = true;
        m_OperateInfo[STEP0].mOperate = OPERATE_CHANGE;
        m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer;
        m_PixmapPointer.mCurPointer = GenerateFloorImage(pFloorInfo);
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = pOperate;

    m_FloorNumInfo = pFloorInfo;
    OperateStep0();
}

DesignFloor::DesignFloor(QGraphicsItem *parent) :
    GraphicsFloor(parent)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("Number"));
    m_ActionGroup->addAction(tr("Number Run"));
    m_ActionGroup->addAction(tr("Number Halt"));
    m_ActionGroup->addAction(tr("Letter"));
    m_ActionGroup->addAction(tr("Letter Run"));
    m_ActionGroup->addAction(tr("Letter Halt"));
    m_ActionGroup->addAction(tr("Mixed"));
    m_ActionGroup->addAction(tr("Mixed Run"));
    m_ActionGroup->addAction(tr("Mixed Halt"));
    tmpDefault->setChecked(true);
}

QSize DesignFloor::GetRcSize(QString pRcName)
{
    int index = GetKey(pRcName);
    if(index == BGRC)
        return size().toSize();
    else
        return QSize(m_FloorRectDiff*3, size().height());
    return size().toSize();
}

QList<QAction*> DesignFloor::GetActionList()
{
    QList<QAction*> tmpList;
    if(m_ActionGroup)
    {
        tmpList = m_ActionGroup->actions();
        bool hasNumber = false;
        bool hasLetter = false;
        bool hasMixed = false;
        QList<int> tmpKeyList = m_EffectPath.keys();
        for(int i=0;i<tmpKeyList.count();i++)
        {
            if(tmpKeyList.at(i)>=0 && tmpKeyList.at(i)<=9)
            {
                hasNumber = true;
                break;
            }
        }
        for(int i=0;i<tmpKeyList.count();i++)
        {
            if(tmpKeyList.at(i)>=10 && tmpKeyList.at(i)<=35)
            {
                hasLetter = true;
                break;
            }
        }
        if(hasNumber && hasLetter)
            hasMixed = true;

        for(int i=0;i<tmpList.count();i++)
        {
            QAction *tmpAction = tmpList.at(i);
            if(tmpAction == 0)
                continue;
            tmpAction->setDisabled(true);
            QString tmpString = tmpAction->text();
            if(tmpString.contains("Number") && (hasNumber || hasMixed))
            {
                tmpAction->setEnabled(true);
            }
            else if(tmpString.contains("Letter") && (hasLetter || hasMixed))
            {
                tmpAction->setEnabled(true);
            }
            else if(tmpString.contains("Mixed") && hasMixed)
            {
                tmpAction->setEnabled(true);
            }
        }
    }
    return tmpList;
}

void DesignFloor::ExecAction(QAction *pAction)
{
    updateEffect(OPERATE_CHANGE, pAction->text());
}



