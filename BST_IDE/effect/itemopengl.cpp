#include "itemopengl.h"

GLtexture::GLtexture(QWidget *parent)
    : QGLWidget(parent)
{}

GLtexture::~GLtexture()
{}

void GLtexture::loadTexture(int pPara, QList<QPixmap*> pList)
{
    switch(pPara)
    {
        case EffectTypeCube:
        {
            if(pList.count() < 6)
                break;
            for(int i=20;i<=25;i++)
            {
                Texture[i]=bindTexture(*(pList.at(i-20)));
            }
            break;
        }
        case EffectTypeCylinder:
        {
            if(pList.count() < 3)
                break;
            for(int i=21;i<=25;i++)
            {
                Texture[i]=bindTexture(*(pList.at(i-21)));
            }
            break;
        }
    }
}

//>@-----------------------------------------------------------
ItemOpengl::ItemOpengl(QGraphicsItem *parent, QRectF rect) :
    QGraphicsWidget(parent)
{
    setFlags(ItemIsMovable|ItemIsFocusable);
    QRectF tmpRect = QRectF(rect.left()+rect.width()/2.0,rect.top()+rect.height()/2.0,rect.width(),rect.height());
    setGeometry(tmpRect);
    Init();
}

void ItemOpengl::Init()
{
    VariableInit();
    OpenglInit();
    TimeLineInit();
}

void ItemOpengl::VariableInit()
{
    loadTextureFlag=0;
    x=y=z=Deep=0;
    xFrame=yFrame=zFrame=dFrame=0;
    oFrame=1;
    xoffset=yoffset=zoffset=0;
    kx=ky=kz=1;
    ObjectType=cube;
}

void ItemOpengl::OpenglInit()
{
    MyTexture = new GLtexture;
}

void ItemOpengl::TimeLineInit()
{
    x_animator = new QTimeLine();
    y_animator = new QTimeLine();
    z_animator = new QTimeLine();
    d_animator = new QTimeLine();
    o_animator = new QTimeLine();
    connect(x_animator, SIGNAL(frameChanged(int)), SLOT(slot_x_turn(int)));
    connect(y_animator, SIGNAL(frameChanged(int)), SLOT(slot_y_turn(int)));
    connect(z_animator, SIGNAL(frameChanged(int)), SLOT(slot_z_turn(int)));
    connect(d_animator, SIGNAL(frameChanged(int)), SLOT(slot_d_turn(int)));
    connect(o_animator, SIGNAL(frameChanged(int)), SLOT(slot_o_turn(int)));

    TimerId_0 = startTimer(Timer0);
    TimerId_1 = startTimer(Timer1);
}

void ItemOpengl::StartTimeline(QTimeLine * TimeLine,int Start,int End,int time,QTimeLine::CurveShape shape)
{
    TimeLine->stop();
    TimeLine->setFrameRange(Start, End);
    TimeLine->setDuration(time);
    TimeLine->setCurveShape(shape);
    TimeLine->start();
}
//>@slot
void ItemOpengl::slot_x_turn(int CurFrame)
{
    xFrame=CurFrame+xoffset;
}
void ItemOpengl::slot_y_turn(int CurFrame)
{
    yFrame=CurFrame+yoffset;
}
void ItemOpengl::slot_z_turn(int CurFrame)
{
    zFrame=CurFrame+zoffset;
}
void ItemOpengl::slot_d_turn(int CurFrame)
{
    dFrame=(float)CurFrame/20.0f;
}
void ItemOpengl::slot_o_turn(int CurFrame)
{
    oFrame=(float)CurFrame/100.0f;
}

QRectF ItemOpengl::boundingRect() const
{
    return QRectF(-m_size_x / 2, -m_size_y / 2, m_size_x, m_size_y).normalized();
}

QPainterPath ItemOpengl::shape() const
{
    QPainterPath path;
    path.addRect(QRectF(-m_size_x / 2, -m_size_y / 2, m_size_x, m_size_y).normalized());
    return path;
}

void ItemOpengl::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    paintGL(painter);
#ifdef fps
    Fraps(painter);
#endif
}

