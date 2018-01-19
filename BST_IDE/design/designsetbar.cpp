#include "designsetbar.h"

DesignSetBar::DesignSetBar(QGraphicsItem *parent, QRectF pRect):
    DesignBase(parent, pRect)
{
    m_Diff = 0;
    m_CurValue = 0;

    QAction *tmpDefault = m_ActionGroup->addAction(tr("Left"));
    m_ActionGroup->addAction(tr("Right"));
    tmpDefault->setChecked(true);
}

bool DesignSetBar::InitEffectRc(STATE_INFO* pEffect)
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
    else if(tmpString.compare(QString("Slip"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Twirl"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeTwirl;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Glide"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeGlide;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_SLIP, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Press"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Zoom"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeZoom;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_KEYPRESS, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool DesignSetBar::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return DesignBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool DesignSetBar::PaintEffect(QPainter *p)
{
    QRectF rect = geometry();
    p->drawPixmap(QRect(0,0,rect.width(),rect.height()), GetPixmap(BGIMG));
    return true;
}

bool DesignSetBar::Start()
{
    DesignBase::Start();
    if(InitItems() == false)
    {
        IDE_TRACE();
        return false;
    }
    updateEffect(m_ItemCount/2);
    return true;
}


QList<QAction*> DesignSetBar::GetActionList()
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
            QString tmpString = tmpAction->text();
            int tmpIndex = GetCurItemIndex();
            if(tmpString.compare("Left")==0 && tmpIndex==m_ItemCount-1)
            {
                tmpAction->setDisabled(true);
            }
            else if(tmpString.compare("Right")==0 && tmpIndex==0)
            {
                tmpAction->setDisabled(true);
            }
            else
                tmpAction->setEnabled(true);
        }
    }
    return tmpList;
}

void DesignSetBar::ExecAction(QAction *pAction)
{
    QString tmpString = pAction->text();
    if(tmpString.compare("Left")==0)
    {
        updateEffect(MOVE_LEFT);
    }
    else if(tmpString.compare("Right")==0)
    {
        updateEffect(MOVE_RIGHT);
    }
}


void DesignSetBar::AddItem(int pIndex, ITEM_TYPE pType, QPixmap pPixmap)
{
    SETITEM_INFO *tmpItemInfo = new SETITEM_INFO;
    tmpItemInfo->mProxy = new QGraphicsProxyWidget(this);
    DesignSetBarItem *tmpItemWidget = new DesignSetBarItem(pPixmap);
    tmpItemInfo->mProxy->setWidget(tmpItemWidget);
    tmpItemInfo->mProxy->setOpacity(0);
    tmpItemInfo->mCurParaIndex = pIndex;
    tmpItemInfo->mNextParaIndex = pIndex;
    m_ItemList.append(tmpItemInfo);
}

//>@获取可绘制组件的区域大小
bool DesignSetBar::GetValidRect(QRect &pRect)
{
    QRectF rect = geometry();
    AREA_ANIMATE *tmpAreaAnimate = m_EffectGroup.value(OPERATE_SLIP);
    if(tmpAreaAnimate)
    {
        switch(tmpAreaAnimate->mEffectType)
        {
            case EffectTypeTwirl:
            {
                qreal tmpHalfWidth = rect.width()/2.0;
                qreal tmpMinLen = (tmpHalfWidth>rect.height())?(rect.height()):(tmpHalfWidth);
                qreal tmpX = (tmpHalfWidth>rect.height())?((rect.width()-rect.height())/2):(0);
                qreal tmpY = (tmpHalfWidth>rect.height())?(0):((rect.height()-tmpHalfWidth)/2);
                pRect = QRect(QPoint(tmpX, tmpY), QSize(tmpMinLen*2, tmpMinLen));
                return true;
            }
            case EffectTypeGlide:
            {
                qreal tmpHalfWidth = rect.width()/2.0;
                qreal tmpMinLen = (tmpHalfWidth>rect.height())?(rect.height()):(tmpHalfWidth);
                qreal tmpX = (tmpHalfWidth>rect.height())?((rect.width()-rect.height())/2):(0);
                qreal tmpY = (tmpHalfWidth>rect.height())?(0):((rect.height()-tmpHalfWidth)/2);
                pRect = QRect(QPoint(tmpX, tmpY), QSize(tmpMinLen*2, tmpMinLen));
                return true;
            }
            default: //>@如果使用不识别的特效，则返回false
                return false;
        }
    }
    //>@如果使用不识别的特效，则占据整个大小
    pRect = rect.toRect();
    return true;
}

