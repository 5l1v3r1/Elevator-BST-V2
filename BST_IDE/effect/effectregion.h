#ifndef EFFECTREGION_H
#define EFFECTREGION_H
#include "effectbase.h"

class EffectRegion : public EffectBase
{
    Q_OBJECT
public:
    EffectRegion(QWidget *parent, QRect pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(int pState, AREA_OPERATE pOperate);
private:
    int            m_RegionState;
};

class EffectDesktop : public EffectRegion
{
    Q_OBJECT
public:
    EffectDesktop(QWidget *parent, QRect pRect);
};

class EffectButtonOperate : public EffectRegion
{
    Q_OBJECT
public:
    EffectButtonOperate(QWidget *parent, QRect pRect);
};

class EffectFuncInfo : public EffectRegion
{
    Q_OBJECT
public:
    EffectFuncInfo(QWidget *parent, QRect pRect);
};

class EffectIntegratedInfo : public EffectRegion
{
    Q_OBJECT
public:
    EffectIntegratedInfo(QWidget *parent, QRect pRect);
};

class EffectMultimedia : public EffectRegion
{
    Q_OBJECT
public:
    EffectMultimedia(QWidget *parent, QRect pRect);
};

class EffectTextBulletin : public EffectRegion
{
    Q_OBJECT
public:
    EffectTextBulletin(QWidget *parent, QRect pRect);
};


#endif // EFFECTREGION_H
