#include "effectbutton.h"

EffectButton::EffectButton(QWidget *parent, QRect pRect) :
    EffectBase(parent, pRect)
{
    m_ItemWSpacer = 0;
    m_ItemHSpacer = 0;
}

bool EffectButton::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    else if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        RESOURCE_INFO tmpRc;
        int index;
        int count = pEffect->Effect.RcFiles.count();
        for(int i=0;i<count;i++)
        {
            tmpRc = pEffect->Effect.RcFiles.at(i);
            index = GetKey(tmpRc.RcName);
            IDE_TRACE_INT(index);
            if(LoadPixmap(index, tmpRc.RcFile) == false)
            {
                IDE_TRACE_STR(tmpRc.RcFile);
            }
        }
    }
    else if(tmpString.compare(QString("Press"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Spread"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSpread;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Zoom"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeZoom;
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
        m_EffectGroup.insert(OPERATE_KEYPRESS, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Release"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Spread"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSpread;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Zoom"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeZoom;
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
        m_EffectGroup.insert(OPERATE_KEYRELEASE, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool EffectButton::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return EffectBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool EffectButton::Start()
{
    if(EffectBase::Start() == false)
        return false;
    m_ButtonState = BUTTON_DARK;
    update();
    return true;
}

bool EffectButton::PaintEffect(QPainter &p)
{
    if(m_EffectValid == false)
        return false;
    if(m_AreaAnimate == 0)
    {
        //>@首先绘制按钮底图
        DrawPixmap(p, m_EffectImage.value((m_ButtonState == BUTTON_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
        //>@最终绘制文字
        DrawPixmap(p, m_EffectImage.value((m_ButtonState == BUTTON_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
        return false;
    }
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeSpread:
        {
            //>@首先绘制按钮底图
            DrawPixmap(p, m_EffectImage.value((m_ButtonState == BUTTON_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
            //>@其次特效绘图
            m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer = m_EffectImage.value(PIC_EFFECT);
            Paint2D_Spread(p, rect());
            //>@最终绘制文字
            DrawPixmap(p, m_EffectImage.value((m_ButtonState == BUTTON_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
            break;
        }
        case EffectTypeWheel:
        {
            //>@首先绘制按钮底图
            DrawPixmap(p, m_EffectImage.value((m_ButtonState == BUTTON_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
            //>@其次特效绘图
            m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer = m_EffectImage.value(PIC_EFFECT);
            Paint2D_Wheel(p, rect());
            //>@最终绘制文字
            DrawPixmap(p, m_EffectImage.value((m_ButtonState == BUTTON_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
            break;
        }
        case EffectTypeRotate:
        {
            QImage tmpImage(size(), QImage::Format_ARGB32_Premultiplied);
            if(tmpImage.isNull())
                return false;
            tmpImage.fill(Qt::transparent);
            QPainter painter(&tmpImage);
            //>@首先绘制按钮底图
            DrawPixmap(painter, m_EffectImage.value((m_ButtonState == BUTTON_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
            //>@绘制文字
            DrawPixmap(painter, m_EffectImage.value((m_ButtonState == BUTTON_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
            painter.end();
            //>@最终特效绘图
            m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer = QPixmap::fromImage(tmpImage);
            Paint2D_Rotate(p, rect());
            break;
        }
        case EffectTypeSlipCycle:
        {
            //>@首先绘制按钮底图
            DrawPixmap(p, m_EffectImage.value((m_ButtonState == BUTTON_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
            //>@其次特效绘图
            m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer = m_EffectImage.value(PIC_EFFECT);
            Paint2D_Slipcycle(p, rect());
            //>@最终绘制文字
            DrawPixmap(p, m_EffectImage.value((m_ButtonState == BUTTON_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
            break;
        }
        default:
            Paint2D_None(p, rect());
            break;
    }
    return true;
}

void EffectButton::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            updateEffect(OPERATE_KEYPRESS);
            break;
        }
        case OPERATE_KEYRELEASE:
        {
            updateEffect(OPERATE_KEYRELEASE);
            break;
        }
        default:
            break;
    }
}

void EffectButton::updateEffect(AREA_OPERATE pOperate)
{
    if(pOperate == OPERATE_KEYPRESS)
    {
        if(m_ButtonState == BUTTON_LIGHT)
        {
            updateEffect(OPERATE_KEYRELEASE);
            return;
        }
        m_ButtonState = BUTTON_LIGHT;
    }
    else if(pOperate == OPERATE_KEYRELEASE)
    {
        if(m_ButtonState == BUTTON_DARK)
            return;
        if(m_Animator.state() == QTimeLine::Running)
            return;
        m_ButtonState = BUTTON_DARK;
    }
    else
        return;

    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    m_OperateInfo[STEP0].mValid = true;
    m_OperateInfo[STEP0].mOperate = pOperate;
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = OPERATE_NONE;
    //>@执行STEP0中的内容
    OperateStep0();
}

//#################################################################################
//#################################################################################


EffectFloorButton::EffectFloorButton(QWidget *parent, QRect pRect) :
    EffectBase(parent, pRect)
{
    m_ItemWSpacer = 0;
    m_ItemHSpacer = 0;
    m_ItemWidth = 0;
    m_ItemHeight = 0;

    m_ButtonNum = 0;
    m_RowNum = 0;
    m_ColunmNum = 0;
}

bool EffectFloorButton::InitEffectPara(COM_INFO *pComInfo)
{
    if(pComInfo == 0)
        return false;
    for(int i=0;i<pComInfo->StateMachine.count();i++)
    {
        STATE_INFO* tmp = pComInfo->StateMachine.at(i);
        if(tmp == 0)
            continue;
        //analysisEffectPara(tmp->Effect.EffectPara);
    }
    qreal width = this->width();
    qreal height = this->height();
    if(m_ColunmNum == 0 || m_RowNum == 0)
        return false;
    m_ItemWidth = (int)(width/(qreal)m_ColunmNum);
    m_ItemHeight = (int)(height/(qreal)m_RowNum);
    if(m_ItemWidth == 0 || m_ItemHeight == 0)
        return false;
    return true;
}

bool EffectFloorButton::InitEffectPara(COM_STRUCT *pComInfo)
{
    if(pComInfo == 0)
        return false;
    for(int i=0;i<pComInfo->StateMachine.count();i++)
    {
        STATE_INFO* tmp = pComInfo->StateMachine.at(i);
        if(tmp == 0)
            continue;
        //analysisEffectPara(tmp->Effect.EffectPara);
    }
    qreal width = this->width();
    qreal height = this->height();
    if(m_ColunmNum == 0 || m_RowNum == 0)
        return false;
    m_ItemWidth = (int)(width/(qreal)m_ColunmNum);
    m_ItemHeight = (int)(height/(qreal)m_RowNum);
    if(m_ItemWidth == 0 || m_ItemHeight == 0)
        return false;
    return true;
}

bool EffectFloorButton::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        if(pEffect->Effect.EffectType == QString("Image"))
        {
            m_RcType = RCIMAGE;
            RESOURCE_INFO tmpRc;
            int index;
            int count = pEffect->Effect.RcFiles.count();
            for(int i=0;i<count;i++)
            {
                tmpRc = pEffect->Effect.RcFiles.at(i);
                index = GetKey(tmpRc.RcName);
                IDE_TRACE_INT(index);
                if(LoadPixmap(index, tmpRc.RcFile) == false)
                {
                    IDE_TRACE_STR(tmpRc.RcFile);
                }
            }
        }
        else if(pEffect->Effect.EffectType == QString("Text"))
        {
            m_RcType = RCTEXT;
            int count = pEffect->Effect.RcFiles.count();
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool EffectFloorButton::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    EffectBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
    if(pLabel.compare(PARA_WSPACE) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            m_ItemWSpacer = dec;
    }
    else if(pLabel.compare(PARA_HSPACE) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            m_ItemHSpacer = dec;
    }
    else if(pLabel.compare(PARA_BTNNUM) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            m_ButtonNum = dec;
    }
    else if(pLabel.compare(PARA_LAYOUT) == 0)
    {
        QStringList tmpList = SplitString(pContent, "x");
        if(tmpList.count() != 2)
        {
            IDE_TRACE();
            return false;
        }
        bool ok;
        int dec = tmpList.at(0).toInt(&ok, 10);
        if(ok)
            m_RowNum = dec;
        dec = tmpList.at(1).toInt(&ok, 10);
        if(ok)
            m_ColunmNum = dec;
    }
    return true;
}

bool EffectFloorButton::Start()
{
    if(EffectBase::Start() == false)
        return false;
    //>@添加按钮
    int row = -1, column = -1;
    for(int i=0;i<m_ButtonNum;i++)
    {
        if(i%m_ColunmNum == 0)
            row += 1;
        column = i - row*m_ColunmNum;
        QString tmpBtnNum = QString("%1").arg(i+1);
        AddButtonItem(i, tmpBtnNum, row, column);
        PrepareRc(BUTTON_DARK);
    }
    return true;
}

bool EffectFloorButton::AddButtonItem(int pKeyNum, QString pBtnNum, int pRow, int pColumn)
{
    FloorInfo *tmpFloorInfo = new FloorInfo;
    tmpFloorInfo->mLabel = pBtnNum;
    //>@为每个按钮计算区域位置
    tmpFloorInfo->mRect = QRect(pColumn*m_ItemWidth+m_ItemWSpacer, pRow*m_ItemHeight+m_ItemHSpacer,
                                 m_ItemWidth-2*(m_ItemWSpacer), m_ItemHeight-2*(m_ItemHSpacer));
    //>@计算图片资源
    QRect tmpRect = tmpFloorInfo->mRect;
    QSize tmpSize = tmpFloorInfo->mRect.size();
    qreal WidthDiff = tmpRect.width() / 10.0;
    if(pBtnNum.count() == 3)//>@表示有百位，百位为0123区域，十位为3456区域，个位为6789区域
    {
        tmpFloorInfo->mTextPointer[Hundred] = pBtnNum.at(0).toAscii();//>@百 十 个
        tmpFloorInfo->mTextPointer[Ten] = pBtnNum.at(1).toAscii();
        tmpFloorInfo->mTextPointer[Single] = pBtnNum.at(2).toAscii();
        tmpFloorInfo->mTextRect[Hundred] = QRect(tmpRect.left(), tmpRect.top(),
                                     WidthDiff*4, tmpSize.height());
        tmpFloorInfo->mTextRect[Ten] = QRect(tmpRect.left()+WidthDiff*3, tmpRect.top(),
                                 WidthDiff*4, tmpSize.height());
        tmpFloorInfo->mTextRect[Single] = QRect(tmpRect.left()+WidthDiff*6, tmpRect.top(),
                                    WidthDiff*4, tmpSize.height());
    }
    else if(pBtnNum.count() == 2)//>@表示没有百位有十位，十位为2345区域，个位为4567区域
    {
        tmpFloorInfo->mTextPointer[Hundred] = 0;
        tmpFloorInfo->mTextPointer[Ten] = pBtnNum.at(0).toAscii();
        tmpFloorInfo->mTextPointer[Single] = pBtnNum.at(1).toAscii();
        tmpFloorInfo->mTextRect[Ten] = QRect(tmpRect.left()+WidthDiff*2, tmpRect.top(),
                                     WidthDiff*4, tmpSize.height());
        tmpFloorInfo->mTextRect[Single] = QRect(tmpRect.left()+WidthDiff*4, tmpRect.top(),
                                        WidthDiff*4, tmpSize.height());
    }
    else if(pBtnNum.count() == 1)   //>@3456
    {
        tmpFloorInfo->mTextPointer[Hundred] = 0;
        tmpFloorInfo->mTextPointer[Ten] = 0;
        tmpFloorInfo->mTextPointer[Single] = pBtnNum.at(0).toAscii();
        tmpFloorInfo->mTextRect[Single] = QRect(tmpRect.left()+WidthDiff*3, tmpRect.top(),
                                        WidthDiff*4, tmpSize.height());
    }
    else
    {
        delete tmpFloorInfo;
        return false;
    }
    //>@
    tmpFloorInfo->mCurState = BUTTON_DARK;
    connect(&(tmpFloorInfo->mAnimator), SIGNAL(frameChanged(int)), SLOT(update()));
    connect(&(tmpFloorInfo->mAnimator), SIGNAL(finished()), SLOT(SLOT_EffectFinished()));
    m_FloorInfo.insert(pKeyNum, tmpFloorInfo);
    m_CurFloorInfo = tmpFloorInfo;
    return true;
}

bool EffectFloorButton::PaintEffect(QPainter &p)
{
//    if(EffectBase::PaintEffect(p) == false)
//        return false;
//    QList<int> tmpList = m_FloorInfo.keys();
//    for(int i=0;i<tmpList.count();i++)
//    {
//        FloorInfo* tmpFloor = m_FloorInfo.value(tmpList.at(i));
//        if(tmpFloor == 0)
//            continue;
//        m_CurFloorInfo = tmpFloor;
//        PrepareRc(tmpFloor->mCurState);
//        switch(m_AreaAnimate->mEffectType)
//        {
//            case EffectTypeSpread:
//            {
//                //>@首先绘制框和按钮底图
//                QPixmap tmpPixmap = m_EffectImage.value(tmpFloor->mBtnPointer.mFadePointer);
//                if(tmpPixmap.isNull())  //>@如果没有底图则使用默认的common底图
//                    tmpPixmap = m_EffectImage.value(PIC_DARK);
//                DrawPixmap(p, tmpPixmap, tmpFloor->mRect);
//                //>@其次特效绘图
//                if(tmpFloor->mDelayState == BUTTON_LIGHT)
//                {
//                    PIXMAP_POINTER tmpPixPointer;
//                    tmpPixPointer.mCurPointer = tmpPixPointer.mLastPointer = m_EffectImage.value(tmpFloor->mBtnPointer.mEffectPointer);
//                    Paint2D_Spread(p, tmpPixPointer, tmpFloor->mRect, tmpFloor->mAnimator);
//                }
//                //>@最终绘制文字
//                for(int i=0;i<3;i++)
//                {
//                    if(tmpFloor->mTextPointer[i] == 0)
//                        continue;
//                    if(tmpFloor->mTextRect[i].isValid() == false)
//                        continue;
//                    QPixmap tmpTextPixmap = m_EffectImage.value(tmpFloor->mTextPointer[i]);
//                    DrawPixmap(p, tmpTextPixmap, tmpFloor->mTextRect[i]);
//                }
//                break;
//            }
//            case EffectTypeWheel:
//            {
//                //>@首先绘制框和按钮底图
//                QPixmap tmpPixmap = m_EffectImage.value(tmpFloor->mBtnPointer.mFadePointer);
//                if(tmpPixmap.isNull())  //>@如果没有底图则使用默认的common底图
//                    tmpPixmap = m_EffectImage.value(CommonFade);
//                DrawPixmap(p, tmpPixmap, tmpFloor->mRect);
//                //>@其次特效绘图
//                if(tmpFloor->mDelayState == STATE_BTNPRESS)
//                {
//                    PIXMAP_POINTER tmpPixPointer;
//                    tmpPixPointer.mCurPointer = tmpPixPointer.mLastPointer = m_EffectImage.value(tmpFloor->mBtnPointer.mEffectPointer);
//                    Paint2D_Wheel(p, tmpPixPointer, tmpFloor->mRect, tmpFloor->mAnimator);
//                }
//                //>@最终绘制文字
//                for(int i=0;i<3;i++)
//                {
//                    if(tmpFloor->mTextPointer[i] == 0)
//                        continue;
//                    if(tmpFloor->mTextRect[i].isValid() == false)
//                        continue;
//                    QPixmap tmpTextPixmap = m_EffectImage.value(tmpFloor->mTextPointer[i]);
//                    DrawPixmap(p, tmpTextPixmap, tmpFloor->mTextRect[i]);
//                }
//                break;
//            }
//            case EffectTypeRotate:
//            {
//                QImage tmpImage(tmpFloor->mRect.size(), QImage::Format_ARGB32_Premultiplied);
//                if(tmpImage.isNull())
//                    return;
//                tmpImage.fill(Qt::transparent);
//                QPainter painter(&tmpImage);
//                //>@首先绘制框和按钮底图
//                QPixmap tmpPixmap = m_EffectImage.value(tmpFloor->mBtnPointer.mFadePointer);
//                if(tmpPixmap.isNull())  //>@如果没有底图则使用默认的common底图
//                    tmpPixmap = m_EffectImage.value(CommonFade);
//                DrawPixmap(painter, tmpPixmap, QRect(0,0,tmpFloor->mRect.width(),tmpFloor->mRect.height()));
//                //>@最终绘制文字
//                for(int i=0;i<3;i++)
//                {
//                    if(tmpFloor->mTextPointer[i] == 0)
//                        continue;
//                    if(tmpFloor->mTextRect[i].isValid() == false)
//                        continue;
//                    QPixmap tmpTextPixmap = m_EffectImage.value(tmpFloor->mTextPointer[i]);
//                    DrawPixmap(painter, tmpTextPixmap, QRect(0,0,tmpFloor->mTextRect[i].width(),tmpFloor->mTextRect[i].height()));
//                }
//                painter.end();

//                //>@其次特效绘图
//                PIXMAP_POINTER tmpPixPointer;
//                tmpPixPointer.mCurPointer = tmpPixPointer.mLastPointer = QPixmap::fromImage(tmpImage);

//                if(tmpFloor->mDelayState == STATE_BTNPRESS)
//                {
//                    Paint2D_Rotate(p, tmpPixPointer, tmpFloor->mRect, tmpFloor->mAnimator);
//                }
//                else
//                    Paint2D_None(p, tmpPixPointer, tmpFloor->mRect);
//                break;
//            }
//            case EffectTypeSlipCycle:
//            {
//                //>@首先绘制框和按钮底图
//                QPixmap tmpPixmap = m_EffectImage.value(tmpFloor->mBtnPointer.mFadePointer);
//                if(tmpPixmap.isNull())  //>@如果没有底图则使用默认的common底图
//                    tmpPixmap = m_EffectImage.value(CommonFade);
//                DrawPixmap(p, tmpPixmap, tmpFloor->mRect);
//                //>@其次特效绘图
//                if(tmpFloor->mDelayState == STATE_BTNPRESS)
//                {
//                    PIXMAP_POINTER tmpPixPointer;
//                    tmpPixPointer.mCurPointer = tmpPixPointer.mLastPointer = m_EffectImage.value(tmpFloor->mBtnPointer.mEffectPointer);
//                    Paint2D_Slipcycle(p, tmpPixPointer, tmpFloor->mRect, tmpFloor->mAnimator);
//                }
//                //>@最终绘制文字
//                for(int i=0;i<3;i++)
//                {
//                    if(tmpFloor->mTextPointer[i] == 0)
//                        continue;
//                    if(tmpFloor->mTextRect[i].isValid() == false)
//                        continue;
//                    QPixmap tmpTextPixmap = m_EffectImage.value(tmpFloor->mTextPointer[i]);
//                    DrawPixmap(p, tmpTextPixmap, tmpFloor->mTextRect[i]);
//                }
//                break;
//            }
//            default:
//                break;
//        }
//    }
    return true;
}

//>@m_CurKeyNum : 指第几个按钮
void EffectFloorButton::PrepareRc(BUTTON_STATE pState)
{
//    FloorInfo* tmpFloor = m_CurFloorInfo;
//    tmpFloor->mCurState = pState;
//    if(tmpFloor == 0)
//        return false;
//    switch(tmpFloor->mCurState)
//    {
//        case STATE_INIT:
//            tmpFloor->mBtnPointer.mFadePointer = CommonFade;
//            break;
//        case STATE_BTNPRESS:
//            tmpFloor->mBtnPointer.mFadePointer = PressFade;
//            break;
//        case STATE_BTNRELEASE:
//            tmpFloor->mBtnPointer.mFadePointer = ReleaseFade;
//            break;
//        case STATE_BTNLIGHTING:
//            tmpFloor->mBtnPointer.mFadePointer = LightingFade;
//            break;
//        default:
//            tmpFloor->mBtnPointer.mFadePointer = CommonFade;
//            break;
//    }
//    switch(tmpFloor->mDelayState)
//    {
//        case STATE_INIT:
//            break;
//        case STATE_BTNPRESS:
//            tmpFloor->mBtnPointer.mFadePointer = PressFade;
//            tmpFloor->mBtnPointer.mEffectPointer = PressEffect;
//            break;
//        case STATE_BTNRELEASE:
//            break;
//        case STATE_BTNLIGHTING:
//            tmpFloor->mBtnPointer.mFadePointer = LightingFade;
//            break;
//        default:
//            break;
//    }
    return;
}

void EffectFloorButton::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{

}

void EffectFloorButton::updateEffect(AREA_OPERATE pOperate)
{
//    if(m_EffectValid == false)
//        return;
//    switch(pState)
//    {
//        case STATE_BTNPRESS:
//        case STATE_BTNRELEASE:
//        case STATE_BTNLIGHTING:
//            break;
//        case STATE_INIT:
//        default:
//            PrepareRc(STATE_INIT);
//            update();
//            return;
//    }
//    if(pParameter.isEmpty())
//        return;
//    QStringList tmpPosList = SplitString(pParameter, ",");
//    if(tmpPosList.count() != 2)
//        return;
//    //>@判断是点击了哪个按钮
//    int tmpPosX = tmpPosList.at(0).toInt();
//    int tmpPosY = tmpPosList.at(1).toInt();
//    int btnCount = GetBtnFromPos(QPoint(tmpPosX, tmpPosY));
//    if(btnCount < 0)
//        return;
//    m_CurKeyNum = btnCount;
//    //>@获取当前点击的按钮信息
//    m_CurFloorInfo = m_FloorInfo.value(m_CurKeyNum);
//    if(m_CurFloorInfo == 0)
//    {
//        IDE_TRACE();
//        return;
//    }
//    //>@开启特效
//    if(m_CurFloorInfo->mCurState == STATE_INIT)
//        m_CurFloorInfo->mDelayState = pState;
//    PrepareRc(pState);
//    //>@如果当前特效还未完成，则要等待其结束
//    if(m_CurFloorInfo->mAnimator.state() != QTimeLine::Running)
//    {
//        StartTimeline(&(m_CurFloorInfo->mAnimator), 0, m_AnimateCount, m_AnimateTime, m_AnimateType);
//    }
}

//void EffectFloorButton::SLOT_EffectFinished()
//{
//    QList<int> tmpList = m_FloorInfo.keys();
//    for(int i=0;i<tmpList.count();i++)
//    {
//        FloorInfo* tmpFloor = m_FloorInfo.value(tmpList.at(i));
//        if(tmpFloor == 0)
//            return;
//        //>@将m_DelayState设置为m_CurState
//        if(tmpFloor->mAnimator.currentFrame() >= m_AnimateCount)
//        {
//            tmpFloor->mDelayState = tmpFloor->mCurState;
//            switch(m_AreaState)
//            {
//                case STATE_BTNPRESS:
//                    StartTimeline(&(tmpFloor->mAnimator), 0, m_AnimateCount, m_AnimateTime, m_AnimateType);
//                    break;
//                default:
//                    m_CurFloorInfo = tmpFloor;
//                    PrepareRc(STATE_INIT);
//                    update();
//                    break;
//            }
//        }
//    }
//}

