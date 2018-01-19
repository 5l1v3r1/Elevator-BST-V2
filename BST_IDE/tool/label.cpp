#include "label.h"

Label::Label(QString text, QWidget *parent) :
    QLabel(text, parent)
{
    setMinimumHeight(25);
    setMaximumHeight(25);
    setStyleSheet(QString("background-color: rgba(0,0,0,0);color:rgba(%1);text-align:center;font-weight:bold;font-size:12px;").arg(D_TEXTCOLOR));
    setText(text);
    setFocusPolicy(Qt::NoFocus);
}
