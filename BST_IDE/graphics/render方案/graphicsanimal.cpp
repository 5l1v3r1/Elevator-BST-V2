#include "graphicsanimal.h"

GraphicsAnimal::GraphicsAnimal(QGraphicsItem *parent, QRectF pRect) :
  GraphicsBase(parent, pRect)
{
    m_AnimalStage = StageUnknow;
}

bool GraphicsAnimal::InitEffectRc(STATE_INFO* pEffect)
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
    else if(tmpString.compare(QString("Change"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Zoom"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeZoom;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Transparent"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeTransparent;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Fireworks"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeFireworks;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_CHANGE, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Turn"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_TURN, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Run"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Move"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeMove;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_RUN, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool GraphicsAnimal::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return GraphicsBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool GraphicsAnimal::Start()
{
    GraphicsBase::Start();
    QPointF tmpPoint = pos();
    m_CurTrackLine = QLineF(tmpPoint, tmpPoint+QPointF(10,0));
    updateEffect(StageBrith, OPERATE_RUN);
    //QTimer::singleShot(1000, this, SLOT(Update()));
    return true;
}

void GraphicsAnimal::Update()
{
    updateEffect(QPointF(400,400), OPERATE_RUN);
}

QLineF GraphicsAnimal::GetTrackLine(QPointF pPoint)
{
    if(pPoint.isNull())
        return QLineF();
    return QLineF(pos(), pPoint);
}

qreal GraphicsAnimal::GetTrackDiffAngle(QPointF pPoint)
{
    QLineF tmpLine = GetTrackLine(pPoint);
    if(tmpLine.isNull())
        return 0;
    m_CurAngle = tmpLine.angle();
    qreal angle = tmpLine.angleTo(m_CurTrackLine);  //>@0~359
    if(angle == 0)
        return 0;
    m_NextTrackLine = tmpLine;
    return angle;
}

void GraphicsAnimal::UpdateCom(int pFrame)
{
    if(m_AreaAnimate && pFrame>0)
    {
        switch(m_AreaAnimate->mEffectType)
        {
            case EffectTypeMove:
                PaintAnimal_Move(pFrame);
                break;
            case EffectTypeRotate:
                PaintAnimal_Turn(pFrame);
                break;
            default:
                break;
        }
    }
    update();
}

bool GraphicsAnimal::PaintEffect(QPainter *p)
{
    if(GraphicsBase::PaintEffect(p) == false)
    {
        return false;
    }
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeMove:
        {
            if(m_Animator.state() != QTimeLine::Running)
            {
                DrawPixmap(p, m_PixmapPointer.mCurPointer, rect());
                return false;
            }
            int index = m_Animator.currentFrame();
            if(index % 2)
                DrawPixmap(p, m_FlipPixmap, rect());
            else
                DrawPixmap(p, m_PixmapPointer.mCurPointer, rect());
            break;
        }
        case EffectTypeZoom:
            Paint2D_Zoom(p, rect());
            break;
        case EffectTypeTransparent:
            Paint2D_Transparent(p, rect());
            break;
        case EffectTypeFireworks:
            //Paint2D_Fireworks(p, rect());
            break;
        default:
            Paint2D_None(p, rect());
            break;
    }
    return true;
}

void GraphicsAnimal::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_KEYPRESS:
        {
            //>@分解坐标，如果点击上半区域，则向上滚动
            QPoint tmpPoint = pPara.toPoint();
            if(tmpPoint.y() < size().height()/3)
            {
                updateEffect(StageBrith, OPERATE_RUN);
            }
            else if(tmpPoint.y() > size().height()/3 && tmpPoint.y() < size().height()*2/3)
            {
                updateEffect(StageMature, OPERATE_RUN);
            }
            else if(tmpPoint.y() > size().height()*2/3)
            {
                updateEffect(StageDeath, OPERATE_NONE);
            }
            break;
        }
        case OPERATE_CHANGE:
        {
            updateEffect((AnimalStage)(pPara.toInt()), OPERATE_RUN);
        }
        default:
            break;
    }
}

void GraphicsAnimal::updateEffect(AnimalStage pStage, AREA_OPERATE pOperate)
{
    bool tmpStageChange = false;
    bool tmpOperateChange = false;
    if(pStage != m_AnimalStage)
        tmpStageChange = true;
    if(pOperate != m_OperateInfo[STEP1].mOperate)
        tmpOperateChange = true;
    if(tmpStageChange == false && tmpOperateChange == false)
        return;
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    if(tmpStageChange)
    {
        m_OperateInfo[STEP0].mValid = true;
        m_OperateInfo[STEP0].mOperate = OPERATE_CHANGE;
        m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer;
        m_PixmapPointer.mCurPointer = GetPixmap(pStage);
    }
    m_CurTrackLine = GetTrackLine(m_CurTrackLine.pointAt(1));//>@更新路径
    if(m_CurTrackLine.length())
    {
        m_OperateInfo[STEP1].mValid = true;
        m_OperateInfo[STEP1].mOperate = pOperate;
        m_FlipPixmap = GetFlipPixmap(m_PixmapPointer.mCurPointer, rect());
    }
    m_AnimalStage = pStage;
    OperateStep0();
}

void GraphicsAnimal::updateEffect(QPointF pDstPos, AREA_OPERATE pOperate)
{
    bool tmpAngleChange = false;
    bool tmpOperateChange = false;
    qreal tmpDiffAngle = GetTrackDiffAngle(pDstPos);
    if(tmpDiffAngle)
    {
        m_DiffAngle = tmpDiffAngle;
        tmpAngleChange = true;
    }
    if(pOperate != m_OperateInfo[STEP1].mOperate)
        tmpOperateChange = true;
    if(tmpAngleChange == false && tmpOperateChange == false)
        return;
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    if(tmpAngleChange)
    {
        m_OperateInfo[STEP0].mValid = true;
        m_OperateInfo[STEP0].mOperate = OPERATE_TURN;
    }
    m_CurTrackLine = m_NextTrackLine;
    if(m_CurTrackLine.length())
    {
        m_OperateInfo[STEP1].mValid = true;
        m_OperateInfo[STEP1].mOperate = pOperate;
        m_FlipPixmap = GetFlipPixmap(m_PixmapPointer.mCurPointer, rect());
    }
    OperateStep0();
}


DesignAnimal::DesignAnimal(QGraphicsItem *parent, QRectF pRect) :
  GraphicsAnimal(parent, pRect)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
    m_ActionGroup->addAction(tr("1"));
    m_ActionGroup->addAction(tr("2"));
    m_ActionGroup->addAction(tr("3"));
    m_ActionGroup->addAction(tr("4"));
    m_ActionGroup->addAction(tr("5"));
    m_ActionGroup->addAction(tr("6"));
    m_ActionGroup->addAction(tr("7"));
    m_ActionGroup->addAction(tr("8"));
    m_ActionGroup->addAction(tr("9"));
    tmpDefault->setChecked(true);
}

QList<QAction*> DesignAnimal::GetActionList()
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
            int tmpState = tmpAction->text().toInt();
            if(m_EffectPath.contains(tmpState))
                tmpAction->setEnabled(true);
            else
                tmpAction->setDisabled(true);
        }
    }
    return tmpList;
}

void DesignAnimal::ExecAction(QAction *pAction)
{
    updateEffect(OPERATE_CHANGE, QVariant(pAction->text()));
}
