#ifndef EFFECTBASE_H
#define EFFECTBASE_H

#include "global.h"

//#define OPENGL 1

#ifdef OPENGL
#include <QGLWidget>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtOpenGL/qglshaderprogram.h>
#include <QTime>
#include <QVector>
#include <QDir>
#include <QKeyEvent>
#endif

class EffectRC
{
public:
    EffectRC(){}

    void Init()
    {
        m_EffectImage.clear();
        m_EffectPath.clear();
        m_EffectGroup.clear();
    }

    int GetKey(QString pString);

    bool LoadPixmap(int pKey, QString pPixmapName, QSize pSize);
    bool LoadPixmap(int pKey, QString pPixmapName);
    bool LoadPath(int pKey, QString pPixmapName);
    bool LoadText(int pKey, QString pTextName, QSize pSize);

public:
    bool InitEffect(QHash<int, QPixmap> pImageList);
    bool InitEffect(QHash<int, QString> pPathList);
    bool InitEffect(COM_INFO *pComInfo);
    bool InitEffect(COM_STRUCT *pComInfo);
    virtual bool InitEffectRc(STATE_INFO* pEffect);
    bool analysisEffectPara(AREA_ANIMATE* pAreaEffect, QString pEffectPara);
    virtual bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    virtual bool Start();  //>@执行完InitEffect需要执行Start来启动特效

public:
    QHash<int, QPixmap>                 m_EffectImage;  //>@加载到内存的资源
    QHash<int, QString>                 m_EffectPath;   //>@只需要记录路径的资源
    QHash<AREA_OPERATE, AREA_ANIMATE*>  m_EffectGroup;  //>@特效描述
    bool                                m_EffectValid;  //>@特效使能
};
Q_DECLARE_METATYPE(EffectRC)

inline int EffectRC::GetKey(QString pString)
{
    if(pString.isEmpty())
        return -1;
    if(pString.startsWith("Pic",Qt::CaseInsensitive) == false)
        return -1;
    pString.remove(0, 3);
    bool ok = false;
    int index = pString.toInt(&ok, 10);
    if(ok == false)
        return -1;
    return index;
}

inline bool EffectRC::LoadPixmap(int pKey, QString pPixmapName, QSize pSize)
{
    if(pPixmapName.isEmpty())
        return false;
    FILEPATH_TYPE tmpType = RegulatRcFile(pPixmapName);
    if(tmpType == PATH_TYPENONE || tmpType == PATH_TYPENETADDR)
        return false;
    if(!pSize.isValid())
        return false;
    QImage tmpImage;
    if(tmpImage.load(pPixmapName) == false)
        return false;
    m_EffectImage.insert(pKey, QPixmap::fromImage(tmpImage.scaled(pSize, Qt::IgnoreAspectRatio)));
    return true;
}

inline bool EffectRC::LoadPixmap(int pKey, QString pPixmapName)
{
    if(pPixmapName.isEmpty())
        return false;
    FILEPATH_TYPE tmpType = RegulatRcFile(pPixmapName);
    if(tmpType == PATH_TYPENONE || tmpType == PATH_TYPENETADDR)
        return false;
    QImage tmpImage;
    if(tmpImage.load(pPixmapName) == false)
        return false;
    m_EffectImage.insert(pKey, QPixmap::fromImage(tmpImage));
    return true;
}

inline bool EffectRC::LoadPath(int pKey, QString pPixmapName)
{

    if(pPixmapName.isEmpty())
        return false;
    if(RegulatRcFile(pPixmapName) == PATH_TYPENONE)
    {
        //IDE_TRACE_STR(pPixmapName);
        return false;
    }
    m_EffectPath.insert(pKey, pPixmapName);
    return true;
}

