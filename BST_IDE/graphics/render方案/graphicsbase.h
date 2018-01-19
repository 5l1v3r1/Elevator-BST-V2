#ifndef D_GraphicsBASE_H
#define D_GraphicsBASE_H

#include "global.h"
#include <QTime>
#include <QVector>
#include <QDir>
#include <QKeyEvent>

#ifdef  SURRPORT_3D
#include <QtOpenGL>
#include <QGLWidget>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtOpenGL/qglshaderprogram.h>
#include "./OpenGL/Windows/include/glut.h"
#endif

#define D_SETCTRL_ZVALUE  10000 //>@设置控件Z序
#define D_MSGCTRL_ZVALUE  10001 //>@消息控件Z序
#define D_PRGCTRL_ZVALUE  10002 //>@进度条控件Z序
#define D_KEYBOARD_ZVALUE  10003//>@键盘控件Z序

#ifdef  SURRPORT_3D
class GraphicsGLWidget;
#endif

inline QRectF ToSceneRect(QRectF pRect) { return QRectF(pRect.left() + StartPosX, pRect.top() + StartPosY, pRect.width(), pRect.height()); }
inline QRectF FromSceneRect(QRectF pRect) { return QRectF(pRect.left() - StartPosX, pRect.top() - StartPosY, pRect.width(), pRect.height()); }

#ifdef IDE
    class DiagramScene;
#elif
    class UiScene;
#endif

class GraphicsBase : public QGraphicsWidget
{
    Q_OBJECT
public:
#ifdef  SURRPORT_3D
    friend class GraphicsGLWidget;
#endif
    GraphicsBase(QGraphicsItem *parent, QRectF pRect);
    ~GraphicsBase();

//>##########################################
//>        资源相关
//>##########################################
    int GetKey(QString pString);

    void ReleaseRC();

    QPixmap LoadPixmap(QString pPixmapName, QSizeF pSize=QSizeF());
    QPixmap ScalePixmap(QPixmap pPixmap, QSizeF pSize=QSizeF());

    QPixmap GetPixmap(int pKey, QSizeF pSize=QSizeF());  //>@从m_EffectXXX中加载图片
#ifdef  SURRPORT_SVG
    QSvgRenderer *GetSvgRender(int pKey);
#endif

    bool LoadPath(int pKey, QString pPixmapName);

public:
    bool InitComponent(QHash<int, QPixmap> pPixmapList, QHash<AREA_OPERATE, AREA_ANIMATE*>  pEffectGroup);
    bool InitComponent(QDomElement pComElement);
    virtual bool InitEffectRc(STATE_INFO* pEffect);
    virtual bool Start();  //>@执行完InitEffect需要执行Start来启动特效
    bool Refresh()
    {
        m_EffectImage.clear();
#ifdef  SURRPORT_3D
        m_EffectTexture.clear();
#endif
#ifdef  SURRPORT_SVG
        m_EffectSvgRender.clear();
#endif
        return Start();
    }
public:
#ifdef IDE
    DiagramScene                       *m_UiScene;
#elif
    UiScene                            *m_UiScene;
#endif
    QDomElement                         m_ComElement;
    QString                             m_ComPath;
    QHash<int, QPixmap>                 m_EffectImage;  //>@加载到内存的资源
#ifdef  SURRPORT_3D
    QHash<int, GLuint>                  m_EffectTexture;
#endif
#ifdef  SURRPORT_SVG
    QHash<int, QSvgRenderer*>           m_EffectSvgRender;
#endif
    QHash<AREA_OPERATE, AREA_ANIMATE*>  m_EffectGroup;  //>@特效描述
    bool                                m_EffectValid;  //>@特效使能

//>##########################################
//>        界面相关
//>##########################################
public:
    QRectF boundingRect() const;
    QPainterPath shape() const;

#ifdef  SURRPORT_3D
    void setGeometry3D(qreal x, qreal y, qreal width, qreal height);
    void setGeometry3D(QRectF rect);
    void BindTexture(GLuint L_Texture,GLfloat R,GLfloat G,GLfloat B);
    void UnbindTexture();
#endif

    virtual bool InitEffectRc(STATE_INFO* pEffect);
    virtual bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    virtual bool Start();

