#ifndef DESIGNBASE_H
#define DESIGNBASE_H

#include "global.h"
#include "protocolinc.h"
#include <QGLWidget>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtOpenGL/qglshaderprogram.h>
#include <QTime>
#include <QVector>
#include <QDir>
#include <QKeyEvent>
#include <QSvgRenderer>

/*
更新流程：
    1、从设备仅下载ui文件，并在上位机打开，并自动创建工程目录（如果用户需要修改某个com，必须点击download下载对应的所有资源）
    2、根据用户操作下载某个COM对应的所有资源，并下载到工程目录的对应路径(Resource路径)
    3、修改完成后，点击整体上传按钮可以将当前工程压缩为一个升级主题包，然后上传至设备
*/

class DesignRC
{
public:
    DesignRC(){Init();}

    void Init()
    {
        m_EffectPath.clear();
        m_EffectGroup.clear();
        m_EffectTexture.clear();
        m_ComInfo = 0;
    }

    int GetKey(QString pString);

    bool LoadPath(int pKey, QString pPixmapName);
    QPixmap GetPixmap(int pKey, QSizeF pSize=QSizeF());  //>@从m_EffectPath中加载图片

public:
    bool InitEffect(COM_INFO *pComInfo);
    virtual bool InitEffectRc(STATE_INFO* pEffect);
    bool analysisEffectPara(AREA_ANIMATE* pAreaEffect, QString pEffectPara);
    virtual bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool isValid();
    virtual bool Start();  //>@执行完InitEffect需要执行Start来启动特效
public:
    COM_INFO *                          m_ComInfo;
    QString                             m_ComName;
    COM_TYPE                            m_ComType;
    QHash<int, QString>                 m_EffectPath;   //>@只需要记录路径的资源
    QHash<int, GLuint>                  m_EffectTexture;
    QHash<AREA_OPERATE, AREA_ANIMATE*>  m_EffectGroup;  //>@特效描述
    bool                                m_EffectValid;  //>@特效使能
};
Q_DECLARE_METATYPE(DesignRC)

inline int DesignRC::GetKey(QString pString)
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

inline bool DesignRC::LoadPath(int pKey, QString pPath)
{
    if(pPath.isEmpty())
        return false;
    if(RegulatRcFile(pPath) == PATH_TYPENONE)
    {
        IDE_TRACE_STR(pPath);
        return false;
    }
    m_EffectPath.insert(pKey, pPath);
    return true;
}

inline QRectF ToSceneRect(QRectF pRect) { return QRectF(pRect.left() + StartPosX, pRect.top() + StartPosY, pRect.width(), pRect.height()); }
inline QRectF FromSceneRect(QRectF pRect) { return QRectF(pRect.left() - StartPosX, pRect.top() - StartPosY, pRect.width(), pRect.height()); }

class DesignBase;
class DesignGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    DesignGLWidget(QWidget *parent = 0);
    ~DesignGLWidget();
    bool InitTexture(DesignBase *pDesignWidget);
    GLuint GetTexture(QString pImagePath, QSizeF pSize);
};

class DesignBase : public QGraphicsWidget, public DesignRC
{
    Q_OBJECT
public:
    friend class DesignGLWidget;
    DesignBase(QGraphicsItem *parent, QRectF pRect);

    QRectF boundingRect() const;
    QPainterPath shape() const;

    void SetDesignGeometry(QRectF rect);    //>@rect是以（20，20）为坐标原点的坐标
    QRectF GetDesignGeometry();             //>@返回的rect是以（20，20）为坐标原点的坐标
    void setGeometry3D(qreal x, qreal y, qreal width, qreal height);
    void setGeometry3D(QRectF rect);
    void BindTexture(GLuint L_Texture,GLfloat R,GLfloat G,GLfloat B);
    void UnbindTexture();

