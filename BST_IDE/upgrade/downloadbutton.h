#ifndef DOWNLOADBUTTON_H
#define DOWNLOADBUTTON_H

#include "global.h"

class DownLoadButton : public QLabel
{
    Q_OBJECT
public:
    enum STATE { OFF = 0, ON, BLINK};
    explicit DownLoadButton(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

signals:
    
public slots:
    void slot_SetState(STATE pState);
    void slot_Blink();
    void slot_StopBlink();
    
public:
    QTimer       m_BlinkTimer;
    STATE        m_State;
    QPixmap      m_OnPixmap, m_OffPixmap;
};

#endif // DOWNLOADBUTTON_H
