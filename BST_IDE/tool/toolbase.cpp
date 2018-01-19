/*
    1、先执行focusin，再执行mousepressevent
    2、
*/

#include "toolbase.h"

ToolBase::ToolBase()
{
    m_IsChanged = false;
    //>@真实创建一个控件时都会在构造函数之前得到focus，但就会造成控件一初始化就获得焦点的bug，因此此处均硬设为初始化没有获得焦点。
    m_LogicState = STATE_UNFOCUS;
}

void ToolBase::FocusInEvent()
{
    if(m_LogicState == STATE_UNFOCUS)
    {
        m_LogicState = STATE_FOCUS;
    }
    PaintBorder();
}

void ToolBase::FocusOutEvent()
{
    if(m_LogicState != STATE_UNFOCUS)
    {
        m_LogicState = STATE_UNFOCUS;
    }
    PaintBorder();
}

void ToolBase::MouseEvent(QMouseEvent* e)
{
    QEvent::Type type = e->type();
    switch(m_LogicState)
    {
        //>@在没有得到键盘输入焦点情况下的处理方法
        case STATE_UNFOCUS:
        {
            if(type == QEvent::MouseButtonPress)       //>@进入焦点模式
            {
#ifdef IDE
                m_LogicState = STATE_EDITABLE;
#else
                m_LogicState = STATE_FOCUS;
#endif
                PaintBorder();
            }
            else if(type == QEvent::MouseButtonDblClick)       //>@直接进入可编辑模式
            {
                m_LogicState = STATE_EDITABLE;
                PaintBorder();
            }
            break;
        }
        //>@在得到键盘输入焦点情况下
        case STATE_FOCUS:
        {
            if(type == QEvent::MouseButtonDblClick)       //>@双击进入可编辑模式
            {
                m_LogicState = STATE_EDITABLE;
                PaintBorder();
            }
#ifdef IDE
            else if(type == QEvent::MouseButtonPress)       //>@进入焦点模式
            {
                m_LogicState = STATE_EDITABLE;
                PaintBorder();
            }
#endif
            break;
        }
        //>@在可编辑状态下的处理方法
        case STATE_EDITABLE:
        {
            ProcessMouseEvent(e);
            break;
        }
        default:
            break;
    }
}

//>@根据保存的m_LogicState状态机触发相关按键操作
void ToolBase::KeyEvent(QKeyEvent *e)
{
    switch(m_LogicState)
    {
        //>@在没有得到键盘输入焦点情况下的处理方法
        case STATE_UNFOCUS:
        {
            //>@do nothing
            break;
        }
        //>@在得到键盘输入焦点情况下，点击（Enter）可进入可编辑状态
        case STATE_FOCUS:
        {
            if(e->key() == Qt::Key_Enter)
            {
                m_LogicState = STATE_EDITABLE;
                PaintBorder();
            }
            break;
        }
        //>@在可编辑状态下的处理方法，点击（ESC）可退出可编辑状态，进入焦点状态
        case STATE_EDITABLE:
        {
            if(e->key() == Qt::Key_Escape)
            {
                m_LogicState = STATE_FOCUS;
                PaintBorder();
            }
            else
                ProcessKeyEvent(e);
            break;
        }
        default:
            break;
    }
    IDE_TRACE_INT(m_LogicState);
}

void ToolBase::WheelEvent(QWheelEvent* e)
{
    switch(m_LogicState)
    {
        //>@在没有得到键盘输入焦点情况下的处理方法
        case STATE_UNFOCUS:
        {
            //>@do nothing
            break;
        }
        //>@在得到键盘输入焦点情况下
        case STATE_FOCUS:
        {
            //>@do nothing
            break;
        }
        //>@在可编辑状态下的处理方法
        case STATE_EDITABLE:
        {
            ProcessWheelEvent(e);
            break;
        }
        default:
            break;
    }
}

void ToolBase::ProcessMouseEvent(QMouseEvent* e)
{

}

void ToolBase::ProcessKeyEvent(QKeyEvent *e)
{

}

void ToolBase::ProcessWheelEvent(QWheelEvent* e)
{

}

bool ToolBase::isFocus()
{
    return false;
}

//>@根据状态机绘制不同状态下的StyleSheet
void ToolBase::PaintBorder()
{

}