    virtual bool InitEffectRc(STATE_INFO* pEffect);
    virtual bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    virtual bool Start();
    virtual void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    QPixmap LoadText(QString pTextPath, QSizeF pSize);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual bool PaintEffect(QPainter *p);
    void OpenGL_PaintBegin(QPainter *painter);
    void OpenGL_PaintEnd(QPainter *painter);
    void OpenGL_Effect(QPainter *painter);
    void StopAnimate();

    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList()
    {
        QList<QAction*> tmpList;
        return tmpList;
    }
    virtual void ExecAction(QAction *pAction);

    //>@任何动作最多只需要两步，如果有两步，则第一步肯定是状态CHANGE，第二步一定是状态的操作变化
    void OperateStep0();
public slots:
    void UpdateCom(int pFrame);
    void OperateStep1();
    void ActionTrigger(QAction *pAction);
signals:
    void MouseRelease(QPointF Pos);
    void reportEffect(AREA_OPERATE pOperate, QVariant pPara);
public:
    QRectF                  m_Rect;         //>@区域大小
    QTimeLine               m_Animator;
    RCTYPE                  m_RcType;

    AREA_ANIMATE           *m_AreaAnimate;
    OPERATE_INFO            m_OperateInfo[2];
    PIXMAP_POINTER          m_PixmapPointer;

    QActionGroup*          m_ActionGroup;

public:
    DesignGLWidget         *m_GLWidget;
    int                     m_3DWidth, m_3DHeight;
    //>@用于XYZ三轴旋转以及深度的变化
    int                     m_3DXDiff,m_3DYDiff,m_3DZDiff; //>@三个方向上的角度偏移
    qreal                   m_3DXScale,m_3DYScale,m_3DZScale; //>@三个方向上的缩放
    float                   m_3DDeepDiff;

    GLfloat                 m_3DVerPoint[7200];
    GLfloat                 m_3DVerTexture[7200];
    GLfloat                 m_3DNormalFront[7200];

public:
    void StartTimeline(int pStart, int pEnd, int pInterval, QTimeLine::CurveShape pShape);
    bool DrawPixmap(QPainter *p, QPixmap pPixmap, QSizeF pSize, QRectF pTargetRect, QRectF pSourceRect);
    bool DrawPixmap(QPainter *p, QPixmap pPixmap, QRectF pTargetRect, QRectF pSourceRect);
    bool DrawPixmap(QPainter *p, QPixmap pPixmap, QRectF pTargetRect);
    bool DrawFlipPixmap(QPainter *p, QPixmap pPixmap, QRectF pTargetRect);
    bool DrawPixmap(QPainter *p, QPixmap pPixmap, QRect pTargetRect) {  DrawPixmap(p, pPixmap, QRectF(pTargetRect)); }
    bool DrawPixmap(QPainter *p, QPixmap pPixmap);
    void Paint2D_None(QPainter *p, QRectF targetRect);
    void Paint2D_Flip(QPainter *p,QRectF targetRect);
    void Paint2D_Roll(QPainter *p,QRectF targetRect);
    void Paint2D_Zoom(QPainter *p,QRectF targetRect);
    void Paint2D_Rotate(QPainter *p,QRectF targetRect);
    void Paint2D_Blink(QPainter *p,QRectF targetRect);
    void Paint2D_Slipcycle(QPainter *p,QRectF targetRect);
    void Paint2D_Transparent(QPainter *p,QRectF targetRect);
    void Paint2D_Spread(QPainter *p,QRectF targetRect);
    void Paint2D_Wheel(QPainter *p,QRectF targetRect);

