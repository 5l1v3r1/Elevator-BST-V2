#ifndef EFFECTARROW_H
#define EFFECTARROW_H

#include "effectbase.h"
#include "parserbase.h"

/**********************************************
********    显示状态       |       动作
********----------------------------------------
********    上行          |       切换
********    下行          |       静止
********    上下行         |       运行
********    初始          |
**********************************************/

class EffectArrow : public EffectBase
{
    Q_OBJECT
public:
    EffectArrow(QWidget *parent, QRect pRect);

    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);   
    bool PaintEffect(QPainter &p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    //>@pState表示希望到达的状态，pOperate表示在此状态下所进行的操作。
    void updateEffect(int pState, AREA_OPERATE pOperate);
private:
    int   m_CurRcLabel;
};

#endif // EFFECTARROW_H
