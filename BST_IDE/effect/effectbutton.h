#ifndef EFFECTBUTTONGRID_H
#define EFFECTBUTTONGRID_H

#include "effectbase.h"

class EffectButton : public EffectBase
{
    Q_OBJECT
public:
    EffectButton(QWidget *parent, QRect pRect);

    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(AREA_OPERATE pOperate);
private:
    RC_POINTER          m_RcPointer;
    BUTTON_STATE        m_ButtonState;
    int                 m_ItemWSpacer,m_ItemHSpacer;
};

class EffectFloorButton : public EffectBase
{
    Q_OBJECT
public:
    EffectFloorButton(QWidget *parent, QRect pRect);

    bool InitEffectPara(COM_INFO *pComInfo);
    bool InitEffectPara(COM_STRUCT *pComInfo);

    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(AREA_OPERATE pOperate);

    void PrepareRc(BUTTON_STATE pState);
    int GetBtnFromPos(QPoint pPoint);
    bool AddButtonItem(int pKeyNum, QString pBtnNum, int pRow, int pColumn);

private:
    QHash<int, FloorInfo*>  m_FloorInfo;  //>@
    FloorInfo*              m_CurFloorInfo;//>@当前点击的按钮信息
    int                     m_ButtonNum,m_RowNum,m_ColunmNum;
    int                     m_ItemWSpacer,m_ItemHSpacer;
    int                     m_ItemWidth,m_ItemHeight;
    int                     m_CurKeyNum;
};

inline int EffectFloorButton::GetBtnFromPos(QPoint pPoint)
{
    int row=-1, column=-1;
    if(m_ItemWidth==0 || m_ItemHeight==0)
    {
        IDE_TRACE();
        return -1;
    }
    row = (qreal)pPoint.y() / m_ItemHeight;
    column = (qreal)pPoint.x() / m_ItemWidth;
    //>@计算按钮的有效点击区域，在此处可以修改按钮的点击灵敏度
    if(pPoint.y() <= row*(qreal)(m_ItemHeight) + m_ItemHSpacer ||
       pPoint.y() >= (row+1)*(qreal)(m_ItemHeight) - m_ItemHSpacer)
    {
        IDE_TRACE();
        return -1;
    }
    if(pPoint.x() <= column*(qreal)(m_ItemWidth) + m_ItemWSpacer ||
       pPoint.x() >= (column+1)*(qreal)(m_ItemWidth) - m_ItemWSpacer)
    {
        IDE_TRACE();
        return -1;
    }
    return (row*m_ColunmNum + column);
}

#endif // EFFECTBUTTONGRID_H
