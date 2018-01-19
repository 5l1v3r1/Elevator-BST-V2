#ifndef STREAMMEDIALABEL_H
#define STREAMMEDIALABEL_H

#include "global.h"
#include <QLabel>
#include <QProcess>

class StreamMediaLabel : public QLabel
{
    Q_OBJECT
public:
    explicit StreamMediaLabel(QWidget *parent = 0);
    ~StreamMediaLabel()
    {
        if(m_Process)
        {
            disconnect(m_Process, SIGNAL(finished(int)), 0, 0);
            m_Process->deleteLater();
        }
    }

    void createMenus()
    {
        m_ChangeUrlAction = new QAction(tr("Change &Url..."), this);
        connect(m_ChangeUrlAction, SIGNAL(triggered()), this, SLOT(open()));
        m_SaveAction = new QAction(tr("&Save..."), this);
        connect(m_SaveAction, SIGNAL(triggered()), this, SLOT(save()));

        m_Menu = new QMenu(tr("&Menu"), this);
        m_Menu->addAction(m_ChangeUrlAction);
        m_Menu->addSeparator();
        m_Menu->addAction(m_SaveAction);
    }

    void SetRepeat(bool pFlag)
    {
        m_Repeat = pFlag;
    }

    void SetUrl(QString pUrl, bool pPrior);

    void mousePressEvent(QMouseEvent *ev);

public slots:
    void open();
    void save();
    void Restart(int exitCode, QProcess::ExitStatus exitStatus);

public:
    QProcess *m_Process;
    QString   m_Url;
    QString   m_Cmd;
    bool      m_Repeat;
    quint32   m_CrashCount;

    QMenu    *m_Menu;
    QAction  *m_ChangeUrlAction, *m_SaveAction;
};

#endif // STREAMMEDIALABEL_H
