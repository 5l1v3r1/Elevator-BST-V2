#include "button.h"

Button::Button(QString pText, QWidget *parent) :
    QPushButton(parent)
{
    setMinimumHeight(25);
    setMaximumHeight(25);
    setCheckable(false);
    setText(pText);
    PaintBorder();
    m_NormalImage = QPixmap(":/SetBar/rc/SetBar/button-off");
    m_DownImage = QPixmap(":/SetBar/rc/SetBar/button-on");
}

void Button::mousePressEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

//>@主要用于去除鼠标移动选中文本的功能
void Button::mouseMoveEvent(QMouseEvent* e)
{

}

void Button::mouseDoubleClickEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

void Button::wheelEvent(QWheelEvent *e)
{
    WheelEvent(e);
}

void Button::keyPressEvent( QKeyEvent *e )
{
    KeyEvent(e);
}

void Button::focusInEvent(QFocusEvent *e)
{
    FocusInEvent();
}

void Button::focusOutEvent(QFocusEvent *e)
{
    FocusOutEvent();
}

void Button::ProcessMouseEvent(QMouseEvent* e)
{
    QPushButton::mousePressEvent(e);
}

void Button::ProcessKeyEvent(QKeyEvent *e)
{
    QPushButton::keyPressEvent(e);
}

void Button::ProcessWheelEvent(QWheelEvent* e)
{

}

bool Button::isFocus()
{
    return hasFocus();
}

void Button::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    if(isChecked())
        p.drawPixmap(rect(), m_DownImage);
    else
        p.drawPixmap(rect(), m_NormalImage);
}

void Button::PaintBorder()
{
    switch(m_LogicState)
    {
        case STATE_UNFOCUS:
        {
            break;
        }
        case STATE_FOCUS:
        {
            break;
        }
        case STATE_EDITABLE:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}

ButtonList::ButtonList(QStringList pParaList, QWidget *parent)
    : QHBoxLayout(parent)
{
    m_ButtonSigMapper = new QSignalMapper;
    m_ButtonList.clear();
    for(int i=0;i<pParaList.count();i++)
    {
        Button *tmpButton=new Button(pParaList.at(i));
        connect(tmpButton, SIGNAL(pressed()), m_ButtonSigMapper, SLOT(map()));
        m_ButtonSigMapper->setMapping(tmpButton, tmpButton);
        connect(m_ButtonSigMapper, SIGNAL(mapped(QObject*)), this, SIGNAL(sChanged(QObject*)));
        addWidget(tmpButton);
        m_ButtonList.append(tmpButton);
    }
}

ButtonList::~ButtonList()
{
    if(m_ButtonSigMapper)
        m_ButtonSigMapper->deleteLater();
    for(int i=m_ButtonList.count()-1;i>=0;i--)
    {
        Button *tmpButton = m_ButtonList.at(i);
        if(tmpButton)
            tmpButton->deleteLater();
    }
}

QStringList ButtonList::GetCheckedButton()
{
    QStringList tmpButtonList;
    for(int i=m_ButtonList.count()-1;i>=0;i--)
    {
        Button *tmpButton = m_ButtonList.at(i);
        if(tmpButton)
        {
            if(tmpButton->isChecked())
                tmpButtonList.append(tmpButton->text());
        }
    }
    return tmpButtonList;
}

void ButtonList::SetButtonCheckable(bool pEnable)
{
    for(int i=m_ButtonList.count()-1;i>=0;i--)
    {
        Button *tmpButton = m_ButtonList.at(i);
        if(tmpButton)
            tmpButton->setCheckable(pEnable);
    }
}
