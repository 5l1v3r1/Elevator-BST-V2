
#include "diagramscene.h"

DiagramFrame::DiagramFrame(QGraphicsItem *parent)
    : QGraphicsWidget(parent, Qt::FramelessWindowHint)
{
    setCursor(Qt::ArrowCursor);
    setFlags(ItemIsMovable | ItemIsSelectable);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    setColor(Qt::red);
    QApplication::setStartDragDistance(5);
    m_SelectFlag = false;
    m_SavedComOperate = DRAGNONE;
    m_ItemGroup = 0;
    m_ItemGroup = new QGraphicsItemGroup;
    m_ChildParentDiff = QPointF(0,0);
    m_Mutex = 0;
    m_Mutex = new QMutex(QMutex::Recursive);
}

QRectF DiagramFrame::AddGroup(QList<COM_INFO*> pChild)
{
    if(pChild.count() <= 0)
        return QRectF(0,0,0,0);
    for(int ilist=0;m_ChildRectList.count()<m_CurCom->ChildComListPtr.count();ilist++)
        m_ChildRectList.append(QRectF(0,0,0,0));
    for(int i = 0;i<pChild.count();i++)
    {
        QGraphicsWidget *tmpWidget = pChild.at(i)->ComponentPtr;
        if(tmpWidget == 0)
            continue;
        m_ChildRectList.replace(i, tmpWidget->sceneBoundingRect());
        m_ItemGroup->addToGroup(tmpWidget);
    }
    return m_ItemGroup->sceneBoundingRect();
}

void DiagramFrame::DestoryGroup(QList<COM_INFO*> pChild)
{
    for(int i=0;i<pChild.count();i++)
    {
        QGraphicsWidget *tmpWidget = pChild.at(i)->ComponentPtr;
        if(tmpWidget == 0)
            continue;
        m_ItemGroup->removeFromGroup(tmpWidget);
        QRectF rect = m_CurCom->ComponentPtr->mapRectFromScene(m_ChildRectList.at(i));
        tmpWidget->setPos(rect.topLeft());
        tmpWidget->setParentItem(m_CurCom->ComponentPtr);
    }
}

void DiagramFrame::RestoreParent()
{
    for(int i = 0;i<m_CurCom->ChildComListPtr.count();i++)
    {
        QGraphicsWidget *tmpWidget = m_CurCom->ChildComListPtr.at(i)->ComponentPtr;
        if(tmpWidget == 0)
            continue;
        tmpWidget->setParentItem(m_CurCom->ComponentPtr);
        QRectF rect = m_CurCom->ComponentPtr->mapRectFromScene(m_ChildRectList.at(i));
        tmpWidget->setPos(rect.topLeft());
    }
}

void DiagramFrame::ReleaseParent()
{
    for(int i = 0;i<m_CurCom->ChildComListPtr.count();i++)
    {
        QGraphicsWidget *tmpWidget = m_CurCom->ChildComListPtr.at(i)->ComponentPtr;
        if(tmpWidget == 0)
            continue;
        tmpWidget->setParentItem(m_CurCom->ParentComPtr->ComponentPtr);
        //QRectF rect = m_CurCom->ParentComPtr->ComponentPtr->mapRectFromScene(m_ChildRectList.at(i));
        //tmpWidget->setPos(rect.topLeft());
    }
}


