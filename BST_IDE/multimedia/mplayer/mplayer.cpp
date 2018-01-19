#include "mplayer.h"

mplayer::mplayer(QObject *parent) :
    QObject(parent)
{
    mAudioThread = new AudioThread(this,0);
    m_RectString.clear();
    m_PlayProcess = 0;
    m_Pause = false;
    m_ProcessBusy = false;
    HaveDing = false;
    HaveDoorOpen = false;
    CreatProcess();
}

mplayer::~mplayer()
{
    ReleaseMediaBuffer();
    m_RectString.clear();
    m_Pause = false;
    ReleaseProcess();
}

void mplayer::CreatProcess()
{
    m_PlayProcess = new QProcess(this);
    connect(m_PlayProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(slot_Finished(int, QProcess::ExitStatus)));
    connect(m_PlayProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(slot_Error(QProcess::ProcessError)));
}

void mplayer::ReleaseProcess()
{
    if(m_PlayProcess !=0)
    {
        disconnect(m_PlayProcess, SIGNAL(finished(int, QProcess::ExitStatus)),0,0);
        m_PlayProcess->kill();
        static quint8 tmpState;
        while((tmpState = m_PlayProcess->state()))
        {
            IDE_TRACE_INT(tmpState);
            QtSleep(20);
        }
        m_PlayProcess->waitForFinished(D_WAITTIME);
        delete m_PlayProcess;
        m_PlayProcess = 0;
    }
}

void mplayer::slot_SetVideoRect(QRect pRect, qreal pRotate)
{
    QDesktopWidget *tmpDesktop = QApplication::desktop();
    QRect tmpDesktopRect = tmpDesktop->rect();
    int x, y, w, h;
    QString tmpRotatePara;
    switch((int)pRotate)
    {
        case 90:
            x = tmpDesktopRect.width() - pRect.y() - pRect.height();
            y = pRect.x();
            w = pRect.width();
            h = pRect.height();
            tmpRotatePara = QString("-vf rotate=1 -zoom -x %1 -y %2 -geometry +%3+%4").arg(w).arg(h).arg(x).arg(y);
            break;
        case 180:
            x = tmpDesktopRect.width() - pRect.x() - pRect.width();
            y = tmpDesktopRect.height() - pRect.y() - pRect.height();
            w = pRect.width();
            h = pRect.height();
            tmpRotatePara = QString("-vf flip -zoom -x %1 -y %2 -geometry +%3+%4").arg(w).arg(h).arg(x).arg(y);
            break;
        case 270:
            x = pRect.y();
            y = tmpDesktopRect.height() - pRect.x() - pRect.width();
            w = pRect.width();
            h = pRect.height();
            tmpRotatePara = QString("-vf rotate=2 -zoom -x %1 -y %2 -geometry +%3+%4").arg(w).arg(h).arg(x).arg(y);
            break;
        default:
            x = pRect.x();
            y = pRect.y();
            w = pRect.width();
            h = pRect.height();
            tmpRotatePara = QString("-zoom -x %1 -y %2 -geometry +%3+%4").arg(w).arg(h).arg(x).arg(y);
            break;
    }
    QStringList environment = QProcess::systemEnvironment();
    QString str;
    foreach(str,environment)
    {
        if (str.startsWith("QWS_DISPLAY="))
        {
            if(str.endsWith("Transformed:Rot180"))
            {
                x = tmpDesktopRect.width() - pRect.x() - pRect.width();
                y = tmpDesktopRect.height() - pRect.y() - pRect.height();
                w = pRect.width();
                h = pRect.height();
                tmpRotatePara = QString("-vf flip -zoom -x %1 -y %2 -geometry +%3+%4").arg(w).arg(h).arg(x).arg(y);
            }
        }
    }
    m_RectString.clear();
#ifdef WINDOWS
    m_RectString = QString("-quiet -framedrop %1").arg(tmpRotatePara);
#else
    m_RectString = QString("-slave -quiet -framedrop %1").arg(tmpRotatePara);
#endif

//#ifdef WINDOWS
//    m_RectString.clear();
//    m_RectString = QString("-quiet -wid %1").arg((unsigned long)pWidget->winId());
//#endif
//#ifdef UBUNTU
//    m_RectString.clear();
//    m_RectString = QString("-slave -quiet -wid %1").arg((unsigned long)pWidget->winId());
//#endif
    IDE_TRACE_STR(m_RectString);
}

void mplayer::ReleaseMediaBuffer()
{
    //m_MediaBuffer.clear();
}

void mplayer::slot_PlayNext()
{
    mAudioThread->run();
}

