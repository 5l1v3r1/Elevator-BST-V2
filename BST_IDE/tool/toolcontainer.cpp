#include "toolcontainer.h"

ToolContainer::ToolContainer(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint)
{
    setWindowFlags(Qt::Window);
}

void ToolContainer::keyPressEvent(QKeyEvent *event)
{
   switch(event->key())
   {
   case Qt::Key_Up:
       focusNextChild();
       break;
   case Qt::Key_Down:
       focusPreviousChild();
       break;
   default:
       QWidget::keyPressEvent(event);
   }
}

bool ToolContainer::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() >= QEvent::MouseButtonPress &&
        event->type() <= QEvent::FocusOut)
    {
        return QWidget::eventFilter(target, event);
    }
}
