#ifndef DESIGNARROW_H
#define DESIGNARROW_H

#include "designbase.h"

class DesignArrow : public DesignBase
{
    Q_OBJECT
public:
    DesignArrow(QGraphicsItem *parent, QRectF pRect);

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
    //>@pState表示希望到达的状态，pOperate表示在此状态下所进行的操作。
    void updateEffect(int pState, AREA_OPERATE pOperate);
private:
    int   m_CurRcLabel;
};

#endif // DESIGNARROW_H
