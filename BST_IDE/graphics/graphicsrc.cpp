#include "graphicsrc.h"
#include "graphicsscene.h"

GraphicsRC::GraphicsRC(GraphicsScene *pScene, QRectF pRect)
{
    m_ComRect = pRect;
    m_UiScene = pScene;
    m_RcStyle = 0;
#ifdef IDE
    m_ComCategory = 0;
#endif
    ReleaseRC();
}

//>@从ui文件中删除此ComElement
GraphicsRC::~GraphicsRC()
{
    ReleaseRC();
}

int GraphicsRC::GetKey(QString pString)
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

bool GraphicsRC::LoadPath(int pKey, QString pPixmapName)
{
    if(pPixmapName.isEmpty())
        return false;
    if(m_UiScene->m_UiManager->RegulatRcFile(pPixmapName) == PATH_TYPENONE)
    {
        IDE_TRACE_STR(pPixmapName);
        return false;
    }
    m_EffectPath.insert(pKey, pPixmapName);
    return true;
}

void GraphicsRC::ReleaseEffectGroup()
{
    if(m_RcStyle)
    {
        delete m_RcStyle;
        m_RcStyle = 0;
    }
    QList<AREA_OPERATE> tmpList = m_EffectGroup.keys();
    for(int i=0;i<tmpList.count();i++)
    {
        AREA_ANIMATE *tmpAnimate = m_EffectGroup.take(tmpList.at(i));
        if(tmpAnimate)
            delete tmpAnimate;
    }
}

void GraphicsRC::ReleasePixmaps()
{
    m_EffectImage.clear();
}

void GraphicsRC::ReleaseMovies()
{
    QList<int> tmpList = m_EffectMovie.keys();
    for(int i=0;i<tmpList.count();i++)
    {
        QMovie* tmpMovie = m_EffectMovie.take(tmpList.at(i));
        if(!tmpMovie)
            continue;
        tmpMovie->deleteLater();
    }
}

void GraphicsRC::ReleaseSvgRender()
{
#ifdef  SURRPORT_SVG
    QList<int> tmpList = m_EffectSvgRender.keys();
    for(int i=0;i<tmpList.count();i++)
    {
        QSvgRenderer *tmpSvgRender = m_EffectSvgRender.take(tmpList.at(i));
        if(!tmpSvgRender)
            continue;
        tmpSvgRender->deleteLater();
    }
#endif
}

void GraphicsRC::ReleaseRC()
{
    m_EffectValid = false;
    m_EffectPath.clear();
    ReleaseEffectGroup();
    ReleasePixmaps();
    ReleaseMovies();
    ReleaseTextures();
    ReleaseSvgRender();
}

bool GraphicsRC::RefreshRC()
{
    ReleasePixmaps();
    ReleaseMovies();
    ReleaseTextures();
    ReleaseSvgRender();
    return Start();
}

void GraphicsRC::InitScene(GraphicsScene* pScene)
{
    m_UiScene = pScene;
}

void GraphicsRC::DeleteComponent()
{
    if(m_UiScene)
        m_UiScene->m_UiManager->DeleteComDom(m_ComType);
}

bool GraphicsRC::InitComponent(QHash<int, QPixmap> pPixmapList, QHash<AREA_OPERATE, AREA_ANIMATE*>  pEffectGroup)
{
    ReleaseRC();
    m_EffectImage = pPixmapList;
    m_EffectGroup = pEffectGroup;
    Start();
    return true;
}

bool GraphicsRC::InitComponent(QDomElement &pElement, bool pOffset)
{
    if(pElement.isNull())
    {
        if(m_ComRect.isNull())
            return false;
//        pElement = m_UiScene->m_UiManager->AddComDom(pComType, m_ComRect);
        if(pElement.isNull())
            return false;
    }
    m_ComType = getComponetType(pElement.tagName());
    m_ComPath = getComponetPath(m_ComType);
#ifdef IDE
    m_ComCategory = GlobalPara.m_ComCategoryGroup.value(m_ComPath);
    if(!m_ComCategory)
        return false;
#endif
    ReleaseRC();
    m_ComElement = pElement;
    //>@Rect
    QRect tmpRect = m_UiScene->m_UiManager->GetComRect(m_ComElement, m_UiScene->m_UiDirection);
    if(pOffset)
        m_ComRect = ToSceneRect(QRectF(tmpRect));
    else
        m_ComRect = QRectF(tmpRect);
    //>@SM
    QDomNodeList tmpStateList = m_UiScene->m_UiManager->GetComStateList(m_ComElement);
    for(int i=0;i<tmpStateList.count();i++)
        InitState(tmpStateList.at(i).toElement());
    return Start();
}

