#ifndef DESIGNAUDIO_H
#define DESIGNAUDIO_H

#include "designbase.h"

class DesignAudio : public DesignBase
{
    Q_OBJECT
public:
    DesignAudio(QGraphicsItem *parent, QRectF pRect);
    ~DesignAudio();
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();
    void ExecAction(QAction *pAction);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(int pIndex);
public:
    int             m_CurRcLabel;
    QString         m_CurRcInfo;
};

#endif // DESIGNAUDIO_H