    int getNextRc(int pCurIndex);
    int getPrevRc(int pCurIndex);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual bool PaintEffect(QPainter *p);

#ifdef  SURRPORT_3D
    void OpenGL_PaintBegin(QPainter *painter);
    void OpenGL_PaintEnd(QPainter *painter);
    void OpenGL_Effect(QPainter *painter);
#endif

    void Play();
    void Stop();
    bool SetEnvironment(QString pEnvirString);  //>@设置COM的现场环境
    bool GetEnvironment(QString &pEnvirString); //>@获取COM的现场环境

    bool SetPara(QDomElement &pElement, xmlParse *pParse); //>@修改pElement为结果Element
    bool GetPara(QDomElement &pElement, xmlParse *pParse);

    bool SetElementText(QDomElement pElement, QString pValue);
    void UpdateProgress(QString pMsg, int pValue);

    //>@获取本节点中的重要信息
    virtual QString GetInformation() { return QString("%1").arg(m_ComPath); }

    //>@任何动作最多只需要两步，如果有两步，则第一步肯定是状态CHANGE，第二步一定是状态的操作变化
    void OperateStep0();

    virtual QSize GetRcSize(QString pRcName) { return QSize(); }
    virtual QList<QAction*> GetActionList(){ QList<QAction*> tmpList; return tmpList; }

public:
    void PressEvent(QGraphicsSceneMouseEvent *event);
    void MoveEvent(QGraphicsSceneMouseEvent *event);
    void ReleaseEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void MouseRelease(QPointF Pos);
    void reportEffect(AREA_OPERATE pOperate, QVariant pPara);

public slots:
    virtual void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    virtual void UpdateCom(int pFrame);
    virtual void ExecAction(QAction *pAction) {}
    void OperateStep1();

public:
    RCTYPE                  m_RcType;
    bool                    m_PaintBusy;

    QActionGroup*           m_ActionGroup;

    QList<int>              m_RcList;       //>@用于查找上一个或下一个资源

    AREA_ANIMATE           *m_AreaAnimate;
    OPERATE_INFO            m_OperateInfo[2];
    PIXMAP_POINTER          m_PixmapPointer;

    //>@用于UpdateEffect中
    AREA_OPERATE            m_UpdateOPerate;
    QVariant                m_UpdatePara;

public:
#ifdef  SURRPORT_3D
    GraphicsGLWidget       *m_GLWidget;
    int                     m_3DWidth, m_3DHeight;
    //>@用于XYZ三轴旋转以及深度的变化
    int                     m_3DXDiff,m_3DYDiff,m_3DZDiff; //>@三个方向上的角度偏移
    qreal                   m_3DXScale,m_3DYScale,m_3DZScale; //>@三个方向上的缩放
    float                   m_3DDeepDiff;
    GLfloat                 m_3DVerPoint[7200];
    GLfloat                 m_3DVerTexture[7200];
    GLfloat                 m_3DNormalFront[7200];
#endif

public:
    void StartTimeline();
    void StopAnimate();

    QPixmap GetFlipPixmap(QPixmap pPixmap, QRectF pTargetRect);
    bool DrawPixmap(QPainter *p, QPixmap pPixmap, QSizeF pSize, QRectF pTargetRect, QRectF pSourceRect);
    bool DrawPixmap(QPainter *p, QPixmap pPixmap, QRectF pTargetRect, QRectF pSourceRect);
    bool DrawFlipPixmap(QPainter *p, QPixmap pPixmap, QRectF pTargetRect);
    bool DrawPixmap(QPainter *p, QPixmap pPixmap, QRectF pTargetRect);
    bool DrawPixmap(QPainter *p, QPixmap pPixmap, QRect pTargetRect) {  return DrawPixmap(p, pPixmap, QRectF(pTargetRect)); }
    bool DrawPixmap(QPainter *p, QPixmap pPixmap);

    void Paint2D_None(QPainter *p, QRectF targetRect);
    void Paint2D_Flip(QPainter *p,QRectF targetRect);
    void Paint2D_Roll(QPainter *p,QRectF targetRect);
    void Paint2D_Zoom(QPainter *p,QRectF targetRect);
    void Paint2D_Rotate(QPainter *p,QRectF targetRect);
    void Paint2D_Turn(QPainter *p, QRectF targetRect);
    void Paint2D_Blink(QPainter *p,QRectF targetRect);
    void Paint2D_Slipcycle(QPainter *p,QRectF targetRect);
    void Paint2D_Fireworks(QPainter *p, QRectF targetRect);
    void Paint2D_Transparent(QPainter *p,QRectF targetRect);
    void Paint2D_Spread(QPainter *p,QRectF targetRect);
    void Paint2D_Wheel(QPainter *p,QRectF targetRect);

#ifdef  SURRPORT_3D
    void Paint3D_CircleSurface(GLuint texture);//>@绘制圆面
    void Paint3D_CamberSurface(GLuint texture);//>@绘制弧面
    void Paint3D_Cube(QPainter *p,QRectF targetRect);    //>@绘制立方体
    void Paint3D_Cylinder(QPainter *p,QRectF targetRect);//>@绘制圆柱体
    void Set3DEffect(EffectType pType);
#endif
};