void DiagramFrame::UpdateCoordinate()
{
    m_CurRect = m_CurCom->ComponentPtr->sceneBoundingRect();
    m_CurLeft = m_CurRect.left();
    m_CurTop = m_CurRect.top();
    m_CurRight = m_CurRect.right();
    m_CurBottom = m_CurRect.bottom();
    //>@计算可拖动的最大大小
    QRectF tmpParRect = m_CurCom->ParentComPtr->ComponentPtr->sceneBoundingRect();
    m_LeftLimitMin = tmpParRect.left();
    m_TopLimitMin = tmpParRect.top();
    m_RightLimitMax = tmpParRect.right();
    m_BottomLimitMax = tmpParRect.bottom();
    m_ChildParentDiff = m_CurRect.topLeft() - tmpParRect.topLeft();
    //>@计算最小大小
    QRectF tmpChildRect = AddGroup(m_CurCom->ChildComListPtr);
    DestoryGroup(m_CurCom->ChildComListPtr);
    if(tmpChildRect == QRectF(0,0,0,0))
    {
        m_LeftLimitMax = m_CurRight - COM_MINSIZE;
        m_TopLimitMax = m_CurBottom - COM_MINSIZE;
        m_RightLimitMin = m_CurLeft + COM_MINSIZE;
        m_BottomLimitMin = m_CurTop + COM_MINSIZE;
    }
    else
    {
        m_LeftLimitMax = tmpChildRect.left();
        m_TopLimitMax = tmpChildRect.top();
        m_RightLimitMin = tmpChildRect.right();
        m_BottomLimitMin = tmpChildRect.bottom();
    }
}

void DiagramFrame::InitFrame(COM_INFO* pComInfo)
{
    QApplication::restoreOverrideCursor();
    m_SavedComOperate = DRAGNONE;

    m_SelectFlag = false;
    m_CurCom = pComInfo;
    if(!(m_CurCom && m_CurCom->ComponentPtr))
        return;
    if(!(m_CurCom->ParentComPtr && m_CurCom->ParentComPtr->ComponentPtr))
        return;
    UpdateCoordinate();
    //>@设置Frame
    setGeometry(m_CurRect.adjusted(-FRAMEVALIDWIDTH, -FRAMEVALIDWIDTH, FRAMEVALIDWIDTH, FRAMEVALIDWIDTH));
    setZValue(m_CurCom->ComponentPtr->zValue()+1);
    m_SelectFlag = true;
    update();
}

COM_OPERTATE DiagramFrame::GetCursorShape(QPointF Point)
{
    if ((Point.x()>m_CurLeft+FRAMEVALIDWIDTH)&&(Point.x()<m_CurRight-FRAMEVALIDWIDTH)&&
             (Point.y()>m_CurTop+FRAMEVALIDWIDTH)&&(Point.y()<m_CurBottom-FRAMEVALIDWIDTH))	//移动
    {
        m_CurCom->ComponentPtr->setParentItem(this);
        m_CurCom->ComponentPtr->setPos(QPointF(FRAMEVALIDWIDTH, FRAMEVALIDWIDTH));
         QApplication::setOverrideCursor(Qt::SizeAllCursor);
        return DRAGALL;
    }
    else if ((Point.x()<=m_CurLeft+FRAMEVALIDWIDTH)&&(Point.x()>=m_CurLeft-FRAMEVALIDWIDTH)&&
             (Point.y()<m_CurTop+FRAMEVALIDWIDTH)&&(Point.y()>=m_CurTop-FRAMEVALIDWIDTH))	//左上角缩放
    {
         QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
        return DRAGLEFTTOP;
    }
    else if ((Point.x()<=m_CurRight+FRAMEVALIDWIDTH)&&(Point.x()>m_CurRight-FRAMEVALIDWIDTH)&&
             (Point.y()<m_CurTop+FRAMEVALIDWIDTH)&&(Point.y()>=m_CurTop-FRAMEVALIDWIDTH))	//右上角缩放
    {
         QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
        return DRAGRIGHTTOP;
    }
    else if ((Point.x()<=m_CurRight+FRAMEVALIDWIDTH)&&(Point.x()>m_CurRight-FRAMEVALIDWIDTH)&&
             (Point.y()<=m_CurBottom+FRAMEVALIDWIDTH)&&(Point.y()>m_CurBottom-FRAMEVALIDWIDTH))	//右下角缩放
    {
         QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
        return DRAGRIGHTBOTTOM;
    }
    else if ((Point.x()<m_CurLeft+FRAMEVALIDWIDTH)&&(Point.x()>=m_CurLeft-FRAMEVALIDWIDTH)&&
             (Point.y()<=m_CurBottom+FRAMEVALIDWIDTH)&&(Point.y()>m_CurBottom-FRAMEVALIDWIDTH))	//左下角缩放
    {
         QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
        return DRAGLEFTBOTTOM;
    }
    else if((Point.x()<m_CurLeft+FRAMEVALIDWIDTH)&&(Point.x()>=m_CurLeft-FRAMEVALIDWIDTH)&&
            (Point.y()<m_CurBottom-FRAMEVALIDWIDTH)&&(Point.y()>m_CurTop+FRAMEVALIDWIDTH))		//左缩放
    {
          QApplication::setOverrideCursor(Qt::SizeHorCursor);
        return DRAGLEFT;
    }
    else if((Point.x()<m_CurRight-FRAMEVALIDWIDTH)&&(Point.x()>m_CurLeft+FRAMEVALIDWIDTH)&&
            (Point.y()<m_CurTop+FRAMEVALIDWIDTH)&&(Point.y()>m_CurTop-FRAMEVALIDWIDTH))		//上缩放
    {
         QApplication::setOverrideCursor(Qt::SizeVerCursor);
        return DRAGTOP;
    }
    else if ((Point.x()<=m_CurRight+FRAMEVALIDWIDTH)&&(Point.x()>m_CurRight-FRAMEVALIDWIDTH)&&
             (Point.y()>m_CurTop+FRAMEVALIDWIDTH)&&(Point.y()<m_CurBottom-FRAMEVALIDWIDTH))	//右缩放
    {
         QApplication::setOverrideCursor(Qt::SizeHorCursor);
        return DRAGRIGHT;
    }
    else if ((Point.x()<m_CurRight-FRAMEVALIDWIDTH)&&(Point.x()>m_CurLeft+FRAMEVALIDWIDTH)&&
             (Point.y()<=m_CurBottom+FRAMEVALIDWIDTH)&&(Point.y()>m_CurBottom-FRAMEVALIDWIDTH))	//下缩放
    {
         QApplication::setOverrideCursor(Qt::SizeVerCursor);
        return DRAGBOTTOM;
    }
    return DRAGNONE;
}

