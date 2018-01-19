#include "downloadbutton.h"

DownLoadButton::DownLoadButton(QWidget *parent) :
    QLabel(parent)
{
    setScaledContents(true);
    m_BlinkTimer.setInterval(1000);
    connect(&m_BlinkTimer, SIGNAL(timeout()), this, SLOT(slot_Blink()));
    m_OnPixmap = QPixmap(":/Item/rc/Item/btnon.png");
    m_OffPixmap = QPixmap(":/Item/rc/Item/btnoff.png");
    slot_SetState(OFF);
}

void DownLoadButton::slot_SetState(STATE pState)
{
    switch(pState)
    {
        case OFF:
        {
            m_State = OFF;
            setPixmap(m_OffPixmap);
            break;
        }
        case ON:
        {
            m_State = ON;
            setPixmap(m_OnPixmap);
            break;
        }
        case BLINK:
        {
            m_BlinkTimer.start();
            break;
        }
        default:
            break;
    }
}

void DownLoadButton::slot_StopBlink()
{
    if(m_BlinkTimer.isActive())
        m_BlinkTimer.stop();
}

void DownLoadButton::slot_Blink()
{
    if(m_State == ON)
        slot_SetState(OFF);
    else if(m_State == OFF)
        slot_SetState(ON);

}

void DownLoadButton::mousePressEvent(QMouseEvent *ev)
{
    slot_SetState(ON);
}

void DownLoadButton::mouseReleaseEvent(QMouseEvent *ev)
{
    slot_SetState(OFF);
}
