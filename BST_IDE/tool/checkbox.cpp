#include "checkbox.h"

Checkbox::Checkbox(QWidget *parent) :
    QCheckBox(parent), ToolBase()
{
    setMinimumHeight(25);
    setMaximumHeight(25);
    PaintBorder();
}
void Checkbox::mousePressEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

//>@主要用于去除鼠标移动选中文本的功能
void Checkbox::mouseMoveEvent(QMouseEvent* e)
{

}

void Checkbox::mouseDoubleClickEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

void Checkbox::wheelEvent(QWheelEvent *e)
{
    WheelEvent(e);
}

void Checkbox::keyPressEvent( QKeyEvent *e )
{
    KeyEvent(e);
}

void Checkbox::focusInEvent(QFocusEvent *e)
{
    FocusInEvent();
}

void Checkbox::focusOutEvent(QFocusEvent *e)
{
    FocusOutEvent();
}

void Checkbox::ProcessMouseEvent(QMouseEvent* e)
{
    toggle();
}

void Checkbox::ProcessKeyEvent(QKeyEvent *e)
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
    else if(e->key() == Qt::Key_Enter)
    {
        toggle();
    }
}

void Checkbox::ProcessWheelEvent(QWheelEvent *e)
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

bool Checkbox::isFocus()
{
    return hasFocus();
}

void Checkbox::PaintBorder()
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

CheckList::CheckList(QStringList pParaList, QStringList pSelectList, QWidget *parent)
    : QHBoxLayout(parent)
{
    m_CheckboxList.clear();
    for(int i=0;i<pParaList.count();i++)
    {
        QString tmpText = pParaList.at(i);
        Checkbox *tmpCheckbox=new Checkbox();
        tmpCheckbox->setText(tmpText);
        tmpCheckbox->setChecked(false);
        connect(tmpCheckbox, SIGNAL(stateChanged(int)), this, SLOT(slot_StateChanged(int)));
        for(int j=0;j<pSelectList.count();j++)
        {
            QString tmpSelectText = pSelectList.at(j);
            if(tmpSelectText.compare(tmpText, Qt::CaseInsensitive) == 0)
            {
                tmpCheckbox->setChecked(true);
                break;
            }
        }
        addWidget(tmpCheckbox);
        m_CheckboxList.append(tmpCheckbox);
    }
    m_SelectList = pSelectList;
}

void CheckList::slot_StateChanged(int pValue)
{
    emit sChanged(this);
}

CheckList::~CheckList()
{
    for(int i=m_CheckboxList.count()-1;i>=0;i--)
    {
        Checkbox *tmpCheckbox = m_CheckboxList.at(i);
        if(tmpCheckbox)
            tmpCheckbox->deleteLater();
    }
}

bool CheckList::IsChanged()
{
    for(int i=m_CheckboxList.count()-1;i>=0;i--)
    {
        Checkbox *tmpCheckbox = m_CheckboxList.at(i);
        if(tmpCheckbox && tmpCheckbox->IsChanged())
            return true;
    }
    return false;
}

void CheckList::InitCheckList(QStringList pList)
{
    for(int i=0;i<m_CheckboxList.count();i++)
    {
        Checkbox *tmpCheckBox = m_CheckboxList.at(i);
        if(!tmpCheckBox)
            continue;
        QString tmpString = tmpCheckBox->text();
        bool searchFlag = false;
        for(int j=0;j<pList.count();j++)
        {
            QString tmpText = pList.at(j);
            if(tmpText.compare(tmpString, Qt::CaseInsensitive) == 0)
            {
                searchFlag = true;
                break;
            }
        }
        tmpCheckBox->setChecked(searchFlag);
    }
}

void CheckList::RestoreCheckList()
{
    InitCheckList(m_SelectList);
}

QStringList CheckList::GetSelectList()
{
    QStringList tmpList;
    if(m_CheckboxList.count() <= 0)
        return tmpList;
    for(int i=0;i<m_CheckboxList.count();i++)
    {
        Checkbox *tmpCheckbox = m_CheckboxList.at(i);
        if(tmpCheckbox->isChecked())
            tmpList.append(tmpCheckbox->text());
    }
    return tmpList;
}

QString CheckList::GetSelectText(QStringList pList)
{
    QString tmpString;
    if(pList.count()>0)
    {
        int i=0;
        for(;i<pList.count()-1;i++)
        {
            tmpString.append(pList.at(i));
            tmpString.append(';');
        }
        tmpString.append(pList.at(i));
    }
    return tmpString;
}

QString CheckList::GetSelectText()
{
    QString tmpString;
    QStringList tmpList = GetSelectList();
    if(tmpList.count()>0)
    {
        int i=0;
        for(;i<tmpList.count()-1;i++)
        {
            tmpString.append(tmpList.at(i));
            tmpString.append(';');
        }
        tmpString.append(tmpList.at(i));
    }
    return tmpString;
}
