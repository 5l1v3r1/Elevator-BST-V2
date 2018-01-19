#include "effectbase.h"

bool EffectRC::InitEffect(QHash<int, QPixmap> pImageList)
{
    m_EffectValid = false;
    m_EffectImage = pImageList;
    Start();
    return true;
}

bool EffectRC::InitEffect(QHash<int, QString> pPathList)
{
    m_EffectValid = false;
    m_EffectPath = pPathList;
    Start();
    return true;
}

bool EffectRC::InitEffect(COM_INFO *pComInfo)
{
    m_EffectValid = false;
    if(pComInfo == 0)
        return false;
    STATE_INFO* tmp;
    for(int i=0;i<pComInfo->StateMachine.count();i++)
    {
        tmp = pComInfo->StateMachine.at(i);
        if(tmp == 0)
            continue;
        InitEffectRc(tmp);
    }
    Start();
    return true;
}

bool EffectRC::InitEffect(COM_STRUCT *pComInfo)
{
    m_EffectValid = false;
    if(pComInfo == 0)
        return false;
    STATE_INFO* tmp;
    for(int i=0;i<pComInfo->StateMachine.count();i++)
    {
        tmp = pComInfo->StateMachine.at(i);
        if(tmp == 0)
            continue;
        InitEffectRc(tmp);
    }
    Start();
    return true;
}

bool EffectRC::InitEffectRc(STATE_INFO* pEffect)
{
    return true;
}

bool EffectRC::Start()
{
    int tmpImageCount = m_EffectImage.count();
    int tmpPathCount = m_EffectPath.count();
    if(tmpImageCount > 0 || tmpPathCount > 0)
    {
        m_EffectValid = true;
        return true;
    }
    m_EffectValid = false;
    //IDE_TRACE();
    return false;
}

bool EffectRC::analysisEffectPara(AREA_ANIMATE* pAreaEffect, QString pEffectPara)
{
    if(pEffectPara.isEmpty())
        return false;
    QStringList tmpList = pEffectPara.split(";", QString::SkipEmptyParts);
    for(int i=0;i<tmpList.count();i++)
    {
        QStringList tmpSubList = tmpList.at(i).split(":", QString::SkipEmptyParts);
        if(tmpSubList.count()<2)
        {
            //IDE_TRACE();
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

bool EffectRC::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
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

EffectBase::EffectBase(QWidget *parent, QRect pRect) :
    QWidget(parent, Qt::FramelessWindowHint),
    EffectRC()
{
    setGeometry(pRect);

    setAttribute(Qt::WA_DeleteOnClose);
    //setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    connect(&m_Animator, SIGNAL(finished()), SLOT(OperateStep1()));
    connect(&m_Animator, SIGNAL(frameChanged(int)), SLOT(update()));

    m_RcType = RCIMAGE;
    m_EffectImage.clear();
    m_EffectValid = false;

    m_AreaAnimate = 0;
}

EffectBase::~EffectBase()
{
    m_Animator.stop();
    m_EffectImage.clear();
    m_EffectPath.clear();
    m_EffectGroup.clear();
}

bool EffectBase::InitEffectRc(STATE_INFO* pEffect)
{
    if(pEffect == 0)
        return false;
    return true;
}

bool EffectBase::Start()
{
    if(EffectRC::Start() == false)
    {
        //IDE_TRACE();
        return false;
    }
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP0].mOperate = OPERATE_NONE;
    m_OperateInfo[STEP1].mValid = false;
    m_OperateInfo[STEP1].mOperate = OPERATE_NONE;
    return true;
}

bool EffectBase::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
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

void EffectBase::Play()
{
    updateEffect(OPERATE_PLAY, QVariant());
}

void EffectBase::Stop()
{
    updateEffect(OPERATE_STOP, QVariant());
    update();
}

bool EffectBase::SetEnvironment(QString pEnvirString)
{
    return true;
}

bool EffectBase::GetEnvironment(QString &pEnvirString)
{
    pEnvirString = QString("Test");
    return true;
}

void EffectBase::OperateStep0()
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

void EffectBase::OperateStep1()
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

void EffectBase::updateEffect(AREA_OPERATE pOperate, quint32 pPara)
{
    updateEffect(pOperate, QVariant(pPara));
}

void EffectBase::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    if(m_EffectValid == false)
        return;
}

void EffectBase::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.setRenderHint(QPainter::Antialiasing);
    PaintEffect(p);
}