void DiagramFrame::ReleaseCursorShape()
{
    switch(m_SavedComOperate)
    {
        case DRAGNONE:
             return;
        case DRAGALL:
        {
            QRectF tmpRect = m_CurCom->ComponentPtr->sceneBoundingRect();
            int OverFlag = 0;
            qreal l = tmpRect.left();
            qreal t = tmpRect.top();
            qreal w = tmpRect.width();
            qreal h = tmpRect.height();
            if(l<m_LeftLimitMin)
            {
               OverFlag=1;
               l = m_LeftLimitMin;
            }
            else if(l+w>m_RightLimitMax)
            {
                OverFlag=1;
                l = m_RightLimitMax-w;
            }
            if(t<m_TopLimitMin)
            {
               OverFlag=1;
               t = m_TopLimitMin;
            }
            else if(t+h>m_BottomLimitMax)
            {
                OverFlag=1;
                t = m_BottomLimitMax-h;
            }
            QRectF tmpNewRect;
            if(OverFlag)
            {
                tmpNewRect = QRectF(l, t, w, h);
                setPos(tmpNewRect.topLeft() - QPointF(FRAMEVALIDWIDTH, FRAMEVALIDWIDTH));
            }
            else
            {
                tmpNewRect = tmpRect;
            }
            QRectF tmpValidRect = m_CurCom->ParentComPtr->ComponentPtr->mapRectFromScene(tmpNewRect);
            m_CurCom->ComponentPtr->setParentItem(m_CurCom->ParentComPtr->ComponentPtr);
            m_CurCom->OldRect = m_CurCom->ComRect;
            m_CurCom->ComRect = tmpValidRect;
            m_CurCom->DragOperate = m_SavedComOperate;
            emit updateControl();
            break;
        }
        case DRAGRIGHT:
        case DRAGBOTTOM:
        case DRAGRIGHTBOTTOM:
        {
            m_CurCom->ComRect.setSize(QSizeF(m_CurRight-m_CurLeft, m_CurBottom-m_CurTop));
            m_CurCom->DragOperate = m_SavedComOperate;
            emit updateControl();
            break;
        }
        case DRAGLEFT:
        case DRAGTOP:
        case DRAGLEFTTOP:
        case DRAGRIGHTTOP:
        case DRAGLEFTBOTTOM:
        {
            QRectF tmpRect = m_CurCom->ParentComPtr->ComponentPtr->mapRectFromScene(m_CurRect);
            m_CurCom->ComRect = tmpRect;
            m_CurCom->DragOperate = m_SavedComOperate;
            emit updateControl();
            break;
        }
        default:
            return;
    }
    QApplication::restoreOverrideCursor();
    m_SavedComOperate = DRAGNONE;
}

