#ifndef EFFECTIMAGE_H
#define EFFECTIMAGE_H

#include "designbase.h"

class EffectImage : public EffectBase
{
    Q_OBJECT
public:
    EffectImage(QWidget *parent, QRect pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(int pIndex, AREA_OPERATE pOperate);
private:
    int            m_ImageIndex;
};

#endif // EFFECTIMAGE_H