void EffectBase::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(m_EffectValid)
        {
            updateEffect(OPERATE_KEYPRESS, QVariant(event->pos()));
        }
        event->accept();
    }
}

void EffectBase::mouseMoveEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(m_EffectValid)
        {
            updateEffect(OPERATE_KEYMOVE, QVariant(event->pos()));
        }
        event->accept();
    }
}

void EffectBase::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(m_EffectValid)
        {
            updateEffect(OPERATE_KEYRELEASE, QVariant(event->pos()));
        }
        event->accept();
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifdef OPENGL
GLtexture::GLtexture(QWidget *parent)
    : QGLWidget(parent)
{}

GLtexture::~GLtexture()
{}

bool GLtexture::loadTexture(EffectBaseItem *pEffectRc)
{
    QString tmpKeyString;
    QList<int> tmpKeyList = pEffectRc->m_EffectImage.keys();
    int count = tmpKeyList.count();
    int tmpKey;
    for(int i=0;i<count;i++)
    {
        tmpKey = tmpKeyList.at(i);
        QPixmap tmpPixmap = pEffectRc->m_EffectImage.value(tmpKey);
        if(tmpPixmap.isNull())
            continue;
        GLuint tmpGLuint = bindTexture(tmpPixmap);
        if(tmpKey == 0)
        {
            tmpKeyString = QString("CubeLeft");
        }
        else
        {
            continue;
        }
        pEffectRc->m_EffectTexture.insert(tmpKeyString, tmpGLuint);
    }
    return true;
}

#endif

EffectBaseItem::EffectBaseItem(QGraphicsItem *parent, QRectF pRect):
    QGraphicsWidget(parent, Qt::FramelessWindowHint),
    EffectRC()
{
    m_Rect = pRect;
    setGeometry(m_Rect);
    m_RcType = RCIMAGE;
    m_EffectImage.clear();
    m_EffectValid = false;
    m_AreaAnimate = 0;

#ifdef OPENGL
    connect(&m_Animator, SIGNAL(frameChanged(int)), SLOT(SLOT_Animator(int)));
    m_EffectTexture.clear();
    m_CurFrame = 0;
    m_XDiff = m_YDiff = m_ZDiff = 0;
    m_XScale = m_YScale = m_ZScale = 0;
    m_DeepDiff = 0;
#endif
}

QRectF EffectBaseItem::boundingRect() const
{
    QRectF tmpRect = this->geometry().normalized();
    return QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized();
}

QPainterPath EffectBaseItem::shape() const
{
    QPainterPath path;
    QRectF tmpRect = this->geometry().normalized();
    path.addRect(QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized());
    return path;
}

bool EffectBaseItem::InitEffectRc(STATE_INFO* pEffect)
{
    if(pEffect == 0)
        return false;
    return true;
}

bool EffectBaseItem::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
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

bool EffectBaseItem::Start()
{
    if(EffectRC::Start() == false)
        return false;
    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP0].mOperate = OPERATE_NONE;
    m_OperateInfo[STEP1].mValid = false;
    m_OperateInfo[STEP1].mOperate = OPERATE_NONE;
    return true;
}

void EffectBaseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(m_EffectValid == false)
        return;
    PaintEffect(painter);
}

void EffectBaseItem::PaintEffect(QPainter *p)
{
#ifdef OPENGL
    OpenGL_PaintBegin(p);
    OpenGL_Effect(p);

    switch(m_EffectType)
    {
        case EffectTypeCube:
            break;
        case EffectTypeCylinder:
            break;
        default:
            break;
    }

    OpenGL_PaintEnd(p);
#endif
}

void EffectBaseItem::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    if(m_EffectValid == false)
        return;
}

void EffectBaseItem::Play()
{

}

void EffectBaseItem::Stop()
{

}

void EffectBaseItem::OperateStep0()
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

void EffectBaseItem::OperateStep1()
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

void EffectBaseItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(m_EffectValid)
        {
            updateEffect(OPERATE_KEYPRESS, QVariant(event->pos()));
        }
        event->accept();
    }
}

void EffectBaseItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(m_EffectValid)
        {
            updateEffect(OPERATE_KEYMOVE, QVariant(event->pos()));
        }
        event->accept();
    }
}

void EffectBaseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(m_EffectValid)
        {
            updateEffect(OPERATE_KEYRELEASE, QVariant(event->pos()));
        }
        event->accept();
    }
}

#ifdef OPENGL


