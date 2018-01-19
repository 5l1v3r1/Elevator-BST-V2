#include "effectrender.h"

EffectRender::EffectRender(QObject *parent) :
    QObject(parent)
{
    mEffectType = EffectTypeNone;
    mDirection = Horizontal;
    mOrientation = Positive;
    mFrameStart = 0;
    mFrameEnd = 10;
    mFrameCount = mFrameEnd - mFrameStart -1;
    mAnimateTime = 200;
    mLoopCount = 1;        //>@循环次数，如果为0，则表示无限循环
    mDutyRatio = 50;        //>@占空比：0~100
    mAnimateType = QTimeLine::LinearCurve;

    connect(&mAnimator, SIGNAL(frameChanged(int)), this, SLOT(slot_PaintFrame(int)));
    connect(&mAnimator, SIGNAL(stateChanged(QTimeLine::State)), this, SLOT(slot_EffectState(QTimeLine::State)));
}

void EffectRender::SetEffectRc(QString pEffectRc)
{

}

void EffectRender::StartAnimate()
{
    mAnimator.stop();
    mAnimator.setFrameRange(mFrameStart, mFrameEnd);
    mFrameCount = mFrameEnd - mFrameStart - 1;
    mAnimator.setDuration(mAnimateTime);
    mAnimator.setCurveShape(mAnimateType);
    mAnimator.setLoopCount(mLoopCount);
    mAnimator.start();
}

void EffectRender::StopAnimate()
{
    if(isAnimateRunning())
        mAnimator.stop();
}

bool EffectRender::isAnimateRunning()
{
    return (mAnimator.state() == QTimeLine::Running);
}

bool EffectRender::InitEffectPara(QString pEffectPara)
{
    if(pEffectPara.isEmpty())
        return false;
    //>@如果当前特效正在执行，则关闭当前特效
    StopAnimate();
    //>@开始解析
    QStringList tmpParaList = pEffectPara.split(";", QString::SkipEmptyParts);
    QString tmpPara;
    foreach(tmpPara, tmpParaList)
    {
        QStringList tmpSubList = tmpPara.split(":", QString::SkipEmptyParts);
        if(tmpSubList.count()<2)
            continue;
        InitSubEffectPara(tmpSubList.at(0), tmpSubList.at(1));
    }
    return true;
}

bool EffectRender::analysisEffectParaItem(QString pLabel, QString pContent)
{
    if(pLabel.isEmpty() || pContent.isEmpty())
        return false;
    if(pLabel.compare(PARA_FRAMERANGE, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(!ok)
            return false;
        mFrameStart = 0;
        mFrameEnd = dec;
    }
    else if(pLabel.compare(PARA_INTERVAL, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            mAnimateTime = dec;
    }
    else if(pLabel.compare(PARA_DUTYRATIO, Qt::CaseInsensitive) == 0)  //>@0~100
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(!ok)
            return false;
        mDutyRatio = dec;
    }
    else if(pLabel.compare(PARA_CURVESHAPE, Qt::CaseInsensitive) == 0)
    {
        if(pContent.compare(QString("EaseIn"), Qt::CaseInsensitive) == 0)
            mAnimateType = QTimeLine::EaseInCurve;
        else if(pContent.compare(QString("EaseOut"), Qt::CaseInsensitive) == 0)
            mAnimateType = QTimeLine::EaseOutCurve;
        else if(pContent.compare(QString("EaseInOut"), Qt::CaseInsensitive) == 0)
            mAnimateType = QTimeLine::EaseInOutCurve;
        else if(pContent.compare(QString("Linear"), Qt::CaseInsensitive) == 0)
            mAnimateType = QTimeLine::LinearCurve;
        else if(pContent.compare(QString("Sine"), Qt::CaseInsensitive) == 0)
            mAnimateType = QTimeLine::SineCurve;
        else if(pContent.compare(QString("Cosine"), Qt::CaseInsensitive) == 0)
            mAnimateType = QTimeLine::CosineCurve;
        else
            return false;
    }
    else if(pLabel.compare(PARA_DIRECTION, Qt::CaseInsensitive) == 0)
    {
        if(pContent.compare(QString("H"), Qt::CaseInsensitive) == 0)
            mDirection = Horizontal;
        else if(pContent.compare(QString("V"), Qt::CaseInsensitive) == 0)
            mDirection = Vertical;
        else
            return false;
    }
    else if(pLabel.compare(PARA_ORIENTATION, Qt::CaseInsensitive) == 0)
    {
        if(pContent.compare(QString("P"), Qt::CaseInsensitive) == 0)
            mOrientation = Positive;
        else if(pContent.compare(QString("N"), Qt::CaseInsensitive) == 0)
            mOrientation = Negative;
        else
            return false;
    }
    return true;
}

void EffectRender::setViewBox(QRectF pViewbox)
{

}

void EffectRender::render(QPainter *pPainter)
{

}

void EffectRender::render(QPainter *pPainter, QRectF pBounds)
{

}

void EffectRender::slot_PaintFrame(int pFrame)
{

}

void EffectRender::slot_EffectState(QTimeLine::State pState)
{

}


bool EffectRender::load(PIXMAP_POINTER pPixPtr)
{
    mPixmapPtr = pPixPtr;
    return true;
}


