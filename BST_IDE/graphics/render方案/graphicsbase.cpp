#include "graphicsbase.h"
#ifdef IDE
    #include "diagramscene.h"
#elif
    #include "logicthememanager.h"
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

GraphicsBase::GraphicsBase(QGraphicsItem *parent, QRectF pRect):
    QGraphicsWidget(parent, Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_DeleteOnClose);
    //setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    ReleaseRC();

#ifdef  SURRPORT_3D
    m_GLWidget = 0;
    m_3DXDiff = m_3DYDiff = m_3DZDiff = 0; //>@三个方向上的角度偏移
    m_3DXScale = m_3DYScale = m_3DZScale = 0; //>@三个方向上的缩放
    m_3DDeepDiff = 0;
#endif
    m_PaintBusy = false;

    m_AreaAnimate = 0;
    m_RcType = RCIMAGE;

    m_UpdateOPerate = OPERATE_NONE;
    m_UpdatePara = QVariant();

    m_ActionGroup = 0;
    m_ActionGroup = new QActionGroup(this);
    connect(m_ActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(ExecAction(QAction *)));

    connect(&m_Animator, SIGNAL(frameChanged(int)), this, SLOT(UpdateCom(int)));
}

GraphicsBase::~GraphicsBase()
{
    ReleaseRC();
    m_Animator.stop();
}

void GraphicsBase::ReleaseRC()
{
    m_EffectValid = false;
    m_EffectImage.clear();
    m_EffectGroup.clear();
#ifdef  SURRPORT_3D
    m_EffectTexture.clear();
#endif
#ifdef  SURRPORT_SVG
    m_EffectSvgRender.clear();
#endif
}

bool GraphicsBase::InitComponent(QHash<int, QPixmap> pPixmapList, QHash<AREA_OPERATE, AREA_ANIMATE*>  pEffectGroup)
{
    ReleaseRC();
    m_EffectImage = pPixmapList;
    m_EffectGroup = pEffectGroup;
    Start();
    return true;
}

