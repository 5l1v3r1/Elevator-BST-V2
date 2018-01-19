#include "graphicscomponent.h"
#include "graphicsscene.h"

GraphicsComponent::GraphicsComponent(QGraphicsItem *parent, GraphicsScene *pScene, QRectF pRect) :
    QGraphicsWidget(parent, Qt::FramelessWindowHint), GraphicsRC(pScene, pRect)
{
    setAttribute(Qt::WA_DeleteOnClose);
    //setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

#ifdef  SURRPORT_3D
    m_GLWidget = 0;
    m_3DXDiff = m_3DYDiff = m_3DZDiff = 0; //>@三个方向上的角度偏移
    m_3DXScale = m_3DYScale = m_3DZScale = 0; //>@三个方向上的缩放
    m_3DDeepDiff = 0;
#endif

    m_PaintBusy = false;
    m_PaintOptimizer = false;

    m_AreaAnimate = 0;
    m_UpdateOPerate = OPERATE_NONE;
    m_UpdatePara = QVariant();

    m_ActionGroup = 0;
    m_ActionGroup = new QActionGroup(this);
    connect(m_ActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(ExecAction(QAction *)));

    connect(&m_Animator, SIGNAL(frameChanged(int)), this, SLOT(UpdateCom(int)));
}

GraphicsComponent::~GraphicsComponent()
{
    m_Animator.stop();
}

void GraphicsComponent::ReleaseTextures()
{
#ifdef  SURRPORT_3D
    QList<int> tmpList = m_EffectTexture.keys();
    for(int i=0;i<tmpList.count();i++)
    {
        GLuint tmpTexture = m_EffectTexture.take(tmpList.at(i));
        if(m_GLWidget)
            m_GLWidget->deleteTexture(tmpTexture);
    }
#endif
}

void GraphicsComponent::UpdateCom(int pFrame)
{
    update(rect());
}

QRectF GraphicsComponent::boundingRect() const
{
    QRectF tmpRect = this->geometry().normalized();
    return QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized();
}

QPainterPath GraphicsComponent::shape() const
{
    QPainterPath path;
    QRectF tmpRect = this->geometry().normalized();
    path.addRect(QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized());
    return path;
}

bool GraphicsComponent::Start()
{
    if(!GraphicsRC::Start())
        return false;
    setData(COMTYPE, QVariant(m_ComType));
    SetComGeometory(m_ComRect);
    m_EffectValid = false;
    if(m_EffectPath.isEmpty())
        return false;
    //>@用于制作循环播放效果
    m_RcList = m_EffectPath.keys();
    m_EffectValid = true;
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP0].mOperate = OPERATE_NONE;
    m_OperateInfo[STEP1].mValid = false;
    m_OperateInfo[STEP1].mOperate = OPERATE_NONE;
    return true;
}

void GraphicsComponent::SetComGeometory(QRectF pRect)
{
    m_ComRect = pRect;
    setGeometry(m_ComRect);
}

void GraphicsComponent::StopAnimate()
{
    if(m_Animator.state() == QTimeLine::Running)
        m_Animator.stop();
    m_AreaAnimate = 0;
    UpdateCom(0);
}

void GraphicsComponent::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!m_PaintBusy)
        m_PaintBusy = true;
    else
        return;
    if(m_PaintOptimizer)
    {
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->setRenderHint(QPainter::Antialiasing);
    }
    PaintEffect(painter);
    m_PaintBusy = false;
}

bool GraphicsComponent::PaintEffect(QPainter *p)
{
    if(m_EffectValid == false)
        return false;
    if(!m_AreaAnimate)
    {
        Paint2D_None(p, rect());
        return false;
    }
    switch(m_AreaAnimate->mEffectType)
    {
        case EffectTypeRoll:
            Paint2D_Roll(p, rect());
            break;
        case EffectTypeFlip:
            Paint2D_Flip(p, rect());
            break;
        case EffectTypeBlink:
            Paint2D_Blink(p, rect());
            break;
        case EffectTypeRotate:
            Paint2D_Rotate(p, rect());
            break;
        case EffectTypeTwirl:
            Paint2D_Twirl(p, rect());
            break;
        case EffectTypeZoom:
            Paint2D_Zoom(p, rect());
            break;
        case EffectTypeTransparent:
            Paint2D_Transparent(p, rect());
            break;
        case EffectTypeSpread:
            Paint2D_Spread(p, rect());
            break;
        case EffectTypeWheel:
            Paint2D_Wheel(p, rect());
            break;
        case EffectTypeSlipCycle:
            Paint2D_Slipcycle(p, rect());
            break;
#ifdef  SURRPORT_3D
        case EffectTypeCube:
            Paint3D_Cube(p, rect());
            break;
        case EffectTypeCylinder:
            Paint3D_Cylinder(p, rect());
            break;
#endif
        default:
            break;
    }
    return true;
}

void GraphicsComponent::Play()
{
    updateEffect(OPERATE_PLAY, QVariant());
}

void GraphicsComponent::Stop()
{
    updateEffect(OPERATE_STOP, QVariant());
}

bool GraphicsComponent::SetEnvironment(QString pEnvirString)
{
    return true;
}

bool GraphicsComponent::GetEnvironment(QString &pEnvirString)
{
    pEnvirString = QString("Test");
    return true;
}

