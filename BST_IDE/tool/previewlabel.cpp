#include "previewlabel.h"

PreviewLabel::PreviewLabel(QWidget *parent) :
    QLabel(parent)
{
    setScaledContents(true);
    m_GifMovie = 0;
    m_SvgRender = 0;
    m_MediaProcess = 0;
}

PreviewLabel::~PreviewLabel()
{
    Release();
}

void PreviewLabel::resizeEvent(QResizeEvent *e)
{
    emit UpdatePreview();
}

void PreviewLabel::Release()
{
    if(m_GifMovie)
    {
        m_GifMovie->deleteLater();
        m_GifMovie = 0;
    }
    if(m_SvgRender)
    {
        m_SvgRender->deleteLater();
        m_SvgRender = 0;
    }
    if(m_MediaProcess)
    {
        m_MediaProcess->deleteLater();
        m_MediaProcess = 0;
    }
}

void PreviewLabel::Preview(QString pFile)
{
    RC_TYPE tmpRcType = getRcType(pFile);
    switch(tmpRcType)
    {
        case RC_BMP:
        case RC_JPG:
        case RC_PNG:
        {
            ShowPixmap(pFile);
            break;
        }
        case RC_GIF:
        {
            ShowGIF(pFile);
            break;
        }
        case RC_SVG:
        {
            ShowSVG(pFile);
            break;
        }
        case RC_QML:
        {
            ShowQML(pFile);
            break;
        }
        case RC_FONT:
        {
            break;
        }
        case RC_MP3:
        case RC_WAV:
        case RC_MP4:
        case RC_AVI:
        case RC_RMVB:
        case RC_FLV:
        {
            PlayMedia(pFile);
            break;
        }
        default:
            break;
    }
}

void PreviewLabel::ShowPixmap(QString pFilePath)
{
    Release();
    setPixmap(QPixmap(pFilePath));
}

void PreviewLabel::ShowGIF(QString pFilePath)
{
    Release();
    if(!m_GifMovie)
    {
        m_GifMovie = new QMovie(this);
        setMovie(m_GifMovie);
    }
    if(!QFile::exists(pFilePath))
        return;
    if(m_GifMovie->state() != QMovie::NotRunning)
        m_GifMovie->stop();
    m_GifMovie->setFileName(pFilePath);
    m_GifMovie->start();
}

void PreviewLabel::ShowSVG(QString pFilePath)
{
    Release();
    if(m_SvgRender)
        m_SvgRender->deleteLater();
    m_SvgRender = new QSvgRenderer(pFilePath);
    if(m_SvgRender->isValid())
    {
        QPainter p(this);
        m_SvgRender->render(&p);
        p.end();
    }
}

void PreviewLabel::ShowQML(QString pFilePath)
{
    Release();
}

void PreviewLabel::PlayMedia(QString pFilePath)
{
    Release();
    if(!m_MediaProcess)
        m_MediaProcess = new QProcess;
    FILEPATH_TYPE tmpFileType = getRcFileType(pFilePath);
    if(tmpFileType == PATH_TYPENETADDR)
    {
        //>@播放当前流媒体视频
        unsigned long id = (unsigned long)winId();
        QString tmpCmd = QString("%1 %2 -nocache -slave -wid %3 &").arg(D_MPLAYER_PATH).arg(pFilePath).arg(id);
        m_MediaProcess->start(tmpCmd);
    }
    else
    {
        RC_TYPE tmpRcType = getRcType(pFilePath);
        if(tmpRcType >= RC_MP4)
        {
            unsigned long id = (unsigned long)winId();
            QString tmpCmd = QString("%1 %2 -framedrop -wid %3 &").arg(D_MPLAYER_PATH).arg(pFilePath).arg(id);
            m_MediaProcess->start(tmpCmd);
        }
    }
}