void ItemOpengl::setGeometry(qreal x, qreal y, qreal width, qreal height)
{
    setPos(x,y);
    m_size_x = width;
    m_size_y = height;
}

void ItemOpengl::setGeometry(QRectF rect)
{
    setPos(rect.left(),rect.top());
    m_size_x = rect.width();
    m_size_y = rect.height();
}

void ItemOpengl::setScalef(GLfloat sx, GLfloat sy, GLfloat sz)
{
    kx=sx;
    ky=sy;
    kz=sz;
}

//>@slot
void ItemOpengl::slot_keyPress(QKeyEvent*event)
{
    keyPressEvent(event);
}

void ItemOpengl::keyPressEvent(QKeyEvent *event)
{
    QGraphicsItem::keyPressEvent(event);
    switch (event->key())
    {
        case Qt::Key_A:
            y+=-2;
            //update(geometry());
        break;
        case Qt::Key_D:
            y+=2;
            //update(geometry());
        break;
        case Qt::Key_W:
            x+=2;
            //update(geometry());
        break;
        case Qt::Key_S:
            x+=-2;
            //update(geometry());
        break;
        case Qt::Key_E:
            z+=2;
            //update(geometry());
        break;
        case Qt::Key_Q:
            z+=-2;
            //update(geometry());
        break;
        case Qt::Key_X:
            Deep+=-0.1;
            //update(geometry());
        break;
        case Qt::Key_V:
            Deep+=0.1;
            //update(geometry());
        break;
    }
}

void ItemOpengl::Fraps(QPainter *painter)
{
    QString framesPerSecond;
    framesPerSecond.setNum(frames /(time.elapsed() / 1000.0), 'f', 2);
    QFont font("Arial", 20, QFont::DemiBold, true);
    font.setUnderline(false);
    painter->setFont(font);
    painter->setPen(QColor(250,250,0));
    painter->drawText(QRectF(-70, -20, 140, 40).normalized(), Qt::AlignCenter, "fps="+framesPerSecond);
    if (!(frames % 100))
    {
        time.start();
        frames = 0;
    }
    frames ++;
}