    void Paint3D_CircleSurface(GLuint texture);//>@绘制圆面
    void Paint3D_CamberSurface(GLuint texture);//>@绘制弧面
    void Paint3D_Cube(QPainter *p,QRectF targetRect);    //>@绘制立方体
    void Paint3D_Cylinder(QPainter *p,QRectF targetRect);//>@绘制圆柱体
    void Set3DEffect(EffectType pType);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

//>@rect是以（20，20）为坐标原点的坐标
inline void DesignBase::SetDesignGeometry(QRectF rect)
{
    setGeometry(ToSceneRect(rect).normalized());
}

//>@返回的rect是以（20，20）为坐标原点的坐标
inline QRectF DesignBase::GetDesignGeometry()
{
    return FromSceneRect(geometry()).normalized();
}

inline QPixmap DesignBase::LoadText(QString pTextPath, QSizeF pSize)
{
    if(pTextPath.isEmpty())
        return false;
    fontManager *tmpFontManager = new fontManager(pTextPath);
    if(tmpFontManager->OpenFont() == false)
        return false;
    QImage tmpImage(pSize.width(), pSize.height(), QImage::Format_ARGB32_Premultiplied);
    if(tmpImage.isNull())
        return false;
    tmpImage.fill(Qt::transparent); //>@创建透明图层
    QPainter p(&tmpImage);
    QStringList tmpParagraphList = tmpFontManager->m_ParagraphGroup.keys();
    QString tmpContent;
    FONT_INFO tmpFontInfo;
    QRectF textRect = QRectF(0, 0, pSize.width(), 0);
    for(int i=0;i<tmpParagraphList.count();i++)
    {
        if(tmpFontManager->GetParagraph(tmpParagraphList.at(i), tmpContent, tmpFontInfo) == true)
        {
            p.save();
            if(tmpFontInfo.isValid())
            {
                p.setFont(tmpFontInfo.mFont);
                p.setPen(tmpFontInfo.mColor);
            }
            QRectF rect = QRectF(0, textRect.height(), pSize.width(), pSize.height());

            QFontMetricsF fm(tmpFontInfo.mFont);
            textRect =  QRectF(fm.boundingRect(rect.toRect(), Qt::AlignLeft, tmpContent));
            p.drawText(rect, tmpContent);
            p.restore();
        }
    }
    p.end();
    return QPixmap::fromImage(tmpImage);
}

inline void DesignBase::StartTimeline(int pStart, int pEnd, int pInterval, QTimeLine::CurveShape pShape)
{
    m_Animator.stop();
    m_Animator.setFrameRange(pStart, pEnd);
    m_Animator.setDuration(pInterval);
    m_Animator.setCurveShape(pShape);
    m_Animator.start();
}

inline bool DesignBase::PaintEffect(QPainter *p)
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

inline void DesignBase::ActionTrigger(QAction *pAction)
{
    if(pAction)
    {
        ExecAction(pAction);
    }
}


inline bool DesignBase::DrawPixmap(QPainter *p, QPixmap pPixmap, QRectF pTargetRect, QRectF pSourceRect)
{
    if(pTargetRect.isValid() == false || pSourceRect.isValid() == false)
    {
        IDE_TRACE();
        return false;
    }
    if(pPixmap.isNull() == false)
    {
        p->drawPixmap(pTargetRect,
                     pPixmap,
                     pSourceRect);
        return true;
    }
    return false;
}

inline bool DesignBase::DrawPixmap(QPainter *p, QPixmap pPixmap, QRectF pTargetRect)
{
    if(pTargetRect.isValid() == false)
    {
        IDE_TRACE();
        return false;
    }
    if(pPixmap.isNull() == false)
    {
        p->drawPixmap(pTargetRect.toRect(), pPixmap);
        return true;
    }
    return false;
}

inline bool DesignBase::DrawFlipPixmap(QPainter *p, QPixmap pPixmap, QRectF pTargetRect)
{
    if(pTargetRect.isValid() == false)
    {
        IDE_TRACE();
        return false;
    }
    if(pPixmap.isNull() == false)
    {
        p->save();
        p->setTransform(QTransform().translate(pTargetRect.left()+pTargetRect.width()/2.0, pTargetRect.top()+pTargetRect.height()/2.0).rotate(180, Qt::YAxis), true);
        p->drawPixmap(pTargetRect.toRect(), pPixmap);
        p->restore();
        return true;
    }
    return false;
}

inline bool DesignBase::DrawPixmap(QPainter *p, QPixmap pPixmap)
{
    if(pPixmap.isNull() == false)
    {
        p->drawPixmap(0, 0, pPixmap);
        return true;
    }
    return false;
}


inline bool DesignBase::DrawPixmap(QPainter *p, QPixmap pPixmap, QSizeF pSize, QRectF pTargetRect, QRectF pSourceRect)
{
    if(pTargetRect.isValid() == false || pSourceRect.isValid() == false || pSize.isValid() == false)
    {
        return false;
    }
    if(pPixmap.isNull() == false)
    {
        p->drawPixmap(pTargetRect,
                      pPixmap.scaled(pSize.toSize(),Qt::IgnoreAspectRatio),
                      pSourceRect);
        return true;
    }
    return false;
}

//>@需要绘制底图和特效图片
inline void DesignBase::Paint2D_Roll(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    qreal tmpDiff;
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
    {
        IDE_TRACE();
        return;
    }
    qreal index = m_Animator.currentFrame();
    qreal x, y;
    //>@首先将坐标原点移到当前需要绘制的区域。
    if(m_AreaAnimate->mDirection == Vertical)
    {
        p->save();
        tmpDiff = hh / FrameCount;
        if(m_AreaAnimate->mOrientation == Positive)
        {
            y = (qreal)(tmpDiff*index);
            p->translate(targetRect.left(), targetRect.top()+y);
            DrawPixmap(p, m_PixmapPointer.mLastPointer, targetRect.size(), QRectF(0,0,hw,hh-y), QRectF(0,0,hw,hh-y));
        }
        else
        {
            y = (qreal)(tmpDiff*index);
            p->translate(targetRect.left(), targetRect.top()-y);
            DrawPixmap(p, m_PixmapPointer.mLastPointer, targetRect.size(), QRectF(0,y,hw,hh-y), QRectF(0,y,hw,hh-y));
        }
        p->restore();
        p->save();
        if(m_AreaAnimate->mOrientation == Positive)
        {
            y = hh - y;
            p->translate(targetRect.left(), targetRect.top()-y);
            DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(0,y,hw,hh-y), QRectF(0,y,hw,hh-y));
        }
        else
        {
            y = hh - y;
            p->translate(targetRect.left(), targetRect.top()+y);
            DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(0,0,hw,hh-y), QRectF(0,0,hw,hh-y));
        }
        p->restore();
    }
    else
    {
        p->save();
        tmpDiff = (qreal)(hw) / FrameCount;
        if(m_AreaAnimate->mOrientation == Positive)
        {
            x = (qreal)(tmpDiff*index);
            p->translate(targetRect.left()+x, targetRect.top());
            DrawPixmap(p, m_PixmapPointer.mLastPointer, targetRect.size(), QRectF(0,0,hw-x,hh), QRectF(0,0,hw-x,hh));
        }
        else
        {
            x = (qreal)(tmpDiff*index);
            p->translate(targetRect.left()-x, targetRect.top());
            DrawPixmap(p, m_PixmapPointer.mLastPointer, targetRect.size(), QRectF(x,0,hw-x,hh), QRectF(x,0,hw-x,hh));
        }
        p->restore();
        p->save();
        if(m_AreaAnimate->mOrientation == Positive)
        {
            x = hw - x;
            p->translate(targetRect.left()-x, targetRect.top());
            DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(x,0,hw-x,hh), QRectF(x,0,hw-x,hh));
        }
        else
        {
            x = hw - x;
            p->translate(targetRect.left()+x, targetRect.top());
            DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(0,0,hw-x,hh), QRectF(0,0,hw-x,hh));
        }
        p->restore();
    }
}

