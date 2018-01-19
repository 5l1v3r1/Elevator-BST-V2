#include "textedit.h"

TextEdit::TextEdit(QWidget *parent) :
    QLineEdit(parent), ToolBase()
{
    setMinimumHeight(25);
    setMaximumHeight(25);
    PaintBorder();
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(slot_TextChanged(QString)));
}

TextEdit::TextEdit(QString pText, QWidget *parent) :
    QLineEdit(parent), ToolBase()
{
    setMinimumHeight(25);
    setMaximumHeight(25);
    setText(pText);
    PaintBorder();
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(slot_TextChanged(QString)));
}

void TextEdit::slot_TextChanged(QString)
{
    emit sChanged(this);
}

void TextEdit::mousePressEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

void TextEdit::mouseMoveEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

void TextEdit::mouseDoubleClickEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

void TextEdit::wheelEvent(QWheelEvent *e)
{
    WheelEvent(e);
}

void TextEdit::keyPressEvent( QKeyEvent *e )
{
    KeyEvent(e);
}

void TextEdit::contextMenuEvent( QContextMenuEvent* e)
{
    //QLineEdit::contextMenuEvent(e);
}

void TextEdit::focusInEvent(QFocusEvent *e)
{
    FocusInEvent();
    emit ShowKeyBoard(geometry().bottomLeft());
}

void TextEdit::focusOutEvent(QFocusEvent *e)
{
    FocusOutEvent();
    emit HideKeyBoard();
}

void TextEdit::ProcessMouseEvent(QMouseEvent* e)
{
    if(e->type() == QEvent::MouseButtonPress)
    {
        QLineEdit::mousePressEvent(e);
    }
    else if(e->type() == QEvent::MouseButtonDblClick)
    {
        QLineEdit::mouseDoubleClickEvent(e);
    }
    else if(e->type() == QEvent::MouseButtonRelease)
    {
        QLineEdit::mouseReleaseEvent(e);
    }
    else if(e->type() == QEvent::MouseMove)
    {
        QLineEdit::mouseMoveEvent(e);
    }
}

void TextEdit::ProcessKeyEvent(QKeyEvent *e)
{
    int index = cursorPosition();
    if(e->key() == Qt::Key_Left)
    {
        UpdateSelection(index-2);
    }
    else if(e->key() == Qt::Key_Right)
    {
        UpdateSelection(index);
    }
    //>@处理改变选中数字的按键信号
    else if(e->key() == Qt::Key_Up ||
            e->key() == Qt::Key_Down)
    {
        UpdateSelectionData((Qt::Key)(e->key()));
    }
    else
    {
        QLineEdit::keyPressEvent(e);
    }
}

void TextEdit::ProcessWheelEvent(QWheelEvent *e)
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

bool TextEdit::isFocus()
{
    return hasFocus();
}

void TextEdit::PaintBorder()
{
    switch(m_LogicState)
    {
        case STATE_UNFOCUS: //>@设置背景透明
        {
            UpdateSelection(-1);
            setStyleSheet("background-color: rgba(0,0,0,0);color:rgba(255,10,10,255);");
            break;
        }
        case STATE_FOCUS:
        {
            UpdateSelection(-1);
            setStyleSheet("background-color: rgba(183,183,183,255);color:rgba(255,10,10,255);border: 2px groove magenta;");
            break;
        }
        case STATE_EDITABLE:
        {
            UpdateSelection(0);
            setStyleSheet(QString("background-color: rgba(%1);color:rgba(%2);border: 2px groove magenta;").arg(D_EDITCOLOR).arg(D_TEXTCOLOR));
            break;
        }
        default:
        {
            UpdateSelection(-1);
            setStyleSheet("background-color: rgba(0,0,0,0);color:rgba(255,10,10,255);");
            break;
        }
    }
}

void TextEdit::UpdateSelection(int pos)
{
    if(pos >= 0 && pos < text().count())
    {
        setSelection(pos, 1);
    }
    else if(pos < 0)
    {
        setCursorPosition(-1);
        deselect();
    }
    else if(pos >= text().count())
    {
        setCursorPosition(text().count());
        deselect();
    }
}

//>@如果为数字或者字母，则根据上下进行增减
void TextEdit::UpdateSelectionData(Qt::Key pKey)
{
    if(hasSelectedText() && selectedText().count() == 1)
    {
        QString tmpChange;
        int start = selectionStart();
        QString tmpData = selectedText();
        //>@判断是字母还是数字
        char tmp = tmpData.toAscii().at(0);
        IDE_TRACE_INT(tmp);
        if(tmp >= 0x30 && tmp <= 0x39)
        {
            if(pKey == Qt::Key_Up)
            {
                tmpChange = QString(QChar((tmp+1 > 0x39)?(0x39):(tmp+1)));
            }
            else if(pKey == Qt::Key_Down)
            {
                tmpChange = QString(QChar((tmp-1 < 0x30)?(0x30):(tmp-1)));
            }
        }
        else if(tmp >= 0x41 && tmp <= 0x5a)
        {
            if(pKey == Qt::Key_Up)
            {
                tmpChange = QString(QChar((tmp+1 > 0x5a)?(0x5a):(tmp+1)));
            }
            else if(pKey == Qt::Key_Down)
            {
                tmpChange = QString(QChar((tmp-1 < 0x41)?(0x41):(tmp-1)));
            }
        }
        else if(tmp >= 0x61 && tmp <= 0x7a)
        {
            if(pKey == Qt::Key_Up)
            {
                tmpChange = QString(QChar((tmp+1 > 0x7a)?(0x7a):(tmp+1)));
            }
            else if(pKey == Qt::Key_Down)
            {
                tmpChange = QString(QChar((tmp-1 < 0x61)?(0x61):(tmp-1)));
            }
        }
        else
        {
            return;
        }
        insert(tmpChange);
        UpdateSelection(start);
        update();
    }
}
