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
#include "QQueue"
#include "audiothread.h"

class AudioThread;

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

class mplayer : public QObject
{
    Q_OBJECT
public:
    mplayer(QObject *parent = 0);
    ~mplayer();
    void CreatProcess();
    void ReleaseProcess();
    void ReleaseMediaBuffer();
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
    QProcess         *m_PlayProcess;
    QString             m_RectString;
    QString             m_CurCmdString;
    bool                    m_Pause;
    AudioThread *mAudioThread;


    bool m_ProcessBusy;
    //QList<PLAY_MEDIA_INFO>     m_MediaBuffer;
//    QQueue<QString>    m_MediaBuffer;
    QStringList     m_MediaBuffer;
//    PLAY_MEDIA_INFO            m_CurMedia;
    QString         m_CurMedia;
//    QMutex       *m_Mutex;
    bool HaveDing;
    bool HaveDoorOpen;
};

#endif // MPLAYER_H
