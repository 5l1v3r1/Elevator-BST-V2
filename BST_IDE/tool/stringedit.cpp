/*
lineEdit的实际文本（text)是"192.168.0.1"，
而它显示和操作的文本(displayText)为"192.168.0  .1  "，
也就是说QLineEdit在显示文本的时候会根据InputMask对实际文本进行填充补位，造成displayText的长度大于text的长度，
而setSelection会用text的长度去检测start是否合法，
如果start的值大于或等于text的长度， 会提示Invalid start position，
同时， 它选中displayText中start指定文本， 这样displayText超出的部分永远都选中不了，
这应该是QT的一个BUG

具体做法：
重载mousePressEvent、mouseMoveEvent、mouseDoubleClickEvent、keyPressEvent、contextMenuEvent.
为什么要重载?:
    mousePressEvent:防止用户点击“提示符”时cursor会移动到“提示符”部分。
    mouseMoveEvent:防止用户用鼠标拖拽选择时选中“提示符”部分的文本。
    mouseDoubleClickEvent:防止用户双击文本输入框时选中输入框中显示的全部文本。
    contextMenuEvent:QLineEdit的右键菜单中有一项叫“Select All”，我们得废掉它。我干的比较绝，直接不让右键菜单出来~
    keyPressEvent:这个就比较多了。
        (1)首先是键盘的左方向键和Backspace有可能会让cursor移动到“提示符”部分。
        (2)然后是上面说到的那个“Select All”的问题，它有个对应的快捷键是Ctrl + A，我们也得修改下它的作用。
        (3)再就是Home键的功能，这个是很容易被忽略的。我们可以选择对Home键按下不做响应
*/

#include "stringedit.h"

ENetAddrEdit::ENetAddrEdit(QString pIP, QWidget *parent) :
    QLineEdit(parent), ToolBase()
{
    setText(pIP);
    //>@因为ToolBase()执行时，ENetAddrEdit还未初始化，因此ToolBase()中PaintBorder无效，如果去掉此处的PaintBorder，会造成文本框初始时未初始化。
    PaintBorder();
}

void ENetAddrEdit::mousePressEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

//>@主要用于去除鼠标移动选中文本的功能
void ENetAddrEdit::mouseMoveEvent(QMouseEvent* e)
{

}

void ENetAddrEdit::mouseDoubleClickEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

void ENetAddrEdit::wheelEvent(QWheelEvent *e)
{
    WheelEvent(e);
}

void ENetAddrEdit::keyPressEvent( QKeyEvent *e )
{
    KeyEvent(e);
}

void ENetAddrEdit::contextMenuEvent( QContextMenuEvent* e)
{
    //Do nothing
}

void ENetAddrEdit::focusInEvent(QFocusEvent *e)
{
    FocusInEvent();
    emit ShowKeyBoard(geometry().bottomLeft());
}

void ENetAddrEdit::focusOutEvent(QFocusEvent *e)
{
    FocusOutEvent();
    emit HideKeyBoard();
}

void ENetAddrEdit::ProcessMouseEvent(QMouseEvent* e)
{
    if(e->type() == QEvent::MouseButtonPress ||
       e->type() == QEvent::MouseButtonDblClick)
    {
        int index = cursorPositionAt(e->pos());
        UpdateSelection(index);
    }
}

void ENetAddrEdit::ProcessKeyEvent(QKeyEvent *e)
{
    int index = cursorPosition();
    if(e->key() == Qt::Key_Left)
    {
        UpdateSelection(index-4);
    }
    else if(e->key() == Qt::Key_Right)
    {
        UpdateSelection(index+4);
    }
    //>@处理改变选中数字的按键信号
    else
    {
        UpdateSelectionData((Qt::Key)(e->key()));
    }
    //>@然后忽视其它信号
    //>@处理全选信号
    //if(event->matches(QKeySequence::SelectAll) ||      //>@有些系统中会更换全选的快捷键，所以使用QKeySequence::SelectAll最可靠。
    //   (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_A))
    //{
    //    //Do nothing
    //}
}

void ENetAddrEdit::ProcessWheelEvent(QWheelEvent *e)
{
    //>@滚轮一次滚动15度，一次得到delta为120，因此滚动的距离（也即滚轮滚动的单位数）为delta()/8/15
    int tmpDelta = e->delta();
    tmpDelta /= 8;
    tmpDelta /= 15;
    if(tmpDelta > 0)
    {
        for(int i=0;i<tmpDelta;i++)
            UpdateSelectionData(Qt::Key_Up);
    }
    else if(tmpDelta < 0)
    {
        for(int i=0;i<qAbs(tmpDelta);i++)
            UpdateSelectionData(Qt::Key_Down);
    }
}

bool ENetAddrEdit::isFocus()
{
    return hasFocus();
}

void ENetAddrEdit::PaintBorder()
{
    switch(m_LogicState)
    {
        case STATE_UNFOCUS: //>@设置背景透明
        {
            UpdateSelection(-1);
            setStyleSheet("background-color: rgba(0,0,0,0);");
            break;
        }
        case STATE_FOCUS:
        {
            UpdateSelection(-1);
            setStyleSheet("background-color: rgba(0,0,0,0);border: 2px groove magenta;");
            break;
        }
        case STATE_EDITABLE:
        {
            UpdateSelection(1);
            setStyleSheet("background-color: rgba(240,240,240,255);border: 2px groove magenta;");
            break;
        }
        default:
        {
            UpdateSelection(-1);
            setStyleSheet("background-color: rgba(0,0,0,0);");
            break;
        }
    }
}

void ENetAddrEdit::UpdateSelection(int pos)
{
    if(pos <= 3)
    {
        setCursorPosition(0);   //>@此处可加可不加，使用此行代码会修改光标位置，并使之前选中的字符串变为不选中
        setSelection(0, 3);
    }
    else if(pos > 3 && pos <= 7)
    {
        setCursorPosition(4);
        setSelection(4, 3);
    }
    else if(pos > 7 && pos <= 11)
    {
        setCursorPosition(8);
        setSelection(8, 3);
    }
    else if(pos > 11)
    {
        setCursorPosition(12);
        setSelection(12, 3);
    }
}

//>@从右边插入/删除一个数字，或者通过上下键增加减少数字
void ENetAddrEdit::UpdateSelectionData(Qt::Key pKey)
{
    if(hasSelectedText())
    {
        int start = selectionStart();
        QString tmpData = selectedText();
        if(pKey == Qt::Key_Backspace)
        {
            tmpData.remove(2,1);
            tmpData.insert(0,'0');
        }
        else if(pKey == Qt::Key_Up)
        {
            int tmpInt = tmpData.toInt();
            if(tmpInt < 255)
            {
                tmpInt++;
                tmpData = QString("%1%2%3").arg(tmpInt/100)
                                           .arg(tmpInt/10%10)
                                           .arg(tmpInt%10);
            }
            else
                return;
        }
        else if(pKey == Qt::Key_Down)
        {
            int tmpInt = tmpData.toInt();
            if(tmpInt > 0)
            {
                tmpInt--;
                tmpData = QString("%1%2%3").arg(tmpInt/100)
                                           .arg(tmpInt/10%10)
                                           .arg(tmpInt%10);
            }
            else
                return;
        }
        else if(pKey >= Qt::Key_0 &&
                pKey <= Qt::Key_9)
        {
            tmpData.remove(0,1);
            tmpData.append((char)pKey);
        }
        else
        {
            return;
        }
        insert(tmpData);
        setSelection(start, 3);
        update();
    }
}