#ifdef  SURRPORT_3D

class GraphicsGLWidget : public QGLWidget
{
public:
    explicit GraphicsGLWidget() : QGLWidget()
    {
    }
    ~GraphicsGLWidget()
    {
    }
    bool InitTexture(GraphicsBase *pGraphicsWidget)
    {
        if(pGraphicsWidget = 0)
            return false;
        QList<int> tmpKeyList = pGraphicsWidget->m_EffectPath.keys();
        int count = tmpKeyList.count();
        int tmpKey;
        for(int i=0;i<count;i++)
        {
            tmpKey = tmpKeyList.at(i);
            QPixmap tmpPixmap;
            tmpPixmap.load(pGraphicsWidget->m_EffectPath.value(tmpKey));
            if(tmpPixmap.isNull())
                continue;
            GLuint tmpGLuint = bindTexture(tmpPixmap);
            pGraphicsWidget->m_EffectTexture.insert(tmpKey, tmpGLuint);
        }
        return true;
    }
    GLuint GetTexture(QString pImagePath, QSizeF pSize)
    {
        QImage tmpImage(pImagePath);
        if(tmpImage.isNull())
        {
            IDE_TRACE_STR(pImagePath);
            return 0;
        }
        return bindTexture(QPixmap::fromImage(tmpImage.scaled(pSize.toSize())));
    }
};

#endif

inline bool GraphicsBase::PaintEffect(QPainter *p)
{
    if(m_EffectValid == false)
    {
        return false;
    }
    if(m_AreaAnimate == 0)
    {
        Paint2D_None(p, rect());
        return false;
    }
    return true;
}