bool DesignSetBar::InitItems()
{
    m_ItemList.clear();
    //>@计算ITEMs的正方形有效区域
    if(GetValidRect(m_ValidRect) == false)
    {
        IDE_TRACE();
        return false;
    }
    //>@计算ITEM个数
    QList<int> tmpList = m_EffectPath.keys();
    m_ItemCount = tmpList.count();
    if(tmpList.contains(BGIMG))
        m_ItemCount -= 1;
    if(m_ItemCount < 1)
        return false;
    IDE_TRACE_INT(m_ItemCount);
    //>@初始化ITEM
    for(int i=0; i<m_ItemCount; i++)
    {
        int tmpKey = tmpList.at(i);
        QPixmap tmpPixmap = GetPixmap(tmpKey);
        if(tmpPixmap.isNull())
            continue;
        AddItem(i, (ITEM_TYPE)tmpKey, tmpPixmap);
    }
    return true;
}

int DesignSetBar::GetCurItemIndex()
{
    int count = m_ItemList.count();
    for(int i=0;i<count;i++)
    {
        SETITEM_INFO* tmpSetItem = m_ItemList.at(i);
        if(tmpSetItem == 0)
            continue;
        if(tmpSetItem->mCurParaIndex == 0)
            return i;
    }
    return -1;
}

SETITEM_INFO* DesignSetBar::GetCurSetItem()
{
    int count = m_ItemList.count();
    for(int i=0;i<count;i++)
    {
        SETITEM_INFO* tmpSetItem = m_ItemList.at(i);
        if(tmpSetItem == 0)
            continue;
        if(tmpSetItem->mCurParaIndex == 0)
            return tmpSetItem;
    }
    return (SETITEM_INFO*)0;
}

//>@表示将pIndex的ITEM放在最前面，也即放在位置0
bool DesignSetBar::UpdateItems(int pIndex)
{
    int count = m_ItemList.count();
    if(pIndex < 0 || pIndex >= count)
    {
        IDE_TRACE();
        return false;
    }
    //>@确定左移右移以及移动个数
    int tmpCurIndex = GetCurItemIndex();
    int tmpIndexDiff = tmpCurIndex - pIndex;
    m_ItemSlipCount = qAbs(tmpIndexDiff);
    IDE_TRACE_INT(m_ItemSlipCount);
    if(tmpIndexDiff == 0)
    {
        IDE_TRACE();
        return false;
    }
    //>@规整Items
    for(int i=0;i<count;i++)
    {
        SETITEM_INFO* tmpSetItem = m_ItemList.at(i);
        if(tmpSetItem == 0)
            continue;
        int tmpValue = tmpSetItem->mCurParaIndex + tmpIndexDiff;
        tmpSetItem->mNextParaIndex = tmpValue;
    }
    //>@修改特效时间
    AREA_ANIMATE* tmpAreaAnimate = m_EffectGroup.value(OPERATE_SLIP);
    if(tmpAreaAnimate == 0)
    {
        IDE_TRACE();
        return true;
    }
    if(tmpIndexDiff > 0)
    {
        tmpAreaAnimate->mDirection = Horizontal;
        tmpAreaAnimate->mOrientation = Negative;
    }
    else if(tmpIndexDiff < 0)
    {
        tmpAreaAnimate->mDirection = Horizontal;
        tmpAreaAnimate->mOrientation = Positive;
    }
    //tmpAreaAnimate->mAnimateTime *= qAbs(tmpIndexDiff);
    return true;
}