void mplayer::slot_Play(QString pSrc, QString pPara, bool pRepeat, bool pPrior)
{
    //IDE_TRACE_STR(pSrc);
    //IDE_TRACE_INT(m_PlayProcess->error());
    //IDE_TRACE_STR(m_PlayProcess->errorString());
    if(pPrior == true)
    {
        m_MediaBuffer.clear();
        HaveDing == false;
        HaveDoorOpen = true;
        m_PlayProcess->kill();
    }

    if(GlobalPara.DoorOpenEnable)
    {
        if(HaveDing && HaveDoorOpen && (pSrc.contains("DING") || pSrc.contains("215")))
        {
            IDE_TRACE();
            return;
        }
        else
        {
            if(HaveDing == false && pSrc.contains("DING"))
            {
                HaveDing = true;
            }
            if(HaveDoorOpen == false && pSrc.contains("215"))
            {
                HaveDoorOpen = true;
            }
        }
    }
    else
    {
        if(HaveDing)
        {
            if(HaveDing && pSrc.contains("DING"))
            {
                return;
            }
            else
            {
                if(pSrc.contains("DING"))
                {
                    HaveDing = true;
                }
            }
        }
    }

    if(isFileExist(pSrc))
    {
        //IDE_TRACE_INT(m_PlayProcess->state());
        if(m_PlayProcess->state() == QProcess::NotRunning)
        {
            //IDE_DEBUG(QString("play src : %1").arg(pSrc));
            m_MediaBuffer.append(pSrc);
            mAudioThread->run();
        }
        else
        {
            //IDE_DEBUG(QString("add src : %1").arg(pSrc));
            m_MediaBuffer.append(pSrc);
        }
    }
}

void mplayer::slot_VideoToPic(QString pSrc, QString pSavepath, QString pSavetype)
{
    QString tmpPath = D_MPLAYER_PATH;
    QString tmpString =
            QString("%1 %2 -vo %3:outdir=%4").
            arg(tmpPath).arg(pSrc).
            arg(pSavetype).arg(pSavepath);
    QDir tmpDir("./Res/temp");
    tmpDir.mkpath("./Res/temp");
    IDE_TRACE_STR(tmpDir.absolutePath());
    m_PlayProcess->start(tmpString);
}

void mplayer::slot_SetVolume(float pVolume)
{
    if(m_PlayProcess != 0)
    {
        QString tmpString = QString("volume %1 1\n").arg(pVolume);
        m_PlayProcess->write(tmpString.toUtf8());
    }
}

void mplayer::slot_Seek(qint64 pMs)
{
    if(m_PlayProcess != 0)
    {
        QString tmpString = QString("seek %1 2\n").arg(pMs);
        m_PlayProcess->write(tmpString.toUtf8());
    }
}

void mplayer::slot_Pause()
{
    if(m_PlayProcess != 0)
    {
        if(m_Pause)
            return;
        QString tmpString = QString("pause\n");
        m_PlayProcess->write(tmpString.toUtf8());
        m_Pause = true;
    }
}

void mplayer::slot_Resume()
{
    if(m_PlayProcess != 0)
    {
        if(!m_Pause)
            return;
        QString tmpString = QString("pause\n");
        m_PlayProcess->write(tmpString.toUtf8());
        m_Pause = false;
    }
}

void mplayer::slot_Stop()
{
    ReleaseProcess();
    m_Pause = false;
}

void mplayer::slot_Finished(int pExitCode, QProcess::ExitStatus pStatus)
{
//    IDE_TRACE_INT(pExitCode);
//    IDE_TRACE_INT(pStatus);

    if(m_MediaBuffer.count() != 0)
    {
        m_MediaBuffer.removeAt(0);
    }

    if(m_MediaBuffer.count() == 0)
    {
        if(GlobalPara.DoorOpenEnable)
        {
            HaveDing = false;
            HaveDoorOpen = false;
        }
        else
        {
            HaveDing = false;
        }
    }
    else
    {
        if(m_CurMedia.contains("DING"))
        {
            if(GlobalPara.DoorOpenEnable)
            {
                HaveDing = false;
                HaveDoorOpen = false;
            }
            else
            {
                HaveDing = false;
            }
        }
        slot_PlayNext();
    }
}

void mplayer::slot_Error(QProcess::ProcessError error)
{
    IDE_DEBUG(QString(">>>>>>>>>>> QProcess Error : Error Code : %1").arg(error));
    disconnect(m_PlayProcess, SIGNAL(finished(int, QProcess::ExitStatus)));
    disconnect(m_PlayProcess, SIGNAL(error(QProcess::ProcessError)));
    m_PlayProcess->kill();
    m_PlayProcess->deleteLater();
    QtSleep(1);
    m_PlayProcess = 0;
    CreatProcess();
    slot_PlayNext();
}
