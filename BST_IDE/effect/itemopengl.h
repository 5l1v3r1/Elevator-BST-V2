#ifndef ITEMOPENGL_H
#define ITEMOPENGL_H

#include "global.h"

#define Timer0 4000
#define Timer1 100
#define PI 3.14159265358979323846

typedef enum{
    cube=0,
    disk=1,
    cylinder=2
} object_type;

class GLtexture : public QGLWidget
{
    Q_OBJECT
public:
    GLtexture(QWidget *parent = 0);
    ~GLtexture();

    void loadTexture(int pPara, QList<QPixmap*> pList);
    GLuint  Texture[100];
};
//>@-------------------------------------------
class ItemOpengl : public QGraphicsWidget
{
    Q_OBJECT
public:
    ItemOpengl(QGraphicsItem *parent, QRectF rect);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setGeometry(qreal x, qreal y, qreal width, qreal height);
    void setGeometry(QRectF rect);
    void setScalef(GLfloat sx, GLfloat sy, GLfloat sz);
        float kx,ky,kz;
    int m_size_x;
    int m_size_y;
    int ObjectType;

    EFFECT_INFO m_Effect;

    void Init();
    void VariableInit();
        int x,y,z;
        float Deep;
    void OpenglInit();
    void TimeLineInit();

    void Fraps(QPainter *painter);
        int frames;
        QTime time;

    GLtexture *MyTexture;
        int loadTextureFlag;

    void paintGL(QPainter *painter);
    void EnableDisable();
    void BindTexture(GLuint L_Texture,GLfloat R,GLfloat G,GLfloat B);
    void unbind();
    void paintWhom();
    void SetPaintWhom(int type);

    GLfloat verPoint[7200];
    GLfloat verTexture[7200];
    GLfloat NormalFront[7200];

    void paint_GL_QUADS();
    void paint_Disk(GLuint texture);
    void paint_Test(GLuint texture);
    void paint_Cylinder();
    void paint_Column();

    void EffectTurn(QTimeLine * TimeLine,int anagle);
    void EffectShake(QTimeLine * TimeLine,int anagle);
    void EffectZoom(QTimeLine * TimeLine,int k);
    void EffectPress(int deep);
private:
    QTimeLine * x_animator;
    QTimeLine * y_animator;
    QTimeLine * z_animator;
    QTimeLine * d_animator;
    QTimeLine * o_animator;
    int xFrame,yFrame,zFrame;
    int xoffset,yoffset,zoffset;
    float dFrame,oFrame;
    void StartTimeline(QTimeLine * TimeLine,int Start,int End,int time,QTimeLine::CurveShape shape);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void timerEvent(QTimerEvent *event);
        int TimerId_0;
        int TimerId_1;
    void keyPressEvent(QKeyEvent *event);
signals:

public slots:
    void slot_keyPress(QKeyEvent*event);
    void slot_x_turn(int CurFrame);
    void slot_y_turn(int CurFrame);
    void slot_z_turn(int CurFrame);
    void slot_d_turn(int CurFrame);
    void slot_o_turn(int CurFrame);
};

#endif // ITEMOPENGL_H
