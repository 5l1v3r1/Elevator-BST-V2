#include "combobox.h"

ComboBox::ComboBox(QStringList pParaList, QString pSelectPara, QWidget *parent) :
    QComboBox(parent), ToolBase()
{
    setMinimumHeight(25);
    setMaximumHeight(25);
    setMinimumWidth(70);
    PaintBorder();
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
    m_PressFlag = false;

    InitEnumList(pParaList, pSelectPara);
}

void ComboBox::InitEnumList(QStringList pList, QString pValue)
{
    clear();
    addItems(pList);
    setCurrentIndex(-1);
    if(pValue.isEmpty() == false)
    {
        for(int i=0;i<pList.count();i++)
        {
            if(pValue.compare(pList.at(i), Qt::CaseInsensitive) == 0)
            {
                setCurrentIndex(i);
                break;
            }
        }
    }
}

bool ComboBox::setCurrentText(QString pText)
{
    if(pText.isEmpty() == false)
    {
        for(int i=0;i<count();i++)
        {
            if(pText.compare(itemText(i), Qt::CaseInsensitive) == 0)
            {
                setCurrentIndex(i);
                return true;
            }
        }
    }
    return false;
}

void ComboBox::mousePressEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

void ComboBox::mouseReleaseEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

//>@主要用于去除鼠标移动选中文本的功能
void ComboBox::mouseMoveEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

void ComboBox::mouseDoubleClickEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

void ComboBox::wheelEvent(QWheelEvent *e)
{
    WheelEvent(e);
}

void ComboBox::contextMenuEvent(QContextMenuEvent * e)
{

}

void ComboBox::keyPressEvent( QKeyEvent *e )
{
    KeyEvent(e);
}

void ComboBox::focusInEvent(QFocusEvent *e)
{
    FocusInEvent();
}

void ComboBox::focusOutEvent(QFocusEvent *e)
{
    FocusOutEvent();
}

void ComboBox::ProcessMouseEvent(QMouseEvent* e)
{
    int index = currentIndex();
    if(e->type() == QEvent::MouseButtonPress ||
       e->type() == QEvent::MouseButtonDblClick)
    {
        //showPopup(); //>@showpopup是一种类阻塞的函数，也就是调用showPopup后必须通过鼠标点击或者Enter键使之hidepopup后才能再一次showPopup。
        ++index;
        if(index > count())
            index = 0;
        else if(index < 0)
            index = 0;
        setCurrentIndex(index);
        m_PressFlag = true;
    }
    //>@调用showPopup后combox会失去焦点，而menupopup得到焦点，这是，release按键信号在combox中不会获得。因此下面的QEvent::MouseButtonRelease信号不会获得。
    else if(e->type() == QEvent::MouseButtonRelease)
    {
        QComboBox::mouseReleaseEvent(e);
    }
    else if(e->type() == QEvent::MouseMove)
    {
        QComboBox::mouseMoveEvent(e);
    }
}

void ComboBox::ProcessKeyEvent(QKeyEvent *e)
{
    int index = currentIndex();
    if(e->key() == Qt::Key_Enter)
    {
        ++index;
        if(index > count())
            index = 0;
        else if(index < 0)
            index = 0;
        setCurrentIndex(index);
        m_PressFlag = true;
    }
    else if(e->key() == Qt::Key_Escape)
    {
        setFocus();
    }
    else if(e->key() == Qt::Key_Up)
    {
        if(++index > count())
            return;
        setCurrentIndex(index);
    }
    else if(e->key() == Qt::Key_Down)
    {
        if(--index < 0)
            return;
        setCurrentIndex(index);
    }
}

void ComboBox::ProcessWheelEvent(QWheelEvent *e)
{
    //>@滚轮一次滚动15度，一次得到delta为120，因此滚动的距离（也即滚轮滚动的单位数）为delta()/8/15
    int tmpDelta = e->delta();
    tmpDelta /= 8;
    tmpDelta /= 15;
    int index = currentIndex();
    if(tmpDelta > 0)
    {
        for(int i=0;i<tmpDelta;i++)
        {
            if(++index > count())
                break;
            setCurrentIndex(index);
        }
    }
    else if(tmpDelta < 0)
    {
        tmpDelta = -tmpDelta;
        for(int i=0;i<tmpDelta;i++)
        {
            if(--index < 0)
                break;
            setCurrentIndex(index);
        }
    }
}

bool ComboBox::isFocus()
{
    return hasFocus();
}

void ComboBox::PaintBorder()
{
    switch(m_LogicState)
    {
        case STATE_UNFOCUS: //>@设置背景透明
        {
            setStyleSheet("background-color: rgba(0,0,0,0);color:rgba(255,10,10,255);border: 1px groove blue;");
            break;
        }
        case STATE_FOCUS:
        {
            setStyleSheet("background-color: rgba(0,0,0,0);color:rgba(255,10,10,255);border: 2px groove magenta;");  //groove凹槽  //>@solid实心
            break;
        }
        case STATE_EDITABLE:
        {
            setStyleSheet(QString("background-color: rgba(%1);color:rgba(%2);border: 2px groove magenta;").arg(D_EDITCOLOR).arg(D_TEXTCOLOR));
            break;
        }
        default:
        {
            setStyleSheet("background-color: rgba(0,0,0,0);color:rgba(255,10,10,255);border: 1px groove blue;");
            break;
        }
    }
}

void ComboBox::currentIndexChanged(int index)
{
    if(m_PressFlag)
        m_LogicState = STATE_EDITABLE;
    PaintBorder();
    //>@当POPUP后并通过key或者mouse选择了一个选项后，此控件会认为已经失去焦点，这里需要让其继续拥有焦点
    setFocus();
    emit sChanged(this);
}