inline void DesignBase::Paint2D_None(QPainter *p, QRectF targetRect)
{
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect);
}

//>@需要绘制底图和特效图片
inline void DesignBase::Paint2D_Flip(QPainter *p, QRectF targetRect)
{
    qreal w = targetRect.width();
    qreal h = targetRect.height();
    qreal hw = w / 2.0;
    qreal hh = h / 2.0;
    // behind is the new pixmap
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect);
    if(m_Animator.state() != QTimeLine::Running)
    {
        setTransform(QTransform().translate(hw / 2, hh / 2)
                                 .rotate(0, Qt::YAxis)
                                 .translate(-hw / 2, -hh / 2));
        setTransform(QTransform().translate(hw / 2, hh / 2)
                                 .rotate(0, Qt::XAxis)
                                 .translate(-hw / 2, -hh / 2));
        Paint2D_None(p, targetRect);
        return;
    }
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
        return;
    qreal halfcount =  FrameCount / 2.0;
    qreal index = m_Animator.currentFrame();

    p->save();
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
        p->translate(targetRect.left()+hw, targetRect.top()+hh);
        if(m_AreaAnimate->mDirection == Horizontal)
            setTransform(QTransform().translate(w / 2, h / 2)
                                     .rotate(angle, Qt::YAxis)
                                     .translate(-w / 2, -h / 2));
        else
            setTransform(QTransform().translate(w / 2, h / 2)
                                     .rotate(angle, Qt::XAxis)
                                     .translate(-w / 2, -h / 2));
        DrawPixmap(p, m_PixmapPointer.mLastPointer, targetRect.size(), QRectF(-hw, -hh, w, h), QRectF(0,0,w,h));

        // the bottom part is still the old pixmap
        p->restore();
        p->save();
        if(m_AreaAnimate->mDirection == Horizontal)
            if(m_AreaAnimate->mOrientation == Positive)
                p->setClipRect(hw, 0, hw, targetRect.height());
            else
                p->setClipRect(0, 0, hw, targetRect.height());
        else
            if(m_AreaAnimate->mOrientation == Positive)
                p->setClipRect(0, hh, targetRect.width(), hh);
            else
                p->setClipRect(0, 0, targetRect.width(), hh);
        DrawPixmap(p, m_PixmapPointer.mLastPointer, targetRect.size(), QRectF(0,0,w,h), QRectF(0,0,w,h));
    }
    else
    {
        p->translate(targetRect.left(), targetRect.top());
        if(m_AreaAnimate->mDirection == Horizontal)
            if(m_AreaAnimate->mOrientation == Positive)
                p->setClipRect(hw, 0, hw, targetRect.height());
            else
                p->setClipRect(0, 0, hw, targetRect.height());
        else
            if(m_AreaAnimate->mOrientation == Positive)
                p->setClipRect(0, hh, targetRect.width(), hh);
            else
                p->setClipRect(0, 0, targetRect.width(), hh);

        // the bottom part is still the old pixmap
        DrawPixmap(p, m_PixmapPointer.mLastPointer, targetRect.size(), QRectF(0,0,w,h), QRectF(0,0,w,h));
        p->restore();
        p->save();
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
        p->translate(targetRect.left()+hw, targetRect.top()+hh);
        if(m_AreaAnimate->mDirection == Horizontal)
            setTransform(QTransform().translate(w / 2, h / 2)
                                     .rotate(angle, Qt::YAxis)
                                     .translate(-w / 2, -h / 2));
        else
            setTransform(QTransform().translate(w / 2, h / 2)
                                     .rotate(angle, Qt::XAxis)
                                     .translate(-w / 2, -h / 2));
        DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(-hw, -hh, w, h), QRectF(0,0,w,h));
    }
    p->restore();
}