inline bool EffectRC::LoadText(int pKey, QString pTextName, QSize pSize)
{
    if(pTextName.isEmpty())
        return false;
    QImage tmpImage(pSize.width(), pSize.height(), QImage::Format_ARGB32_Premultiplied);
    if(tmpImage.isNull())
        return false;
    tmpImage.fill(Qt::transparent); //>@创建透明图层
    QPainter p(&tmpImage);
    //>@计算最优的文本绘制位置
    WriteText(&p, pTextName, QRect(QPoint(0,0), pSize), PRIOR_HEIGHT);
    p.end();
    m_EffectImage.insert(pKey, QPixmap::fromImage(tmpImage));
    return true;
}

class EffectBase : public QWidget, public EffectRC
{
    Q_OBJECT
public:
    explicit EffectBase(QWidget *parent, QRect pRect);
    ~EffectBase();
    virtual bool InitEffectRc(STATE_INFO* pEffect);
    virtual bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    virtual bool Start();
public slots:
    void updateEffect(AREA_OPERATE, quint32);
    virtual void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    void paintEvent(QPaintEvent *);
    virtual bool PaintEffect(QPainter &p);
    void Play();
    void Stop();
    bool SetEnvironment(QString pEnvirString);  //>@设置COM的现场环境
    bool GetEnvironment(QString &pEnvirString); //>@获取COM的现场环境
    //>@任何动作最多只需要两步，如果有两步，则第一步肯定是状态CHANGE，第二步一定是状态的操作变化
    void OperateStep0();
public slots:
    void OperateStep1();
public:
    QTimeLine                           m_Animator;
    RCTYPE                              m_RcType;
    QRect                               m_Rect;         //>@区域大小

    AREA_ANIMATE                       *m_AreaAnimate;
    OPERATE_INFO                        m_OperateInfo[2];
    PIXMAP_POINTER                      m_PixmapPointer;

public:
    bool DrawPixmap(QPainter &p, QPixmap pPixmap, QSize pSize, QRect pTargetRect, QRect pSourceRect);
    bool DrawPixmap(QPainter &p, QPixmap pPixmap, QRect pTargetRect, QRect pSourceRect);
    bool DrawPixmap(QPainter &p, QPixmap pPixmap, QRect pTargetRect);
    bool DrawPixmap(QPainter &p, QPixmap pPixmap);
    void Paint2D_None(QPainter &p, QRect targetRect);
    void Paint2D_Flip(QPainter &p,QRect targetRect);
    void Paint2D_Roll(QPainter &p,QRect targetRect);
    void Paint2D_Zoom(QPainter &p,QRect targetRect);
    void Paint2D_Blink(QPainter &p,QRect targetRect);
    void Paint2D_Rotate(QPainter &p,QRect targetRect);
    void Paint2D_Slipcycle(QPainter &p,QRect targetRect);
    void Paint2D_Transparent(QPainter &p,QRect targetRect);
    void Paint2D_Spread(QPainter &p,QRect targetRect);
    void Paint2D_Wheel(QPainter &p,QRect targetRect);
    void StartTimeline(int pStart, int pEnd, int pInterval, QTimeLine::CurveShape pShape);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

inline bool EffectBase::PaintEffect(QPainter &p)
{
    if(m_EffectValid == false)
        return false;
    if(m_AreaAnimate == 0)
    {
        Paint2D_None(p, rect());
        return false;
    }
    return true;
}

inline void EffectBase::StartTimeline(int pStart, int pEnd, int pInterval, QTimeLine::CurveShape pShape)
{
    m_Animator.stop();
    m_Animator.setFrameRange(pStart, pEnd);
    m_Animator.setDuration(pInterval);
    m_Animator.setCurveShape(pShape);
    m_Animator.start();
}

inline bool EffectBase::DrawPixmap(QPainter &p, QPixmap pPixmap, QSize pSize, QRect pTargetRect, QRect pSourceRect)
{
    if(pTargetRect.isValid() == false || pSourceRect.isValid() == false || pSize.isValid() == false)
    {
        return false;
    }
    if(pPixmap.isNull() == false)
    {
        p.drawPixmap(pTargetRect.left(), pTargetRect.top(), pTargetRect.width(), pTargetRect.height(),
                     pPixmap.scaled(pSize.width(),
                                     pSize.height(),
                                     Qt::IgnoreAspectRatio,
                                     Qt::SmoothTransformation),
                    pSourceRect.left(), pSourceRect.top(), pSourceRect.width(), pSourceRect.height());
        return true;
    }
    return false;
}

inline bool EffectBase::DrawPixmap(QPainter &p, QPixmap pPixmap, QRect pTargetRect, QRect pSourceRect)
{
    if(pTargetRect.isValid() == false || pSourceRect.isValid() == false)
    {
        //IDE_TRACE();
        return false;
    }
    if(pPixmap.isNull() == false)
    {
        p.drawPixmap(pTargetRect.left(), pTargetRect.top(), pTargetRect.width(), pTargetRect.height(),
                     pPixmap,
                     pSourceRect.left(), pSourceRect.top(), pSourceRect.width(), pSourceRect.height());
        return true;
    }
    return false;
}

inline bool EffectBase::DrawPixmap(QPainter &p, QPixmap pPixmap, QRect pTargetRect)
{
    if(pTargetRect.isValid() == false)
    {
        //IDE_TRACE();
        return false;
    }
    if(pPixmap.isNull() == false)
    {
        p.drawPixmap(pTargetRect, pPixmap);
        return true;
    }
    //IDE_TRACE();
    return false;
}

inline bool EffectBase::DrawPixmap(QPainter &p, QPixmap pPixmap)
{
    if(pPixmap.isNull() == false)
    {
        p.drawPixmap(0, 0, pPixmap);
        return true;
    }
    return false;
}

inline void EffectBase::Paint2D_None(QPainter &p, QRect targetRect)
{
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect);
}

