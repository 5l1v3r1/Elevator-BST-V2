#ifndef DESIGNTEXT_H
#define DESIGNTEXT_H

#include "designbase.h"

class DesignText : public DesignBase
{
    Q_OBJECT
public:
    DesignText(QGraphicsItem *parent, QRectF pRect);
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
    void updateEffect(int pState, AREA_OPERATE pOperate);
    void Paint2D_TextRoll(QPainter *p, QRectF targetRect);
private:
    int            m_TextWidth, m_TextHeight;
    int            m_TextState;
};

#endif // EFFECTTEXT_H