//>@需要绘制底图和特效图片
inline void DesignBase::Paint2D_Zoom(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
    {
        IDE_TRACE();
        return;
    }
    qreal halfcount =  FrameCount / 2.0;
    qreal index = m_Animator.currentFrame();

    qreal x,y,w,h;
    p->save();
    p->translate(targetRect.left()+hw/2, targetRect.top()+hh/2);
    if(index < halfcount)
    {
        index = halfcount - index;
        x = -hw*index/FrameCount;
        y = -hh*index/FrameCount;
        w = 2*hw*index/FrameCount;
        h = 2*hh*index/FrameCount;
        DrawPixmap(p, m_PixmapPointer.mLastPointer, targetRect.size(), QRectF(x,y,w,h), QRectF(0,0,hw,hh));
    }
    else
    {
        index = index - halfcount;
        x = -hw*index/FrameCount;
        y = -hh*index/FrameCount;
        w = 2*hw*index/FrameCount;
        h = 2*hh*index/FrameCount;
        DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(x,y,w,h), QRectF(0,0,hw,hh));
    }
    p->restore();
}

inline void DesignBase::Paint2D_Blink(QPainter *p,QRectF targetRect)
{
    if(m_Animator.state() != QTimeLine::Running)
    {
        setOpacity(1);
        Paint2D_None(p, targetRect);
        return;
    }
    int index = m_Animator.currentFrame();
    if(index % 2)
        setOpacity(0.5);
    else
        setOpacity(1);
    Paint2D_None(p, targetRect);
}