void EffectBaseItem::setGeometry3D(qreal x, qreal y, qreal width, qreal height)
{
    setPos(x,y);
    m_Width = width;
    m_Height = height;
}

void EffectBaseItem::setGeometry3D(QRectF rect)
{
    setPos(rect.left(),rect.top());
    m_Width = rect.width();
    m_Height = rect.height();
}

void EffectBaseItem::BindTexture(GLuint L_Texture,GLfloat R,GLfloat G,GLfloat B)
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

void EffectBaseItem::UnbindTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void EffectBaseItem::OpenGL_PaintBegin(QPainter *painter)
{
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

void EffectBaseItem::OpenGL_PaintEnd(QPainter *painter)
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

void EffectBaseItem::OpenGL_Effect(QPainter *painter)
{
    //>@旋转
    //glTranslatef(0.0f,0.5f,0.0f);
    glRotatef(-m_XDiff,1.0f,0.0f,0.0f);
    glRotatef(-m_YDiff,0.0f,1.0f,0.0f);
    glRotatef(-m_ZDiff,0.0f,0.0f,1.0f);
    //>@缩放特效
    glScalef(m_XScale, m_YScale, m_ZScale);
}

void EffectBaseItem::Paint3D_Cube()
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
    BindTexture(m_EffectTexture.value("CubeFront"),1.0f,1.0f,1.0f);
    glTexCoordPointer(2,GL_FLOAT,0,afTexCoordFront);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //>@Top
    glPopMatrix();
    BindTexture(m_EffectTexture.value("CubeTop"),1.0f,1.0f,1.0f);
    glRotatef(90,1.0f,0.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //>@Back
    glPopMatrix();
    BindTexture(m_EffectTexture.value("CubeBack"),1.0f,1.0f,1.0f);
    glRotatef(180,1.0f,0.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //>@Bottom
    glPopMatrix();
    BindTexture(m_EffectTexture.value("CubeBottom"),1.0f,1.0f,1.0f);
    glRotatef(270,1.0f,0.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //>@Right
    glPopMatrix();
    BindTexture(m_EffectTexture.value("CubeRight"),1.0f,1.0f,1.0f);
    glRotatef(90,0.0f,1.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //>@Left
    glPopMatrix();
    BindTexture(m_EffectTexture.value("CubeLeft"),1.0f,1.0f,1.0f);
    glRotatef(270,0.0f,1.0f,0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    UnbindTexture();
    glPopMatrix();
    glPopMatrix();
}

void EffectBaseItem::Paint3D_CircleSurface(GLuint texture)
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
    UnbindTexture();
    glPopMatrix();
}

void EffectBaseItem::Paint3D_CamberSurface(GLuint texture)
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
            BindTexture(texture,1.0f,1.0f,1.0f);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(3,GL_FLOAT,0,verTexture);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glRotatef(fragAngle, 0, 1, 0);
        }
    }
    UnbindTexture();
    glPopMatrix();
}

void EffectBaseItem::Paint3D_Cylinder()
{
    glPushMatrix();
    glScalef(0.5, 0.2, 0.5);
    glPushMatrix();
    Paint3D_CamberSurface(m_EffectTexture.value("SideSurface"));
    glTranslatef(0.0f,0.5f,0.0f);
    glRotatef(90,1.0f,0.0f,0.0f);
    glRotatef(180,0.0f,0.0f,1.0f);
    glScalef(1.94, 1.94, 1);
    Paint3D_CircleSurface(m_EffectTexture.value("TopCircleSurface"));
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f,-0.5f,0.0f);
    glRotatef(-90,1.0f,0.0f,0.0f);
    glScalef(1.94, 1.94, 1);
    Paint3D_CircleSurface(m_EffectTexture.value("BottomCircleSurface"));
    glPopMatrix();
    glPopMatrix();
}

void EffectBaseItem::StartRotateEffect(ROTATE_STAGE pStage, QTimeLine *TimeLine, int angle)
{
    switch(pStage)
    {
        case STAGE_90:
            StartTimeline(TimeLine,
                         0,angle,
                         m_AnimateTime,QTimeLine::EaseInOutCurve);
            break;
        case STAGE_180:
            StartTimeline(TimeLine,
                          angle,angle*2,
                          m_AnimateTime,QTimeLine::EaseInOutCurve);
            break;
        case STAGE_270:
            StartTimeline(TimeLine,
                          angle*2,angle*3,
                          m_AnimateTime,QTimeLine::EaseInOutCurve);
            break;
        case STAGE_360:
            StartTimeline(TimeLine,
                          angle*3,angle*4,
                          m_AnimateTime,QTimeLine::EaseInOutCurve);
            break;
        default:
            break;
    }
}