bool GraphicsBase::InitComponent(QDomElement pComElement)
{
    if(pComElement.isNull())
        return false;
    ReleaseRC();
    m_ComElement = pComElement;
    //>@设置坐标
    QRect tmpRect = m_UiScene->m_UiManager->GetComRect(m_ComElement, m_UiScene->m_UiDirection);
    if(!tmpRect.isNull())
        setGeometry(QRectF(tmpRect));
    //>@设置状态机
    m_UiScene->m_UiManager->ClearItemElement()
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

bool GraphicsBase::InitEffect(COM_STRUCT *pComInfo)
{
    Init();
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
    //>@判断资源
    return Start();
}


 QPixmap GraphicsBase::ScalePixmap(QPixmap pPixmap, QSizeF pSize)
{
    QSize tmpSize = pSize.toSize();
    if(!(tmpSize.isEmpty()) && pPixmap.size() != tmpSize)
        return pPixmap.scaled(tmpSize);
    return pPixmap;
}

#ifdef  SURRPORT_SVG
 QSvgRenderer *GraphicsBase::GetSvgRender(int pKey)
{
    QSvgRenderer *tmpSvgRenderer = 0;
    //>@查找m_EffectSvgRender
    tmpSvgRenderer = m_EffectSvgRender.value(pKey);
    if(tmpSvgRenderer)
    {
        return tmpSvgRenderer;
    }
    //>@其次查找m_EffectPath，并将其插入m_EffectImage
    QString tmpPath = m_EffectPath.value(pKey);
    if(tmpPath.isEmpty() == false)
    {
        RC_TYPE tmpType = getRcType(tmpPath);
        if(tmpType == RC_SVG)
        {
            tmpSvgRenderer = new QSvgRenderer(tmpPath);
            if(tmpSvgRenderer->isValid() == false)
            {
                tmpSvgRenderer->deleteLater();
                tmpSvgRenderer = 0;
                return tmpSvgRenderer;
            }
            m_EffectSvgRender.insert(pKey, tmpSvgRenderer);
            return tmpSvgRenderer;
        }
    }
    return tmpSvgRenderer;
}
#endif

 QPixmap GraphicsBase::GetPixmap(int pKey, QSizeF pSize)
{
    QPixmap tmpPixmap;
    //>@首先查找m_EffectImage
    tmpPixmap = m_EffectImage.value(pKey);
    if(tmpPixmap.isNull() == false)
    {
        return ScalePixmap(tmpPixmap, pSize);
    }
    //>@其次查找m_EffectPath，并将其插入m_EffectImage
    tmpPixmap = LoadPixmap(m_EffectPath.value(pKey), pSize);
    if(tmpPixmap.isNull() == false)
    {
        m_EffectImage.insert(pKey, tmpPixmap);
        return tmpPixmap;
    }
    return tmpPixmap;
}

 QPixmap GraphicsBase::LoadPixmap(QString pPixmapName, QSizeF pSize)
{
    if(pPixmapName.isEmpty())
        return QPixmap();
    FILEPATH_TYPE tmpType = RegulatRcFile(pPixmapName);
    if(tmpType == PATH_TYPENONE || tmpType == PATH_TYPENETADDR)
        return QPixmap();
    RC_TYPE tmpRcType = getRcType(pPixmapName);
    switch(tmpRcType)
    {
        case RC_BMP:
        case RC_JPG:
        case RC_PNG:
        {
            QPixmap tmpPixmap;
#ifdef RGB565  //>@主要是对图像进行像素调整
            QImage tmpImage;
            if(tmpImage.load(pPixmapName) == false)
                return QPixmap();
            if(ConvertImageTo565(tmpImage) == false)
                return QPixmap();
            tmpPixmap = QPixmap::fromImage(tmpImage);
#else
            if(tmpPixmap.load(pPixmapName) == false)
                return QPixmap();
#endif
            return ScalePixmap(tmpPixmap, pSize);
        }
        case RC_GIF:
        {
            break;
        }
        case RC_SVG:
        {
#ifdef  SURRPORT_SVG
            QSvgRenderer *tmpRenderer = new QSvgRenderer(pPixmapName);
            if(tmpRenderer->isValid() == false)
            {
                tmpRenderer->deleteLater();
                return QPixmap();
            }
            else
            {
                if(pSize.isEmpty())
                    return QPixmap();
                QImage tmpImage(pSize.width(), pSize.height(), QImage::Format_ARGB32_Premultiplied);
                if(tmpImage.isNull())
                    return QPixmap();
                tmpImage.fill(Qt::transparent); //>@创建透明图层
                QPainter p(&tmpImage);
                tmpRenderer->render(&p);
                p.end();
#ifdef RGB565  //>@主要是对图像进行像素调整
                if(ConvertImageTo565(tmpImage) == false)
                    return QPixmap();
#endif
                return QPixmap::fromImage(tmpImage);
            }
#endif
            break;
        }
        case RC_TXT:
        {
            QPixmap tmpPixmap;
            fontManager *tmpFontManager = new fontManager(pPixmapName);
            if(tmpFontManager->m_Valid)
            {
                if(pSize.isEmpty())
                    return QPixmap();
                QImage tmpImage(pSize.width(), pSize.height(), QImage::Format_ARGB32_Premultiplied);
                if(tmpImage.isNull())
                    return QPixmap();
                tmpImage.fill(Qt::transparent); //>@创建透明图层
                QPainter p(&tmpImage);
                //>@获取内容
                QStringList tmpDescList = tmpFontManager->m_ParagraphGroup.keys();
                int count = tmpDescList.count();
                int diff = (qreal)(pSize.height()) / (qreal)count;
                for(int i=0;i<count;i++)
                {
                    QString tmpString;
                    FONT_INFO tmpFontInfo;
                    if(tmpFontManager->GetParagraph(tmpDescList.at(i), tmpString, tmpFontInfo) == false)
                        continue;
                    WriteText(&p, tmpString, QRect(0, diff*i, pSize.width(), diff), tmpFontInfo.mFont.family(), tmpFontInfo.mColor, PRIOR_HEIGHT);
                }
                p.end();
#ifdef RGB565  //>@主要是对图像进行像素调整
                if(ConvertImageTo565(tmpImage) == false)
                    return QPixmap();
#endif
                tmpPixmap = QPixmap::fromImage(tmpImage);
            }
            tmpFontManager->CloseFont();
            delete tmpFontManager;
            return tmpPixmap;
        }
        default:break;
    }
    return QPixmap();
}

bool GraphicsBase::InitEffectRc(STATE_INFO* pEffect)
{
    return true;
}

bool GraphicsBase::Start()
{
    int tmpCount = m_EffectPath.count()
                   +m_EffectImage.count()
#ifdef  SURRPORT_3D
                   +m_EffectTexture.count()
#endif
#ifdef  SURRPORT_SVG
                   +m_EffectSvgRender.count()
#endif
            ;
    if(tmpCount > 0)
    {
        m_EffectValid = true;
        return true;
    }
    m_EffectValid = false;
    return false;
}

void GraphicsBase::UpdateCom(int pFrame)
{
    update(rect());
}

QRectF GraphicsBase::boundingRect() const
{
    QRectF tmpRect = this->geometry().normalized();
    return QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized();
}

QPainterPath GraphicsBase::shape() const
{
    QPainterPath path;
    QRectF tmpRect = this->geometry().normalized();
    path.addRect(QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized());
    return path;
}

bool GraphicsBase::InitEffectRc(STATE_INFO* pEffect)
{
    if(pEffect == 0)
        return false;
    return true;
}

bool GraphicsBase::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return GraphicsBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool GraphicsBase::Start()
{
    if(m_ComPath.isEmpty() == false)
    {
        setData(COMNAME, QVariant(m_ComPath));
        setData(COMTYPE, QVariant(getCurComType(m_ComPath)));
    }

    if(GraphicsRC::Start() == false)
    {
        return false;
    }

    m_RcList = m_EffectPath.keys();
    if(m_RcList.count() == 0)
    {
        return false;
    }

    m_OperateInfo[STEP0].mValid = false;
    m_OperateInfo[STEP0].mOperate = OPERATE_NONE;
    m_OperateInfo[STEP1].mValid = false;
    m_OperateInfo[STEP1].mOperate = OPERATE_NONE;
    return true;
}

int GraphicsBase::getNextRc(int pCurIndex)
{
    for(int i=0;i<m_RcList.count();i++)
    {
        if(i==pCurIndex)
        {
            if((i+1) < m_RcList.count())
                return i+1;
            else
                return i;
        }
    }
    if(m_RcList.count() > 0)
        return m_RcList.at(m_RcList.count()-1);
    return -1;
}

int GraphicsBase::getPrevRc(int pCurIndex)
{
    for(int i=0;i<m_RcList.count();i++)
    {
        if(i==pCurIndex)
        {
            if((i-1) >= 0)
                return i-1;
            else
                return i;
        }
    }
    if(m_RcList.count() > 0)
        return m_RcList.at(0);
    return -1;
}

void GraphicsBase::StopAnimate()
{
    if(m_Animator.state() == QTimeLine::Running)
        m_Animator.stop();
    m_AreaAnimate = 0;
    UpdateCom(0);
}

void GraphicsBase::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!m_PaintBusy)
        m_PaintBusy = true;
    else
        return;
    //painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    //painter->setRenderHint(QPainter::Antialiasing);
    PaintEffect(painter);
    m_PaintBusy = false;
}


