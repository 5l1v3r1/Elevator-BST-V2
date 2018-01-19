#include "effectfloor.h"

EffectFloor::EffectFloor(QWidget *parent, QRect pRect) :
    EffectBase(parent, pRect)
{
    m_FloorStyle = STYLE_INTEGRATE;
}

bool EffectFloor::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        m_FloorRectDiff = width() / 10.0;
        //IDE_TRACE_INT(m_FloorRectDiff);
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
                if(index == BGIMG)
                {
                    LoadPixmap(index, tmpRc.RcFile);
                }
                else
                {
                    LoadPixmap(index, tmpRc.RcFile, QSize(m_FloorRectDiff*4, height()));
                }
            }
        }
        else if(pEffect->Effect.EffectType == QString("Text"))
        {
            m_RcType = RCTEXT;
            m_FloorTextSize = CalcFontSizeFromSize("Arial", QSize(width()/3.0, height()), PRIOR_WIDTH);
            for(int i=0;i<count;i++)
            {
                tmpRc = pEffect->Effect.RcFiles.at(i);
                index = GetKey(tmpRc.RcName);
                //IDE_TRACE_INT(index);
                if(LoadPixmap(index, tmpRc.RcFile) == false)
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

bool EffectFloor::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    EffectBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
    if(pLabel.compare(PARA_STYLE, Qt::CaseInsensitive) == 0)
    {
        if(pContent.compare(QString("Discrete"), Qt::CaseInsensitive) == 0)
            m_FloorStyle = STYLE_DISCRETE;
        else if(pContent.compare(QString("Integrate"), Qt::CaseInsensitive) == 0)
            m_FloorStyle = STYLE_INTEGRATE;
    }
    return true;
}

bool EffectFloor::Start()
{
    if(EffectBase::Start() == false)
        return false;
    m_PixmapPointer.mLastPointer.fill(Qt::transparent);
    m_PixmapPointer.mCurPointer.fill(Qt::transparent);
    m_FloorNumInfo = 0xffffffff;
    updateEffect(m_FloorNumInfo, OPERATE_NONE);  //>@隐藏楼层
    return true;
}

//>@从图片标号转换为字母
quint8 EffectFloor::GetFloorRcText(quint8 pFloorRcIndex)
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
int EffectFloor::GetFloorRcIndex(quint8 pFloorText)
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

QPixmap EffectFloor::GenerateFloorImage(FLOOR_INFO pFloorInfo)
{
    //>@准备资源
    QRect tmpRect = rect();
    QSize tmpSize = size();
    QImage tmpImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
    if(tmpImage.isNull())
        return false;
    tmpImage.fill(Qt::transparent); //>@创建透明图层
    QPainter painter(&tmpImage);
    DrawPixmap(painter, m_EffectImage.value(BGIMG));
    //>@绘制文字
    int tmpHundredRcIndex = pFloorInfo.mHundredBits;
    int tmpTenRcIndex = pFloorInfo.mTenBits;
    int tmpSingleRcIndex = pFloorInfo.mSingleBits;
    //IDE_TRACE_INT(tmpHundredRcIndex);
    //IDE_TRACE_INT(tmpTenRcIndex);
    //IDE_TRACE_INT(tmpSingleRcIndex);
    if(tmpHundredRcIndex != -1)//>@表示有百位，百位为0123区域，十位为3456区域，个位为6789区域
    {
        if(m_RcType == RCIMAGE)
        {
            m_FloorNumRect[Hundred] = QRect(tmpRect.left(), tmpRect.top(), m_FloorRectDiff*4, tmpRect.height());
            m_FloorNumRect[Ten] = QRect(tmpRect.left()+m_FloorRectDiff*3, tmpRect.top(), m_FloorRectDiff*4, tmpRect.height());
            m_FloorNumRect[Single] = QRect(tmpRect.left()+m_FloorRectDiff*6, tmpRect.top(), m_FloorRectDiff*4, tmpRect.height());
            DrawPixmap(painter, m_EffectImage.value(tmpHundredRcIndex), m_FloorNumRect[Hundred]);
            DrawPixmap(painter, m_EffectImage.value(tmpTenRcIndex), m_FloorNumRect[Ten]);
            DrawPixmap(painter, m_EffectImage.value(tmpSingleRcIndex), m_FloorNumRect[Single]);
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
            //IDE_TRACE();
            m_FloorNumRect[Ten] = QRect(tmpRect.left()+m_FloorRectDiff*2, tmpRect.top(), m_FloorRectDiff*4, tmpSize.height());
            m_FloorNumRect[Single] = QRect(tmpRect.left()+m_FloorRectDiff*4, tmpRect.top(), m_FloorRectDiff*4, tmpSize.height());
            DrawPixmap(painter, m_EffectImage.value(tmpTenRcIndex), m_FloorNumRect[Ten]);
            DrawPixmap(painter, m_EffectImage.value(tmpSingleRcIndex), m_FloorNumRect[Single]);
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
            DrawPixmap(painter, m_EffectImage.value(tmpSingleRcIndex), m_FloorNumRect[Single]);
        }
        else
        {
            //WriteText(&painter, pFloorText, QRect(tmpRect.left()+m_FloorRectDiff*3, tmpRect.top(), m_FloorRectDiff*4, tmpSize.height()), m_FloorTextSize);
        }
    }
    painter.end();
    return QPixmap::fromImage(tmpImage);
}

bool EffectFloor::PaintEffect(QPainter &p)
{
    if(EffectBase::PaintEffect(p) == false)
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

void EffectFloor::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    static AREA_OPERATE sOPerate = OPERATE_NONE;
    static QVariant sPara = QVariant();
    if(sOPerate != pOperate && pOperate != OPERATE_PLAY && pOperate != OPERATE_STOP)
    {
        sOPerate = pOperate;
        sPara = pPara;
    }
    switch(pOperate)
    {
        case OPERATE_PLAY:
        {
            updateEffect(sOPerate, sPara);
            break;
        }
        case OPERATE_STOP:
        {
            m_Animator.stop();
            break;
        }
        case OPERATE_KEYPRESS:
        {
            //>@分解坐标，如果点击上半区域，则向上滚动
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < height()/3)
            {
                FLOOR_INFO tmpFloorInfo(3,2,1);
                updateEffect(tmpFloorInfo, OPERATE_RUN);
            }
            else if(tmpPoint.y() > height()/3 && tmpPoint.y() < height()*2/3)
            {
                FLOOR_INFO tmpFloorInfo(6,5,4);
                updateEffect(tmpFloorInfo, OPERATE_RUN);
            }
            else if(tmpPoint.y() > height()*2/3)
            {
                FLOOR_INFO tmpFloorInfo(9,8,7);
                updateEffect(tmpFloorInfo, OPERATE_RUN);
            }
            break;
        }
        case OPERATE_CHANGE:
        {
            FLOOR_INFO tmpFloorInfo(pPara.toInt());
            updateEffect(tmpFloorInfo, OPERATE_HALT);
            break;
        }
        default:
            break;
    }
}

void EffectFloor::updateEffect(FLOOR_INFO pFloorInfo, AREA_OPERATE pOperate)
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
