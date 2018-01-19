#ifndef DESIGNBUTTONGRID_H
#define DESIGNBUTTONGRID_H

#include "designbase.h"

class DesignAbstractButton : public DesignBase
{
    Q_OBJECT
public:
    DesignAbstractButton(QGraphicsItem *parent, QRectF pRect);
    bool InitEffect(QHash<int, QString> pPathList, QHash<AREA_OPERATE, AREA_ANIMATE*>  pEffectGroup);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter *p);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void updateEffect(AREA_OPERATE pOperate);
public:
    BUTTON_STATE        m_ButtonState;
};

class DesignButton : public DesignAbstractButton
{
    Q_OBJECT
public:
    DesignButton(QGraphicsItem *parent, QRectF pRect);
    QSize GetRcSize(QString pRcName){ return size().toSize(); }
    QList<QAction*> GetActionList();
    void ExecAction(QAction *pAction);
};

class DesignFloorButton : public DesignBase
{
    Q_OBJECT
public:
    class FloorBtnInfo
    {
    public:
        FloorBtnInfo()
        {
            mButtonItem = 0;
        }
        ~FloorBtnInfo()
        {
            if(mButtonItem)
            {
                mButtonItem->scene()->removeItem(mButtonItem);
                mButtonItem->deleteLater();
            }
        }
    public:
        DesignAbstractButton       *mButtonItem;
        QRectF                      mRect;
        union PATTERN_PARA{
            struct GridPara{
            };
            struct EllipsePara{
            };
            struct GuidePara{
                QPixmap             mPixmap;
            };
        } mBtnPatternPara;
    };
public:
    DesignFloorButton(QGraphicsItem *parent, QRectF pRect);
    ~DesignFloorButton()
    {
        ReleaseFloorBtnInfoList();
    }
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter *p) {}
    QSize GetRcSize(QString pRcName);

    bool InitFloorBtnInfoList();
    void ReleaseFloorBtnInfoList();
    bool AddButton(int i, DesignAbstractButton *&pButton, QRectF pRect);
    int PrepareBtnChar(char pAscii, bool pLight);
    QString GetBtnRc(QSizeF pSize, QString pBtnText, bool pLight);

public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara) {}

public:
    QList<FloorBtnInfo*>       m_FloorBtnInfoList;  //>@
    FB_PATTERN                 m_Pattern;

private:
    int   mRow, mColunm, mCount;
    qreal mWidth, mHeight, mTextWidth, mTextHeight;
    qreal mRSpacer, mCSpacer, mItemLSpace, mItemRSpace;
    QString mRcPath;
};

class DesignKeyboardButton : public DesignBase
{
    Q_OBJECT
public:
    class KeyboardBtnInfo{
    public:
        KeyboardBtnInfo()
        {
            mButtonItem = 0;
        }
        ~KeyboardBtnInfo()
        {
            if(mButtonItem)
            {
                mButtonItem->scene()->removeItem(mButtonItem);
                mButtonItem->deleteLater();
            }
        }
    public:
        DesignAbstractButton       *mButtonItem;
        QRectF                      mRect;
        union PATTERN_PARA{
            struct GridPara{
            };
            struct EllipsePara{
            };
            struct GuidePara{
                QPixmap             mPixmap;
            };
        } mBtnPatternPara;
    };
public:
    DesignKeyboardButton(QGraphicsItem *parent, QRectF pRect);
    ~DesignKeyboardButton()
    {
        ReleaseKeyboardBtnInfoList();
    }
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter *p) {}
    QSize GetRcSize(QString pRcName){ return size().toSize(); }

    bool InitKeyboardBtnInfoList();
    void ReleaseKeyboardBtnInfoList();
    bool AddButton(KEBOARDBUTTON_RC pType, DesignAbstractButton *&pButton, QRectF pRect);

public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara) {}

public:
    QList<KeyboardBtnInfo*>    m_KeyboardBtnInfoList;  //>@
    KB_PATTERN                 m_Pattern;
};

#endif // EFFECTBUTTONGRID_H
