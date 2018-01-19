#ifndef EFFECTMESSAGE_H
#define EFFECTMESSAGE_H
#include "effectbase.h"

class EffectMessage : public EffectBase
{
    Q_OBJECT
public:
    EffectMessage(QWidget *parent, QRect pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
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