bool GraphicsRC::InitState(QDomElement pElement)
{
    if(pElement.isNull())
        return false;
    QString tmpStateName = pElement.tagName();
    AREA_OPERATE tmpOperate = GetOperateType(tmpStateName);
    if(tmpOperate == OPERATE_NONE)
        return false;
    QString tmpType = pElement.attribute(D_EFFECTTYPE);
    if(tmpType.isEmpty())
        return false;
    //>@初始化
    if(tmpOperate == OPERATE_LOADRC)
    {
        //>@添加资源
        QDomNodeList tmpList = pElement.childNodes();
        int count = tmpList.count();
        for(int i=0;i<count;i++)
        {
            QDomElement tmpRcElement = tmpList.at(i).toElement();
            if(tmpRcElement.isNull())
                continue;
            int index = GetKey(tmpRcElement.tagName());
            if(index == -1)
                continue;
            //IDE_TRACE_INT(index);
            LoadPath(index, tmpRcElement.text());
        }
        //>@查看当前系统中是否已经有资源样式
        if(m_RcStyle)
            delete m_RcStyle;
        //>@添加资源样式
        m_RcStyle = new AREA_STYLE;
        m_RcStyle->mRcType = tmpType;
        InitCommonPara(m_RcStyle, pElement.attribute(D_EFFECTPARA));
    }
    else
    {
        EffectType tmpEffectType = GetEffectType(tmpType);
        if(tmpEffectType == EffectTypeNone)
            return false;
        //>@添加特效参数
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        tmpAreaAnimate->mEffectType = tmpEffectType;
        InitEffectPara(tmpAreaAnimate, pElement.attribute(D_EFFECTPARA));
        //>@查看当前系统中是否已经有此状态机
        AREA_ANIMATE *tmpOldAreaAnimate = m_EffectGroup.take(tmpOperate);
        if(tmpOldAreaAnimate)
            delete tmpOldAreaAnimate;
        m_EffectGroup.insert(tmpOperate, tmpAreaAnimate);
    }
    return true;
}

bool GraphicsRC::InitSubPara(QString pStateName, QString pLabel, QString pContent)
{
    AREA_OPERATE tmpOperate = GetOperateType(pStateName);
    bool ret = false;
    if(tmpOperate == OPERATE_LOADRC)  //>@common
        ret = InitSubCommonPara(m_RcStyle, pLabel, pContent);
    else
        ret = InitSubEffectPara(m_EffectGroup.value(tmpOperate), pLabel, pContent);
    return ret;
}

bool GraphicsRC::InitCommonPara(AREA_STYLE* pAreaStyle, QString pCommonPara)
{
    if(pCommonPara.isEmpty())
        return false;
    QStringList tmpList = pCommonPara.split(";", QString::SkipEmptyParts);
    for(int i=0;i<tmpList.count();i++)
    {
        QStringList tmpSubList = tmpList.at(i).split(":", QString::SkipEmptyParts);
        if(tmpSubList.count()<2)
            continue;
        InitSubCommonPara(pAreaStyle, tmpSubList.at(0), tmpSubList.at(1));
    }
    return true;
}

bool GraphicsRC::InitSubCommonPara(AREA_STYLE* pAreaStyle, QString pLabel, QString pContent)
{
    if(pAreaStyle == 0)
        return false;
    return true;
}

bool GraphicsRC::InitEffectPara(AREA_ANIMATE* pAreaEffect, QString pEffectPara)
{
    if(pEffectPara.isEmpty())
        return false;
    QStringList tmpList = pEffectPara.split(";", QString::SkipEmptyParts);
    for(int i=0;i<tmpList.count();i++)
    {
        QStringList tmpSubList = tmpList.at(i).split(":", QString::SkipEmptyParts);
        if(tmpSubList.count()<2)
            continue;
        InitSubEffectPara(pAreaEffect, tmpSubList.at(0), tmpSubList.at(1));
    }
    return true;
}

