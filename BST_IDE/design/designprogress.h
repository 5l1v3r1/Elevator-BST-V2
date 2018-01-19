#ifndef DESIGNPROGRESS_H
#define DESIGNPROGRESS_H
#include "designbase.h"

class DesignProgress : public DesignBase
{
    Q_OBJECT
public:
    DesignProgress(QGraphicsItem *parent, QRectF pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter *p);
    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList() {}
    void ExecAction(QAction *pAction) {}
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(int pValue, AREA_OPERATE pOperate);

    void UpdateValue(int pValue);

private:
    int            m_FontSizeValue;
    int            m_ProgressValue;
    QSize          m_FontSize;
};

#endif // EFFECTPROGRESS_H
