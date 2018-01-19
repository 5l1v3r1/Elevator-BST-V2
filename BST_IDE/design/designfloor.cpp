#include "designfloor.h"

DesignFloor::DesignFloor(QGraphicsItem *parent, QRectF pRect) :
    DesignBase(parent, pRect)
{
    m_FloorStyle = STYLE_INTEGRATE;
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

bool DesignFloor::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        RESOURCE_INFO tmpRc;
        int index;
        int count = pEffect->Effect.RcFiles.count();
        if(pEffect->Effect.EffectType == QString("Image"))
        {
            m_RcType = RCIMAGE;
            for(int i=0;i<count;i++)
            {
                tmpRc = pEffect->Effect.RcFiles.at(i);
                index = GetKey(tmpRc.RcName);
                IDE_TRACE_INT(index);
                if(LoadPath(index, tmpRc.RcFile) == false)
                {
                    IDE_TRACE_STR(tmpRc.RcFile);
                }
            }
        }
        else if(pEffect->Effect.EffectType == QString("Text"))
        {
            m_RcType = RCTEXT;
            for(int i=0;i<count;i++)
            {
                tmpRc = pEffect->Effect.RcFiles.at(i);
                index = GetKey(tmpRc.RcName);
                IDE_TRACE_INT(index);
                if(LoadPath(index, tmpRc.RcFile) == false)
                {
                    IDE_TRACE_STR(tmpRc.RcFile);
                }
            }
        }
    }
    else if(tmpString.compare(QString("Change"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Flip"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeFlip;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Roll"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRoll;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Transparent"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeTransparent;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_CHANGE, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Run"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Transparent"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeTransparent;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_RUN, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Halt"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Blink"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeBlink;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Zoom"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeZoom;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_HALT, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool DesignFloor::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    DesignBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
    if(pLabel.compare(PARA_STYLE, Qt::CaseInsensitive) == 0)
    {
        if(pContent.compare(QString("Discrete"), Qt::CaseInsensitive) == 0)
            m_FloorStyle = STYLE_DISCRETE;
        else if(pContent.compare(QString("Integrate"), Qt::CaseInsensitive) == 0)
            m_FloorStyle = STYLE_INTEGRATE;
    }
    return true;
}

bool DesignFloor::Start()
{
    if(DesignBase::Start() == false)
        return false;
    m_FloorRectDiff = size().width() / 10.0;
    m_FloorTextSize = CalcFontSizeFromSize("Arial", QSize(size().width()/3.0, size().height()), PRIOR_WIDTH);

    m_PixmapPointer.mLastPointer.fill(Qt::transparent);
    m_PixmapPointer.mCurPointer.fill(Qt::transparent);
    m_FloorNumInfo = 0xffffffff;

    FLOOR_INFO tmpFloorInfo(qrand()%10,qrand()%10,qrand()%10);
    updateEffect(tmpFloorInfo, OPERATE_NONE);
    return true;
}

QSize DesignFloor::GetRcSize(QString pRcName)
{
    int index = GetKey(pRcName);
    if(index == BGIMG)
        return size().toSize();
    else
        return QSize(m_FloorRectDiff*4, size().height());
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

//>@从图片标号转换为字母
quint8 DesignFloor::GetFloorRcText(quint8 pFloorRcIndex)
{
    if(pFloorRcIndex>=0 && pFloorRcIndex<=9)
    {
        return pFloorRcIndex+'0';
    }
    if(pFloorRcIndex>=10 && pFloorRcIndex<=35)
    {
        return pFloorRcIndex+'A';
    }
    return -1;  //>@表示没有图片资源，隐藏此区域
}

//>@从字母转换为图片标号
int DesignFloor::GetFloorRcIndex(quint8 pFloorText)
{
    if(pFloorText>='0' && pFloorText<='9')
    {
        return pFloorText-'0';
    }
    if(pFloorText>='A' && pFloorText<='Z')
    {
        return pFloorText+10-'A';
    }
    return -1;  //>@表示没有图片资源，隐藏此区域
}

QPixmap DesignFloor::GenerateFloorImage(FLOOR_INFO pFloorInfo)
{
    //>@准备资源
    QRect tmpRect = rect().toRect();
    QSize tmpSize = size().toSize();
    QImage tmpImage(tmpSize.width(), tmpSize.height(), QImage::Format_ARGB32_Premultiplied);
    if(tmpImage.isNull())
        return false;
    tmpImage.fill(Qt::transparent); //>@创建透明图层
    QPainter painter(&tmpImage);
    DrawPixmap(&painter, GetPixmap(BGIMG));
    //>@绘制文字
    int tmpHundredRcIndex = pFloorInfo.mHundredBits;
    int tmpTenRcIndex = pFloorInfo.mTenBits;
    int tmpSingleRcIndex = pFloorInfo.mSingleBits;
    if(tmpHundredRcIndex != -1)//>@表示有百位，百位为0123区域，十位为3456区域，个位为6789区域
    {
        if(m_RcType == RCIMAGE)
        {
            m_FloorNumRect[Hundred] = QRect(tmpRect.left(), tmpRect.top(), m_FloorRectDiff*4, tmpRect.height());
            m_FloorNumRect[Ten] = QRect(tmpRect.left()+m_FloorRectDiff*3, tmpRect.top(), m_FloorRectDiff*4, tmpRect.height());
            m_FloorNumRect[Single] = QRect(tmpRect.left()+m_FloorRectDiff*6, tmpRect.top(), m_FloorRectDiff*4, tmpRect.height());
            DrawPixmap(&painter, GetPixmap(tmpHundredRcIndex), m_FloorNumRect[Hundred]);
            DrawPixmap(&painter, GetPixmap(tmpTenRcIndex), m_FloorNumRect[Ten]);
            DrawPixmap(&painter, GetPixmap(tmpSingleRcIndex), m_FloorNumRect[Single]);
        }
        else
        {
            //WriteText(&painter, pFloorText, rect(), m_FloorTextSize);
        }
    }
    else if(tmpTenRcIndex != -1)//>@表示没有百位有十位，十位为2345区域，个位为4567区域
    {

        if(m_RcType == RCIMAGE)
        {
            m_FloorNumRect[Ten] = QRect(tmpRect.left()+m_FloorRectDiff*2, tmpRect.top(), m_FloorRectDiff*4, tmpSize.height());
            m_FloorNumRect[Single] = QRect(tmpRect.left()+m_FloorRectDiff*4, tmpRect.top(), m_FloorRectDiff*4, tmpSize.height());
            DrawPixmap(&painter, GetPixmap(tmpTenRcIndex), m_FloorNumRect[Ten]);
            DrawPixmap(&painter, GetPixmap(tmpSingleRcIndex), m_FloorNumRect[Single]);
        }
        else
        {
            //WriteText(&painter, pFloorText, QRect(tmpRect.left()+m_FloorRectDiff*2, tmpRect.top(), m_FloorRectDiff*8, tmpSize.height()), m_FloorTextSize);
        }
    }
    else if(tmpSingleRcIndex != -1)           //>@3456
    {
        if(m_RcType == RCIMAGE)
        {
            m_FloorNumRect[Single] = QRect(tmpRect.left()+m_FloorRectDiff*3, tmpRect.top(), m_FloorRectDiff*4, tmpSize.height());
            DrawPixmap(&painter, GetPixmap(tmpSingleRcIndex), m_FloorNumRect[Single]);
        }
        else
        {
            //WriteText(&painter, pFloorText, QRect(tmpRect.left()+m_FloorRectDiff*3, tmpRect.top(), m_FloorRectDiff*4, tmpSize.height()), m_FloorTextSize);
        }
    }
    painter.end();
    return QPixmap::fromImage(tmpImage);
}

bool DesignFloor::PaintEffect(QPainter *p)
{
    if((DesignBase::PaintEffect(p) == false)||(m_AreaAnimate == 0))
        return false;
    if(m_FloorStyle == STYLE_INTEGRATE)
    {
        switch(m_AreaAnimate->mEffectType)
        {
            case EffectTypeRoll:
                Paint2D_Roll(p, rect());
                break;
            case EffectTypeRotate:
                Paint2D_Rotate(p, rect());
                break;
            case EffectTypeFlip:
                Paint2D_Flip(p, rect());
                break;
            case EffectTypeZoom:
                Paint2D_Zoom(p, rect());
                break;
            case EffectTypeTransparent:
                Paint2D_Transparent(p, rect());
                break;
            case EffectTypeBlink:
                Paint2D_Blink(p, rect());
                break;
            default:
                Paint2D_None(p, rect());
                break;
        }
    }
    else if(m_FloorStyle == STYLE_DISCRETE)
    {

    }
    return true;
}

void DesignFloor::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_CHANGE:
        {
            QString tmpString = pPara.toString();
            //>@分解坐标，如果点击上半区域，则向上滚动
            if(tmpString.compare("Number") == 0)
            {
                FLOOR_INFO tmpFloorInfo(qrand()%10, qrand()%10, qrand()%10);
                updateEffect(tmpFloorInfo, OPERATE_NONE);
            }
            else if(tmpString.compare("Number Run") == 0)
            {
                FLOOR_INFO tmpFloorInfo(qrand()%10, qrand()%10, qrand()%10);
                updateEffect(tmpFloorInfo, OPERATE_RUN);
            }
            else if(tmpString.compare("Number Halt") == 0)
            {
                FLOOR_INFO tmpFloorInfo(qrand()%10, qrand()%10, qrand()%10);
                updateEffect(tmpFloorInfo, OPERATE_HALT);
            }
            else if(tmpString.compare("Letter") == 0)
            {
                FLOOR_INFO tmpFloorInfo(qrand()%26+10, qrand()%26+10, qrand()%26+10);
                updateEffect(tmpFloorInfo, OPERATE_NONE);
            }
            else if(tmpString.compare("Letter Run") == 0)
            {
                FLOOR_INFO tmpFloorInfo(qrand()%26+10, qrand()%26+10, qrand()%26+10);
                updateEffect(tmpFloorInfo, OPERATE_RUN);
            }
            else if(tmpString.compare("Letter Halt") == 0)
            {
                FLOOR_INFO tmpFloorInfo(qrand()%26+10, qrand()%26+10, qrand()%26+10);
                updateEffect(tmpFloorInfo, OPERATE_HALT);
            }
            else if(tmpString.compare("Mixed") == 0)
            {
                FLOOR_INFO tmpFloorInfo(qrand()%36, qrand()%36, qrand()%36);
                updateEffect(tmpFloorInfo, OPERATE_NONE);
            }
            else if(tmpString.compare("Mixed Run") == 0)
            {
                FLOOR_INFO tmpFloorInfo(qrand()%36, qrand()%36, qrand()%36);
                updateEffect(tmpFloorInfo, OPERATE_RUN);
            }
            else if(tmpString.compare("Mixed Halt") == 0)
            {
                FLOOR_INFO tmpFloorInfo(qrand()%36, qrand()%36, qrand()%36);
                updateEffect(tmpFloorInfo, OPERATE_HALT);
            }
            break;
        }
        default:
            break;
    }
}

void DesignFloor::updateEffect(FLOOR_INFO pFloorInfo, AREA_OPERATE pOperate)
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
        if(m_FloorStyle == STYLE_INTEGRATE)
        {
            m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer;
            m_PixmapPointer.mCurPointer = GenerateFloorImage(pFloorInfo);
        }
        else
        {

        }
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = pOperate;

    m_FloorNumInfo = pFloorInfo;
    OperateStep0();
}
