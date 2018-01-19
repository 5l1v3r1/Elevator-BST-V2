#ifndef DESIGNMESSAGE_H
#define DESIGNMESSAGE_H

#include "designbase.h"

class DesignMessage : public DesignBase
{
    Q_OBJECT
public:
    DesignMessage(QGraphicsItem *parent, QRectF pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter *p);
    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();
    void ExecAction(QAction *pAction);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(MSG_INFO pMsgInfo);

    QPixmap LoadMessage(MSG_LEVEL pMsgLevel, QString pContent, QSize pSize);

    void info(QString pContent, int pDelay);
    void warn(QString pContent, int pDelay);
    void error(QString pContent, int pDelay);
};

#endif // EFFECTMESSAGE_H