inline bool GraphicsBase::DrawPixmap(QPainter *p, QPixmap pPixmap, QRectF pTargetRect, QRectF pSourceRect)
{
    if(pTargetRect.isValid() == false || pSourceRect.isValid() == false)
    {
        //IDE_TRACE();
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

inline bool GraphicsBase::DrawPixmap(QPainter *p, QPixmap pPixmap, QRectF pTargetRect)
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

//>@只能用于整幅图片的翻转显示
inline bool GraphicsBase::DrawFlipPixmap(QPainter *p, QPixmap pPixmap, QRectF pTargetRect)
{
    if(pTargetRect.isValid() == false)
    {
        IDE_TRACE();
        return false;
    }
    if(pPixmap.isNull() == false)
    {
        p->save();
        p->translate(pTargetRect.width()/2.0, pTargetRect.height()/2.0);
        p->setTransform((QTransform().translate(pTargetRect.width()/2.0, pTargetRect.height()/2.0)
                        .rotate(180, Qt::XAxis)
                        .translate(-pTargetRect.width()/2.0, -pTargetRect.height()/2.0)));
        p->drawPixmap(pTargetRect.toRect(), pPixmap);
        p->restore();
        return true;
    }
    return false;
}

inline QPixmap GraphicsBase::GetFlipPixmap(QPixmap pPixmap, QRectF pTargetRect)
{
    if(pPixmap.isNull())
    {
        IDE_TRACE();
        return QPixmap();
    }
    QImage tmpImage(pTargetRect.width(), pTargetRect.height(), QImage::Format_ARGB32_Premultiplied);
    if(tmpImage.isNull())
        return QPixmap();
    tmpImage.fill(Qt::transparent); //>@创建透明图层
    QPainter p(&tmpImage);
    p.translate(pTargetRect.width()/2.0, pTargetRect.height()/2.0);
    p.setTransform((QTransform().translate(pTargetRect.width()/2.0, pTargetRect.height()/2.0)
                    .rotate(180, Qt::XAxis)
                    .translate(-pTargetRect.width()/2.0, -pTargetRect.height()/2.0)));
    DrawPixmap(&p, pPixmap, pTargetRect);
    p.end();
    return QPixmap::fromImage(tmpImage);
}

inline bool GraphicsBase::DrawPixmap(QPainter *p, QPixmap pPixmap)
{
    if(pPixmap.isNull() == false)
    {
        p->drawPixmap(0, 0, pPixmap);
        return true;
    }
    return false;
}

inline bool GraphicsBase::DrawPixmap(QPainter *p, QPixmap pPixmap, QSizeF pSize, QRectF pTargetRect, QRectF pSourceRect)
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
inline void GraphicsBase::Paint2D_Roll(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    qreal tmpDiff;
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    if(m_AreaAnimate->mFrameCount <= 0)
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
        tmpDiff = hh / m_AreaAnimate->mFrameCount;
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
        tmpDiff = (qreal)(hw) / m_AreaAnimate->mFrameCount;
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

inline void GraphicsBase::Paint2D_None(QPainter *p, QRectF targetRect)
{
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect);
}

//>@需要绘制底图和特效图片
inline void GraphicsBase::Paint2D_Flip(QPainter *p, QRectF targetRect)
{
    qreal w = targetRect.width();
    qreal h = targetRect.height();
    qreal hw = w / 2.0;
    qreal hh = h / 2.0;
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    // behind is the new pixmap
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect);
    if(m_AreaAnimate->mFrameCount <= 0)
        return;
    qreal halfcount =  m_AreaAnimate->mFrameCount / 2.0;
    qreal index = m_Animator.currentFrame();

    p->save();
    if (index <= halfcount)
    {
        // the top part of the old pixmap is flipping
        qreal angle;
        if(m_AreaAnimate->mOrientation == Positive)
        {
            angle = -180.0 * index / m_AreaAnimate->mFrameCount;
        }
        else
        {
            angle = 180.0 * index / m_AreaAnimate->mFrameCount;
        }
        p->translate(targetRect.left() + hw, targetRect.top() + hh);
        if(m_AreaAnimate->mDirection == Horizontal)
            p->setTransform(QTransform().rotate(angle, Qt::YAxis), true);
        else
            p->setTransform(QTransform().rotate(angle, Qt::XAxis), true);
        DrawPixmap(p, m_PixmapPointer.mLastPointer, targetRect.size(), QRectF(-hw, -hh, w, h), QRectF(0,0,w,h));

        // the bottom part is still the old pixmap
        p->restore();
        p->save();
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
        // the bottom part of the new pixmap is flipping
        qreal angle;
        if(m_AreaAnimate->mOrientation == Positive)
        {
            angle = 180.0 - 180.0 * index / m_AreaAnimate->mFrameCount;
        }
        else
        {
            angle = - 180.0 * (m_AreaAnimate->mFrameCount-index) / m_AreaAnimate->mFrameCount;
        }
        p->translate(hw, hh);
        if(m_AreaAnimate->mDirection == Horizontal)
            p->setTransform(QTransform().rotate(angle, Qt::YAxis), true);
        else
            p->setTransform(QTransform().rotate(angle, Qt::XAxis), true);
        DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(-hw, -hh, w, h), QRectF(0,0,w,h));
    }
    p->restore();
}

//>@需要绘制底图和特效图片
inline void GraphicsBase::Paint2D_Zoom(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    if(m_AreaAnimate->mFrameCount <= 0)
    {
        IDE_TRACE();
        return;
    }
    qreal halfcount =  m_AreaAnimate->mFrameCount / 2.0;
    qreal index = m_Animator.currentFrame();

    qreal x,y,w,h;
    p->save();
    p->translate(targetRect.left()+hw/2, targetRect.top()+hh/2);
    if(index < halfcount)
    {
        index = halfcount - index;
        x = -hw*index/m_AreaAnimate->mFrameCount;
        y = -hh*index/m_AreaAnimate->mFrameCount;
        w = 2*hw*index/m_AreaAnimate->mFrameCount;
        h = 2*hh*index/m_AreaAnimate->mFrameCount;
        DrawPixmap(p, m_PixmapPointer.mLastPointer, targetRect.size(), QRectF(x,y,w,h), QRectF(0,0,hw,hh));
    }
    else
    {
        index = index - halfcount;
        x = -hw*index/m_AreaAnimate->mFrameCount;
        y = -hh*index/m_AreaAnimate->mFrameCount;
        w = 2*hw*index/m_AreaAnimate->mFrameCount;
        h = 2*hh*index/m_AreaAnimate->mFrameCount;
        DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(x,y,w,h), QRectF(0,0,hw,hh));
    }
    p->restore();
}

