#include "designbase.h"

bool DesignRC::InitEffect(COM_INFO *pComInfo)
{
//    if(m_ComInfo == pComInfo && pComInfo->GetUpdateLevel()==COM_INFO::UPDATENONE)
//    {
//        IDE_TRACE();
//        return true;
//    }
    if(pComInfo == 0)
    {
        IDE_TRACE();
        return false;
    }
    m_ComInfo = pComInfo;
    m_EffectValid = false;
    m_EffectPath.clear();
    STATE_INFO* tmp;
    for(int i=0;i<pComInfo->StateMachine.count();i++)
    {
        tmp = pComInfo->StateMachine.at(i);
        if(tmp == 0)
            continue;
        InitEffectRc(tmp);
    }
    //>@判断资源
    return Start();
}

bool DesignRC::InitEffectRc(STATE_INFO* pEffect)
{
    return true;
}

bool DesignRC::isValid()
{
    if(m_EffectPath.count() > 0)
    {
        m_EffectValid = true;
        return true;
    }
    m_EffectValid = false;
    return false;
}

QPixmap DesignRC::GetPixmap(int pKey, QSizeF pSize)
{
    QPixmap tmpPixmap;
    //>@首先查找m_EffectPath
    QString tmpPath = m_EffectPath.value(pKey);
    if(tmpPath.isEmpty())
        return tmpPixmap;
    RC_TYPE tmpRcType = getRcType(tmpPath);
    switch(tmpRcType)
    {
        case RC_BMP:
        case RC_JPG:
        case RC_PNG:
        {
            QImage tmpImage;
            if(tmpImage.load(tmpPath) == false)
                return tmpPixmap;
            if(pSize.isEmpty())
                tmpPixmap = QPixmap::fromImage(tmpImage);
            else
                tmpPixmap = QPixmap::fromImage(tmpImage.scaled(pSize.toSize(), Qt::IgnoreAspectRatio));
            break;
        }
        case RC_GIF:
        {
            break;
        }
        case RC_SVG:
        {
            QSvgRenderer *tmpRenderer = new QSvgRenderer(tmpPath);
            if(tmpRenderer->isValid() == false)
            {
                tmpRenderer->deleteLater();
                return tmpPixmap;
            }
            if(pSize.isEmpty())
                return tmpPixmap;
            QImage tmpImage(pSize.width(), pSize.height(), QImage::Format_ARGB32_Premultiplied);
            if(tmpImage.isNull())
                return tmpPixmap;
            tmpImage.fill(Qt::transparent); //>@创建透明图层
            QPainter p(&tmpImage);
            tmpRenderer->render(&p);
            p.end();
            tmpPixmap = QPixmap::fromImage(tmpImage);
            break;
        }
        case RC_TXT:
        {
            break;
        }
        default:break;
    }
    return tmpPixmap;
}

bool DesignRC::Start()
{
    if(m_ComInfo)
    {
        m_ComName = getCurComName(m_ComInfo->ComPath);
        m_ComType = getCurComType(m_ComInfo->ComPath);
    }
    return isValid();
}

bool DesignRC::analysisEffectPara(AREA_ANIMATE* pAreaEffect, QString pEffectPara)
{
    if(pEffectPara.isEmpty())
        return false;
    QStringList tmpList = pEffectPara.split(";", QString::SkipEmptyParts);
    for(int i=0;i<tmpList.count();i++)
    {
        QStringList tmpSubList = tmpList.at(i).split(":", QString::SkipEmptyParts);
        if(tmpSubList.count()<2)
        {
            IDE_TRACE();
            continue;
        }
        QString tmpString1 = tmpSubList.at(0);
        QString tmpString2 = tmpSubList.at(1);
        if(tmpString1.isEmpty() || tmpString2.isEmpty())
        {
            continue;
        }
        analysisEffectParaItem(pAreaEffect, tmpString1, tmpString2);
    }
    return true;
}

