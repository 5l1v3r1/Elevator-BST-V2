#ifndef GIFPLAYER_H
#define GIFPLAYER_H

#include "define.h"
#include <QLabel>
#include <QMovie>
#include <QImageReader>

class gifplayer : public QLabel
{
    Q_OBJECT
public:
    explicit gifplayer(QString pGifFile, bool pRepeat, QWidget *parent = 0);
    void Play(QString pGifFile, bool pRepeat);

signals:
    
public slots:
    void slot_Finished();
    void slot_Error(QImageReader::ImageReaderError pErr);

public:
    QMovie  *m_GifMovie;
    bool     m_Repeat;
};

#endif // GIFPLAYER_H
