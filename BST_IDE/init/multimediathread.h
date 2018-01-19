#ifndef MULTIMEDIATHREAD_H
#define MULTIMEDIATHREAD_H

#include "global.h"
#include <QThread>

class MiddleTopTabInit;

class MultimediaThread : public QThread
{
public:
    MultimediaThread(MiddleTopTabInit *pUi, QString FileName, QString Url);
    void stopThread(bool isStop);
protected:
    void run();
private:
    bool stopThreads ;
    MiddleTopTabInit *ui;
    QString FilmName;
    QString UrlString;
};

#endif // CASETHREAD_H