//>@需要绘制底图和特效图片
inline void DesignBase::Paint2D_Rotate(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        setTransform(QTransform().translate(hw / 2, hh / 2)
                                 .rotate(0, Qt::YAxis)
                                 .translate(-hw / 2, -hh / 2));
        setTransform(QTransform().translate(hw / 2, hh / 2)
                                 .rotate(0, Qt::XAxis)
                                 .translate(-hw / 2, -hh / 2));
        Paint2D_None(p, targetRect);
        return;
    }
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
    {
        IDE_TRACE();
        return;
    }
    qreal halfCount =  FrameCount / 2.0;
    qreal index = m_Animator.currentFrame();
    qreal scaleCount1 = (qreal)FrameCount*1.0/20.0;
    qreal scaleCount2 = (qreal)FrameCount*19.0/20.0;
    if(scaleCount1<=0 || scaleCount2<=0)
    {
        IDE_TRACE();
        return;
    }
    qreal pad = hw / 10.0;
    p->save();
    QRectF fr = targetRect.adjusted(-pad, -pad, pad, pad);
    p->setClipRect(fr);
    p->translate(targetRect.left()+hw/2.0, targetRect.top()+hh/2.0);
    QPixmap pix = (index <= halfCount) ? m_PixmapPointer.mLastPointer : m_PixmapPointer.mCurPointer;
    if(index <= scaleCount1)
    {
        qreal diff = (qreal)pad * index / (qreal)scaleCount1;
        if(m_AreaAnimate->mDirection == Horizontal)
        {
            DrawPixmap(p, pix, targetRect.size(),
                       QRectF(-hw/2.0, -hh/2.0+diff, hw, hh-2*diff),
                       QRectF(0,0,hw,hh));
        }
        else
        {
            DrawPixmap(p, pix, targetRect.size(),
                       QRectF(-hw/2.0+diff, -hh/2.0, hw-2*diff, hh),
                       QRectF(0,0,hw,hh));
        }
    }
    else if(index > scaleCount2)
    {
        qreal diff = (qreal)pad * (FrameCount - index) / (qreal)scaleCount1;
        if(m_AreaAnimate->mDirection == Horizontal)
        {
            DrawPixmap(p, pix, targetRect.size(),
                       QRectF(-hw / 2.0, -hh / 2.0 + diff, hw, hh - 2*diff),
                       QRectF(0,0,hw,hh));
        }
        else
        {
            DrawPixmap(p, pix, targetRect.size(),
                       QRectF(-hw / 2.0 + diff, -hh / 2.0, hw - 2*diff, hh),
                       QRectF(0,0,hw,hh));
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
            setTransform(QTransform().translate(hw / 2, hh / 2)
                                     .rotate(angle, Qt::YAxis)
                                     .translate(-hw / 2, -hh / 2));
            DrawPixmap(p, pix, QRectF(-hw / 2.0, -hh / 2.0 + pad, hw, hh - 2*pad));
        }
        else
        {
            setTransform(QTransform().translate(hw / 2, hh / 2)
                                     .rotate(angle, Qt::XAxis)
                                     .translate(-hw / 2, -hh / 2));
            DrawPixmap(p, pix, QRectF(-hw / 2.0 + pad, -hh / 2.0, hw - 2*pad, hh));
        }
    }
    p->restore();
}

