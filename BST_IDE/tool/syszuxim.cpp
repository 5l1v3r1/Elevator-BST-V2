/***************************************************************************
**
**  <SYSZUXpinyin 1.0 , a chinese input method based on Qt for Embedded linux>
**  Copyright (C) <2010> <Gemfield> <gemfield@civilnet.cn>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License version 3 as published
**  by the Free Software Foundation.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**  If you have questions regarding the use of this file, please contact
**  Gemfield at gemfield@civilnet.cn or post your questions at
**  http://civilnet.cn/syszux/bbs
**
****************************************************************************/

#include "syszuxim.h"
#include "syszuxpinyin.h"
#include "QDebug"

SyszuxIM::SyszuxIM()
{
        syszuxpinyin = new SyszuxPinyin(this);
        connect(syszuxpinyin,SIGNAL(sendPinyin(QString)),this,SLOT(confirmString(QString)));
}
SyszuxIM::~SyszuxIM()
{
    syszuxpinyin->deleteLater();
}

void SyszuxIM::confirmString(QString gemfield)
{
     if(!gemfield.compare("Backspace"))
    {
        QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Backspace,Qt::NoModifier);
        QApplication::sendEvent(QApplication::focusWidget(),&upPress);
    }
    else if(!gemfield.compare("Tab"))
    {
        sendCommitString("      ");
    }
    else if(!gemfield.compare("Del"))
    {
        QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Delete,Qt::NoModifier);
        QApplication::sendEvent(QApplication::focusWidget(),&upPress);
    }
    else if(!gemfield.compare("Caps"))
    {
        if(syszuxpinyin->upper_letter == 0)
        {
            syszuxpinyin->upper_letter = 1;
        }
        else
        {
            syszuxpinyin->upper_letter = 0;
        }
        syszuxpinyin->changeLowerUpper();
    }
    else if(!gemfield.compare("Enter"))
    {
        QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Enter,Qt::NoModifier);
        QApplication::sendEvent(QApplication::focusWidget(),&upPress);
    }
    else if(!gemfield.compare("Shift"))
    {
        QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Shift,Qt::NoModifier);
        QApplication::sendEvent(QApplication::focusWidget(),&upPress);
    }
    else if(!gemfield.compare("Ctrl"))
    {
        QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Control,Qt::NoModifier);
        QApplication::sendEvent(QApplication::focusWidget(),&upPress);
    }
    else if(!gemfield.compare("Win"))
    {
        QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Meta,Qt::NoModifier);
        QApplication::sendEvent(QApplication::focusWidget(),&upPress);
    }
    else if(!gemfield.compare("Alt"))
    {
        QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Alt,Qt::NoModifier);
        QApplication::sendEvent(QApplication::focusWidget(),&upPress);
    }
    else if(!gemfield.compare("Up"))
    {
        QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Up,Qt::NoModifier);
        QApplication::sendEvent(QApplication::focusWidget(),&upPress);
    }
    else if(!gemfield.compare("Down"))
    {
        QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Down,Qt::NoModifier);
        QApplication::sendEvent(QApplication::focusWidget(),&upPress);
    }
    else if(!gemfield.compare("Left"))
    {
        QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Left,Qt::NoModifier);
        QApplication::sendEvent(QApplication::focusWidget(),&upPress);
    }
    else if(!gemfield.compare("Right"))
    {
        QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Right,Qt::NoModifier);
        QApplication::sendEvent(QApplication::focusWidget(),&upPress);
    }
     else if(!gemfield.compare("Hide"))
     {
         QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Tab,Qt::NoModifier);
         QApplication::sendEvent(QApplication::focusWidget(),&upPress);
     }
    else
    {
        sendCommitString(gemfield);
    }
}

void SyszuxIM::updateHandler(int type)
{
        switch(type)
        {
        case QWSInputMethod::FocusIn:
            syszuxpinyin->show();
            break;
        case QWSInputMethod::FocusOut:
            syszuxpinyin->hide();
            break;
        default:
            break;
        }
}

void SyszuxIM::SYSZUXsetposition(QPoint m_Position,QSize m_Size,QPoint m_Widget_Position,QSize m_Widget_Size)
{
    int position_x;
    int position_y;

    int syszuxpinyin_width = syszuxpinyin->width();
    int syszuxpinyin_height = syszuxpinyin->height();
    qDebug()<<"syszuxpinyin_width = " <<syszuxpinyin_width;
    qDebug()<<"syszuxpinyin_height = " <<syszuxpinyin_height;
    \
    int m_Position_x = m_Position.x();
    int m_Position_y = m_Position.y();
    int m_Size_Width = m_Size.width();
    int m_Size_height = m_Size.height();
    qDebug()<<"m_Position_x = " <<m_Position_x;
    qDebug()<<"m_Position_y = "<<m_Position_y;
    qDebug()<<"m_Size_Width = "<<m_Size_Width;
    qDebug()<<"m_Size_height = "<<m_Size_height;

    int m_Widget_Position_x = m_Widget_Position.x();
    int m_Widget_Position_y = m_Widget_Position.y();
    int m_Widget_Size_Width = m_Widget_Size.width();
    int m_Widget_Size_height = m_Widget_Size.height();
    qDebug()<<"m_Widget_Position_x = "<<m_Widget_Position_x;
    qDebug()<<"m_Widget_Position_y = "<<m_Widget_Position_y;
    qDebug()<<"m_Widget_Size_Width = "<<m_Widget_Size_Width;
    qDebug()<<"m_Widget_Size_height = "<<m_Widget_Size_height;

    if(m_Widget_Size_Width - m_Position_x >= syszuxpinyin_width)
    {
        position_x = m_Widget_Position_x + m_Position_x;
    }
    else
    {
        position_x = m_Widget_Position_x + m_Widget_Size_Width - syszuxpinyin_width;
    }

    if(m_Widget_Size_height -(m_Position_y + m_Size_height) >= syszuxpinyin_height)
    {
        position_y = m_Widget_Position_y + m_Position_y + m_Size_height +50;
    }
    else if(m_Position_y >= syszuxpinyin_height)
    {
        position_y = m_Widget_Position_y + m_Position_y - syszuxpinyin_height;
    }
    else
    {
        if(m_Widget_Size_Width - m_Position_x - m_Size_Width -50 >= syszuxpinyin_width)
            position_x = m_Widget_Position_x + m_Position_x +m_Size_Width +50;
        position_y = m_Widget_Position_y + m_Widget_Size_height - syszuxpinyin_height;
    }
    qDebug()<<"position_x = " <<position_x;
    qDebug()<<"position_y = " <<position_y;
    syszuxpinyin->setGeometry(position_x,position_y,syszuxpinyin_width,syszuxpinyin_height);
}