void ItemOpengl::paintGL(QPainter *painter)
{
    QRectF rect = boundingRect().translated(pos()).normalized();
    float width = float(painter->device()->width());
    float height = float(painter->device()->height());
    //qDebug()<<pos().x()<<"---"<<pos().y();
    //qDebug()<<m_size_x<<"+++"<<m_size_y;
    float left = 2.0f * float(rect.left()) / width - 1.0f;
    float right = 2.0f * float(rect.right()) / width - 1.0f;
    float top = 1.0f - 2.0f * float(rect.top()) / height;
    float bottom = 1.0f - 2.0f * float(rect.bottom()) / height;
    float moveToRectMatrix[] = {
        0.5f* (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f* (bottom - top), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f* (right + left), 0.5f* (bottom + top), 0.0f, Deep+dFrame+1.0f
    };
//    glClearColor(0.1f, 0.1f, 0.2f, 0.0f);
    painter->beginNativePainting();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(moveToRectMatrix);
//>@视角不支持
//    glFrustum(-1, 1, -1, 1, 20.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    EnableDisable();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    painter->endNativePainting();
}

void ItemOpengl::EnableDisable()
{
//>@视角不支持，Z轴深度不支持，采用修改投影矩阵/glscale实现缩放
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_FLAT);
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
//>@非可见面删除，关闭极大影响性能
    glEnable(GL_CULL_FACE);
//>@GL_CCW 表示窗口坐标上投影多边形的顶点顺序为逆时针方向的表面为正面。
//>@GL_CW 表示顶点顺序为顺时针方向的表面为正面。顶点的方向又称为环绕。
    glFrontFace(GL_CCW);
//>@混色不支持，采用物体自发光方式绕过
//    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
//    glEnable(GL_DEPTH_TEST);
//    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_LIGHTING);
//>@透明度支持存在问题
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LEQUAL);
//    glDisable(GL_DEPTH_TEST);
//    glColor4f( 1.0, 1.0, 1.0, 0.13 );
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//    glEnable(GL_BLEND);
//>@禁用抖动特效，以提高性能。
    glDisable(GL_DITHER);
    float lightColour0[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float lightDir0[] = {0.0f, 0.0f, 10.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColour0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightDir0);
    glEnable(GL_LIGHT0);

    float lightColour1[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float lightDir1[] = {0.0f, 0.0f, -10.0f, 1.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColour1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightDir1);
    glEnable(GL_LIGHT1);

    float lightColour2[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float lightDir2[] = {0.0f, 10.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightColour2);
    glLightfv(GL_LIGHT2, GL_POSITION, lightDir2);
    glEnable(GL_LIGHT2);

    float lightColour3[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float lightDir3[] = {0.0f, -10.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT3, GL_DIFFUSE, lightColour3);
    glLightfv(GL_LIGHT3, GL_POSITION, lightDir3);
    glEnable(GL_LIGHT3);

    float lightColour4[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float lightDir4[] = {10.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT4, GL_DIFFUSE, lightColour4);
    glLightfv(GL_LIGHT4, GL_POSITION, lightDir4);
    glEnable(GL_LIGHT4);

    float lightColour5[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float lightDir5[] = {-10.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT5, GL_DIFFUSE, lightColour5);
    glLightfv(GL_LIGHT5 ,GL_POSITION, lightDir5);
    glEnable(GL_LIGHT5);

//    GLfloat light_ambient[]={0.7,0.8,0.9,1.0};
//    GLfloat light_diffuse[]={1.0,1.0,1.0,1.0};
//    GLfloat light_specular[]={1.0,1.0,1.0,1.0};
//    GLfloat light_position[]={2.0,2.0,2.0,0.0};
//    GLfloat mat_diffuse[]={0.8,0.8,0.8,1.0};

//    glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
//    glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
//    glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);
//    glLightfv(GL_LIGHT0,GL_POSITION,light_position);

//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    glEnable(GL_AUTO_NORMAL);
//    glEnable(GL_NORMALIZE);
//    //glEnable(GL_CULL_FACE);
//    //启用双面光照
//    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
//    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mat_diffuse);

    paintWhom();

    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_NORMALIZE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHT0);
}

void ItemOpengl::BindTexture(GLuint L_Texture,GLfloat R,GLfloat G,GLfloat B)
{
    glPushMatrix();
    glMatrixMode(GL_TEXTURE);
    //>@物体自身发光
    GLfloat matDiff[] = {R, G, B, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
    //>@绑定纹理
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, L_Texture);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void ItemOpengl::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void ItemOpengl::timerEvent( QTimerEvent *event )
{
    QGraphicsWidget::timerEvent(event);
    if(loadTextureFlag==0)
    {
        QList<QPixmap*> tmpList;
        MyTexture->loadTexture(EffectTypeCube, tmpList);
        loadTextureFlag=1;
    }
    if(event->timerId()==TimerId_0)
    {
        if(ObjectType == cylinder)
        {
            //EffectTurn(x_animator,180);
        }

        EffectTurn(x_animator,180);
        EffectTurn(y_animator,360);
        EffectTurn(z_animator,360);

        //EffectZoom(o_animator,30);
        //EffectPress(100);
        static int i=0;

        //SetPaintWhom(i);
        i++;
        if(i>1)i=0;
    }
    if(event->timerId()==TimerId_1)
    {

    }
}

void ItemOpengl::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    EffectTurn(x_animator,180);
    EffectTurn(y_animator,360);
    EffectTurn(z_animator,360);
    //EffectZoom(o_animator,50);
    //EffectShake(y_animator,6);
}

void ItemOpengl::EffectTurn(QTimeLine * TimeLine,int anagle)
{
    xoffset=yoffset=zoffset=0;
    static int i=0;
    switch(i)
    {
        case 0:StartTimeline(TimeLine,0,anagle,3200,QTimeLine::EaseInOutCurve);break;
        case 1:StartTimeline(TimeLine,anagle,anagle*2,3200,QTimeLine::EaseInOutCurve);break;
        case 2:StartTimeline(TimeLine,anagle*2,anagle*3,3200,QTimeLine::EaseInOutCurve);break;
        case 3:StartTimeline(TimeLine,anagle*3,anagle*4,3200,QTimeLine::EaseInOutCurve);break;
        default:break;
    }
    i++;
    if(i>3)i=0;
}

void ItemOpengl::EffectZoom(QTimeLine * TimeLine,int k)
{
    xoffset=yoffset=zoffset=0;
    static int i=0;
    switch(i)
    {
        case 0:StartTimeline(TimeLine,100,k,3200,QTimeLine::EaseInOutCurve);break;
        case 1:StartTimeline(TimeLine,k,100,3200,QTimeLine::EaseInOutCurve);break;
        default:break;
    }
    i++;
    if(i>1)i=0;
}

void ItemOpengl::EffectPress(int deep)
{
    xoffset=yoffset=zoffset=0;
    static int i=0;
    switch(i)
    {
        case 0:StartTimeline(d_animator,0,deep,3200,QTimeLine::CosineCurve);break;
        case 1:StartTimeline(d_animator,deep,0,3200,QTimeLine::EaseInOutCurve);break;
        default:break;
    }
    i++;
    if(i>1)i=0;
}

void ItemOpengl::EffectShake(QTimeLine * TimeLine,int anagle)
{
    StartTimeline(TimeLine,-anagle,anagle,60,QTimeLine::CosineCurve);
    if(TimeLine==x_animator)xoffset=1;
    else if(TimeLine==y_animator)yoffset=1;
    else if(TimeLine==z_animator)zoffset=1;
}

void ItemOpengl::paint_GL_QUADS()
{
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
    BindTexture(MyTexture->Texture[20],1.0f,1.0f,1.0f);
    glTexCoordPointer(2,GL_FLOAT,0,afTexCoordFront);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//>@Top
    glPopMatrix();
    BindTexture(MyTexture->Texture[21],1.0f,1.0f,1.0f);
    glRotatef(90,1.0f,0.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//>@Back
    glPopMatrix();
    BindTexture(MyTexture->Texture[22],1.0f,1.0f,1.0f);
    glRotatef(180,1.0f,0.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//>@Bottom
    glPopMatrix();
    BindTexture(MyTexture->Texture[23],1.0f,1.0f,1.0f);
    glRotatef(270,1.0f,0.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//>@Right
    glPopMatrix();
    BindTexture(MyTexture->Texture[24],1.0f,1.0f,1.0f);
    glRotatef(90,0.0f,1.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//>@Left
    glPopMatrix();
    BindTexture(MyTexture->Texture[25],1.0f,1.0f,1.0f);
    glRotatef(270,0.0f,1.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    unbind();
    glPopMatrix();
    glPopMatrix();
}

void ItemOpengl::paint_Test(GLuint texture)
{
    glPushMatrix();
    glFrontFace(GL_CCW);

    glPushMatrix();
    glTranslatef(0.0f,0.0f,0.5f);
    paint_Disk(MyTexture->Texture[20]);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180,1.0f,0.0f,0.0f);
    glTranslatef(0.0f,0.0f,0.5f);
    paint_Disk(MyTexture->Texture[20]);
    glPopMatrix();

    glTranslatef(-0.5f,-0.5f,0.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    for(int i=0;i<=360;i+=6)
    {
        double aa=(double)PI*i/180;
        verPoint[i]=(float)((cos(aa)+1.0)*0.5);   //纹理横坐标
        verPoint[i+1]=(float)((sin(aa)+1.0)*0.5);  //纹理纵坐标
        verPoint[i+2]=0.5;  //纹理纵坐标

        verPoint[i+3]=(float)((cos(aa)+1.0)*0.5);   //纹理横坐标
        verPoint[i+4]=(float)((sin(aa)+1.0)*0.5);  //纹理纵坐标
        verPoint[i+5]=-0.5;  //纹理纵坐标
    }
    glVertexPointer(3, GL_FLOAT, 0, verPoint);

    for(int i=0;i<=360;i+=6)
    {
        double aa=(double)PI*i/180;
        verPoint[i]=(float)((cos(aa)+1.0)*0.5);   //纹理横坐标
        verPoint[i+1]=(float)((sin(aa)+1.0)*0.5);  //纹理纵坐标
    }
    BindTexture(texture,1.0f,1.0f,1.0f);
    glTexCoordPointer(3,GL_FLOAT,0,verPoint);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 122);
    unbind();
    glPopMatrix();
}

void ItemOpengl::paint_Disk(GLuint texture)
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
        verPoint[i]=(float)((cos(aa)+1.0)*0.5);   //纹理横坐标
        verPoint[i+1]=(float)((sin(aa)+1.0)*0.5);  //纹理纵坐标
    }
    glVertexPointer(2, GL_FLOAT, 0, verPoint);
    BindTexture(texture,1.0f,1.0f,1.0f);
    glTexCoordPointer(2,GL_FLOAT,0,verPoint);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 180);
    unbind();
    glPopMatrix();
}

void ItemOpengl::paint_Cylinder()
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
            verPoint[0] = -0.0;verPoint[1] = 0.0;verPoint[2] = r;
            verPoint[3] = b;verPoint[4] = -1.0;verPoint[5] = r;
            verPoint[6] = -0.0;verPoint[7] = -1.0;verPoint[8] = r;

            verPoint[9] = b;verPoint[10] = -1.0;verPoint[11] = r;
            verPoint[12] = -0.0;verPoint[13] = 0.0;verPoint[14] = r;
            verPoint[15] = b;verPoint[16] = 0.0;verPoint[17] = r;
            GLfloat NormalFront[] = {
                0,0,-1, 0,0,-1, 0,0,-1,
                0,0,-1, 0,0,-1, 0,0,-1
            };
            memcpy(verTexture,verPoint,18*sizeof(GLfloat));
            verTexture[0] = verPoint[0]+b*i;
            verTexture[3] = verPoint[3]+b*i;
            verTexture[6] = verPoint[6]+b*i;

            verTexture[9] = verPoint[9]+b*i;
            verTexture[12] = verPoint[12]+b*i;
            verTexture[15] = verPoint[15]+b*i;

            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3,GL_FLOAT,0,verPoint);
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT,0,NormalFront);
            BindTexture(MyTexture->Texture[22],1.0f,1.0f,1.0f);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(3,GL_FLOAT,0,verTexture);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glRotatef(fragAngle, 0, 1, 0);
        }
    }
    unbind();
    glPopMatrix();
}