//>@需要绘制底图和特效图片
inline void EffectBase::Paint2D_Flip(QPainter &p, QRect targetRect)
{
    qreal w = targetRect.width();
    qreal h = targetRect.height();
    qreal hw = w / 2.0;
    qreal hh = h / 2.0;
    // behind is the new pixmap
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect);
    if(m_Animator.state() != QTimeLine::Running)
        return;
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart;
    if(FrameCount <= 0)
        return;
    qreal halfcount =  FrameCount / 2.0;
    QTransform transform;
    qreal index = m_Animator.currentFrame();

    p.save();
    if (index <= halfcount)
    {
        // the top part of the old pixmap is flipping
        qreal angle;
        if(m_AreaAnimate->mOrientation == Positive)
        {
            angle = -180.0 * index / FrameCount;
        }
        else
        {
            angle = 180.0 * index / FrameCount;
        }
        transform.translate(targetRect.left()+hw, targetRect.top()+hh);
        if(m_AreaAnimate->mDirection == Horizontal)
            transform.rotate(angle, Qt::YAxis);
        else
            transform.rotate(angle, Qt::XAxis);
        p.setTransform(transform);
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(-hw, -hh, w, h), QRect(0,0,w,h));

        // the bottom part is still the old pixmap
        p.resetTransform();
        if(m_AreaAnimate->mDirection == Horizontal)
            if(m_AreaAnimate->mOrientation == Positive)
                p.setClipRect(hw, 0, hw, targetRect.height());
            else
                p.setClipRect(0, 0, hw, targetRect.height());
        else
            if(m_AreaAnimate->mOrientation == Positive)
                p.setClipRect(0, hh, targetRect.width(), hh);
            else
                p.setClipRect(0, 0, targetRect.width(), hh);
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(0,0,w,h), QRect(0,0,w,h));
    }
    else
    {
        transform.translate(targetRect.left(), targetRect.top());
        p.setTransform(transform);
        if(m_AreaAnimate->mDirection == Horizontal)
            if(m_AreaAnimate->mOrientation == Positive)
                p.setClipRect(hw, 0, hw, targetRect.height());
            else
                p.setClipRect(0, 0, hw, targetRect.height());
        else
            if(m_AreaAnimate->mOrientation == Positive)
                p.setClipRect(0, hh, targetRect.width(), hh);
            else
                p.setClipRect(0, 0, targetRect.width(), hh);

        // the bottom part is still the old pixmap
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(0,0,w,h), QRect(0,0,w,h));
        p.resetTransform();
        // the bottom part of the new pixmap is flipping
        qreal angle;
        if(m_AreaAnimate->mOrientation == Positive)
        {
            angle = 180.0 - 180.0 * index / FrameCount;
        }
        else
        {
            angle = - 180.0 * (FrameCount-index) / FrameCount;
        }
        transform.translate(targetRect.left()+hw, targetRect.top()+hh);
        if(m_AreaAnimate->mDirection == Horizontal)
            transform.rotate(angle, Qt::YAxis);
        else
            transform.rotate(angle, Qt::XAxis);
        p.setTransform(transform);
        DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(-hw, -hh, w, h), QRect(0,0,w,h));
    }
    p.restore();
}

