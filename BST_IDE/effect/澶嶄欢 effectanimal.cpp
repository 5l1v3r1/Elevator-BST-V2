#include "effectanimal.h"

EffectAnimal::EffectAnimal(QGraphicsItem *parent, QRectF pRect) :
  QGraphicsWidget(parent, Qt::FramelessWindowHint)
{
    setGeometry(pRect);

    connect(&m_Animator, SIGNAL(frameChanged(int)), SLOT(updateAnimal(int)));

    m_AnimateCount = 100;
    m_AnimateTime = 1000;
    m_AnimateType = QTimeLine::EaseInOutCurve;

    m_EffectType = EffectTypeNone;

    m_EffectImage.clear();
    m_EffectValid = false;
}

QRectF EffectAnimal::boundingRect() const
{
    QRectF tmpRect = this->geometry().normalized();
    return QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized();
}

QPainterPath EffectAnimal::shape() const
{
    QPainterPath path;
    QRectF tmpRect = this->geometry().normalized();
    path.addRect(QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized());
    return path;
}

bool EffectAnimal::InitEffect(COM_STRUCT *pComInfo)
{
    m_EffectValid = false;
    if(pComInfo == 0)
        return false;
    for(int i=0;i<pComInfo->StateMachine.count();i++)
    {
        STATE_INFO* tmp = pComInfo->StateMachine.at(i);
        if(tmp == 0)
            continue;
        InitEffectRc(tmp);
    }
    m_EffectValid = true;
    Start();
    return true;
}

bool EffectAnimal::InitEffectRc(STATE_INFO* pEffect)
{
    if(pEffect == 0)
        return false;
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        QString tmpAnimal = pEffect->Effect.EffectType;
        if(tmpAnimal.compare(QString("Fish"),Qt::CaseInsensitive) == 0)
        {
            int count = pEffect->Effect.RcFiles.count();
            for(int i=0;i<count;i++)
            {
                RESOURCE_INFO tmpRc = pEffect->Effect.RcFiles.at(i);
                int index = 0;
                if(tmpRc.RcName.compare(QString("ImageTailLeft"),Qt::CaseInsensitive)==0)
                    index = FishTailLeft;
                else if(tmpRc.RcName.compare(QString("ImageTailMiddle"),Qt::CaseInsensitive)==0)
                    index = FishTailMiddle;
                else if(tmpRc.RcName.compare(QString("ImageTailRight"),Qt::CaseInsensitive)==0)
                    index = FishTailRight;
                else
                    continue;
                QPixmap *tmpPix = LoadPixmap(tmpRc.RcFile);
                if(tmpPix == 0)
                    continue;
                m_EffectImage.insert(index, tmpPix);
                m_AnimalType = AnimalFish;
            }
        }
        else if(tmpAnimal.compare(QString("Insect"),Qt::CaseInsensitive) == 0)
        {
            int count = pEffect->Effect.RcFiles.count();
            for(int i=0;i<count;i++)
            {
                RESOURCE_INFO tmpRc = pEffect->Effect.RcFiles.at(i);
                int index = 0;
                if(tmpRc.RcName.compare(QString("ImageStepLeft"),Qt::CaseInsensitive)==0)
                    index = InsectStepLeft;
                else if(tmpRc.RcName.compare(QString("ImageStepMiddle"),Qt::CaseInsensitive)==0)
                    index = InsectStepMiddle;
                else if(tmpRc.RcName.compare(QString("ImageStepRight"),Qt::CaseInsensitive)==0)
                    index = InsectStepRight;
                else
                    continue;
                QPixmap *tmpPix = LoadPixmap(tmpRc.RcFile);
                if(tmpPix == 0)
                    continue;
                m_EffectImage.insert(index, tmpPix);
                m_AnimalType = AnimalInsect;
            }
        }
        else
            return false;
    }
    else
    {
        return false;
    }
    analysisEffectPara(pEffect->Effect.EffectPara);
    return true;
}

bool EffectAnimal::Start()
{
    if(m_EffectValid == false)
        return false;
    PrepareRc(PARA_START);
    return true;
}

bool EffectAnimal::analysisEffectPara(QString pEffectPara)
{
    if(pEffectPara.isEmpty())
        return false;
    QStringList tmpList = SplitString(pEffectPara, ";");
    for(int i=0;i<tmpList.count();i++)
    {
        QStringList tmpSubList = SplitString(tmpList.at(i), ":");
        if(tmpSubList.count()<2)
        {
            IDE_TRACE();
            continue;
        }
        QString tmpString1 = tmpSubList.at(0);
        QString tmpString2 = tmpSubList.at(1);
        if(tmpString1.isEmpty() || tmpString2.isEmpty())
        {
            IDE_TRACE();
            continue;
        }
        analysisEffectParaItem(tmpString1, tmpString2);
    }
    return true;
}

