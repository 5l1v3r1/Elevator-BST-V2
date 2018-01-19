#include "designregion.h"

DesignRegion::DesignRegion(QGraphicsItem *parent, QRectF pRect) :
    DesignBase(parent, pRect)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
    for(int i=1;i<10;i++)
    {
        m_ActionGroup->addAction(QString("%1").arg(i));
    }
    tmpDefault->setChecked(true);
}

bool DesignRegion::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        RESOURCE_INFO tmpRc;
        char index;
        int count = pEffect->Effect.RcFiles.count();
        for(int i=0;i<count;i++)
        {
            tmpRc = pEffect->Effect.RcFiles.at(i);
            index = GetKey(tmpRc.RcName);
            IDE_TRACE_INT(index);
            IDE_TRACE_STR(tmpRc.RcFile);
            if(LoadPath(index, tmpRc.RcFile) == false)
            {
                IDE_TRACE_STR(tmpRc.RcFile);
            }
        }
    }
    else if(tmpString.compare(QString("Change"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("SlipCycle"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSlipCycle;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Roll"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRoll;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Transparent"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeTransparent;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Zoom"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeZoom;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Flip"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeFlip;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Cube"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeCube;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Cylinder"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeCylinder;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_CHANGE, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Steady"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("SlipCycle"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeSlipCycle;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Roll"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRoll;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Transparent"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeTransparent;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Zoom"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeZoom;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Flip"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeFlip;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeRotate;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Cube"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeCube;
        }
        else if(pEffect->Effect.EffectType.compare(QString("Cylinder"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeCylinder;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_STEADY, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool DesignRegion::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return DesignBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool DesignRegion::Start()
{
    if(DesignBase::Start() == false)
    {
        IDE_TRACE();
        return false;
    }
    m_RegionState = 0;
    m_PixmapPointer.mLastPointer = GetPixmap(0, size());
    m_PixmapPointer.mCurPointer = m_PixmapPointer.mLastPointer;
    updateEffect(0, OPERATE_NONE);
    return true;
}

QList<QAction*> DesignRegion::GetActionList()
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

void DesignRegion::ExecAction(QAction *pAction)
{
    updateEffect(pAction->text().toInt(), OPERATE_STEADY);
}

bool DesignRegion::PaintEffect(QPainter *p)
{
    if(DesignBase::PaintEffect(p) == false)
        return false;
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeRoll:
            Paint2D_Roll(p, rect());
            break;
        case EffectTypeSlipCycle:
            Paint2D_Slipcycle(p, rect());
            break;
        case EffectTypeTransparent:
            Paint2D_Transparent(p, rect());
            break;
        case EffectTypeZoom:
            Paint2D_Zoom(p, rect());
            break;
        case EffectTypeFlip:
            Paint2D_Flip(p, rect());
            break;
        case EffectTypeRotate:
            Paint2D_Rotate(p, rect());
            break;
        case EffectTypeCube:
            Paint3D_Cube(p, rect());
            break;
        case EffectTypeCylinder:
            Paint3D_Cylinder(p, rect());
            break;
        default:
            Paint2D_None(p, rect());
            break;
    }
    return true;
}

void DesignRegion::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_CHANGE:
        {
            updateEffect(pPara.toInt(), OPERATE_STEADY);
            break;
        }
        default:
            break;
    }
}

void DesignRegion::updateEffect(int pState, AREA_OPERATE pOperate)
{
    bool tmpStateChange = false;
    bool tmpOperateChange = false;
    if(pState != m_RegionState)
        tmpStateChange = true;
    if(pOperate != m_OperateInfo[STEP1].mOperate)
        tmpOperateChange = true;
    if(tmpStateChange == false && tmpOperateChange == false)
    {
        IDE_TRACE();
        return;
    }
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP1].mValid = false;
    m_Animator.stop();
    if(tmpStateChange)
    {
        m_OperateInfo[STEP0].mValid = true;
        m_OperateInfo[STEP0].mOperate = OPERATE_CHANGE;
        m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer;
        m_PixmapPointer.mCurPointer = GetPixmap(pState);
    }
    m_OperateInfo[STEP1].mValid = true;
    m_OperateInfo[STEP1].mOperate = pOperate;
    m_RegionState = pState;
    //>@执行STEP0中的内容
    OperateStep0();
}

DesignDesktop::DesignDesktop(QGraphicsItem *parent, QRectF pRect) :
    DesignRegion(parent, pRect)
{
}

DesignButtonOperate::DesignButtonOperate(QGraphicsItem *parent, QRectF pRect) :
    DesignRegion(parent, pRect)
{
}

DesignFuncInfo::DesignFuncInfo(QGraphicsItem *parent, QRectF pRect) :
    DesignRegion(parent, pRect)
{
}

DesignIntegratedInfo::DesignIntegratedInfo(QGraphicsItem *parent, QRectF pRect) :
    DesignRegion(parent, pRect)
{
}

DesignMultimedia::DesignMultimedia(QGraphicsItem *parent, QRectF pRect) :
    DesignRegion(parent, pRect)
{
}

DesignTextBulletin::DesignTextBulletin(QGraphicsItem *parent, QRectF pRect) :
    DesignRegion(parent, pRect)
{
}

DesignFunction::DesignFunction(QGraphicsItem *parent, QRectF pRect) :
    DesignRegion(parent, pRect)
{
}

DesignImage::DesignImage(QGraphicsItem *parent, QRectF pRect) :
    DesignRegion(parent, pRect)
{
}

DesignLogo::DesignLogo(QGraphicsItem *parent, QRectF pRect) :
    DesignRegion(parent, pRect)
{
}

DesignWeather::DesignWeather(QGraphicsItem *parent, QRectF pRect) :
    DesignRegion(parent, pRect)
{
}

DesignNamePlate::DesignNamePlate(QGraphicsItem *parent, QRectF pRect) :
    DesignRegion(parent, pRect)
{
}