//>@需要绘制底图和特效图片
inline void EffectBase::Paint2D_Roll(QPainter &p, QRect targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    qreal tmpDiff;
    if(m_Animator.state() != QTimeLine::Running)
        return;
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
    {
        //IDE_TRACE();
        return;
    }
    qreal index = m_Animator.currentFrame();
    qreal x, y;
    //>@首先将坐标原点移到当前需要绘制的区域。
    p.save();
    QTransform transform1, transform2;
    if(m_AreaAnimate->mDirection == Vertical)
    {
        tmpDiff = hh / FrameCount;
        if(m_AreaAnimate->mOrientation == Positive)
        {
            y = (qreal)(tmpDiff*index);
            transform1.translate(targetRect.left(), targetRect.top()+y);
            p.setTransform(transform1);
            DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(0,0,hw,hh-y), QRect(0,0,hw,hh-y));
        }
        else
        {
            y = (qreal)(tmpDiff*index);
            transform1.translate(targetRect.left(), targetRect.top()-y);
            p.setTransform(transform1);
            DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(0,y,hw,hh-y), QRect(0,y,hw,hh-y));
        }
        p.resetTransform();
        if(m_AreaAnimate->mOrientation == Positive)
        {
            y = hh - y;
            transform2.translate(targetRect.left(), targetRect.top()-y);
            p.setTransform(transform2);
            DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(0,y,hw,hh-y), QRect(0,y,hw,hh-y));
        }
        else
        {
            y = hh - y;
            transform2.translate(targetRect.left(), targetRect.top()+y);
            p.setTransform(transform2);
            DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(0,0,hw,hh-y), QRect(0,0,hw,hh-y));
        }
    }
    else
    {
        tmpDiff = (qreal)(hw) / FrameCount;
        if(m_AreaAnimate->mOrientation == Positive)
        {
            x = (qreal)(tmpDiff*index);
            transform1.translate(targetRect.left()+x, targetRect.top());
            p.setTransform(transform1);
            DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(0,0,hw-x,hh), QRect(0,0,hw-x,hh));
        }
        else
        {
            x = (qreal)(tmpDiff*index);
            transform1.translate(targetRect.left()-x, targetRect.top());
            p.setTransform(transform1);
            DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(x,0,hw-x,hh), QRect(x,0,hw-x,hh));
        }
        p.resetTransform();
        if(m_AreaAnimate->mOrientation == Positive)
        {
            x = hw - x;
            transform2.translate(targetRect.left()-x, targetRect.top());
            p.setTransform(transform2);
            DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(x,0,hw-x,hh), QRect(x,0,hw-x,hh));
        }
        else
        {
            x = hw - x;
            transform2.translate(targetRect.left()+x, targetRect.top());
            p.setTransform(transform2);
            DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(0,0,hw-x,hh), QRect(0,0,hw-x,hh));
        }
    }
    p.restore();
}

