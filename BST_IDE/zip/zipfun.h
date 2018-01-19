#ifndef ZIPFUN_H
#define ZIPFUN_H

#include "globalfun.h"
#ifdef GCC
#include "quazipfile.h"
#endif

class ZipTask : public QObject
{
    Q_OBJECT
public:
    ZipTask() {}
    ~ZipTask() {}
signals:
    void start();
    void error();
    void progress(int ratio);
    void message(QString pInfo);
    void end();
public slots:
    bool Zip(QString pSrcPath, QString pDstPath);
    //>@ex:(d:/abc/src, d:/dfc/a.zip, true)
    bool Zip(QString pSrcPath, QString pZipFile, bool pCover);
    //>@ex:(d:/dfc/a.zip, d:/abc/, true)
    bool Unzip(QString pZipFile, QString pDstPath, bool pCover);
    void Stop(){}
};

class ZipTaskThread : public QThread
{
    Q_OBJECT
public:
    ZipTaskThread(QObject *parent = 0);
    void stop()
    {
        if (QThread::currentThread() != this)
            wait();
        exit();
    }
public slots:
    bool Zip(QString pSrcPath, QString pZipFile, bool pCover);
    bool Unzip(QString pZipFile, QString pDstPath, bool pCover);
public:
    ZipTask m_ZipTask;
};

#endif // ZIPFUN_H