void EffectBaseItem::StartZoomEffect(ZOOM_STAGE pStage, QTimeLine *TimeLine, int deep)
{
    switch(pStage)
    {
        case STAGE_IN:
            StartTimeline(TimeLine,
                          100,deep,
                          m_AnimateTime,QTimeLine::EaseInOutCurve);
            break;
        case STAGE_OUT:
            StartTimeline(TimeLine,
                          deep,100,
                          m_AnimateTime,QTimeLine::EaseInOutCurve);
            break;
        default:
            break;
    }
}

void EffectBaseItem::StartIndentEffect(ZOOM_STAGE pStage, QTimeLine *TimeLine, int deep)
{
    switch(pStage)
    {
        case STAGE_IN:
            StartTimeline(TimeLine,
                          100,deep,
                          m_AnimateTime,QTimeLine::EaseInOutCurve);
            break;
        case STAGE_OUT:
            StartTimeline(TimeLine,
                          deep,100,
                          m_AnimateTime,QTimeLine::EaseInOutCurve);
            break;
        default:
            break;
    }
}

void EffectBaseItem::StartShakeEffect(QTimeLine *TimeLine,int angle)
{
    StartTimeline(TimeLine,
                  -angle,angle,
                  m_AnimateTime,QTimeLine::CosineCurve);
}

void EffectBaseItem::keyPressEvent(QKeyEvent *event)
{
    if(m_EffectValid == false)
    {
        return;
    }
    switch (event->key())
    {
        case Qt::Key_Up:
            m_YDiff -= 2;
            break;
        case Qt::Key_Down:
            m_YDiff += 2;
            break;
        case Qt::Key_Left:
            m_XDiff += 2;
            break;
        case Qt::Key_Right:
            m_XDiff -= 2;
            break;
        case Qt::Key_Home:
            m_DeepDiff -= 0.1;
            break;
        case Qt::Key_End:
            m_DeepDiff += 0.1;
            break;
        default:
            return;
    }
    QGraphicsItem::keyPressEvent(event);
}
#endif

KeyButton::KeyButton(QWidget *parent) :
    QPushButton(parent)
{
    m_FontSize = 10;
}

void KeyButton::InitRc(QPixmap pNormal, QPixmap pHighLight)
{
    if(pNormal.isNull() == false)
        m_NormalBg = pNormal;
    if(pNormal.isNull() == false)
        m_HightlightBg = pHighLight;
    if(m_NormalBg.isNull() && m_HightlightBg.isNull())
        return;
    m_ButtonBg = m_NormalBg;
}

//>@计算文字坐标和大小
void KeyButton::InitText(QString pText)
{
    setText(pText);
    m_Text = pText;
    int count = m_Text.count();
    if(count > 0)
    {
        //int w = width()/count; int h = height(); ::qMin(CalcFontSizeFromSize("Arial", QSize(w, h), PRIOR_HEIGHT), CalcFontSizeFromSize("Arial", QSize(w, h), PRIOR_WIDTH)) ;
        QSize tmpSize = CalcSizeFromFontSize("Arial", m_Text, m_FontSize);
        int x = (width() - tmpSize.width()) / 2;
        int y = (height()- tmpSize.height()) / 2;
        m_TextRect = QRect(x, y, width()-x, tmpSize.height());
    }
}

void KeyButton::paintEvent(QPaintEvent *p)
{
    if(m_ButtonBg.isNull() == false)
    {
        QPainter p(this);
        p.drawPixmap(rect(), m_ButtonBg);
        if(m_Text.isEmpty() == false)
        {
            QFont font("Arial", m_FontSize, QFont::Bold, true);
            p.setFont(font);
            p.setPen(QPen(Qt::black));
            p.drawText(m_TextRect, m_Text);
        }
    }
    else
    {
        QPushButton::paintEvent(p);
    }
}

void KeyButton::mousePressEvent(QMouseEvent *e)
{
    m_ButtonBg = m_HightlightBg;
    update();
    QPushButton::mousePressEvent(e);
}

void KeyButton::mouseReleaseEvent(QMouseEvent *e)
{
    m_ButtonBg = m_NormalBg;
    update();
    QPushButton::mouseReleaseEvent(e);
}

