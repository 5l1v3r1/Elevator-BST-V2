#ifndef EFFECTNAMEPLATE_H
#define EFFECTNAMEPLATE_H

#include "effectbase.h"

class EffectNamePlate : public EffectBase
{
    Q_OBJECT
public:
    EffectNamePlate(QWidget *parent, QRect pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(int pState, AREA_OPERATE pOperate);
private:
    int            m_NameplateState;
};

#endif // EFFECTNAMEPLATE_H