//>@需要绘制底图和特效图片
inline void EffectBase::Paint2D_Zoom(QPainter &p, QRect targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        //IDE_TRACE();
        return;
    }
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
    {
        //IDE_TRACE();
        return;
    }
    qreal halfcount =  FrameCount / 2.0;
    qreal index = m_Animator.currentFrame();

    qreal x,y,w,h;
    p.save();
    p.translate(targetRect.left()+hw/2, targetRect.top()+hh/2);
    if(index < halfcount)
    {
        index = halfcount - index;
        x = -hw*index/FrameCount;
        y = -hh*index/FrameCount;
        w = 2*hw*index/FrameCount;
        h = 2*hh*index/FrameCount;
        DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(x,y,w,h), QRect(0,0,hw,hh));
    }
    else
    {
        index = index - halfcount;
        x = -hw*index/FrameCount;
        y = -hh*index/FrameCount;
        w = 2*hw*index/FrameCount;
        h = 2*hh*index/FrameCount;
        DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(x,y,w,h), QRect(0,0,hw,hh));
    }
    p.restore();
}

inline void EffectBase::Paint2D_Blink(QPainter &p,QRect targetRect)
{
    if(m_Animator.state() != QTimeLine::Running)
    {
        p.setOpacity(1);
        Paint2D_None(p, targetRect);
        return;
    }
    int index = m_Animator.currentFrame();
    if(index % 2)
        p.setOpacity(0.5);
    else
        p.setOpacity(1);
    Paint2D_None(p, targetRect);
}

//>@需要绘制底图和特效图片
inline void EffectBase::Paint2D_Rotate(QPainter &p, QRect targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        //IDE_TRACE();
        return;
    }
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
    {
        //IDE_TRACE();
        return;
    }
    qreal halfCount =  FrameCount / 2.0;
    qreal index = m_Animator.currentFrame();
    qreal scaleCount1 = (qreal)FrameCount*1.0/20.0;
    qreal scaleCount2 = (qreal)FrameCount*19.0/20.0;
    if(scaleCount1<=0 || scaleCount2<=0)
    {
        //IDE_TRACE();
        return;
    }
    qreal pad = hw / 10.0;
    p.save();
    QRect fr = targetRect.adjusted(-pad, -pad, pad, pad);
    p.setClipRect(fr);
    QTransform transform;
    transform.translate(targetRect.left()+hw/2.0, targetRect.top()+hh/2.0);
    QPixmap pix = (index <= halfCount) ? m_PixmapPointer.mLastPointer : m_PixmapPointer.mCurPointer;
    if(index <= scaleCount1)
    {
        qreal diff = (qreal)pad * index / (qreal)scaleCount1;
        p.setTransform(transform);
        if(m_AreaAnimate->mDirection == Horizontal)
        {
            DrawPixmap(p, pix,
                       QRect(-hw/2.0, -hh/2.0+diff, hw, hh-2*diff),
                       QRect(0,0,hw,hh));
        }
        else
        {
            DrawPixmap(p, pix,
                       QRect(-hw/2.0+diff, -hh/2.0, hw-2*diff, hh),
                       QRect(0,0,hw,hh));
        }
    }
    else if(index > scaleCount2)
    {
        qreal diff = (qreal)pad * (FrameCount - index) / (qreal)scaleCount1;
        p.setTransform(transform);
        if(m_AreaAnimate->mDirection == Horizontal)
        {
            DrawPixmap(p, pix,
                       QRect(-hw / 2.0, -hh / 2.0 + diff, hw, hh - 2*diff),
                       QRect(0,0,hw,hh));
        }
        else
        {
            DrawPixmap(p, pix,
                       QRect(-hw / 2.0 + diff, -hh / 2.0, hw - 2*diff, hh),
                       QRect(0,0,hw,hh));
        }
    }
    else
    {
        qreal index1 = index - scaleCount1;
        qreal angle1, angle2;
        if(m_AreaAnimate->mOrientation == Positive)
        {
            angle1 = -180.0 * index1 / FrameCount;
            angle2 = 180.0 - 180.0 * index1 / FrameCount;
        }
        else
        {
            angle1 = 180.0 * index1 / FrameCount;
            angle2 = - 180.0 * (FrameCount-index1) / FrameCount;
        }
        qreal angle = (index <= halfCount) ? angle1 : angle2;
        if(m_AreaAnimate->mDirection == Horizontal)
        {
            transform.rotate(angle, Qt::YAxis);
            p.setTransform(transform);
            DrawPixmap(p, pix, QRect(-hw / 2.0, -hh / 2.0 + pad, hw, hh - 2*pad));
        }
        else
        {
            transform.rotate(angle, Qt::XAxis);
            p.setTransform(transform);
            DrawPixmap(p, pix, QRect(-hw / 2.0 + pad, -hh / 2.0, hw - 2*pad, hh));
        }
    }
    p.restore();
}

