#ifndef PREVIEWLABEL_H
#define PREVIEWLABEL_H

#include "global.h"
#include <QLabel>
#include <QMovie>
#include <QSvgRenderer>

/*
    ͼƬ
    QML
    gif svg
    video
    stream
    audio
*/

class PreviewLabel : public QLabel
{
    Q_OBJECT
public:
    explicit PreviewLabel(QWidget *parent = 0);
    ~PreviewLabel();
    void Release();

    void ShowPixmap(QString pFilePath);
    void ShowGIF(QString pFilePath);
    void ShowSVG(QString pFilePath);
    void ShowQML(QString pFilePath);
    void PlayMedia(QString pFilePath);

    void Preview(QString pFile);

    void resizeEvent(QResizeEvent *e);

signals:
    void UpdatePreview();
    
public slots:
    
public:
    QMovie          *m_GifMovie;
    QSvgRenderer    *m_SvgRender;
    QProcess        *m_MediaProcess;
};

#endif // PREVIEWLABEL_H