bool DesignRC::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    if(pAreaEffect == 0)
        return false;
    if(pLabel.compare(PARA_FRAMERANGE, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
        {
            pAreaEffect->mFrameStart = 0;
            pAreaEffect->mFrameEnd = dec;
        }
    }
    else if(pLabel.compare(PARA_INTERVAL, Qt::CaseInsensitive) == 0)
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            pAreaEffect->mAnimateTime = dec;
    }
    else if(pLabel.compare(PARA_DIRECTION, Qt::CaseInsensitive) == 0)
    {
        if(pContent.compare(QString("H"), Qt::CaseInsensitive) == 0)
            pAreaEffect->mDirection = Horizontal;
        else if(pContent.compare(QString("V"), Qt::CaseInsensitive) == 0)
            pAreaEffect->mDirection = Vertical;
    }
    else if(pLabel.compare(PARA_ORIENTATION, Qt::CaseInsensitive) == 0)
    {
        if(pContent.compare(QString("P"), Qt::CaseInsensitive) == 0)
            pAreaEffect->mOrientation = Positive;
        else if(pContent.compare(QString("N"), Qt::CaseInsensitive) == 0)
            pAreaEffect->mOrientation = Negative;
    }
    else if(pLabel.compare(PARA_CURVESHAPE, Qt::CaseInsensitive) == 0)
    {
        if(pContent.compare(QString("EaseIn"), Qt::CaseInsensitive) == 0)
            pAreaEffect->mAnimateType = QTimeLine::EaseInCurve;
        else if(pContent.compare(QString("EaseOut"), Qt::CaseInsensitive) == 0)
            pAreaEffect->mAnimateType = QTimeLine::EaseOutCurve;
        else if(pContent.compare(QString("EaseInOut"), Qt::CaseInsensitive) == 0)
            pAreaEffect->mAnimateType = QTimeLine::EaseInOutCurve;
        else if(pContent.compare(QString("Linear"), Qt::CaseInsensitive) == 0)
            pAreaEffect->mAnimateType = QTimeLine::LinearCurve;
        else if(pContent.compare(QString("Sine"), Qt::CaseInsensitive) == 0)
            pAreaEffect->mAnimateType = QTimeLine::SineCurve;
        else if(pContent.compare(QString("Cosine"), Qt::CaseInsensitive) == 0)
            pAreaEffect->mAnimateType = QTimeLine::CosineCurve;
    }
    return true;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

DesignGLWidget::DesignGLWidget(QWidget *parent)
    : QGLWidget(parent)
{}

DesignGLWidget::~DesignGLWidget()
{}

bool DesignGLWidget::InitTexture(DesignBase *pDesignWidget)
{
    if(pDesignWidget = 0)
        return false;
    QList<int> tmpKeyList = pDesignWidget->m_EffectPath.keys();
    int count = tmpKeyList.count();
    int tmpKey;
    for(int i=0;i<count;i++)
    {
        tmpKey = tmpKeyList.at(i);
        QPixmap tmpPixmap;
        tmpPixmap.load(pDesignWidget->m_EffectPath.value(tmpKey));
        if(tmpPixmap.isNull())
            continue;
        GLuint tmpGLuint = bindTexture(tmpPixmap);
        pDesignWidget->m_EffectTexture.insert(tmpKey, tmpGLuint);
    }
    return true;
}

GLuint DesignGLWidget::GetTexture(QString pImagePath, QSizeF pSize)
{
    QImage tmpImage(pImagePath);
    if(tmpImage.isNull())
    {
        IDE_TRACE_STR(pImagePath);
        return 0;
    }
    return bindTexture(QPixmap::fromImage(tmpImage.scaled(pSize.toSize())));
}

DesignBase::DesignBase(QGraphicsItem *parent, QRectF pRect):
    QGraphicsWidget(parent, Qt::FramelessWindowHint),
    DesignRC()
{
    //setFlags(ItemIsMovable);
    m_Rect = pRect;
    setGeometry(m_Rect);
    m_GLWidget = 0;
    m_3DXDiff = m_3DYDiff = m_3DZDiff = 0; //>@三个方向上的角度偏移
    m_3DXScale = m_3DYScale = m_3DZScale = 0; //>@三个方向上的缩放
    m_3DDeepDiff = 0;
    m_RcType = RCIMAGE;
    m_EffectValid = false;
    m_AreaAnimate = 0;
    m_ActionGroup = 0;
    m_ActionGroup = new QActionGroup(this);
    connect(m_ActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(ActionTrigger(QAction *)));

    connect(&m_Animator, SIGNAL(finished()), SLOT(OperateStep1()));
    connect(&m_Animator, SIGNAL(frameChanged(int)), SLOT(UpdateCom(int)));
}

void DesignBase::UpdateCom(int pFrame)
{
    update(rect());
}

QRectF DesignBase::boundingRect() const
{
    QRectF tmpRect = this->geometry().normalized();
    return QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized();
}

QPainterPath DesignBase::shape() const
{
    QPainterPath path;
    QRectF tmpRect = this->geometry().normalized();
    path.addRect(QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized());
    return path;
}

bool DesignBase::InitEffectRc(STATE_INFO* pEffect)
{
    if(pEffect == 0)
        return false;
    return true;
}