//>@只需要绘制特效图片即可
inline void EffectBase::Paint2D_Wheel(QPainter &p, QRect targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        //IDE_TRACE();
        return;
    }
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
    {
        //IDE_TRACE();
        return;
    }
    qreal index = m_Animator.currentFrame();

    qreal tmpDiff = (qreal)(2.0 * PI) * 2.0 / (qreal)FrameCount;
    qreal rotate = 360.0 * normalizeAngle(tmpDiff*index) / (qreal)(2.0 * PI);

    QTransform transform;
    p.save();
    transform.translate(targetRect.left()+hw/2, targetRect.top()+hh/2);
    transform.rotate(rotate);
    p.setTransform(transform);
    DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(-hw/2.0, -hh/2.0, hw, hh), QRect(0,0,hw,hh));
    p.restore();
}

//>@需要绘制底图和特效图片
inline void EffectBase::Paint2D_Slipcycle(QPainter &p, QRect targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
        return;
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
        return;
    qreal halfCount =  FrameCount / 2.0;
    if(halfCount <= 0)
        return;
    qreal index = m_Animator.currentFrame();

    p.save();
    QPixmap tmpPixmap;
    qreal x, y, tmpDiff;
    if(m_AreaAnimate->mDirection == Vertical)
    {
        tmpDiff = (qreal)(hh) / halfCount;
        if(index <= halfCount)
        {
            tmpPixmap = m_PixmapPointer.mLastPointer;
            y = (int)(tmpDiff*index);
            if(y < 0)
                y = 0;
            if(m_AreaAnimate->mOrientation == Positive)
            {
                p.translate(targetRect.left(), targetRect.top()+y);
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, QRect(0,0,hw,hh-y), QRect(0,0,hw,hh-y));
            }
            else
            {
                p.translate(targetRect.left(), targetRect.top()-y);
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, QRect(0,y,hw,hh-y), QRect(0,y,hw,hh-y));
            }
        }
        else
        {
            y = (int)(tmpDiff*(m_AreaAnimate->mFrameEnd - index));
            tmpPixmap = m_PixmapPointer.mCurPointer;
            if(y < 0)
                y = 0;
            if(m_AreaAnimate->mOrientation == Positive)
            {
                p.translate(targetRect.left(), targetRect.top()+y);
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, QRect(0,0,hw,hh-y), QRect(0,0,hw,hh-y));
            }
            else
            {
                p.translate(targetRect.left(), targetRect.top()-y);
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, QRect(0,y,hw,hh-y), QRect(0,y,hw,hh-y));
            }
        }
    }
    else
    {
        tmpDiff = (qreal)(hw) / halfCount;
        if(index <= halfCount)
        {
            x = (int)(tmpDiff*index);
            if(x < 0)
                x = 0;
            tmpPixmap = m_PixmapPointer.mLastPointer;
            if(m_AreaAnimate->mOrientation == Positive)
            {
                p.translate(targetRect.left()+x, targetRect.top());
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, QRect(0,0,hw-x,hh), QRect(0,0,hw-x,hh));
            }
            else
            {
                p.translate(targetRect.left()-x, targetRect.top());
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, QRect(x,0,hw-x,hh), QRect(x,0,hw-x,hh));
            }
        }
        else
        {
            x = (int)(tmpDiff*(m_AreaAnimate->mFrameEnd - index));
            if(x < 0)
                x = 0;
            tmpPixmap = m_PixmapPointer.mCurPointer;
            if(m_AreaAnimate->mOrientation == Positive)
            {
                p.translate(targetRect.left()+x, targetRect.top());
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, QRect(0,0,hw-x,hh), QRect(0,0,hw-x,hh));
            }
            else
            {
                p.translate(targetRect.left()-x, targetRect.top());
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, QRect(x,0,hw-x,hh), QRect(x,0,hw-x,hh));
            }
        }
    }
    p.restore();
}

