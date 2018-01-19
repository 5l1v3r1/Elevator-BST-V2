#include "designwebview.h"

DesignWebview::DesignWebview(QGraphicsItem *parent, QRectF pRect) :
    QGraphicsWebView(parent),
    DesignRC()
{
    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    settings()->setAttribute(QWebSettings::PluginsEnabled, true);//重要
    //解决打不开页面中的二级链接问题
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    setFlags(ItemIsMovable);
    m_Rect = pRect;
    setGeometry(m_Rect);
    m_EffectValid = false;

    m_ActionGroup = 0;
    m_ActionGroup = new QActionGroup(this);
    connect(m_ActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(ActionTrigger(QAction *)));

    QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
    for(int i=1;i<20;i++)
    {
        m_ActionGroup->addAction(QString("%1").arg(i));
    }
    tmpDefault->setChecked(true);
}

QRectF DesignWebview::boundingRect() const
{
    QRectF tmpRect = this->geometry().normalized();
    return QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized();
}

QPainterPath DesignWebview::shape() const
{
    QPainterPath path;
    QRectF tmpRect = this->geometry().normalized();
    path.addRect(QRectF(0,0,tmpRect.width(),tmpRect.height()).normalized());
    return path;
}

bool DesignWebview::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        RESOURCE_INFO tmpRc;
        char index;
        int count = pEffect->Effect.RcFiles.count();
        for(int i=0;i<count;i++)
        {
            tmpRc = pEffect->Effect.RcFiles.at(i);
            tmpString = tmpRc.RcName;
            if(tmpString.startsWith("Rc",Qt::CaseInsensitive) == false)
                continue;
            tmpString.remove(0, 2);
            IDE_TRACE_STR(tmpString);
            index = tmpString.toInt();
            if(LoadPath(index, tmpRc.RcFile) == false)
            {
                IDE_TRACE_STR(tmpRc.RcFile);
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool DesignWebview::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return DesignRC::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool DesignWebview::Start()
{
    DesignRC::Start();
    setData(COMNAME, QVariant(m_ComInfo->ComPath));
    updateEffect(0);
    return true;
}

void DesignWebview::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //>@绘制文字和边框
    QRectF tmpRect = this->geometry().normalized();
    qreal Width = tmpRect.width();
    qreal Height = tmpRect.height();
    painter->save();
    painter->translate(Width / 2, Height / 2);
    painter->setPen(Qt::DashDotDotLine);
    painter->setPen(QColor(10,10,255));

    QFont font("Arial", 10, QFont::Black, true);
    font.setUnderline(false);
    painter->setFont(font);
    //>@绘制文字和框
    painter->drawText(QRectF(-50, -20, 100, 40), Qt::AlignCenter, m_ComName);
    painter->drawRect(QRect(-Width/2, -Height/2, Width, Height));
    painter->drawRect(QRect(-Width/2+1, -Height/2+1, Width-2, Height-2));
    painter->restore();

    QGraphicsWebView::paint(painter, option, widget);
}

void DesignWebview::updateEffect(AREA_OPERATE pOperate, quint32 pPara)
{
    updateEffect(pOperate, QVariant(pPara));
}

void DesignWebview::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_CHANGE:
        {
            updateEffect(pPara.toInt());
            break;
        }
        default:
            break;
    }
}

void DesignWebview::updateEffect(int pIndex)
{
    bool tmpUrlChange = false;
    if(m_CurRcLabel != pIndex)
        tmpUrlChange = true;
    if(tmpUrlChange)
    {
        QString tmpRc = m_EffectPath.value(pIndex);
        IDE_TRACE_STR(tmpRc);
        FILEPATH_TYPE tmpType = GetRcFileType(tmpRc);
        if(tmpType == PATH_TYPENETADDR)
            load(QUrl(tmpRc));
        else
            setHtml(tmpRc);
    }
    m_CurRcLabel = pIndex;
    show();
}


QList<QAction*> DesignWebview::GetActionList()
{
    QList<QAction*> tmpList;
    if(m_ActionGroup)
    {
        tmpList = m_ActionGroup->actions();
        //>@根据当前已有的资源使能响应功能Action
        for(int i=0;i<tmpList.count();i++)
        {
            QAction *tmpAction = tmpList.at(i);
            if(tmpAction == 0)
                continue;
            int tmpState = tmpAction->text().toInt();
            if(m_EffectPath.contains(tmpState))
                tmpAction->setEnabled(true);
            else
                tmpAction->setDisabled(true);
        }
    }
    return tmpList;
}

void DesignWebview::ExecAction(QAction *pAction)
{
    updateEffect(pAction->text().toInt());
}

void DesignWebview::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_ComType != COM_DESKTOP)
        setCursor(Qt::ClosedHandCursor);
    QGraphicsWidget::mousePressEvent(event);
}

void DesignWebview::focusInEvent(QFocusEvent *event)
{
    if(m_ComType != COM_DESKTOP)
        setCursor(Qt::OpenHandCursor);
    QGraphicsWidget::focusOutEvent(event);
}

void DesignWebview::focusOutEvent(QFocusEvent *event)
{
    if(m_ComType != COM_DESKTOP)
        setCursor(Qt::ArrowCursor);
    QGraphicsWidget::focusOutEvent(event);
}

void DesignWebview::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsWidget::mouseMoveEvent(event);
}

void DesignWebview::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_ComType != COM_DESKTOP)
    {
        setCursor(Qt::ArrowCursor);
        emit MouseRelease(event->scenePos());
    }
    QGraphicsWidget::mouseReleaseEvent(event);
}