void DiagramFrame::UpdateFrame(QGraphicsSceneMouseEvent *event)
{
    //m_Mutex->lock();
    QPointF ScenePoint = event->scenePos();
    if(ScenePoint.isNull())
    {
        //m_Mutex->unlock();
        return;
    }
    //if (QLineF(Point, event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance())
    //{
    //    //m_Mutex->unlock();
    //    return;
    //}
    switch(m_SavedComOperate)
    {
        case DRAGALL:
        {
            QGraphicsWidget::mouseMoveEvent(event);
            //m_Mutex->unlock();
            return;
        }
        case DRAGLEFT:                  //左缩放
        {
            if(ScenePoint.x() >= m_LeftLimitMin && ScenePoint.x() <= m_LeftLimitMax)
            {
                m_CurLeft = ScenePoint.x();
            }
            else if(ScenePoint.x() < m_LeftLimitMin)
            {
                m_CurLeft = m_LeftLimitMin;
            }
            else if(ScenePoint.x() > m_LeftLimitMax)
            {
                m_CurLeft = m_LeftLimitMax;
            }
            break;
        }
        case DRAGTOP:                   //上缩放
        {
            if(ScenePoint.y() >= m_TopLimitMin && ScenePoint.y() <= m_TopLimitMax)
            {
                m_CurTop = ScenePoint.y();
            }
            else if(ScenePoint.y() < m_TopLimitMin)
            {
                m_CurTop = m_TopLimitMin;
            }
            else if(ScenePoint.y() > m_TopLimitMax)
            {
                m_CurTop = m_TopLimitMax;
            }
            break;
        }
        case DRAGRIGHT:                 //右缩放
        {
            if(ScenePoint.x() >= m_RightLimitMin && ScenePoint.x() <= m_RightLimitMax)
            {
                m_CurRight = ScenePoint.x();
            }
            else if(ScenePoint.x() < m_RightLimitMin)
            {
                m_CurRight = m_RightLimitMin;
            }
            else if(ScenePoint.x() > m_RightLimitMax)
            {
                m_CurRight = m_RightLimitMax;
            }
            break;
        }
        case DRAGBOTTOM:                //下缩放
        {
            if(ScenePoint.y() >= m_BottomLimitMin && ScenePoint.y() <= m_BottomLimitMax)
            {
                m_CurBottom = ScenePoint.y();
            }
            else if(ScenePoint.y() < m_BottomLimitMin)
            {
                m_CurBottom = m_BottomLimitMin;
            }
            else if(ScenePoint.y() > m_BottomLimitMax)
            {
                m_CurBottom = m_BottomLimitMax;
            }
            break;
        }
        case DRAGLEFTTOP:               //左上角缩放
        {
            if((ScenePoint.x() >= m_LeftLimitMin && ScenePoint.x() <= m_LeftLimitMax ))
            {
                m_CurLeft = ScenePoint.x();
            }
            else if(ScenePoint.x() < m_LeftLimitMin)
            {
                m_CurLeft = m_LeftLimitMin;
            }
            else if(ScenePoint.x() > m_LeftLimitMax)
            {
                m_CurLeft = m_LeftLimitMax;
            }
            if(ScenePoint.y() >= m_TopLimitMin && ScenePoint.y() <= m_TopLimitMax)
            {
                m_CurTop = ScenePoint.y();
            }
            else if(ScenePoint.y() < m_TopLimitMin)
            {
                m_CurTop = m_TopLimitMin;
            }
            else if(ScenePoint.y() > m_TopLimitMax)
            {
                m_CurTop = m_TopLimitMax;
            }
            break;
        }
        case DRAGRIGHTTOP:          //右上角缩放
        {
            if(ScenePoint.x() >= m_RightLimitMin && ScenePoint.x() <= m_RightLimitMax)
            {
                m_CurRight = ScenePoint.x();
            }
            else if(ScenePoint.x() < m_RightLimitMin)
            {
                m_CurRight = m_RightLimitMin;
            }
            else if(ScenePoint.x() > m_RightLimitMax)
            {
                m_CurRight = m_RightLimitMax;
            }
            if(ScenePoint.y() >= m_TopLimitMin && ScenePoint.y() <= m_TopLimitMax)
            {
                m_CurTop = ScenePoint.y();
            }
            else if(ScenePoint.y() < m_TopLimitMin)
            {
                m_CurTop = m_TopLimitMin;
            }
            else if(ScenePoint.y() > m_TopLimitMax)
            {
                m_CurTop = m_TopLimitMax;
            }
            break;
        }
        case DRAGRIGHTBOTTOM:       //右下角缩放
        {
            if(ScenePoint.x() >= m_RightLimitMin && ScenePoint.x() <= m_RightLimitMax)
            {
                m_CurRight = ScenePoint.x();
            }
            else if(ScenePoint.x() < m_RightLimitMin)
            {
                m_CurRight = m_RightLimitMin;
            }
            else if(ScenePoint.x() > m_RightLimitMax)
            {
                m_CurRight = m_RightLimitMax;
            }
            if(ScenePoint.y() >= m_BottomLimitMin && ScenePoint.y() <= m_BottomLimitMax)
            {
                m_CurBottom = ScenePoint.y();
            }
            else if(ScenePoint.y() < m_BottomLimitMin)
            {
                m_CurBottom = m_BottomLimitMin;
            }
            else if(ScenePoint.y() > m_BottomLimitMax)
            {
                m_CurBottom = m_BottomLimitMax;
            }
            break;
        }
        case DRAGLEFTBOTTOM:        //左下角缩放
        {
            if((ScenePoint.x() >= m_LeftLimitMin && ScenePoint.x() <= m_LeftLimitMax ))
            {
                m_CurLeft = ScenePoint.x();
            }
            else if(ScenePoint.x() < m_LeftLimitMin)
            {
                m_CurLeft = m_LeftLimitMin;
            }
            else if(ScenePoint.x() > m_LeftLimitMax)
            {
                m_CurLeft = m_LeftLimitMax;
            }
            if(ScenePoint.y() >= m_BottomLimitMin && ScenePoint.y() <= m_BottomLimitMax)
            {
                m_CurBottom = ScenePoint.y();
            }
            else if(ScenePoint.y() < m_BottomLimitMin)
            {
                m_CurBottom = m_BottomLimitMin;
            }
            else if(ScenePoint.y() > m_BottomLimitMax)
            {
                m_CurBottom = m_BottomLimitMax;
            }
            break;
        }
        default:
        {
            //m_Mutex->unlock();
            return;
        }
    }
    //>@设置Frame大小
    m_CurRect = QRectF(m_CurLeft, m_CurTop, m_CurRight-m_CurLeft, m_CurBottom-m_CurTop);
    setGeometry(m_CurRect.adjusted(-FRAMEVALIDWIDTH, -FRAMEVALIDWIDTH, FRAMEVALIDWIDTH, FRAMEVALIDWIDTH));
    update();
    //m_Mutex->unlock();
}

