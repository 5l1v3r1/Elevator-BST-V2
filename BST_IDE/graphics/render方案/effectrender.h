#ifndef EFFECTRENDER_H
#define EFFECTRENDER_H

#include "global.h"

class EffectRender : public QObject
{
    Q_OBJECT
public:
    explicit EffectRender(QObject *parent = 0);

    QRectF viewBoxF() {return mViewBox;}
    void setViewBox(QRectF pViewbox);

    //>@设置渲染时的效果图片
    void SetEffectRc(QString pEffectRc);

    bool InitEffectPara(QString pEffectPara);
    virtual bool InitSubEffectPara(QString pLabel, QString pContent);

    void render(QGraphicsItem *pPainter, QRectF pRect);
    void render(QPainter *pPainter);
    void render(QPainter *pPainter, QRectF pRect);

    void StartAnimate();
    void StopAnimate();
    bool isAnimateRunning();  //>@特效是否正在运行

signals:
    void sRenderFinished();
    
public slots:
    void slot_PaintFrame(int pFrame);
    void slot_EffectState(QTimeLine::State pState);

public:
    QTimeLine               mAnimator;
    QRectF                  mViewBox;

    EffectType              mEffectType;   //>@特效类型，默认为无特效
    DIRECTION               mDirection;    //>@图片特效方向
    ORIENTATION             mOrientation;  //>@特效正逆序
    int                     mLoopCount;    //>@循环次数
    int                     mFrameStart, mFrameEnd, mFrameCount;
    int                     mAnimateTime;  //>@持续时间
    int                     mDutyRatio;
    QTimeLine::CurveShape   mAnimateType;
};

class PixmapRender : public EffectRender
{
    Q_OBJECT
public:
    explicit PixmapRender(QObject *parent = 0);
    ~PixmapRender();

    //>@加载特效最终渲染的图片
    bool load(PIXMAP_POINTER pPixPtr);



public:
    PIXMAP_POINTER          mPixmapPtr;
};

#endif // EFFECTRENDER_H
