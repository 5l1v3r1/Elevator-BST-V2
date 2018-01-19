#ifndef DESIGNVIDEO_H
#define DESIGNVIDEO_H

#include "designbase.h"

class DesignVideo : public DesignBase
{
    Q_OBJECT
public:
    DesignVideo(QGraphicsItem *parent, QRectF pRect);
    ~DesignVideo();
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();
    void ExecAction(QAction *pAction);
    void updateEffect(int pIndex);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    int             m_FilmIndex;
};

#endif // EFFECTVIDEO_H