bool EffectAnimal::analysisEffectParaItem(QString pLabel, QString pContent)
{
    if(pLabel.isEmpty() || pContent.isEmpty())
    {
        return false;
    }
    if(pLabel.compare(PARA_FRAMERANGE, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            m_AnimateCount = dec;
    }
    else if(pLabel.compare(PARA_INTERVAL, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            m_AnimateTime = dec;
    }
    else if(pLabel.compare(PARA_CURVESHAPE, Qt::CaseInsensitive) == 0)
    {
        if(pContent.compare(QString("EaseInCurve"), Qt::CaseInsensitive) == 0)
            m_AnimateType = QTimeLine::EaseInCurve;
        else if(pContent.compare(QString("EaseOutCurve"), Qt::CaseInsensitive) == 0)
            m_AnimateType = QTimeLine::EaseOutCurve;
        else if(pContent.compare(QString("EaseInOutCurve"), Qt::CaseInsensitive) == 0)
            m_AnimateType = QTimeLine::EaseInOutCurve;
        else if(pContent.compare(QString("LinearCurve"), Qt::CaseInsensitive) == 0)
            m_AnimateType = QTimeLine::LinearCurve;
        else if(pContent.compare(QString("SineCurve"), Qt::CaseInsensitive) == 0)
            m_AnimateType = QTimeLine::SineCurve;
        else if(pContent.compare(QString("CosineCurve"), Qt::CaseInsensitive) == 0)
            m_AnimateType = QTimeLine::CosineCurve;
    }
    else if(pLabel.compare(QString("Moveto"), Qt::CaseInsensitive) == 0)
    {
        QStringList tmpList = SplitString(pContent);
        if(tmpList.count() != 4)
            return false;
        QRectF rect = this->geometry();
        int xDst = tmpList.at(0).toInt();
        int yDst = tmpList.at(1).toInt();
        m_XDiff = xDst - rect.left();
        m_YDiff = yDst - rect.top();
    }
    return true;
}

bool EffectAnimal::PrepareRc(int pPara)
{
    if(m_EffectValid == false)
        return false;
    switch(pPara)
    {
        case PARA_START:
        case PARA_MOVE:
        case PARA_ROTATE:
        {
            switch(m_AnimalType)
            {
                case AnimalFish:
                {
                    int index = m_PixPointer;
                    if(index < FishTailLeft)
                        index = FishTailLeft;
                    index++;
                    if(index > FishTailRight)
                        m_PixPointer = FishTailLeft;
                    break;
                }
                case AnimalInsect:
                {
                    int index = m_PixPointer;
                    if(index < InsectStepLeft)
                        index = InsectStepLeft;
                    index++;
                    if(index > InsectStepRight)
                        m_PixPointer = InsectStepLeft;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    return true;
}

//>@TimeLine每次换帧时调用
void EffectAnimal::updateAnimal(int pFrame)
{
    switch(m_EffectType)
    {
        case EffectTypeMove:
        {
            PrepareRc(PARA_MOVE);
            //>@设置坐标
            QRectF rect = this->geometry();
            qreal tmpCurX = rect.left() + m_XDiff / (qreal)m_AnimateCount;
            qreal tmpCurY = rect.top() + m_YDiff / (qreal)m_AnimateCount;
            this->setGeometry(tmpCurX, tmpCurY, rect.width(), rect.height());
            break;
        }
        case EffectTypeRotate:
        {
            PrepareRc(PARA_ROTATE);
            //>@设置角度
            this->rotate(0);
            break;
        }
        default:
            return;
    }
}

void EffectAnimal::updateEffect(QString pParameter)
{
    if(m_EffectValid == false)
        return;
    if(pParameter.isEmpty())
        return;
    QStringList tmpList = SplitString(pParameter, ":");
    QString tmpString = tmpList.at(0);
    QString tmpPara = tmpList.at(1);
    if(tmpString.isEmpty())
        return;
    if(tmpString.compare(QString("Move"), Qt::CaseInsensitive) == 0)
    {
        m_EffectType = EffectTypeMove;
    }
    else if(tmpString.compare(QString("Rotate"), Qt::CaseInsensitive) == 0)
    {
        m_EffectType = EffectTypeRotate;
    }
    else
        return;
    if(tmpPara.isEmpty() == false)
    {
        analysisEffectPara(tmpPara);
    }
    m_Animator.stop();
    m_Animator.setFrameRange(0, m_AnimateCount);
    m_Animator.setDuration(m_AnimateTime);
    m_Animator.setCurveShape(m_AnimateType);
    m_Animator.start();
}

void EffectAnimal::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(m_EffectValid==false)
        return;
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setRenderHint(QPainter::Antialiasing);

    PaintEffect(painter);
}

void EffectAnimal::PaintEffect(QPainter *painter)
{
    QPixmap* tmpPixmap = m_EffectImage.value(m_PixPointer);
    if(tmpPixmap == 0)
        return;
    painter->drawPixmap(this->geometry(), *tmpPixmap);
}