//>@需要绘制底图和特效图片
inline void DesignBase::Paint2D_Slipcycle(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
        return;
    qreal halfCount =  FrameCount / 2.0;
    if(halfCount <= 0)
        return;
    qreal index = m_Animator.currentFrame();

    p->save();
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
                p->translate(targetRect.left(), targetRect.top()+y);
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, targetRect.size(), QRectF(0,0,hw,hh-y), QRectF(0,0,hw,hh-y));
            }
            else
            {
                p->translate(targetRect.left(), targetRect.top()-y);
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, targetRect.size(), QRectF(0,y,hw,hh-y), QRectF(0,y,hw,hh-y));
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
                p->translate(targetRect.left(), targetRect.top()+y);
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, targetRect.size(), QRectF(0,0,hw,hh-y), QRectF(0,0,hw,hh-y));
            }
            else
            {
                p->translate(targetRect.left(), targetRect.top()-y);
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, targetRect.size(), QRectF(0,y,hw,hh-y), QRectF(0,y,hw,hh-y));
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
                p->translate(targetRect.left()+x, targetRect.top());
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, targetRect.size(), QRectF(0,0,hw-x,hh), QRectF(0,0,hw-x,hh));
            }
            else
            {
                p->translate(targetRect.left()-x, targetRect.top());
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, targetRect.size(), QRectF(x,0,hw-x,hh), QRectF(x,0,hw-x,hh));
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
                p->translate(targetRect.left()+x, targetRect.top());
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, targetRect.size(), QRectF(0,0,hw-x,hh), QRectF(0,0,hw-x,hh));
            }
            else
            {
                p->translate(targetRect.left()-x, targetRect.top());
                //>@如果使用这种方法，则会刷出按钮范围，不过依然是一个比较好看的特效。
                //painter->drawPixmap(0, 0, m_PixList[PressEffect]);
                DrawPixmap(p, tmpPixmap, targetRect.size(), QRectF(x,0,hw-x,hh), QRectF(x,0,hw-x,hh));
            }
        }
    }
    p->restore();
}

//>@需要绘制底图和特效图片
inline void DesignBase::Paint2D_Transparent(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
    {
        IDE_TRACE();
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

    p->save();
    p->translate(targetRect.left(), targetRect.top());
    p->setOpacity(tmpLastAlpha);
    DrawPixmap(p, m_PixmapPointer.mLastPointer, targetRect.size(), QRectF(0,0,hw,hh), QRectF(0,0,hw,hh));
    p->setOpacity(tmpAlpha);
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(0,0,hw,hh), QRectF(0,0,hw,hh));
    p->restore();
}

//>@只需要绘制特效图片即可
inline void DesignBase::Paint2D_Spread(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    qreal x,y,h,w;
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
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
    p->save();
    p->translate(targetRect.left()+hw/2, targetRect.top()+hh/2);
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(x,y,w,h), QRectF(0,0,hw,hh));
    p->restore();
}

//>@只需要绘制特效图片即可
inline void DesignBase::Paint2D_Wheel(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    qreal FrameCount = m_AreaAnimate->mFrameEnd - m_AreaAnimate->mFrameStart - 1;
    if(FrameCount <= 0)
        return;
    qreal index = m_Animator.currentFrame();

    qreal tmpDiff = (qreal)(2.0 * PI) / (qreal)FrameCount * 2.0;
    qreal rotate = 360.0 * normalizeAngle(tmpDiff*index) / 2.0 /PI;

    p->save();
    p->translate(targetRect.left()+hw/2, targetRect.top()+hh/2);
    p->rotate(rotate);
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(-hw/2.0, -hh/2.0, hw, hh), QRectF(0,0,hw,hh));
    p->restore();
}

#endif // DesignBase_H
