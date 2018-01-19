#ifndef EFFECTFUNCTION_H
#define EFFECTFUNCTION_H

#include "designbase.h"

class EffectFunction : public EffectBase
{
    Q_OBJECT
public:
    EffectFunction(QWidget *parent, QRect pRect);

    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(int pState, AREA_OPERATE pOperate);
private:
    int       m_FuncState;
};
#endif // EFFECTFUNCTION_H
