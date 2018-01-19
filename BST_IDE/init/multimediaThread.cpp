#include "multimediathread.h"

MultimediaThread::MultimediaThread(MiddleTopTabInit *pUi, QString FileName, QString Url) : QThread()
{
    stopThreads = false;

    ui = pUi;
    FilmName = FileName;
    UrlString = Url;
}

void MultimediaThread::stopThread(bool isStop)
{
    stopThreads = isStop;
}

void MultimediaThread::run() {
    for(int i = 0; (!stopThreads); i++)
    {
    }
}

