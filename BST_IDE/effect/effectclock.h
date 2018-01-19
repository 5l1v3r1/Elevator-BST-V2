#ifndef EFFECTCLOCK_H
#define EFFECTCLOCK_H

#include "effectbase.h"

class EffectClock : public EffectBase
{
    Q_OBJECT
public:
    EffectClock(QWidget *parent, QRect pRect);

    bool InitEffectRc(STATE_INFO* pEffect);
    void CalcElementInfo(CLOCK_TYPE pType);
    void CalcAnalogClockPointer();
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(CLOCK_INFO pTime);
private:
    CLOCK_LAYOUT             m_Layout;
    CLOCK_ALIGN              m_ClockAlign;
    CLOCK_TYPE               m_Type;

    //>@Digital : 图元(指显示数字的一个个区域)个数为19个：RectYear0~RectSecond1 ---- 0~9 SUN~WES : -
    //>@          图片资源个数为10+7+1+1 = 19
    //>@          0~9 Sun~Sat : -
    //>@Analog : 图元个数为11+3个：RectYear0~RectWeek  时指针 分指针 秒指针
    //>@         图片资源个数为10+7+1+4+1 = 23(1个表针最上面的点)个
    //>@         0~9 Sun~Sat - 时指针 分指针 秒指针 时钟底图
    QHash<CLOCKE_ELEMENT, CLOCK_ELEMENTINFO*>    m_ClockElement;
    qreal                    m_Diff;                 //>@图片一次移动(旋转)距离
    CLOCK_INFO               m_SavedTime;
    qreal                    m_ItemWidth, m_ItemHeight;
    int                      m_FontSize;

    //>@针对Analog时钟
    qreal       m_PointerLength[3];
    qreal       m_PointerAngle[3][2];   //>@对于指针，区域指长度和角度(待旋转的角度，也即角度差)
};

#endif // EFFECTCLOCK_H