void DesignSetBar::OperateItem(int pFrameNum)
{
    if(m_AreaAnimate == 0)
    {
        IDE_TRACE();
        return;
    }
    SETITEM_INFO* tmpItemInfo = GetCurSetItem();
    if(!(tmpItemInfo && tmpItemInfo->mProxy))
    {
        IDE_TRACE();
        return;
    }
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeTwirl:
        {
            static qreal tmpSingleAngelDiff = 2*PI/m_ItemCount;
            static qreal tmpTwirlA = m_ValidRect.width() / 3.0;   //>@取椭圆的长半径a为1/3的长度，b为1/4的长度
            static qreal tmpTwirlB = m_ValidRect.height() / 4.0;
            static qreal tmpMaxHeight = m_ValidRect.height()*7.0/10.0;//>@设置最前方的旋转Item的宽为高度的2/3，高度为有效区域高度的7/10
            static QSizeF tmpMaxSize = QSize(tmpMaxHeight*2.0/3.0,  tmpMaxHeight);
            //>@将中心点上移1/8，否则设置框会偏下方
            static QPointF tmpCentrePoint = QPointF(m_ValidRect.x()+m_ValidRect.width()/2.0,
                                                  m_ValidRect.y()+m_ValidRect.height()/2.0 - m_ValidRect.height()/8.0);
            qreal tmpAngelDiff = 2*PI*m_ItemSlipCount/m_ItemCount;
            m_CurValue = pFrameNum * m_Diff * tmpAngelDiff;
            int count = m_ItemList.count();
            for(int i=0; i< count; i++)
            {
                SETITEM_INFO* tmpSetItemInfo = m_ItemList.at(i);
                if(tmpSetItemInfo == 0)
                {
                    IDE_TRACE();
                    continue;
                }
                qreal tmpBeginAngle = RegularAngle(3.0 * PI / 2.0 + tmpSetItemInfo->mCurParaIndex * tmpSingleAngelDiff);  //>@从270度开始
                qreal tmpAngle;
                if(m_AreaAnimate->mOrientation == Positive)
                    tmpAngle = RegularAngle(tmpBeginAngle - m_CurValue);
                else if(m_AreaAnimate->mOrientation == Negative)
                    tmpAngle = RegularAngle(tmpBeginAngle + m_CurValue);
                qreal tmpHalfAngleRatio = 1.0 - CalDiffBeginAngle(tmpAngle)/PI;  //>@用于计算其它渐变的参数
                QPointF tmpItemCentrePoint = tmpCentrePoint + QPointF(tmpTwirlA*qCos(tmpAngle), -tmpTwirlB*qSin(tmpAngle));
                QSizeF tmpItemSize = tmpMaxSize * tmpHalfAngleRatio;
                QRectF tmpRect = QRectF(tmpItemCentrePoint, tmpItemSize);
                tmpRect.moveCenter(tmpItemCentrePoint);
                //>@此处会出现虽然tmpRect很小，但mProxy代理的窗口有最小大小限制从而导致最终mProxy的大小并不为tmpRect的问题
                tmpSetItemInfo->mProxy->setGeometry(tmpRect);
                //tmpSetItemInfo->mProxy->setTransform(QTransform().translate(width()/2, height() / 2).rotate(0, Qt::YAxis).translate(-width()/2, -height()/2));
                //>@最小透明度为0.5，最大为1
                tmpSetItemInfo->mProxy->setOpacity(tmpHalfAngleRatio/2.0+0.5);
                tmpSetItemInfo->mProxy->setZValue(zValue() + 100 * tmpHalfAngleRatio);  //>@此处限制最大有100个ITEM
            }
            break;
        }
        case EffectTypeGlide:
        {
            static qreal tmpMaxHeight = m_ValidRect.height()*9.0/10.0;
            static QSizeF tmpMaxSize = QSize(tmpMaxHeight*2.0/3.0,  tmpMaxHeight);
            static QPointF tmpCentrePoint = QPointF(m_ValidRect.x()+m_ValidRect.width()/2.0,
                                                    m_ValidRect.y()+m_ValidRect.height()/2.0);
            qreal tmpXPosDiff = m_ValidRect.width()/(2.0*m_ItemCount);
            m_CurValue = pFrameNum * m_Diff * m_ItemSlipCount;
            int count = m_ItemList.count();
            for(int i=0; i< count; i++)
            {
                SETITEM_INFO* tmpSetItemInfo = m_ItemList.at(i);
                if(tmpSetItemInfo == 0)
                {
                    IDE_TRACE();
                    continue;
                }
                qreal tmpX;
                if(m_AreaAnimate->mOrientation == Positive)
                {
                    tmpX = tmpSetItemInfo->mCurParaIndex - m_CurValue;
                }
                else if(m_AreaAnimate->mOrientation == Negative)
                {
                    tmpX = tmpSetItemInfo->mCurParaIndex + m_CurValue;
                }
                qreal tmpCurRatio = 1-qAbs(tmpX/m_ItemCount);
                qreal tmpXPos = tmpX*tmpXPosDiff;
                QPointF tmpItemCentrePoint = QPointF(tmpCentrePoint.x() + tmpXPos, tmpCentrePoint.y());
                QSizeF tmpItemSize = tmpMaxSize * tmpCurRatio;
                QRectF tmpRect = QRectF(tmpItemCentrePoint, tmpItemSize);
                tmpRect.moveCenter(tmpItemCentrePoint);
                tmpSetItemInfo->mProxy->setGeometry(tmpRect);
                tmpSetItemInfo->mProxy->setOpacity(tmpCurRatio);
                tmpSetItemInfo->mProxy->setZValue(zValue() + 100 * tmpCurRatio);  //>@此处限制最大有100个ITEM
            }
            break;
        }
        case EffectTypeRotate:
        {

            break;
        }
        case EffectTypeZoom:
        {

            break;
        }
        default:
            break;
    }
}

