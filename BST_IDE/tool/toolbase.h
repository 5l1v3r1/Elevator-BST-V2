#ifndef TOOLBASE_H
#define TOOLBASE_H

#include "globalfun.h"
#include <QRegExpValidator>
#include <QKeyEvent>

#define D_BGCOLOR           QString("241,241,241,20")
#define D_TEXTCOLOR         QString("83,83,83,255")
#define D_FRAMECOLOR        QString("83,83,83,255")
#define D_EDITCOLOR         QString("230,230,230,255")

#define D_BUTTONTIP         QString("Button")
#define D_CHECKBOXTIP       QString("CheckBox")
#define D_COMBOXTIP         QString("Combox")
#define D_ENETTIP           QString("EnetAddr")
#define D_SWITCHTIP         QString("Switch")
#define D_TEXTEDITTIP       QString("TextEdit")

typedef enum{
    STATE_UNFOCUS=0,
    STATE_FOCUS=1,
    STATE_EDITABLE=2
} LOGICSTATE;

class ToolBase
{
public:
    ToolBase();
    void KeyEvent(QKeyEvent *e);        //>@键盘处理函数
    void MouseEvent(QMouseEvent* e);    //>@鼠标处理函数
    void WheelEvent(QWheelEvent* e);    //>@滚轮处理函数
    void FocusInEvent();
    void FocusOutEvent();
    virtual void ProcessKeyEvent(QKeyEvent *e);
    virtual void ProcessMouseEvent(QMouseEvent* e);
    virtual void ProcessWheelEvent(QWheelEvent* e);
    virtual bool isFocus();
    virtual void PaintBorder();
    bool IsChanged() { return m_IsChanged; }
public:
    LOGICSTATE m_LogicState;
    bool       m_IsChanged;
};

#endif // TOOLBASE_H
