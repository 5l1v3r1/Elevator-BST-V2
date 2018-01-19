#ifndef DESIGNQML_H
#define DESIGNQML_H

#include "designbase.h"
#include <QDeclarativeItem>

class DesignQML : public DesignBase
{
    Q_OBJECT
public:
    DesignQML(QGraphicsItem *parent, QRectF pRect);
    ~DesignQML()
    {
        UnloadQML();
        if(m_QMLEngine)
            m_QMLEngine->deleteLater();
    }
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter *p);
    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();
    void ExecAction(QAction *pAction);
    void UnloadQML();
    bool LoadQML(QString pFileName);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(int pState);
private:
    int                 m_CurRcLabel;
    QDeclarativeItem   *m_QMLItem;
    QDeclarativeEngine *m_QMLEngine;
};

#endif // DESIGNQML_H