void DesignSetBar::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            //>@分解坐标，如果点击左半区域，则向左滚动
            QPoint tmpPoint = pPara.toPoint();
            QRectF tmpRect = this->geometry();
            if(tmpPoint.x() < tmpRect.width()/2)
            {
                updateEffect(MOVE_LEFT);
            }
            else if(tmpPoint.x() > tmpRect.width()/2)
            {
                updateEffect(MOVE_RIGHT);
            }
            break;
        }
        default:
            break;
    }
}

void DesignSetBar::updateEffect(int pIndex)
{
    //>@等待上次特效完成
    if(m_Animator.state() == QTimeLine::Running)
        return;
    if(UpdateItems(pIndex) == false)
    {
        IDE_TRACE();
        return;
    }
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    //>@Operate0
    SETITEM_INFO* tmpItemInfo = GetCurSetItem();
    if(tmpItemInfo == 0)
    {
        IDE_TRACE();
        return;
    }
    //>@Operate1
    m_SlipOperateInfo.mValid = true;
    m_SlipOperateInfo.mOperate = OPERATE_SLIP;
    //>@Operate2
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = OPERATE_KEYPRESS;
    //>@exec
    Operate0();
}

void DesignSetBar::updateEffect(MOVE_DIRECTION pDirection)
{
    int tmpCurIndex = GetCurItemIndex();
    if(pDirection == MOVE_LEFT && tmpCurIndex<m_ItemList.count()-1)
    {
        updateEffect(tmpCurIndex += 1);
    }
    else if(pDirection == MOVE_RIGHT && tmpCurIndex>0)
    {
        updateEffect(tmpCurIndex -= 1);
    }
}

//>@翻转操作
void DesignSetBar::Operate0()
{
    if(m_EffectValid == false)
    {
        IDE_TRACE();
        return;
    }
    //>@执行STEP0中的内容，如果执行不成功，尝试执行STEP1中的内容
    if(m_OperateInfo[STEP0].mValid)
    {
        m_AreaAnimate = m_EffectGroup.value(m_OperateInfo[STEP0].mOperate);
        if(m_AreaAnimate)
        {
            m_AreaAnimate->mOrientation = Negative;
            //>@计算ITEM特效相关的值, 建议设置一次翻转角度为18，共翻转10次
            m_CurValue = 0;
            m_Diff = 1.0 / (qreal)(m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart);
            disconnect(&m_Animator, SIGNAL(frameChanged(int)), 0, 0);
            disconnect(&m_Animator, SIGNAL(finished()), 0, 0);
            connect(&m_Animator, SIGNAL(frameChanged(int)), this, SLOT(OperateItem(int)));
            connect(&m_Animator, SIGNAL(finished()), this, SLOT(Operate1()));
            StartTimeline(m_AreaAnimate->mFrameStart,
                          m_AreaAnimate->mFrameEnd,
                          m_AreaAnimate->mAnimateTime,
                          m_AreaAnimate->mAnimateType);
            return;
        }
    }
    Operate1();
}