bool DesignBase::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return DesignRC::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool DesignBase::Start()
{
    if(m_ComInfo)
        setData(COMNAME, QVariant(m_ComInfo->ComPath));
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP0].mOperate = OPERATE_NONE;
    m_OperateInfo[STEP1].mValid = false;
    m_OperateInfo[STEP1].mOperate = OPERATE_NONE;
    update();
    return DesignRC::Start();
}


void DesignBase::StopAnimate()
{
    if(m_Animator.state() == QTimeLine::Running)
        m_Animator.stop();
}

void DesignBase::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(m_EffectValid)
        PaintEffect(painter);

    //>@绘制边框
    QRectF tmpRect = this->geometry().normalized();
    qreal Width = tmpRect.width();
    qreal Height = tmpRect.height();
    painter->translate(Width / 2, Height / 2);
    painter->setPen(Qt::DashDotDotLine);
    painter->setPen(QColor(10,10,255));
    painter->drawRect(QRect(-Width/2, -Height/2, Width, Height));
    painter->drawRect(QRect(-Width/2+1, -Height/2+1, Width-2, Height-2));
}

void DesignBase::ExecAction(QAction *pAction)
{

}

void DesignBase::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    if(m_EffectValid == false)
        return;
}

void DesignBase::OperateStep0()
{
    if(m_EffectValid == false)
        return;
    update();
    //>@执行STEP0中的内容，如果执行不成功，尝试执行STEP1中的内容
    if(m_OperateInfo[STEP0].mValid)
    {
        m_AreaAnimate = m_EffectGroup.value(m_OperateInfo[STEP0].mOperate);
        if(m_AreaAnimate)
        {
            StartTimeline(m_AreaAnimate->mFrameStart,
                          m_AreaAnimate->mFrameEnd,
                          m_AreaAnimate->mAnimateTime,
                          m_AreaAnimate->mAnimateType);
            return;
        }
    }
    OperateStep1();
}

void DesignBase::OperateStep1()
{
    if(m_EffectValid == false)
        return;
    //>@准备资源，两个必定为同一张图片
    m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer;
    update();
    if(m_OperateInfo[STEP1].mValid)
    {
        m_AreaAnimate = m_EffectGroup.value(m_OperateInfo[STEP1].mOperate);
        if(m_AreaAnimate)
        {
            StartTimeline(m_AreaAnimate->mFrameStart,
                          m_AreaAnimate->mFrameEnd,
                          m_AreaAnimate->mAnimateTime,
                          m_AreaAnimate->mAnimateType);
        }
    }
}

void DesignBase::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_ComType != COM_DESKTOP)
        setCursor(Qt::ClosedHandCursor);
    QGraphicsWidget::mousePressEvent(event);
}

void DesignBase::focusInEvent(QFocusEvent *event)
{
    if(m_ComType != COM_DESKTOP)
        setCursor(Qt::OpenHandCursor);
    QGraphicsWidget::focusOutEvent(event);
}

void DesignBase::focusOutEvent(QFocusEvent *event)
{
    if(m_ComType != COM_DESKTOP)
        setCursor(Qt::ArrowCursor);
    QGraphicsWidget::focusOutEvent(event);
}

void DesignBase::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsWidget::mouseMoveEvent(event);
}

void DesignBase::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_ComType != COM_DESKTOP)
    {
        setCursor(Qt::ArrowCursor);
        emit MouseRelease(event->scenePos());
    }
    QGraphicsWidget::mouseReleaseEvent(event);
}

void DesignBase::setGeometry3D(qreal x, qreal y, qreal width, qreal height)
{
    setPos(x,y);
    m_3DWidth = width;
    m_3DHeight = height;
}

void DesignBase::setGeometry3D(QRectF rect)
{
    setPos(rect.left(),rect.top());
    m_3DWidth = rect.width();
    m_3DHeight = rect.height();
}

void DesignBase::BindTexture(GLuint L_Texture,GLfloat R,GLfloat G,GLfloat B)
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