void ItemOpengl::paint_Column()
{
    glPushMatrix();
        glScalef(0.5, 0.2, 0.5);
        glPushMatrix();
            paint_Cylinder();
            glTranslatef(0.0f,0.5f,0.0f);
            glRotatef(90,1.0f,0.0f,0.0f);
            glRotatef(180,0.0f,0.0f,1.0f);
            glScalef(1.94, 1.94, 1);
            paint_Disk(MyTexture->Texture[22]);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.0f,-0.5f,0.0f);
            glRotatef(-90,1.0f,0.0f,0.0f);
            glScalef(1.94, 1.94, 1);
            paint_Disk(MyTexture->Texture[22]);
        glPopMatrix();
    glPopMatrix();
}

void ItemOpengl::SetPaintWhom(int type)
{
    ObjectType = type;
}

void ItemOpengl::paintWhom()
{
    //glTranslatef(0.0f,0.5f,0.0f);
    glRotatef(-x-xFrame,1.0f,0.0f,0.0f);
    glRotatef(-y-yFrame,0.0f,1.0f,0.0f);
    glRotatef(-z-zFrame,0.0f,0.0f,1.0f);
    glScalef(kx*oFrame,ky*oFrame,kz*oFrame);
    if(ObjectType == cylinder)
    {
        x+=1;
        y+=2;
        z+=1;
    }
    //ObjectType=disk;
    switch(ObjectType)
    {
        case cube:
            paint_GL_QUADS();
            break;
        case cylinder:
            paint_Test(MyTexture->Texture[20]);
            break;
        //case cylinder:paint_Cylinder();break;
        default:
            paint_GL_QUADS();
            break;
    }
    //paint_Disk(MyTexture->Texture[15]);
}