void DesignSetBar::Operate1()
{
    if(m_EffectValid == false)
    {
        IDE_TRACE();
        return;
    }
    //>@执行STEP1中的内容，如果执行不成功，尝试执行STEP2中的内容
    if(m_SlipOperateInfo.mValid)
    {
        m_AreaAnimate = m_EffectGroup.value(m_SlipOperateInfo.mOperate);
        if(m_AreaAnimate)
        {
            m_CurValue = 0;
            m_Diff = 1.0 / (qreal)(m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart);
            disconnect(&m_Animator, SIGNAL(frameChanged(int)), 0, 0);
            disconnect(&m_Animator, SIGNAL(finished()), 0, 0);
            connect(&m_Animator, SIGNAL(frameChanged(int)), SLOT(OperateItem(int)));
            connect(&m_Animator, SIGNAL(finished()), this, SLOT(Operate2()));
            StartTimeline(m_AreaAnimate->mFrameStart,
                          m_AreaAnimate->mFrameEnd,
                          m_AreaAnimate->mAnimateTime,
                          m_AreaAnimate->mAnimateType);
            return;
        }
        else
        {
            //>@直接设置为下一状态值
            int count = m_ItemList.count();
            for(int i=0; i< count; i++)
            {
                SETITEM_INFO* tmpSetItemInfo = m_ItemList.at(i);
                if(tmpSetItemInfo == 0)
                    continue;
                if(tmpSetItemInfo->mNextParaIndex == 0)
                {
                    tmpSetItemInfo->mProxy->setGeometry(QRect(20,20,m_ValidRect.width()-40,m_ValidRect.height()-40));
                    tmpSetItemInfo->mProxy->setOpacity(1);
                    tmpSetItemInfo->mProxy->setZValue(zValue() + 100);
                }
                else
                {
                    tmpSetItemInfo->mProxy->setOpacity(0);
                    tmpSetItemInfo->mProxy->setZValue(zValue() + 50);
                }
            }
        }
    }
    Operate2();
}

void DesignSetBar::Operate2()
{
    int count = m_ItemList.count();
    for(int i=0;i<count;i++)
    {
        SETITEM_INFO* tmpSetItem = m_ItemList.at(i);
        if(tmpSetItem == 0)
            continue;
        tmpSetItem->mCurParaIndex = tmpSetItem->mNextParaIndex;
    }
    if(m_EffectValid == false)
    {
        IDE_TRACE();
        return;
    }
    if(m_OperateInfo[STEP1].mValid)
    {
        m_AreaAnimate = m_EffectGroup.value(m_OperateInfo[STEP1].mOperate);
        if(m_AreaAnimate)
        {
            m_AreaAnimate->mOrientation = Positive;
            //>@计算ITEM特效相关的值, 建议设置一次翻转角度为18，共翻转10次
            m_CurValue = 0;
            m_Diff = 1.0 / (qreal)(m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart);
            disconnect(&m_Animator, SIGNAL(frameChanged(int)), 0, 0);
            disconnect(&m_Animator, SIGNAL(finished()), 0, 0);
            connect(&m_Animator, SIGNAL(frameChanged(int)), this, SLOT(OperateItem(int)));
            StartTimeline(m_AreaAnimate->mFrameStart,
                          m_AreaAnimate->mFrameEnd,
                          m_AreaAnimate->mAnimateTime,
                          m_AreaAnimate->mAnimateType);
            return;
        }
    }
}

DesignSetBarItem::DesignSetBarItem(QPixmap pPixmap, QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint)
{
    m_Pixmap = pPixmap;
}

void DesignSetBarItem::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    if(m_Pixmap.isNull() == false)
        p.drawPixmap(rect(), m_Pixmap);
}
