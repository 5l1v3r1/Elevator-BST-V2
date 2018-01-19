#ifndef EFFECTPROGRESS_H
#define EFFECTPROGRESS_H
#include "effectbase.h"

class EffectProgress : public EffectBase
{
    Q_OBJECT
public:
    EffectProgress(QWidget *parent, QRect pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);

    void updateEffect(int pValue, AREA_OPERATE pOperate);

public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    void UpdateValue(quint32 pValue);

private:
    int            m_FontSizeValue;
    int            m_ProgressValue;
    QSize          m_FontSize;
};

#endif // EFFECTPROGRESS_H
