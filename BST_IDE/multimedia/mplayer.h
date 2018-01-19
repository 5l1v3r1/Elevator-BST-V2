#ifndef MPLAYER_H
#define MPLAYER_H

#include "global.h"
#include <QProcess>
#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QObject>
#include <QMutex>
#include <QThread>
#include <QQueue>

typedef struct _PLAY_MEDIA_INFO
{
    QString         mPath;
    QString         mPara;
    bool            mRepeat;

    struct _PLAY_MEDIA_INFO & operator=(const struct _PLAY_MEDIA_INFO &s)
    {
        mPath = s.mPath;
        mPara = s.mPara;
        mRepeat = s.mRepeat;
        return *this;
    }
}PLAY_MEDIA_INFO;

class MplayerThread;

class mplayer : public QObject
{
    Q_OBJECT
    friend class MplayerThread;
public:
    mplayer(QObject *parent = 0);
    ~mplayer();
    void CreatProcess();
    void ReleaseProcess();
    void ReleaseMediaBuffer();
    void PlayMedia();
signals:
    void sFinished();
public slots:
    void slot_SetVideoRect(QRect pRect, qreal pRotate = 0);
    void slot_PlayNext();
    void slot_Play(QString pSrc, QString pPara, bool pRepeat, bool pPrior);
    void slot_VideoToPic(QString pSrc, QString pSavepath, QString pSavetype);
    void slot_SetVolume(float pVolume);
    void slot_Seek(qint64 pMs);
    void slot_Pause();
    void slot_Resume();
    void slot_Stop();
    void slot_Finished(int pExitCode, QProcess::ExitStatus pStatus);
    void slot_Error(QProcess::ProcessError);
public:
    QProcess                  *m_PlayProcess;
    QString                    m_RectString;
    QString                    m_CurCmdString;
    bool                       m_Pause;
    bool                    m_ProcessBusy;
    bool                    HaveDing;
    bool                    HaveDoorOpen;
    MplayerThread             *m_MplayerThread;

    QList<PLAY_MEDIA_INFO>     m_MediaBuffer;
    PLAY_MEDIA_INFO            m_CurMedia;
};

class MplayerThread : QThread
{
    Q_OBJECT
    friend class mplayer;
public:
    MplayerThread(mplayer *qesp);
    ~MplayerThread();
public:
    void run();
public:
    mplayer         *m_Mplayer;
};

#endif // MPLAYER_H