inline void GraphicsBase::Paint2D_Blink(QPainter *p,QRectF targetRect)
{
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    p->save();
    int index = m_Animator.currentFrame();
    if(index % 2)
        p->setOpacity(0.2);
    else
        p->setOpacity(1);
    Paint2D_None(p, targetRect);
    p->restore();
}

//>@翻转
inline void GraphicsBase::Paint2D_Turn(QPainter *p, QRectF targetRect)
{
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, rect());
        return;
    }
    if(m_AreaAnimate->mFrameCount <= 0)
    {
        IDE_TRACE();
        return;
    }
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    qreal index = m_Animator.currentFrame();
    p->save();
    p->translate(targetRect.left()+hw/2, targetRect.top()+hh/2);
    qreal angle = 0;
    if(m_AreaAnimate->mOrientation == Positive)
    {
        angle = 360.0 * index / m_AreaAnimate->mFrameCount;
    }
    else if(m_AreaAnimate->mOrientation == Negative)
    {
        angle = - 360.0 * index / m_AreaAnimate->mFrameCount;
    }
    p->rotate(angle);
    p->restore();
}

//>@需要绘制底图和特效图片
inline void GraphicsBase::Paint2D_Rotate(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, rect());
        return;
    }
    if(m_AreaAnimate->mFrameCount <= 0)
    {
        IDE_TRACE();
        return;
    }
    qreal halfCount =  m_AreaAnimate->mFrameCount / 2.0;
    qreal index = m_Animator.currentFrame();

    p->save();
    p->translate(targetRect.left()+hw/2.0, targetRect.top()+hh/2.0);
    QPixmap pix = (index <= halfCount) ? m_PixmapPointer.mLastPointer : m_PixmapPointer.mCurPointer;

    qreal angle = (index <= halfCount) ? (-180.0 * index / m_AreaAnimate->mFrameCount) : (180.0 - 180.0 * index / m_AreaAnimate->mFrameCount);
    if(m_AreaAnimate->mDirection == Horizontal)
    {
        p->setTransform(QTransform().rotate(angle, Qt::YAxis), true);
        DrawPixmap(p, pix, QRect(-hw / 2.0, -hh / 2.0, hw, hh));
    }
    else
    {
        p->setTransform(QTransform().rotate(angle, Qt::XAxis), true);
        DrawPixmap(p, pix, QRect(-hw / 2.0, -hh / 2.0, hw, hh));
    }
    p->restore();
}