int GraphicsBase::GetKey(QString pString)
{
    if(pString.isEmpty())
        return -1;
    if(pString.startsWith("RC",Qt::CaseInsensitive) == false)
        return -1;
    pString.remove(0, 2);
    bool ok = false;
    int index = pString.toInt(&ok, 10);
    if(ok == false)
        return -1;
    return index;
}


bool GraphicsBase::LoadPath(int pKey, QString pPixmapName)
{
    if(pPixmapName.isEmpty())
        return false;
    if(RegulatRcFile(pPixmapName) == PATH_TYPENONE)
    {
        return false;
    }
    m_EffectPath.insert(pKey, pPixmapName);
    return true;
}

bool GraphicsBase::analysisEffectPara(AREA_ANIMATE* pAreaEffect, QString pEffectPara)
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



void GraphicsBase::Play()
{
    updateEffect(OPERATE_PLAY, QVariant());
}

void GraphicsBase::Stop()
{
    updateEffect(OPERATE_STOP, QVariant());
}

bool GraphicsBase::SetEnvironment(QString pEnvirString)
{
    return true;
}

bool GraphicsBase::GetEnvironment(QString &pEnvirString)
{
    pEnvirString = QString("Test");
    return true;
}

//>@路径 《属性列表》 值
bool GraphicsBase::SetPara(QDomElement &pElement, xmlParse *pParse)
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
            GraphicsBase *tmpParBase = VAR_CAST<GraphicsBase*>(parent());
            if(tmpParBase == 0)
                continue;
            QRectF tmpRect = QRectF(tmpGeoList.at(0).toInt(), tmpGeoList.at(1).toInt(), tmpGeoList.at(2).toInt(), tmpGeoList.at(3).toInt());
            if(tmpParBase->geometry().contains(tmpRect))
            {
                setGeometry(tmpRect);
            }
        }
    }
    return true;
}

bool GraphicsBase::GetPara(QDomElement &pElement, xmlParse *pParse)
{
    return true;
}

void GraphicsBase::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
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

void GraphicsBase::OperateStep0()
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

void GraphicsBase::OperateStep1()
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

void GraphicsBase::PressEvent(QGraphicsSceneMouseEvent *event)
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

void GraphicsBase::MoveEvent(QGraphicsSceneMouseEvent *event)
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

void GraphicsBase::ReleaseEvent(QGraphicsSceneMouseEvent *event)
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

void GraphicsBase::keyPressEvent(QKeyEvent *event)
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
void GraphicsBase::setGeometry3D(qreal x, qreal y, qreal width, qreal height)
{
    setPos(x,y);
    m_3DWidth = width;
    m_3DHeight = height;
}

void GraphicsBase::setGeometry3D(QRectF rect)
{
    setPos(rect.left(),rect.top());
    m_3DWidth = rect.width();
    m_3DHeight = rect.height();
}

void GraphicsBase::BindTexture(GLuint L_Texture,GLfloat R,GLfloat G,GLfloat B)
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

void GraphicsBase::UnbindTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void GraphicsBase::OpenGL_PaintBegin(QPainter *painter)
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

void GraphicsBase::OpenGL_PaintEnd(QPainter *painter)
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

void GraphicsBase::OpenGL_Effect(QPainter *painter)
{
    //>@旋转
    //glTranslatef(0.0f,0.5f,0.0f);
    glRotatef(-m_3DXDiff,1.0f,0.0f,0.0f);
    glRotatef(-m_3DYDiff,0.0f,1.0f,0.0f);
    glRotatef(-m_3DZDiff,0.0f,0.0f,1.0f);
    //>@缩放特效
    glScalef(m_3DXScale, m_3DYScale, m_3DZScale);
}

void GraphicsBase::Set3DEffect(EffectType pType)
{

}
#endif
