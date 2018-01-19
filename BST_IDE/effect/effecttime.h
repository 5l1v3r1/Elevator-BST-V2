#ifndef EFFECTTIME_H
#define EFFECTTIME_H

#include "effectbase.h"

class EffectTime : public EffectBase
{
    Q_OBJECT
public:
    EffectTime(QWidget *parent, QRect pRect);

    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(QString pLabel, QString pContent);

    bool PrepareRc();
    void PaintEffect(QPainter &p);

private:
    QTimeLine       m_Animator;
    RC_POINTER      m_RcPointer;

public slots:
    void updateEffect(QString pParameter);
};

#endif // EFFECTTIME_H
