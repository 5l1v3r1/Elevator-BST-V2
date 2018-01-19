#include "graphicswebview.h"

GraphicsWebview::GraphicsWebview(QGraphicsItem *parent, QRectF pRect) :
    GraphicsBase(parent, pRect)
{
#ifdef SURRPORT_WEBKIT
    m_Webview = 0;
#endif
}

bool GraphicsWebview::InitEffectRc(STATE_INFO* pEffect)
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
            index = GetKey(tmpRc.RcName);
            IDE_TRACE_INT(index);
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

bool GraphicsWebview::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return GraphicsBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool GraphicsWebview::Start()
{
    if(GraphicsBase::Start() == false)
        return false;
    CreateWebview();
    m_PixmapPointer.mLastPointer = m_PixmapPointer.mCurPointer = GetPixmap(BGRC);

    updateEffect(m_RcList.first());
    return true;
}

bool GraphicsWebview::PaintEffect(QPainter *p)
{
    if(GraphicsBase::PaintEffect(p) == false)
    {
        return false;
    }
}

bool GraphicsWebview::CreateWebview()
{
#ifdef SURRPORT_WEBKIT
    if(m_Webview)
        m_Webview->deleteLater();
    m_Webview = new QGraphicsWebView(this);
    m_Webview->setGeometry(rect());
    m_Webview->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    m_Webview->settings()->setAttribute(QWebSettings::PluginsEnabled, true);//??¨°?
    //?a??¡ä¨°????¨°????D¦Ì??t??¨¢¡ä?¨®?¨º¨¬a
    m_Webview->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    m_Webview->show();
#endif
    return true;
}

bool GraphicsWebview::DestoryWebview()
{
#ifdef SURRPORT_WEBKIT
    if(m_Webview == 0)
        return true;
    m_Webview->stop();
    m_Webview->deleteLater();
    m_Webview = 0;
#endif
    return true;
}

void GraphicsWebview::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    switch(pOperate)
    {
        case OPERATE_CHANGE:
        {
            int index = pPara.toInt();
            updateEffect(index);
            break;
        }
        default:
            break;
    }
}

void GraphicsWebview::updateEffect(int pIndex)
{
    if(PREVRC == pIndex)
        pIndex = getPrevRc(m_CurRcLabel);
    else if(NEXTRC == pIndex)
        pIndex = getNextRc(m_CurRcLabel);

    if(BGRC == pIndex)
    {
        return;
    }

    bool tmpUrlChange = false;
    if(m_CurRcLabel != pIndex)
        tmpUrlChange = true;
#ifdef SURRPORT_WEBKIT
    if(tmpUrlChange && m_Webview)
    {
        QString tmpRc = m_EffectPath.value(pIndex);
        FILEPATH_TYPE tmpType = GetRcFileType(tmpRc);
        if(tmpType == PATH_TYPENETADDR)
            m_Webview->load(QUrl(tmpRc));
        else
            m_Webview->setHtml(tmpRc);
    }
#endif
    m_CurRcLabel = pIndex;
}


DesignWebview::DesignWebview(QGraphicsItem *parent, QRectF pRect) :
    GraphicsWebview(parent, pRect)
{
    QAction *tmpDefault = m_ActionGroup->addAction(tr("0"));
    for(int i=1;i<20;i++)
    {
        m_ActionGroup->addAction(QString("%1").arg(i));
    }
    tmpDefault->setChecked(true);
}

QList<QAction*> DesignWebview::GetActionList()
{
    QList<QAction*> tmpList;
    if(m_ActionGroup)
    {
        tmpList = m_ActionGroup->actions();
        //>@?¨´?Y¦Ì¡À?¡ã¨°?¨®D¦Ì?¡Á¨º?¡ä¨º??¨¹?¨¬¨®|?|?¨¹Action
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
