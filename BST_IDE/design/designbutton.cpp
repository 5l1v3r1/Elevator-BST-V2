#include "designbutton.h"

DesignAbstractButton::DesignAbstractButton(QGraphicsItem *parent, QRectF pRect) :
    DesignBase(parent, pRect)
{
}

bool DesignAbstractButton::InitEffect(QHash<int, QString> pPathList, QHash<AREA_OPERATE, AREA_ANIMATE*>  pEffectGroup)
{
    m_EffectValid = false;
    m_EffectPath = pPathList;
    m_EffectGroup = pEffectGroup;
    Start();
    return true;
}

bool DesignAbstractButton::InitEffectRc(STATE_INFO* pEffect)
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
            if(LoadPath(index, tmpRc.RcFile) == false)
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

bool DesignAbstractButton::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return DesignBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool DesignAbstractButton::Start()
{
    if(DesignBase::Start() == false)
        return false;
    m_ButtonState = BUTTON_DARK;
    update();
    return true;
}

bool DesignAbstractButton::PaintEffect(QPainter *p)
{
    if(m_EffectValid == false)
        return false;
    if(m_AreaAnimate == 0)
    {
        //>@首先绘制按钮底图
        DrawPixmap(p, GetPixmap((m_ButtonState == BUTTON_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
        //>@最终绘制文字
        DrawPixmap(p, GetPixmap((m_ButtonState == BUTTON_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
        return false;
    }
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeSpread:
        {
            //>@首先绘制按钮底图
            DrawPixmap(p, GetPixmap((m_ButtonState == BUTTON_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
            //>@其次特效绘图
            m_PixmapPointer.mLastPointer = GetPixmap(PIC_EFFECT);
            m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer;
            Paint2D_Spread(p, rect());
            //>@最终绘制文字
            DrawPixmap(p, GetPixmap((m_ButtonState == BUTTON_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
            break;
        }
        case EffectTypeWheel:
        {
            //>@首先绘制按钮底图
            DrawPixmap(p, GetPixmap((m_ButtonState == BUTTON_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
            //>@其次特效绘图
            m_PixmapPointer.mLastPointer = GetPixmap(PIC_EFFECT);
            m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer;
            Paint2D_Wheel(p, rect());
            //>@最终绘制文字
            DrawPixmap(p, GetPixmap((m_ButtonState == BUTTON_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
            break;
        }
        case EffectTypeRotate:
        {
            QImage tmpImage(size().toSize(), QImage::Format_ARGB32_Premultiplied);
            if(tmpImage.isNull())
                return false;
            tmpImage.fill(Qt::transparent);
            QPainter painter(&tmpImage);
            //>@首先绘制按钮底图
            DrawPixmap(p, GetPixmap((m_ButtonState == BUTTON_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
            //>@绘制文字
            DrawPixmap(p, GetPixmap((m_ButtonState == BUTTON_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
            painter.end();
            //>@最终特效绘图
            m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer = QPixmap::fromImage(tmpImage);
            Paint2D_Rotate(p, rect());
            break;
        }
        case EffectTypeSlipCycle:
        {
            //>@首先绘制按钮底图
            DrawPixmap(p, GetPixmap((m_ButtonState == BUTTON_DARK)?(PIC_DARK):(PIC_LIGHT)), rect());
            //>@其次特效绘图
            m_PixmapPointer.mLastPointer = GetPixmap(PIC_EFFECT);
            m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer;
            Paint2D_Slipcycle(p, rect());
            //>@最终绘制文字
            DrawPixmap(p, GetPixmap((m_ButtonState == BUTTON_DARK)?(TEXT_DARK):(TEXT_LIGHT)), rect());
            break;
        }
        default:
            Paint2D_None(p, rect());
            break;
    }
    return true;
}

void DesignAbstractButton::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
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

void DesignAbstractButton::updateEffect(AREA_OPERATE pOperate)
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

DesignButton::DesignButton(QGraphicsItem *parent, QRectF pRect) :
    DesignAbstractButton(parent, pRect)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("None"));
    m_ActionGroup->addAction(tr("Press"));
    m_ActionGroup->addAction(tr("Release"));
    tmpDefault->setChecked(true);
}

QList<QAction*> DesignButton::GetActionList()
{
    QList<QAction*> tmpList;
    if(m_ActionGroup)
    {
        tmpList = m_ActionGroup->actions();
        //>@根据当前已有的资源使能响应功能Action
        for(int i=0;i<tmpList.count();i++)
        {
            QAction *tmpAction = tmpList.at(i);
            if(tmpAction == 0)
                continue;
            tmpAction->setEnabled(true);
        }
    }
    return tmpList;
}

void DesignButton::ExecAction(QAction *pAction)
{
    if(pAction->text().compare("Press")==0)
        updateEffect(OPERATE_KEYPRESS);
    else if(pAction->text().compare("Release")==0)
        updateEffect(OPERATE_KEYRELEASE);
}

//#################################################################################
//#################################################################################


DesignFloorButton::DesignFloorButton(QGraphicsItem *parent, QRectF pRect) :
    DesignBase(parent, pRect)
{
    mRow = mColunm = mCount = 0;
    mWidth = mHeight = mTextWidth = mTextHeight = 0;
    mRSpacer = mCSpacer = mItemLSpace = mItemRSpace = 0;
}

bool DesignFloorButton::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        if(pEffect->Effect.EffectType.compare(QString("Image"), Qt::CaseInsensitive) == 0)
        {
            m_RcType = RCIMAGE;
            analysisEffectPara((AREA_ANIMATE *)0, pEffect->Effect.EffectPara);
            RESOURCE_INFO tmpRc;
            int index;
            int count = pEffect->Effect.RcFiles.count();
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
            mRcPath = getFileDirectory(m_EffectPath.values().first());
            if(mRcPath.endsWith('/') == false)
                mRcPath.append('/');
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

bool DesignFloorButton::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    DesignBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
    if(pLabel.compare(PARA_PATTERN, Qt::CaseInsensitive) == 0)
    {
        if(pContent == QString("Grid"))
            m_Pattern = PTN_FB_GRID;
        else if(pContent == QString("Ellipse"))
            m_Pattern = PTN_FB_ELLIPSE;
        else if(pContent == QString("Guide"))
            m_Pattern = PTN_FB_GUIDE;
    }
    else if(pLabel.compare(PARA_RSPACE, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            mRSpacer = dec;
    }
    else if(pLabel.compare(PARA_CSPACE, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            mCSpacer = dec;
    }
    else if(pLabel.compare(PARA_AMOUNT, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            mCount = dec;
    }
    else if(pLabel.compare(PARA_ROW, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            mRow = dec;
    }
    else if(pLabel.compare(PARA_COLUMN, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            mColunm = dec;
    }
    return true;
}

QSize DesignFloorButton::GetRcSize(QString pRcName)
{
    int tmpKey = GetKey(pRcName);
    if(tmpKey >= 0 && tmpKey <= 27)
        return QSize(mTextWidth, mTextHeight);
    return QSize(mWidth-mCSpacer*2, mHeight-mRSpacer*2);
}

void DesignFloorButton::ReleaseFloorBtnInfoList()
{
    for(int i=m_FloorBtnInfoList.count()-1;i>=0;i--)
    {
        FloorBtnInfo *tmpFloorBtnInfo = m_FloorBtnInfoList.takeAt(i);
        if(tmpFloorBtnInfo == 0)
            continue;
        delete tmpFloorBtnInfo;
    }
}

bool DesignFloorButton::AddButton(int i, DesignAbstractButton *&pButton, QRectF pRect)
{
    if(pButton)
        return true;
    pButton = new DesignAbstractButton(this, pRect);
    QString tmpBtnText = QString("%1").arg(i);
    QHash<int, QString> tmpRcPaths;
    tmpRcPaths.insert(PIC_DARK, m_EffectPath.value(PIC_DARK));
    tmpRcPaths.insert(PIC_LIGHT, m_EffectPath.value(PIC_LIGHT));
    tmpRcPaths.insert(PIC_EFFECT, m_EffectPath.value(PIC_EFFECT));
    tmpRcPaths.insert(TEXT_DARK, GetBtnRc(pRect.size(), tmpBtnText, false));
    tmpRcPaths.insert(TEXT_LIGHT, GetBtnRc(pRect.size(), tmpBtnText, true));
    pButton->InitEffect(tmpRcPaths, m_EffectGroup);
    return true;
}

bool DesignFloorButton::InitFloorBtnInfoList()
{
    ReleaseFloorBtnInfoList();
    switch(m_Pattern)
    {
        case PTN_FB_GRID:
        {
            qreal width = size().width();
            qreal height = size().height();
            if(mCount == 0 || mRow == 0 || mColunm == 0)
                return false;
            mWidth = width/(qreal)mColunm;
            mHeight = height/(qreal)mRow;
            mTextHeight = mHeight * 3.0 / 4.0;
            mTextWidth = mTextHeight * 3.0 / 5.0;
            int row = -1, column = -1;
            for(int i=0;i<mCount;i++)
            {
                if(i%mColunm == 0)
                    row += 1;
                column = i - row*mColunm;
                FloorBtnInfo *tmpFloorBtnInfo = new FloorBtnInfo;
                tmpFloorBtnInfo->mRect = QRect(column*mWidth+mCSpacer,
                                               row*mHeight+mRSpacer,
                                               mWidth-2*(mCSpacer),
                                               mHeight-2*(mRSpacer));
                AddButton(i, tmpFloorBtnInfo->mButtonItem, tmpFloorBtnInfo->mRect);
                m_FloorBtnInfoList.append(tmpFloorBtnInfo);
            }
            break;
        }
        case PTN_FB_ELLIPSE:
        {
            qreal width = size().width();
            qreal height = size().height();
            if(mCount == 0)
                return false;
            qreal tmpAngelDiff = 2*PI/(qreal)mCount;
            qreal tmpBeginAngle = 0;  //>@从0度开始排列
            qreal tmpTwirlA = width / 3.0;   //>@取椭圆的长半径a为1/3的长度，b为1/3的长度
            qreal tmpTwirlB = height / 3.0;
            int halfcount = (int)((qreal)mCount / 2.0 - 1);
            mWidth = tmpTwirlA/(qreal)halfcount;
            mHeight = tmpTwirlB/(qreal)halfcount;
            mTextHeight = mHeight * 2.0 / 3.0;
            mTextWidth = mTextHeight * 3.0 / 5.0;
            QPointF tmpCentrePoint = QPointF(width/2.0, height/2.0);
            for(int i=0;i<mCount;i++)
            {
                FloorBtnInfo *tmpFloorBtnInfo = new FloorBtnInfo;
                qreal tmpAngle = RegularAngle(tmpBeginAngle + tmpAngelDiff * i);
                QPointF tmpItemCentrePoint = tmpCentrePoint + QPointF(tmpTwirlA*qCos(tmpAngle), -tmpTwirlB*qSin(tmpAngle));
                tmpFloorBtnInfo->mRect = QRect(tmpItemCentrePoint.x() - mWidth/2.0 + mCSpacer,
                                               tmpItemCentrePoint.y() - mHeight/2.0 + mRSpacer,
                                               mWidth - 2*(mCSpacer),
                                               mHeight - 2*(mRSpacer));
                AddButton(i, tmpFloorBtnInfo->mButtonItem, tmpFloorBtnInfo->mRect);
                m_FloorBtnInfoList.append(tmpFloorBtnInfo);
            }
            break;
        }
        case PTN_FB_GUIDE:
        {
            break;
        }
        default:
            return false;
    }
    return true;
}

int DesignFloorButton::PrepareBtnChar(char pAscii, bool pLight)
{
    if(pAscii >= '0' && pAscii <= '9')
        return pLight ? (pAscii-'0'+14) : (pAscii-'0');
    else if(pAscii >= 'A' && pAscii <= 'D')
        return pLight ? (pAscii-'A'+24) : (pAscii-'A'+10);
    return -1;
}

QString DesignFloorButton::GetBtnRc(QSizeF pSize, QString pBtnText, bool pLight)
{
    //>@准备资源
    QImage tmpImage(pSize.width(), pSize.height(), QImage::Format_ARGB32_Premultiplied);
    if(tmpImage.isNull())
        return QString();
    tmpImage.fill(Qt::transparent); //>@创建透明图层
    QPainter painter(&tmpImage);
    painter.translate(pSize.width()/2.0, pSize.height()/2.0);
    //>@绘制文字
    if(pBtnText.count() == 3)
    {
        DrawPixmap(&painter,
                   GetPixmap(PrepareBtnChar(pBtnText.at(0).toAscii(), pLight), QSizeF(mTextWidth,mTextHeight)),
                   QRectF(-mTextWidth*3.0/2.0, -mTextHeight/2.0, mTextWidth, mTextHeight));
        DrawPixmap(&painter,
                   GetPixmap(PrepareBtnChar(pBtnText.at(1).toAscii(), pLight), QSizeF(mTextWidth,mTextHeight)),
                   QRectF(-mTextWidth/2.0, -mTextHeight/2.0, mTextWidth, mTextHeight));
        DrawPixmap(&painter,
                   GetPixmap(PrepareBtnChar(pBtnText.at(2).toAscii(), pLight), QSizeF(mTextWidth,mTextHeight)),
                   QRectF(mTextWidth/2.0, -mTextHeight/2.0, mTextWidth, mTextHeight));
    }
    else if(pBtnText.count() == 2)//>@表示没有百位有十位
    {
        DrawPixmap(&painter,
                   GetPixmap(PrepareBtnChar(pBtnText.at(0).toAscii(), pLight), QSizeF(mTextWidth,mTextHeight)),
                   QRectF(-mTextWidth, -mTextHeight/2.0, mTextWidth, mTextHeight));
        DrawPixmap(&painter,
                   GetPixmap(PrepareBtnChar(pBtnText.at(1).toAscii(), pLight), QSizeF(mTextWidth,mTextHeight)),
                   QRectF(0, -mTextHeight/2.0, mTextWidth, mTextHeight));
    }
    else if(pBtnText.count() == 1)   //>@3456
    {
        DrawPixmap(&painter,
                   GetPixmap(PrepareBtnChar(pBtnText.at(0).toAscii(), pLight), QSizeF(mTextWidth,mTextHeight)),
                   QRectF(-mTextWidth/2.0, -mTextHeight/2.0, mTextWidth, mTextHeight));
    }
    painter.end();
    QString tmpRcPath;
    if(pLight)
        tmpRcPath = mRcPath + pBtnText + QString("-L.png");
    else
        tmpRcPath = mRcPath + pBtnText + QString("-D.png");
    tmpImage.save(tmpRcPath);
    return tmpRcPath;
}

bool DesignFloorButton::Start()
{
    if(DesignBase::Start() == false)
        return false;
    InitFloorBtnInfoList();
    return true;
}

//#################################################################################
//#################################################################################


DesignKeyboardButton::DesignKeyboardButton(QGraphicsItem *parent, QRectF pRect) :
    DesignBase(parent, pRect)
{
}

bool DesignKeyboardButton::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        if(pEffect->Effect.EffectType.compare(QString("Image"), Qt::CaseInsensitive) == 0)
        {
            m_RcType = RCIMAGE;
            analysisEffectPara((AREA_ANIMATE *)0, pEffect->Effect.EffectPara);
            RESOURCE_INFO tmpRc;
            int index;
            int count = pEffect->Effect.RcFiles.count();
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

bool DesignKeyboardButton::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    DesignBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
    if(pLabel.compare(PARA_PATTERN, Qt::CaseInsensitive) == 0)
    {
        if(pContent == QString("GRID_LQQ"))
            m_Pattern = PTN_KB_GRID_LQQ;
        else if(pContent == QString("GRID_HZF"))
            m_Pattern = PTN_KB_GRID_HZF;
    }
    return true;
}

void DesignKeyboardButton::ReleaseKeyboardBtnInfoList()
{
    for(int i=m_KeyboardBtnInfoList.count()-1;i>=0;i--)
    {
        KeyboardBtnInfo *tmpKeyboardBtnInfo = m_KeyboardBtnInfoList.takeAt(i);
        if(tmpKeyboardBtnInfo == 0)
            continue;
        delete tmpKeyboardBtnInfo;
    }
}

bool DesignKeyboardButton::AddButton(KEBOARDBUTTON_RC pType, DesignAbstractButton *&pButton, QRectF pRect)
{
    if(pButton)
        return true;
    pButton = new DesignAbstractButton(this, pRect);
    QHash<int, QString> tmpRcPaths;
    tmpRcPaths.insert(PIC_DARK, m_EffectPath.value(PIC_DARK));
    tmpRcPaths.insert(PIC_LIGHT, m_EffectPath.value(PIC_LIGHT));
    tmpRcPaths.insert(PIC_EFFECT, m_EffectPath.value(PIC_EFFECT));
    tmpRcPaths.insert(TEXT_DARK, m_EffectPath.value(pType));
    tmpRcPaths.insert(TEXT_LIGHT, m_EffectPath.value(pType+50));
    pButton->InitEffect(tmpRcPaths, m_EffectGroup);
    return true;
}

bool DesignKeyboardButton::InitKeyboardBtnInfoList()
{
    ReleaseKeyboardBtnInfoList();
    switch(m_Pattern)
    {
        case PTN_KB_GRID_LQQ:
        {
            QGraphicsGridLayout *tmpLayout = new QGraphicsGridLayout;
            KeyboardBtnInfo *tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_NUM7, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 0, 0, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_NUM8, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 0, 1, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_NUM9, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 0, 2, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_NUM4, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 1, 0, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_NUM5, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 1, 1, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_NUM6, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 1, 2, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_NUM1, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 2, 0, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_NUM2, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 2, 1, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_NUM3, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 2, 2, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_STAR, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 3, 0, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_NUM0, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 3, 1, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_MINUS, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 3, 2, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_ESC, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 0, 3, 1, 1);

            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_ENTER, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 1, 3, 1, 2);

            QGraphicsGridLayout *tmpLetterLayout = new QGraphicsGridLayout;
            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_LETTERA, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLetterLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 0, 0, 1, 1);
            //>@
            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_LETTERB, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLetterLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 0, 1, 1, 1);
            //>@
            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_LETTERC, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLetterLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 0, 2, 1, 1);
            //>@
            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_LETTERD, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLetterLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 1, 0, 1, 1);
            //>@
            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_LETTERG, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLetterLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 1, 1, 1, 1);
            //>@
            tmpKeyboardBtnInfo = new KeyboardBtnInfo;
            AddButton(PIC_KB_LETTERH, tmpKeyboardBtnInfo->mButtonItem, QRectF(0,0,100,100));
            tmpLetterLayout->addItem(tmpKeyboardBtnInfo->mButtonItem, 1, 2, 1, 1);

            tmpLayout->addItem(tmpLetterLayout, 3, 3, 1, 1);

            setLayout(tmpLayout);
            break;
        }
        case PTN_KB_GRID_HZF:
        {

            break;
        }
        default:
            return false;
    }
    return true;
}

bool DesignKeyboardButton::Start()
{
    if(DesignBase::Start() == false)
        return false;
    InitKeyboardBtnInfoList();
    return true;
}

