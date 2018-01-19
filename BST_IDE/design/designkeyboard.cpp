#include "designkeyboard.h"

DesignKeyboard::DesignKeyboard(QGraphicsItem *parent, QRectF pRect) :
    DesignBase(parent, pRect)
{
}

bool DesignKeyboard::InitEffectRc(STATE_INFO* pEffect)
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
            if(LoadPath(index, tmpRc.RcFile) == false)
            {
                IDE_TRACE_STR(tmpRc.RcFile);
            }
        }
    }
    else if(tmpString.compare(QString("Press"), Qt::CaseInsensitive) == 0)
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
        m_EffectGroup.insert(OPERATE_KEYPRESS, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Release"), Qt::CaseInsensitive) == 0)
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
        m_EffectGroup.insert(OPERATE_KEYRELEASE, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool DesignKeyboard::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return DesignBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool DesignKeyboard::Start()
{
    if(DesignBase::Start() == false)
    {
        IDE_TRACE();
        return false;
    }
    QPixmap tmpNormalPixmap, tmpHightlightPixmap;
    tmpNormalPixmap.load(m_EffectPath.value(0));
    tmpHightlightPixmap.load(m_EffectPath.value(1));
    update();
    return true;
}

bool DesignKeyboard::PaintEffect(QPainter *p)
{
    QPixmap tmpPixmap(m_EffectPath.value(BGIMG));
    DrawPixmap(p, tmpPixmap, rect());
    return true;
}

void DesignKeyboard::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
}

