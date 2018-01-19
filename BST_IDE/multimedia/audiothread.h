#ifndef AIDEOTHREAD_H
#define AIDEOTHREAD_H

#include <QThread>
#include "mplayer.h"

class mplayer;

class AudioThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioThread(mplayer *m_mplayer,QObject *parent = 0);

public:
    void run();

    mplayer *tmpMplayer;
signals:

public slots:

};

#endif // AIDEOTHREAD_H