void DiagramFrame::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event == 0 || !m_SelectFlag)
        return;
    m_Mutex->lock();
    UpdateCoordinate();
    m_SavedPoint = event->scenePos();
    m_SavedComOperate = GetCursorShape(event->scenePos());  //>@设置了OverideCursor
    if(m_SavedComOperate == DRAGALL)
        QGraphicsWidget::mousePressEvent(event);
    m_Mutex->unlock();
}
void DiagramFrame::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(event == 0 || !m_SelectFlag)
        return;
    static bool busy = false;
    if(!busy)
        busy = true;
    else
        return;

    m_Mutex->lock();
    UpdateFrame(event);
    m_Mutex->unlock();

    busy = false;
}

void DiagramFrame::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event == 0 || !m_SelectFlag)
        return;
    m_Mutex->lock();
    if(m_SavedComOperate == DRAGALL)
        QGraphicsWidget::mouseReleaseEvent(event);
    ReleaseCursorShape();
    m_Mutex->unlock();
}

void DiagramFrame::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(!m_SelectFlag)
        return;
    static bool busy = false;
    if(!busy)
        busy = true;
    else
        return;

    m_Mutex->lock();
    //painter->translate(size().width()/2.0, size().height()/2.0);
    //>@绘制线段
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(m_Color, 2, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
    QRectF tmpRect = rect().adjusted(FRAMEVALIDWIDTH, FRAMEVALIDWIDTH, -FRAMEVALIDWIDTH, -FRAMEVALIDWIDTH);
    painter->drawRect(tmpRect);
    //>@绘制实心方形
    painter->setBrush(m_Color);
    painter->setPen(QPen(m_Color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPointF tmpCenterPoint = tmpRect.center();
    //>@绘制上边框
    //>@绘制大方块
    QPointF tmpPoint = tmpRect.topLeft();
    QPointF topleftPoint = tmpPoint - QPointF(VERTEXRADIUS, VERTEXRADIUS);
    QPointF bottomrightPoint = tmpPoint + QPointF(VERTEXRADIUS, VERTEXRADIUS);
    painter->drawRect(QRect(topleftPoint.toPoint(), bottomrightPoint.toPoint()));
    //>@绘制小方块
    QPointF referPoint = QPointF(tmpCenterPoint.x(), tmpPoint.y());
    topleftPoint = referPoint - QPointF(SIDERADIUS, SIDERADIUS);
    bottomrightPoint = referPoint + QPointF(SIDERADIUS, SIDERADIUS);
    painter->drawRect(QRect(topleftPoint.toPoint(), bottomrightPoint.toPoint()));
    //>@绘制右边框
    //>@绘制大方块
    tmpPoint = tmpRect.topRight();
    topleftPoint = tmpPoint - QPointF(VERTEXRADIUS, VERTEXRADIUS);
    bottomrightPoint = tmpPoint + QPointF(VERTEXRADIUS, VERTEXRADIUS);
    painter->drawRect(QRect(topleftPoint.toPoint(), bottomrightPoint.toPoint()));
    //>@绘制小方块
    referPoint = QPointF(tmpPoint.x(), tmpCenterPoint.y());
    topleftPoint = referPoint - QPointF(SIDERADIUS, SIDERADIUS);
    bottomrightPoint = referPoint + QPointF(SIDERADIUS, SIDERADIUS);
    painter->drawRect(QRect(topleftPoint.toPoint(), bottomrightPoint.toPoint()));
    //>@绘制下边框
    //>@绘制大方块
    tmpPoint = tmpRect.bottomRight();
    topleftPoint = tmpPoint - QPointF(VERTEXRADIUS, VERTEXRADIUS);
    bottomrightPoint = tmpPoint + QPointF(VERTEXRADIUS, VERTEXRADIUS);
    painter->drawRect(QRect(topleftPoint.toPoint(), bottomrightPoint.toPoint()));
    //>@绘制小方块
    referPoint = QPointF(tmpCenterPoint.x(), tmpPoint.y());
    topleftPoint = referPoint - QPointF(SIDERADIUS, SIDERADIUS);
    bottomrightPoint = referPoint + QPointF(SIDERADIUS, SIDERADIUS);
    painter->drawRect(QRect(topleftPoint.toPoint(), bottomrightPoint.toPoint()));
    //>@绘制左边框
    //>@绘制大方块
    tmpPoint = tmpRect.bottomLeft();
    topleftPoint = tmpPoint - QPointF(VERTEXRADIUS, VERTEXRADIUS);
    bottomrightPoint = tmpPoint + QPointF(VERTEXRADIUS, VERTEXRADIUS);
    painter->drawRect(QRect(topleftPoint.toPoint(), bottomrightPoint.toPoint()));
    //>@绘制小方块
    referPoint = QPointF(tmpPoint.x(), tmpCenterPoint.y());
    topleftPoint = referPoint - QPointF(SIDERADIUS, SIDERADIUS);
    bottomrightPoint = referPoint + QPointF(SIDERADIUS, SIDERADIUS);
    painter->drawRect(QRect(topleftPoint.toPoint(), bottomrightPoint.toPoint()));

    painter->translate(tmpRect.width() / 2.0, tmpRect.height() / 2.0);
    QFont font("Arial", 10, QFont::Black, true);
    font.setUnderline(false);
    painter->setFont(font);
    //>@绘制文字
    painter->drawText(QRectF(-50, -20, 100, 40), Qt::AlignCenter, getCurComName(m_CurCom->ComPath));
    m_Mutex->unlock();

    busy = false;
}

DiagramFrameItem::DiagramFrameItem(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsLineItem(parent, scene)
{
    //setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::red;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

QRectF DiagramFrameItem::boundingRect() const
{
    QPointF LineStartPos = line().p1();
    QSizeF LineEndSize = QSizeF(line().p2().x() - line().p1().x(),
                                line().p2().y() - line().p1().y());
    return QRectF(LineStartPos.rx()-5,LineStartPos.ry()-5,
                  LineEndSize.width()+10,LineEndSize.height()+10);
}

QPainterPath DiagramFrameItem::shape() const
{
    QPointF LineStartPos = line().p1();
    QSizeF LineEndSize = QSizeF(line().p2().x() - line().p1().x(),
                                line().p2().y() - line().p1().y());
    QRectF targetf = QRectF(LineStartPos.rx()-10,LineStartPos.ry()-10,
                            LineEndSize.width()+20,LineEndSize.height()+20);
    QPainterPath path;
    path.addRect(targetf);
    return path;
}

void DiagramFrameItem::updatePosition(const QPolygonF &rect, int startIndex, int endIndex)
{
    int count = rect.count();
    if(!(count == 4 && startIndex < count && endIndex < count))
    {
        IDE_TRACE();
        return;
    }
    QLineF line(rect.at(startIndex), rect.at(endIndex));
    int nextIndex = endIndex+1;
    if(nextIndex > count-1)
        nextIndex=0;
    QLineF nextLine(rect.at(endIndex), rect.at(nextIndex));
    myValidLen = nextLine.length();
    setLine(line);
}

void DiagramFrameItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    //>@绘制线段
    painter->setPen(QPen(myColor, 2, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line());
    if (isSelected())
    {
        painter->setPen(QPen(Qt::darkGray, 1, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0,-8.0);
        painter->drawLine(myLine);
    }
    //>@绘制实心方形
    painter->setBrush(myColor);
    painter->setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPointF topleftPoint = line().p1() - QPointF(VERTEXRADIUS, VERTEXRADIUS);
    QPointF bottomrightPoint = line().p1() + QPointF(VERTEXRADIUS, VERTEXRADIUS);
    painter->drawRect(QRect(topleftPoint.toPoint(), bottomrightPoint.toPoint()));
    //>@绘制小方块
    QPointF referPoint = line().pointAt(0.5);
    topleftPoint = referPoint - QPointF(SIDERADIUS, SIDERADIUS);
    bottomrightPoint = referPoint + QPointF(SIDERADIUS, SIDERADIUS);
    painter->drawRect(QRect(topleftPoint.toPoint(), bottomrightPoint.toPoint()));
}