//>@需要绘制底图和特效图片
inline void EffectBase::Paint2D_Transparent(QPainter &p, QRect targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        //IDE_TRACE();
        return;
    }
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
    {
        //IDE_TRACE();
        return;
    }
    qreal Stage1 = FrameCount*3.0/10.0;
    qreal Stage2 = FrameCount*4.0/10.0;
    //qreal Stage3 = (qreal)FrameCount*3.0/10.0;

    qreal index = m_Animator.currentFrame();
    //>@循环改变透明度图片
    qreal tmpLastAlpha, tmpAlpha;
    if(index <= Stage1)
    {
        tmpLastAlpha = (qreal)(Stage1 - index) / (qreal)Stage1;;
        tmpAlpha = (qreal)(index) / (qreal)Stage1;
    }
    else if(index <= Stage2 + Stage1)
    {
        tmpLastAlpha = 0;
        tmpAlpha = 1;
    }
    else  //>@必须得添加，否则一次动画完成后也即index>Stage2 + Stage1时会将两幅图片都进行透明处理
    {
        tmpLastAlpha = 0;
        tmpAlpha = 1;
    }

    p.save();
    p.translate(targetRect.left(), targetRect.top());
    p.setOpacity(tmpLastAlpha);
    DrawPixmap(p, m_PixmapPointer.mLastPointer, QRect(0,0,hw,hh), QRect(0,0,hw,hh));
    p.setOpacity(tmpAlpha);
    DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(0,0,hw,hh), QRect(0,0,hw,hh));
    p.restore();
}

//>@只需要绘制特效图片即可
inline void EffectBase::Paint2D_Spread(QPainter &p, QRect targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    qreal x,y,h,w;
    if(m_Animator.state() != QTimeLine::Running)
        return;
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
        return;
    qreal halfCount =  FrameCount / 2.0;
    qreal index = m_Animator.currentFrame();

    if(index < halfCount)
    {
        index = halfCount - index;
        x = -hw*index/FrameCount;
        y = -hh*index/FrameCount;
        w = 2*hw*index/FrameCount;
        h = 2*hh*index/FrameCount;
    }
    else
    {
        index = index - halfCount;
        x = -hw*index/FrameCount;
        y = -hh*index/FrameCount;
        w = 2*hw*index/FrameCount;
        h = 2*hh*index/FrameCount;
    }
    p.save();
    p.translate(targetRect.left()+hw/2, targetRect.top()+hh/2);
    DrawPixmap(p, m_PixmapPointer.mCurPointer, QRect(x,y,w,h), QRect(0,0,hw,hh));
    p.restore();
}

#ifdef OPENGL
class EffectBaseItem;
class GLtexture : public QGLWidget
{
    Q_OBJECT
public:
    GLtexture(QWidget *parent = 0);
    ~GLtexture();
    bool loadTexture(EffectBaseItem *pEffectRc);
};
#endif

