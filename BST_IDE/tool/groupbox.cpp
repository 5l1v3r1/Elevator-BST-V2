#include "groupbox.h"

Groupbox::Groupbox(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint)
{
    m_VLayout = new QVBoxLayout(this);
    m_VLayout->setSpacing(4);
    m_VLayout->setContentsMargins(20, 25, 20, 20);
    setFocusPolicy(Qt::NoFocus);
}

Groupbox::Groupbox(QString pText, QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint)
{
    m_VLayout = new QVBoxLayout(this);
    m_VLayout->setSpacing(4);
    m_VLayout->setContentsMargins(20, 25, 20, 20);
    if(!pText.isEmpty())
    {
        Label *tmpTitle = new Label(pText+tr(" Settings"));
#ifdef IDE
        tmpTitle->setStyleSheet(QString("background-color:rgba(%1);color:rgba(%2);font-weight:bold;font-size:16px;").arg(D_BGCOLOR).arg(D_TEXTCOLOR));
#elif DISPLAY
        tmpTitle->setStyleSheet("background-color:rgba(200,200,200,20);color:rgba(255,10,10,255);font-weight:bold;font-size:16px;");
#endif
        tmpTitle->setAlignment(Qt::AlignHCenter);
        m_VLayout->addWidget(tmpTitle);
    }
    setFocusPolicy(Qt::NoFocus);
}

void Groupbox::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
#ifdef IDE
    QLinearGradient pgBaseGradient(QPointF(0,0), QPointF(0,height()));
    pgBaseGradient.setColorAt(0, QColor(213,212,210,255));
    pgBaseGradient.setColorAt(0.5, QColor(194, 194, 194, 255));
    pgBaseGradient.setColorAt(1, QColor(177,177,177,255));//设置渐变的颜色和路径比例
    p.setBrush(QBrush(pgBaseGradient));
#elif DISPLAY
    p.setBrush(QBrush(QColor(40,41,44,255)));
#endif
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect(), 20, 20, Qt::AbsoluteSize);
}
