#ifndef EFFECTVIDEO_H
#define EFFECTVIDEO_H

#include "basevideo.h"
#include "effectbase.h"

class EffectVideo : public EffectBase
{
    Q_OBJECT
public:
    EffectVideo(QWidget *parent, QRect pRect, qreal pAngle=0);
    ~EffectVideo();
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
    void updateEffect(int pIndex);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    int             m_FilmIndex;
    bool            m_Repeat;
    basevideo       *m_Video;
    qreal           m_ScreenRotate;
public slots:
    void slot_finished();
};

#endif // EFFECTVIDEO_H
