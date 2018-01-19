#ifndef EFFECTANIMAL_H
#define EFFECTANIMAL_H

#include "global.h"

enum _ANIMAL{
    FishTailLeft=0,
    FishTailMiddle,
    FishTailRight,
    InsectStepLeft,
    InsectStepMiddle,
    InsectStepRight
};

typedef enum{
    AnimalFish=0,
    AnimalInsect
}AnimalType;

#define PARA_MOVE 10
#define PARA_ROTATE 11

//>@应用于装饰的小动物
class EffectAnimal : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit EffectAnimal(QGraphicsItem *parent, QRectF pRect);

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;

    bool InitEffect(COM_STRUCT *pComInfo);
    virtual bool InitEffectRc(STATE_INFO* pEffect);

    virtual bool Start();  //>@执行完InitEffect需要执行Start来启动特效

    virtual bool PrepareRc(int pPara);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void PaintEffect(QPainter *painter);

    bool analysisEffectPara(QString pEffectPara);
    virtual bool analysisEffectParaItem(QString pLabel, QString pContent);

public:
    QHash<int, QPixmap *>   m_EffectImage;  //>@图片资源
    AnimalType              m_AnimalType;
    EffectType              m_EffectType;   //>@特效类型，默认为无特效
    bool                    m_EffectValid;  //>@特效使能
    QRect                   m_Rect;         //>@区域大小

    int                     m_PixPointer;

    qreal                   m_XDiff,m_YDiff;

    QTimeLine               m_Animator;
    int                     m_AnimateCount;
    int                     m_AnimateTime;
    QTimeLine::CurveShape   m_AnimateType;

public slots:
    virtual void updateEffect(QString pParameter);
    void updateAnimal(int pFrame);
};

#endif // EFFECTANIMAL_H