//>@路径 《属性列表》 值
bool GraphicsComponent::SetPara(QDomElement &pElement, xmlParse *pParse)
{
    if(pElement.isNull() || pParse==0)
        return false;
    QDomNodeList tmpList = pElement.childNodes();
    for(int i=0;i<tmpList.count();i++)
    {
        QDomElement tmpElement = tmpList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(tmpName.compare("Pos", Qt::CaseInsensitive) == 0)
        {
            //>@如果是在其父窗口中移动，则执行移动操作
            QStringList tmpGeoList = tmpElement.firstChildElement("string").text().split(',');
            if(tmpGeoList.count() != 4)
                continue;
            GraphicsComponent *tmpParBase = VAR_CAST<GraphicsComponent*>(parent());
            if(tmpParBase == 0)
                continue;
            QRectF tmpRect = QRectF(tmpGeoList.at(0).toInt(), tmpGeoList.at(1).toInt(), tmpGeoList.at(2).toInt(), tmpGeoList.at(3).toInt());
            if(tmpParBase->geometry().contains(tmpRect))
            {
                SetComGeometory(tmpRect);
            }
        }
    }
    return true;
}

bool GraphicsComponent::GetPara(QDomElement &pElement, xmlParse *pParse)
{
    return true;
}

void GraphicsComponent::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    if(m_UpdateOPerate != pOperate &&
       pOperate != OPERATE_PLAY && pOperate != OPERATE_STOP)
    {
        m_UpdateOPerate = pOperate;
        m_UpdatePara = pPara;
    }
    if(m_UpdateOPerate == OPERATE_PLAY)
    {
        updateEffect(m_UpdateOPerate, m_UpdatePara);
    }
    else if(m_UpdatePara == OPERATE_STOP)
    {
        m_OperateInfo[STEP0].mValid = false;
        m_OperateInfo[STEP1].mValid = false;
        m_Animator.stop();
        m_AreaAnimate = 0;
        resetTransform();
        update();
    }
    else if(m_UpdatePara == OPERATE_SHOW)
    {
        if(!isVisible())
            show();
    }
    else if(m_UpdatePara == OPERATE_HIDE)
    {
        if(isVisible())
            hide();
    }
}

void GraphicsComponent::OperateStep0()
{
    if(m_EffectValid == false)
        return;
    UpdateCom(0);
    //>@执行STEP0中的内容，如果执行不成功，尝试执行STEP1中的内容
    if(m_OperateInfo[STEP0].mValid)
    {
        m_AreaAnimate = m_EffectGroup.value(m_OperateInfo[STEP0].mOperate);
        if(m_AreaAnimate)
        {
            connect(&m_Animator, SIGNAL(finished()), SLOT(OperateStep1()));
            StartTimeline();
            return;
        }
    }
    OperateStep1();
}

void GraphicsComponent::OperateStep1()
{
    if(m_EffectValid == false)
        return;
    //>@准备资源，两个必定为同一张图片
    m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer;
    UpdateCom(0);
    if(m_OperateInfo[STEP1].mValid)
    {
        m_AreaAnimate = m_EffectGroup.value(m_OperateInfo[STEP1].mOperate);
        if(m_AreaAnimate)
        {
            disconnect(&m_Animator, SIGNAL(finished()), 0, 0);
            StartTimeline();
            return;
        }
    }
}

void GraphicsComponent::PressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event && event->button() == Qt::LeftButton)
    {
        if(m_EffectValid)
        {
            updateEffect(OPERATE_KEYPRESS, QVariant(mapFromScene(event->scenePos())));
        }
    }
    //QGraphicsWidget::mousePressEvent(event);
}

void GraphicsComponent::MoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event && event->button() == Qt::LeftButton)
    {
        if(m_EffectValid)
        {
            updateEffect(OPERATE_KEYMOVE, QVariant(mapFromScene(event->scenePos())));
        }
    }
    //QGraphicsWidget::mouseMoveEvent(event);
}

void GraphicsComponent::ReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event && event->button() == Qt::LeftButton)
    {
        if(m_EffectValid)
        {
            updateEffect(OPERATE_KEYRELEASE, QVariant(mapFromScene(event->scenePos())));
        }
        emit MouseRelease(event->scenePos());
    }
    //QGraphicsWidget::mouseReleaseEvent(event);
}

void GraphicsComponent::keyPressEvent(QKeyEvent *event)
{
    if(m_EffectValid == false)
        return;
    switch (event->key())
    {
#ifdef  SURRPORT_3D
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
#endif
        default:
            return;
    }
    QGraphicsItem::keyPressEvent(event);
}

#ifdef  SURRPORT_3D
void GraphicsComponent::setGeometry3D(qreal x, qreal y, qreal width, qreal height)
{
    setPos(x,y);
    m_3DWidth = width;
    m_3DHeight = height;
}

void GraphicsComponent::setGeometry3D(QRectF rect)
{
    setPos(rect.left(),rect.top());
    m_3DWidth = rect.width();
    m_3DHeight = rect.height();
}

void GraphicsComponent::BindTexture(GLuint L_Texture,GLfloat R,GLfloat G,GLfloat B)
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

void GraphicsComponent::UnbindTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void GraphicsComponent::OpenGL_PaintBegin(QPainter *painter)
{
    //>@创建GLWidget资源
    if(m_GLWidget == 0)
        m_GLWidget = new GraphicsGLWidget;

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

void GraphicsComponent::OpenGL_PaintEnd(QPainter *painter)
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

void GraphicsComponent::OpenGL_Effect(QPainter *painter)
{
    //>@旋转
    //glTranslatef(0.0f,0.5f,0.0f);
    glRotatef(-m_3DXDiff,1.0f,0.0f,0.0f);
    glRotatef(-m_3DYDiff,0.0f,1.0f,0.0f);
    glRotatef(-m_3DZDiff,0.0f,0.0f,1.0f);
    //>@缩放特效
    glScalef(m_3DXScale, m_3DYScale, m_3DZScale);
}

void GraphicsComponent::Set3DEffect(EffectType pType)
{

}
#endif
