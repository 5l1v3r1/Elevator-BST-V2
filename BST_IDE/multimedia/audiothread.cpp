#include "audiothread.h"

AudioThread::AudioThread(mplayer *m_mplayer,QObject *parent) :
    QThread(parent)
{
    tmpMplayer = m_mplayer;
}

void AudioThread::run()
{
    if(tmpMplayer->m_MediaBuffer.isEmpty() == false)
    {
        tmpMplayer->m_CurMedia = tmpMplayer->m_MediaBuffer.at(0);
        GlobalPara.m_Logger->info(QString("Prepare To Play Audio Name: %1").arg(tmpMplayer->m_CurMedia));
        tmpMplayer->m_CurCmdString = QString("%1 -slave -quiet -novideo -volume 60 %2").arg(D_MPLAYER_PATH).arg(tmpMplayer->m_CurMedia);
        //if(tmpMplayer->m_PlayProcess->state() == QProcess::NotRunning)
        //{
            tmpMplayer->m_PlayProcess->start(tmpMplayer->m_CurCmdString);
            tmpMplayer->m_PlayProcess->waitForStarted(3000);
            GlobalPara.m_Logger->info(QString("Playing Audio Name: %1").arg(tmpMplayer->m_CurMedia));
//            IDE_TRACE_STR(tmpMplayer->m_CurCmdString);
        //}
    }
}
