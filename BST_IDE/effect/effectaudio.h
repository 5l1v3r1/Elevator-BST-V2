#ifndef EFFECTAUDIO_H
#define EFFECTAUDIO_H

#include "baseaudio.h"
#include "effectbase.h"

class EffectAudio : public EffectBase
{
    Q_OBJECT
public:
    EffectAudio(QWidget *parent, QRect pRect);
    ~EffectAudio();
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(int pIndex);
private:
    int             m_CurRcLabel;
    bool            m_Repeat;
    baseaudio      *m_BaseAudio;
public slots:
    void slot_finished();
};

#endif // EFFECTAUDIO_H