class EffectBaseItem : public QGraphicsWidget, public EffectRC
{
    Q_OBJECT
public:
    explicit EffectBaseItem(QGraphicsItem *parent, QRectF pRect);

    QRectF boundingRect() const;
    QPainterPath shape() const;

    virtual bool InitEffectRc(STATE_INFO* pEffect);
    virtual bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    virtual bool Start();
    virtual void updateEffect(AREA_OPERATE pOperate, QVariant pPara);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void PaintEffect(QPainter *p);
    void Play();
    void Stop();
    //>@任何动作最多只需要两步，如果有两步，则第一步肯定是状态CHANGE，第二步一定是状态的操作变化
    void OperateStep0();
public slots:
    void OperateStep1();
public:
    QRectF                  m_Rect;         //>@区域大小
    QTimeLine               m_Animator;
    RCTYPE                  m_RcType;

    AREA_ANIMATE           *m_AreaAnimate;
    OPERATE_INFO            m_OperateInfo[2];
    PIXMAP_POINTER          m_PixmapPointer;
public:
    void StartTimeline(int pStart, int pEnd, int pInterval, QTimeLine::CurveShape pShape);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

#ifdef OPENGL
public:
    void setGeometry3D(qreal x, qreal y, qreal width, qreal height);
    void setGeometry3D(QRectF rect);
    void BindTexture(GLuint L_Texture,GLfloat R,GLfloat G,GLfloat B);
    void UnbindTexture();

    void OpenGL_PaintBegin(QPainter *painter);
    void OpenGL_PaintEnd(QPainter *painter);
    void OpenGL_Effect(QPainter *painter);

    void Paint3D_CircleSurface(GLuint texture);//>@绘制圆面
    void Paint3D_CamberSurface(GLuint texture);//>@绘制弧面
    //>@3D特效
    //>@绘制立方体
    void Paint3D_Cube();
    //>@绘制圆柱体
    void Paint3D_Cylinder();

    void StartRotateEffect(ROTATE_STAGE pStage, QTimeLine *TimeLine, int angle);
    void StartZoomEffect(ZOOM_STAGE pStage, QTimeLine *TimeLine, int angle);
    void StartIndentEffect(ZOOM_STAGE pStage, QTimeLine *TimeLine, int deep);
    void StartShakeEffect(QTimeLine *TimeLine,int deep);
protected:
    void keyPressEvent(QKeyEvent *event);

public:
    int                     m_Width, m_Height;
    QHash<QString, GLuint>  m_EffectTexture;
    int                     m_CurFrame;
    //>@用于XYZ三轴旋转以及深度的变化
    int                     m_XDiff,m_YDiff,m_ZDiff; //>@三个方向上的角度偏移
    qreal                   m_XScale,m_YScale,m_ZScale; //>@三个方向上的缩放
    float                   m_DeepDiff;

    GLfloat                 verPoint[7200];
    GLfloat                 verTexture[7200];
    GLfloat                 NormalFront[7200];

    friend class GLtexture;
#endif
};

inline void EffectBaseItem::StartTimeline(int pStart, int pEnd, int pInterval, QTimeLine::CurveShape pShape)
{
    m_Animator.stop();
    m_Animator.setFrameRange(pStart, pEnd);
    m_Animator.setDuration(pInterval);
    m_Animator.setCurveShape(pShape);
    m_Animator.start();
}

class KeyButton : public QPushButton
{
    Q_OBJECT
public:
    KeyButton(QWidget *parent=0);
    void InitRc(QPixmap pNormal, QPixmap pHighLight);
    void InitText(QString pText);
    void paintEvent(QPaintEvent *p);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
public:
    QPixmap     m_NormalBg,
                m_HightlightBg;
    QPixmap     m_ButtonBg;
    QString     m_Text;
    int         m_FontSize;
    QRect       m_TextRect;
};

#endif // EFFECTBASE_H
