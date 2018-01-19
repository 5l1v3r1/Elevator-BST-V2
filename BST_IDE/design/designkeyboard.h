#ifndef DESIGNKEYBOARD_H
#define DESIGNKEYBOARD_H

#include "designbase.h"

class DesignKeyboard : public DesignBase
{
    Q_OBJECT
public:
    DesignKeyboard(QGraphicsItem *parent, QRectF pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter *p);
    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList(){}
    void ExecAction(QAction *pAction){}
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
};

#endif // EFFECTKEYBOARD_H
