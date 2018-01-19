#include "switch.h"

Switch::Switch(QString pSwitch, QString pOnText, QString pOffText, QWidget *parent) :
    QPushButton(parent), ToolBase()
{
    setMinimumHeight(25);
    setMaximumHeight(25);
    setMinimumWidth(60);
    setCheckable(true);

    if(!pSwitch.compare(pOnText, Qt::CaseInsensitive))
        setChecked(true);
    else
        setChecked(false);

    m_OnText = pOnText;
    m_OffText = pOffText;

    if(m_OnText.compare("TRUE", Qt::CaseInsensitive)==0)
        m_NormalImage = QPixmap(":/SetBar/rc/SetBar/switch-true");
    else if(m_OnText.compare("ON", Qt::CaseInsensitive)==0)
        m_CheckImage = QPixmap(":/SetBar/rc/SetBar/switch-on");

    if(m_OnText.compare("FALSE", Qt::CaseInsensitive)==0)
        m_NormalImage = QPixmap(":/SetBar/rc/SetBar/switch-false");
    else if(m_OnText.compare("OFF", Qt::CaseInsensitive)==0)
        m_CheckImage = QPixmap(":/SetBar/rc/SetBar/switch-off");

    connect(this, SIGNAL(toggled(bool)), this, SLOT(slot_Toggled(bool)));
    PaintBorder();
}

void Switch::slot_Toggled(bool flag)
{
    setChecked(flag);
    emit sChanged(this);
}

void Switch::mousePressEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

//>@主要用于去除鼠标移动选中文本的功能
void Switch::mouseMoveEvent(QMouseEvent* e)
{

}

void Switch::mouseDoubleClickEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

void Switch::wheelEvent(QWheelEvent *e)
{
    WheelEvent(e);
}

void Switch::keyPressEvent( QKeyEvent *e )
{
    KeyEvent(e);
}

void Switch::focusInEvent(QFocusEvent *e)
{
    FocusInEvent();
}

void Switch::focusOutEvent(QFocusEvent *e)
{
    FocusOutEvent();
}

void Switch::ProcessMouseEvent(QMouseEvent* e)
{
    QPushButton::mousePressEvent(e);
}

void Switch::ProcessKeyEvent(QKeyEvent *e)
{
    bool checkflag = isChecked();
    if(e->key() == Qt::Key_Up)
    {
        if(!checkflag)
            setChecked(true);
    }
    else if(e->key() == Qt::Key_Down)
    {
        if(checkflag)
            setChecked(false);
    }
}

void Switch::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    if(isChecked())
        p.drawPixmap(rect().adjusted(1, 1, -1, -1), m_CheckImage);
    else
        p.drawPixmap(rect().adjusted(1, 1, -1, -1), m_NormalImage);
    switch(m_LogicState)
    {
        case STATE_FOCUS:
        case STATE_EDITABLE:
        {
            p.setPen(QPen(QBrush(Qt::magenta), 4));
            p.drawRect(rect());
            break;
        }
        default:
        {
            p.setPen(QPen(QBrush(Qt::blue), 2));
            p.drawRect(rect());
            break;
        }
    }
}

void Switch::ProcessWheelEvent(QWheelEvent *e)
{
    //>@滚轮一次滚动15度，一次得到delta为120，因此滚动的距离（也即滚轮滚动的单位数）为delta()/8/15
    int tmpDelta = e->delta();
    tmpDelta /= 8;
    tmpDelta /= 15;
    bool checkflag = isChecked();
    if(tmpDelta > 0)
    {
        if(!checkflag)
            setChecked(true);
    }
    else if(tmpDelta < 0)
    {
        if(checkflag)
            setChecked(false);
    }
}

bool Switch::isFocus()
{
    return hasFocus();
}

void Switch::PaintBorder()
{
    update();
}