bool GraphicsRC::InitSubEffectPara(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
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
    else if(pLabel.compare(PARA_DUTYRATIO, Qt::CaseInsensitive) == 0)  //>@0~100
    {
        bool ok;
        int dec = pContent.toInt(&ok, 10);  //>@10进制
        if(ok)
            pAreaEffect->mDutyRatio = dec;
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
    return true;
}

QPixmap GraphicsRC::ScalePixmap(QPixmap pPixmap, QSizeF pSize)
{
    QSize tmpSize = pSize.toSize();
    if(!(tmpSize.isEmpty()) && pPixmap.size() != tmpSize)
        return pPixmap.scaled(tmpSize);
    return pPixmap;
}

#ifdef  SURRPORT_SVG
QSvgRenderer *GraphicsRC::GetSvgRender(int pKey)
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

QPixmap GraphicsRC::GetPixmap(int pKey, QSizeF pSize)
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

QPixmap GraphicsRC::LoadPixmap(QString pPixmapName, QSizeF pSize)
{
    if(pPixmapName.isEmpty())
        return QPixmap();
    FILEPATH_TYPE tmpType = m_UiScene->m_UiManager->RegulatRcFile(pPixmapName);
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
        case RC_FONT:
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

//>@根据提供的资源标号，获取下一个资源的标号
int GraphicsRC::getNextRc(int pCurRcIndex)
{
    if(m_RcList.count() > 0)
    {
        for(int i=0;i<m_RcList.count();i++)
        {
            int value = m_RcList.at(i);
            if(value == pCurRcIndex)
            {
                if((i+1) < m_RcList.count())
                    return m_RcList.at(i+1);
                else
                    return m_RcList.first();
            }
        }
        //>@如果找不到提供的资源标号，则默认返回最后一个资源的标号
        return m_RcList.last();
    }
    return -1;
}

int GraphicsRC::getPrevRc(int pCurRcIndex)
{
    if(m_RcList.count() > 0)
    {
        for(int i=0;i<m_RcList.count();i++)
        {
            int value = m_RcList.at(i);
            if(value == pCurRcIndex)
            {
                if((i-1) >= 0)
                    return m_RcList.at(i-1);
                else
                    return m_RcList.last();
            }
        }
        //>@如果找不到提供的资源标号，则默认返回第一个资源的标号
        return m_RcList.first();
    }
    return -1;
}

QList<EFFECT*> GraphicsRC::GetDefaultEffects(QString pStateName)
{
    QList<EFFECT*> tmpList;
#ifdef IDE
    for(int i=0;i<m_ComCategory->StateMachines.count();i++)
    {
        STATEMACHINE *tmpSM = m_ComCategory->StateMachines.at(i);
        if(!tmpSM)
            continue;
        if(tmpSM->StateName.compare(pStateName))
            continue;
        tmpList = tmpSM->Effects;
        break;
    }
#endif
    return tmpList;
}

EFFECT* GraphicsRC::GetDefaultEffect(QList<EFFECT*> pEffects, QString pEffectName)
{
    for(int j=0;j<pEffects.count();j++)
    {
        EFFECT *tmpFindEffect = pEffects.at(j);
        if(!tmpFindEffect)
            continue;
        if(tmpFindEffect->EffectName.compare(pEffectName))
            continue;
        return tmpFindEffect;
    }
    return (EFFECT *)0;
}

EFFECT* GraphicsRC::GetDefaultEffect(QString pStateName, QString pEffectName)
{
    return GetDefaultEffect(GetDefaultEffects(pStateName), pEffectName);
}

QDomElement GraphicsRC::GetComStateElement(QString pStateName, QString &pEffectName, QString &pEffectPara)
{
    QDomElement tmpStateElement = m_UiScene->m_UiManager->GetComState(m_ComElement, pStateName);
    if(!tmpStateElement.isNull())
    {
        pEffectName = tmpStateElement.attribute(D_EFFECTTYPE);
        pEffectPara = tmpStateElement.attribute(D_EFFECTPARA);
    }
    return tmpStateElement;
}

QString GraphicsRC::GetComStateEffect(QString pStateName)
{
    QDomElement tmpStateElement = m_UiScene->m_UiManager->GetComState(m_ComElement, pStateName);
    if(!tmpStateElement.isNull())
    {
        return tmpStateElement.attribute(D_EFFECTTYPE);
    }
    return QString();
}

QString GraphicsRC::GetComStateEffectPara(QString pStateName, QString pParaName)
{
    QDomElement tmpStateElement = m_UiScene->m_UiManager->GetComState(m_ComElement, pStateName);
    if(!tmpStateElement.isNull())
    {
        QStringList tmpParaList = tmpStateElement.attribute(D_EFFECTPARA).split(';');
        for(int i=0;i<tmpParaList.count();i++)
        {
            QStringList tmpParaValueList = tmpParaList.at(i).split(':');
            if(tmpParaValueList.count() != 2)
                continue;
            if(tmpParaValueList.first().compare(pParaName) == 0)
                return tmpParaValueList.at(1);
        }
    }
    return QString();
}

QString GraphicsRC::GetComStateRcFile(QString pStateName, QString pRcName)
{
    QDomElement tmpStateElement = m_UiScene->m_UiManager->GetComState(m_ComElement, pStateName);
    if(tmpStateElement.isNull())
        return QString();
    QDomElement tmpRcElement = tmpStateElement.firstChildElement(pRcName);
    if(tmpRcElement.isNull())
        return QString();
    return tmpRcElement.text();
}

