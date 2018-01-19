#include "effecttime.h"

EffectTime::EffectTime(QWidget *parent, QRect pRect) :
    EffectBase(parent, pRect)
{
    connect(&m_Animator, SIGNAL(frameChanged(int)), SLOT(update()));
}

bool EffectTime::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        int count = pEffect->Effect.RcFiles.count();
        for(int i=0;i<count;i++)
        {
            RESOURCE_INFO tmpRc = pEffect->Effect.RcFiles.at(i);
            QPixmap *tmpPix = LoadPixmap(tmpRc.RcFile);
            if(tmpPix == 0)
                continue;
            int index;
            if(tmpRc.RcName.compare(QString("Image0"),Qt::CaseInsensitive)==0)
                index = Label0;
            else if(tmpRc.RcName.compare(QString("Image1"),Qt::CaseInsensitive)==0)
                index = Label1;
            else if(tmpRc.RcName.compare(QString("Image2"),Qt::CaseInsensitive)==0)
                index = Label2;
            else if(tmpRc.RcName.compare(QString("Image3"),Qt::CaseInsensitive)==0)
                index = Label3;
            else if(tmpRc.RcName.compare(QString("Image4"),Qt::CaseInsensitive)==0)
                index = Label4;
            else if(tmpRc.RcName.compare(QString("Image5"),Qt::CaseInsensitive)==0)
                index = Label5;
            else if(tmpRc.RcName.compare(QString("Image6"),Qt::CaseInsensitive)==0)
                index = Label6;
            else if(tmpRc.RcName.compare(QString("Image7"),Qt::CaseInsensitive)==0)
                index = Label7;
            else if(tmpRc.RcName.compare(QString("Image8"),Qt::CaseInsensitive)==0)
                index = Label8;
            else if(tmpRc.RcName.compare(QString("Image9"),Qt::CaseInsensitive)==0)
                index = Label9;
            else
                continue;
            m_EffectImage.insert(index, tmpPix);
        }
    }
    else if(tmpString.compare(QString("Updating"), Qt::CaseInsensitive) == 0)
    {
        if(pEffect->Effect.EffectType == QString("Roll"))
        {
            m_EffectType = EffectTypeRoll;
        }
        else if(pEffect->Effect.EffectType == QString("Rotate"))
        {
            m_EffectType = EffectTypeRotate;
        }
        else if(pEffect->Effect.EffectType == QString("Zoom"))
        {
            m_EffectType = EffectTypeZoom;
        }
        else if(pEffect->Effect.EffectType == QString("Transparent"))
        {
            m_EffectType = EffectTypeTransparent;
        }
        else if(pEffect->Effect.EffectType == QString("Fish"))
        {
            m_EffectType = EffectTypeFish;
        }
    }
    else
    {
        return false;
    }
    analysisEffectPara(pEffect->Effect.EffectPara);
    return true;
}

bool EffectTime::analysisEffectParaItem(QString pLabel, QString pContent)
{
    EffectBase::analysisEffectParaItem(pLabel, pContent);

    return true;
}

bool EffectTime::Start()
{
    m_RcPointer.mCurPointer = 0;
    m_RcPointer.mLastPointer = 0;
    PrepareRc();
    return true;
}

bool EffectTime::PrepareRc()
{
    if(EffectBase::PrepareRc() == false)
        return false;
    int index = m_RcPointer.mCurPointer;
    m_RcPointer.mLastPointer = index++;
    m_RcPointer.mCurPointer = index % m_EffectImage.count();
}

void EffectTime::PaintEffect(QPainter &p)
{
    PIXMAP_POINTER tmpPixmap;
    tmpPixmap.mLastPointer = m_EffectImage.value(m_RcPointer.mLastPointer);
    tmpPixmap.mCurPointer = m_EffectImage.value(m_RcPointer.mCurPointer);

    switch(m_EffectType)
    {
    case EffectTypeNone:
        Paint2D_None(p, tmpPixmap, rect());
        break;
    case EffectTypeFlip:
        Paint2D_Flip(p, tmpPixmap, rect(), m_Animator);
        break;
    }
}

void EffectTime::updateEffect(QString pParameter)
{
    if(m_EffectValid == false)
        return;
    if(pParameter.compare(QString("LeftMousePress"), Qt::CaseInsensitive) == 0)
    {
        PrepareRc();
        m_Animator.stop();
        m_Animator.setFrameRange(0, m_AnimateCount);
        m_Animator.setDuration(m_AnimateTime);
        m_Animator.setCurveShape(m_AnimateType);
        m_Animator.start();
    }
}