//>@需要绘制底图和特效图片
inline void GraphicsBase::Paint2D_Slipcycle(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    if(m_AreaAnimate->mFrameCount <= 0)
        return;
    qreal halfCount =  m_AreaAnimate->mFrameCount / 2.0;
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

inline void GraphicsBase::Paint2D_Fireworks(QPainter *p, QRectF targetRect)
{

}

//>@需要绘制底图和特效图片
inline void GraphicsBase::Paint2D_Transparent(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    if(m_AreaAnimate->mFrameCount <= 0)
    {
        IDE_TRACE();
        return;
    }
    qreal Stage1 = m_AreaAnimate->mFrameCount*3.0/10.0;
    qreal Stage2 = m_AreaAnimate->mFrameCount*4.0/10.0;
    //qreal Stage3 = (qreal)(m_AreaAnimate->mFrameCount)*3.0/10.0;

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
inline void GraphicsBase::Paint2D_Spread(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    qreal x,y,h,w;
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    if(m_AreaAnimate->mFrameCount <= 0)
        return;
    qreal halfCount =  m_AreaAnimate->mFrameCount / 2.0;
    qreal index = m_Animator.currentFrame();

    if(index < halfCount)
    {
        index = halfCount - index;
        x = -hw*index/m_AreaAnimate->mFrameCount;
        y = -hh*index/m_AreaAnimate->mFrameCount;
        w = 2*hw*index/m_AreaAnimate->mFrameCount;
        h = 2*hh*index/m_AreaAnimate->mFrameCount;
    }
    else
    {
        index = index - halfCount;
        x = -hw*index/m_AreaAnimate->mFrameCount;
        y = -hh*index/m_AreaAnimate->mFrameCount;
        w = 2*hw*index/m_AreaAnimate->mFrameCount;
        h = 2*hh*index/m_AreaAnimate->mFrameCount;
    }
    p->save();
    p->translate(targetRect.left()+hw/2, targetRect.top()+hh/2);
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(x,y,w,h), QRectF(0,0,hw,hh));
    p->restore();
}

//>@只需要绘制特效图片即可
inline void GraphicsBase::Paint2D_Wheel(QPainter *p, QRectF targetRect)
{
    qreal hh = targetRect.height();
    qreal hw = targetRect.width();
    if(m_Animator.state() != QTimeLine::Running)
    {
        Paint2D_None(p, targetRect);
        return;
    }
    if(m_AreaAnimate->mFrameCount <= 0)
        return;
    qreal index = m_Animator.currentFrame();

    qreal tmpDiff = (qreal)(2.0 * PI) / (qreal)(m_AreaAnimate->mFrameCount) * 2.0;
    qreal rotate = 360.0 * normalizeAngle(tmpDiff*index) / 2.0 /PI;

    p->save();
    p->translate(targetRect.left()+hw/2, targetRect.top()+hh/2);
    p->rotate(rotate);
    DrawPixmap(p, m_PixmapPointer.mCurPointer, targetRect.size(), QRectF(-hw/2.0, -hh/2.0, hw, hh), QRectF(0,0,hw,hh));
    p->restore();
}

#ifdef  SURRPORT_3D
inline void GraphicsBase::Paint3D_CircleSurface(GLuint texture)
{
    glPushMatrix();
    glFrontFace(GL_CCW);
    glTranslatef(-0.5f,-0.5f,0.0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    //本质上是画360个点，来画360个三角形。
    for(int i=0;i<360;i+=2)
    {
        double aa=(double)PI*i/180;
        m_3DVerPoint[i]=(float)((cos(aa)+1.0)*0.5);   //纹理横坐标
        m_3DVerPoint[i+1]=(float)((sin(aa)+1.0)*0.5);  //纹理纵坐标
    }
    glVertexPointer(2, GL_FLOAT, 0, m_3DVerPoint);
    BindTexture(texture,1.0f,1.0f,1.0f);
    glTexCoordPointer(2,GL_FLOAT,0,m_3DVerPoint);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 180);
    UnbindTexture();
    glPopMatrix();
}

inline void GraphicsBase::Paint3D_CamberSurface(GLuint texture)
{
    int fragAngle = 30;
    int n = 180/fragAngle;
    float b = (float)fragAngle/180;
    float r = (float)1/(2*PI)-0.005;
    glPushMatrix();
    glRotatef(-90,0.0f,1.0f,0.0f);
    glScalef(2*PI, 1.0f, 2*PI);
    glTranslatef(0.0f,0.5f,0.0f);
    for(int j=0;j<2;j++)
    {
        for(int i=0;i<n;i++)
        {
            m_3DVerPoint[0] = -0.0;m_3DVerPoint[1] = 0.0;m_3DVerPoint[2] = r;
            m_3DVerPoint[3] = b;m_3DVerPoint[4] = -1.0;m_3DVerPoint[5] = r;
            m_3DVerPoint[6] = -0.0;m_3DVerPoint[7] = -1.0;m_3DVerPoint[8] = r;

            m_3DVerPoint[9] = b;m_3DVerPoint[10] = -1.0;m_3DVerPoint[11] = r;
            m_3DVerPoint[12] = -0.0;m_3DVerPoint[13] = 0.0;m_3DVerPoint[14] = r;
            m_3DVerPoint[15] = b;m_3DVerPoint[16] = 0.0;m_3DVerPoint[17] = r;
            GLfloat NormalFront[] = {
                                        0,0,-1, 0,0,-1, 0,0,-1,
                                        0,0,-1, 0,0,-1, 0,0,-1
                                    };
            memcpy(m_3DVerTexture,m_3DVerPoint,18*sizeof(GLfloat));
            m_3DVerTexture[0] = m_3DVerPoint[0]+b*i;
            m_3DVerTexture[3] = m_3DVerPoint[3]+b*i;
            m_3DVerTexture[6] = m_3DVerPoint[6]+b*i;

            m_3DVerTexture[9] = m_3DVerPoint[9]+b*i;
            m_3DVerTexture[12] = m_3DVerPoint[12]+b*i;
            m_3DVerTexture[15] = m_3DVerPoint[15]+b*i;

            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3,GL_FLOAT,0,m_3DVerPoint);
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT,0,NormalFront);
            BindTexture(texture,1.0f,1.0f,1.0f);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(3,GL_FLOAT,0,m_3DVerTexture);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glRotatef(fragAngle, 0, 1, 0);
        }
    }
    UnbindTexture();
    glPopMatrix();
}

