#include "gifplayer.h"

gifplayer::gifplayer(QString pGifFile, bool pRepeat, QWidget *parent) :
    QLabel(parent)
{
    setStyleSheet("background-color: transparent;");
    setScaledContents(true);

    m_Repeat = false;
    m_GifMovie = new QMovie(this);
    connect(m_GifMovie, SIGNAL(finished()), this, SLOT(slot_Finished()));
    connect(m_GifMovie, SIGNAL(error(QImageReader::ImageReaderError)), this, SLOT(slot_Error(QImageReader::ImageReaderError)));
    setMovie(m_GifMovie);

    Play(pGifFile, pRepeat);
}

void gifplayer::Play(QString pGifFile, bool pRepeat)
{
    if(!QFile::exists(pGifFile))
        return;
    if(m_GifMovie->state() != QMovie::NotRunning)
        m_GifMovie->stop();
    m_Repeat = pRepeat;
    m_GifMovie->setFileName(pGifFile);
    m_GifMovie->start();
}

void gifplayer::slot_Finished()
{
    if(m_Repeat)
        m_GifMovie->start();
}

void gifplayer::slot_Error(QImageReader::ImageReaderError pErr)
{
    m_GifMovie->stop();
    IDE_TRACE_INT(pErr);
}
