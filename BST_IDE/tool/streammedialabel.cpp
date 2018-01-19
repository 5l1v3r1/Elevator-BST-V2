#include "streammedialabel.h"

StreamMediaLabel::StreamMediaLabel(QWidget *parent) :
    QLabel(parent)
{
    m_Process = 0;
    m_Process = new QProcess;
    connect(m_Process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(Restart(int, QProcess::ExitStatus)));
    setPixmap(QPixmap(":/Page/rc/Page/VideoScreen.png"));
    setScaledContents(true);
    m_CrashCount = 0;
    m_Repeat = true;
    createMenus();
}

void StreamMediaLabel::SetUrl(QString pUrl, bool pPrior)
{
    if(m_Process && m_Process->state() != QProcess::NotRunning)
    {
        if(pPrior)
        {
            disconnect(m_Process, SIGNAL(finished(int)),0,0);
            m_Process->kill();
            static quint8 tmpState;
            while((tmpState = m_Process->state()))
                QtSleep(20);
            m_Process->waitForFinished(D_WAITTIME);
        }
        else
            return;
    }
    FILEPATH_TYPE tmpFileType = getRcFileType(pUrl);
    if(tmpFileType == PATH_TYPENETADDR)
    {
        //>@播放当前流媒体视频
        unsigned long id = (unsigned long)winId();
        m_Cmd = QString("%1 %2 -nocache -slave -wid %3 &").arg(D_MPLAYER_PATH).arg(pUrl).arg(id);
        m_Process->start(m_Cmd);
    }
    else
    {
        RC_TYPE tmpRcType = getRcType(pUrl);
        if(tmpRcType >= RC_MP4)
        {
            unsigned long id = (unsigned long)winId();
            m_Cmd = QString("%1 %2 -framedrop -wid %3 &").arg(D_MPLAYER_PATH).arg(pUrl).arg(id);
            m_Process->start(m_Cmd);
        }
    }
    m_Url = pUrl;
    setWindowTitle(pUrl);
}

void StreamMediaLabel::Restart(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::CrashExit)
    {
        m_CrashCount ++;
        if(m_CrashCount <= 5)   //>@如果出现错误，则重复开启5次，如果还是失败则退出。
            m_Process->start(m_Cmd);
        return;
    }
    else
    {
        m_CrashCount = 0;
    }
    if(m_Repeat)
        m_Process->start(m_Cmd);
}

void StreamMediaLabel::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::RightButton)
    {
        if(m_Menu)
        {
            m_Menu->exec(QCursor::pos());//在当前鼠标位置显示
        }
    }
}

void StreamMediaLabel::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        SetUrl(fileName, true);
    }
}

void StreamMediaLabel::save()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (!fileName.isEmpty())
    {

    }
}