void DesignBase::UnbindTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void DesignBase::OpenGL_PaintBegin(QPainter *painter)
{
    //>@创建GLWidget资源
    if(m_GLWidget == 0)
        m_GLWidget = new DesignGLWidget;

    QRectF rect = boundingRect().translated(pos()).normalized();
    float width = float(painter->device()->width());
    float height = float(painter->device()->height());
    float left = 2.0f * float(rect.left()) / width - 1.0f;
    float right = 2.0f * float(rect.right()) / width - 1.0f;
    float top = 1.0f - 2.0f * float(rect.top()) / height;
    float bottom = 1.0f - 2.0f * float(rect.bottom()) / height;
    float moveToRectMatrix[] = {
                                    0.5f* (right - left), 0.0f, 0.0f, 0.0f,
                                    0.0f, 0.5f* (bottom - top), 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0.5f* (right + left), 0.5f* (bottom + top), 0.0f, 1.0f
                                };
    painter->beginNativePainting();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(moveToRectMatrix);
    //>@视角不支持
    //glFrustum(-1, 1, -1, 1, 20.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    //>Enable  Disable
    //>@视角不支持，Z轴深度不支持，采用修改投影矩阵/glscale实现缩放
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_FLAT);
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    //>@非可见面删除，如果关闭则极大影响性能
    glEnable(GL_CULL_FACE);
    //>@GL_CCW 表示窗口坐标上投影多边形的顶点顺序为逆时针方向的表面为正面。
    //>@GL_CW 表示顶点顺序为顺时针方向的表面为正面。顶点的方向又称为环绕。
    glFrontFace(GL_CCW);
    //>@混色不支持，采用物体自发光方式绕过
    //glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    //glEnable(GL_DEPTH_TEST);
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_LIGHTING);
    //>@透明度支持存在问题
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);
    //glDisable(GL_DEPTH_TEST);
    //glColor4f( 1.0, 1.0, 1.0, 0.13 );
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //glEnable(GL_BLEND);
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

    //GLfloat light_ambient[]={0.7,0.8,0.9,1.0};
    //GLfloat light_diffuse[]={1.0,1.0,1.0,1.0};
    //GLfloat light_specular[]={1.0,1.0,1.0,1.0};
    //GLfloat light_position[]={2.0,2.0,2.0,0.0};
    //GLfloat mat_diffuse[]={0.8,0.8,0.8,1.0};

    //glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
    //glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
    //glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);
    //glLightfv(GL_LIGHT0,GL_POSITION,light_position);

    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    //glEnable(GL_AUTO_NORMAL);
    //glEnable(GL_NORMALIZE);
    //glEnable(GL_CULL_FACE);
    //启用双面光照
    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
    //glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mat_diffuse);
}

void DesignBase::OpenGL_PaintEnd(QPainter *painter)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_NORMALIZE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHT0);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    painter->endNativePainting();
}

void DesignBase::OpenGL_Effect(QPainter *painter)
{
    //>@旋转
    //glTranslatef(0.0f,0.5f,0.0f);
    glRotatef(-m_3DXDiff,1.0f,0.0f,0.0f);
    glRotatef(-m_3DYDiff,0.0f,1.0f,0.0f);
    glRotatef(-m_3DZDiff,0.0f,0.0f,1.0f);
    //>@缩放特效
    glScalef(m_3DXScale, m_3DYScale, m_3DZScale);
}

void DesignBase::Set3DEffect(EffectType pType)
{

}

void DesignBase::Paint3D_CircleSurface(GLuint texture)
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

void DesignBase::Paint3D_CamberSurface(GLuint texture)
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

void DesignBase::Paint3D_Cube(QPainter *p,QRectF targetRect)
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

void DesignBase::Paint3D_Cylinder(QPainter *p,QRectF targetRect)
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

void DesignBase::keyPressEvent(QKeyEvent *event)
{
    if(m_EffectValid == false)
        return;
    switch (event->key())
    {
        case Qt::Key_Up:
            if(event->modifiers() == Qt::ControlModifier)
                m_3DYDiff += 2;
            else if(event->modifiers() == Qt::AltModifier)
                m_3DYScale += 2;
            else if(event->modifiers() == Qt::ShiftModifier)
                m_3DDeepDiff += 0.1;
            break;
        case Qt::Key_Down:
            if(event->modifiers() == Qt::ControlModifier)
                m_3DYDiff -= 2;
            else if(event->modifiers() == Qt::AltModifier)
                m_3DYScale -= 2;
            else if(event->modifiers() == Qt::ShiftModifier)
                m_3DDeepDiff -= 0.1;
            break;
        case Qt::Key_Left:
            if(event->modifiers() == Qt::ControlModifier)
                m_3DXDiff += 2;
            else if(event->modifiers() == Qt::AltModifier)
                m_3DXScale += 2;
            else if(event->modifiers() == Qt::ShiftModifier)
                m_3DDeepDiff += 0.1;
            break;
        case Qt::Key_Right:
            if(event->modifiers() == Qt::ControlModifier)
                m_3DXDiff -= 2;
            else if(event->modifiers() == Qt::AltModifier)
                m_3DXScale -= 2;
            else if(event->modifiers() == Qt::ShiftModifier)
                m_3DDeepDiff -= 0.1;
            break;
        default:
            return;
    }
    QGraphicsItem::keyPressEvent(event);
}