inline void GraphicsBase::Paint3D_Cube(QPainter *p,QRectF targetRect)
{
    OpenGL_PaintBegin(p);
    OpenGL_Effect(p);

    glPushMatrix();
    //glRotatef(180,0.0f,0.0f,1.0f);
    glPushMatrix();
    glFrontFace(GL_CCW);
    GLfloat afVerticesFront[] = {
                                    -0.5f,  0.5f,   0.5f,
                                    0.5f,   0.5f,   0.5f,
                                    -0.5f,  -0.5f,  0.5f,
                                    0.5f,   -0.5f,  0.5f,
                                };
    GLfloat afNormalsFront[] = {
                                    0,0,1, 0,0,1, 0,0,1,
                                    0,0,1, 0,0,1, 0,0,1
                                };
    GLfloat afTexCoordFront[] = {
                                    0.0f, 1.0f,
                                    1.0f, 1.0f,
                                    0.0f, 0.0f,
                                    1.0f, 0.0f
                                };
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    //>@Front
    glVertexPointer(3,GL_FLOAT,0,afVerticesFront);
    glNormalPointer(GL_FLOAT,0,afNormalsFront);
    BindTexture(m_GLWidget->GetTexture(m_EffectPath.value(0), targetRect.size()),1.0f,1.0f,1.0f); //CubeFront
    glTexCoordPointer(2,GL_FLOAT,0,afTexCoordFront);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //>@Top
    glPopMatrix();
    BindTexture(m_GLWidget->GetTexture(m_EffectPath.value(1), targetRect.size()),1.0f,1.0f,1.0f);//CubeTop
    glRotatef(90,1.0f,0.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //>@Back
    glPopMatrix();
    BindTexture(m_GLWidget->GetTexture(m_EffectPath.value(2), targetRect.size()),1.0f,1.0f,1.0f);//CubeBack
    glRotatef(180,1.0f,0.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //>@Bottom
    glPopMatrix();
    BindTexture(m_GLWidget->GetTexture(m_EffectPath.value(3), targetRect.size()),1.0f,1.0f,1.0f);//CubeBottom
    glRotatef(270,1.0f,0.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //>@Right
    glPopMatrix();
    BindTexture(m_GLWidget->GetTexture(m_EffectPath.value(4), targetRect.size()),1.0f,1.0f,1.0f);//CubeRight
    glRotatef(90,0.0f,1.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //>@Left
    glPopMatrix();
    BindTexture(m_GLWidget->GetTexture(m_EffectPath.value(5), targetRect.size()),1.0f,1.0f,1.0f);//CubeLeft
    glRotatef(270,0.0f,1.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    UnbindTexture();
    glPopMatrix();
    glPopMatrix();

    OpenGL_PaintEnd(p);
}

inline void GraphicsBase::Paint3D_Cylinder(QPainter *p,QRectF targetRect)
{
    OpenGL_PaintBegin(p);
    OpenGL_Effect(p);

    glPushMatrix();
    glScalef(0.5, 0.2, 0.5);
    glPushMatrix();
    Paint3D_CamberSurface(m_GLWidget->GetTexture(m_EffectPath.value(0), targetRect.size()));//SideSurface
    glTranslatef(0.0f,0.5f,0.0f);
    glRotatef(90,1.0f,0.0f,0.0f);
    glRotatef(180,0.0f,0.0f,1.0f);
    glScalef(1.94, 1.94, 1);
    Paint3D_CircleSurface(m_GLWidget->GetTexture(m_EffectPath.value(1), targetRect.size()));//TopCircleSurface
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f,-0.5f,0.0f);
    glRotatef(-90,1.0f,0.0f,0.0f);
    glScalef(1.94, 1.94, 1);
    Paint3D_CircleSurface(m_GLWidget->GetTexture(m_EffectPath.value(2), targetRect.size()));//BottomCircleSurface
    glPopMatrix();
    glPopMatrix();

    OpenGL_PaintEnd(p);
}
#endif

#endif // GraphicsBase_H
