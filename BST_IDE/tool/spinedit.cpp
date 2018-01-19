#include "spinedit.h"

SpinEdit::SpinEdit(int pValue, int mimValue, int maxValue, QWidget *parent) :
    QSpinBox(parent), ToolBase()
{
    setMinimumHeight(25);
    setMaximumHeight(25);
    setMinimumWidth(50);
    setRange(mimValue, maxValue);
    setValue(pValue);
    PaintBorder();
    mSpeedup = 0;

    connect(this, SIGNAL(valueChanged(int)), this, SLOT(slot_ValueChanged(int)));
}

void SpinEdit::slot_ValueChanged(int pValue)
{
    emit sChanged(this);
}

void SpinEdit::mousePressEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

//>@主要用于去除鼠标移动选中文本的功能
void SpinEdit::mouseMoveEvent(QMouseEvent* e)
{

}

void SpinEdit::mouseDoubleClickEvent(QMouseEvent* e)
{
    MouseEvent(e);
}

void SpinEdit::wheelEvent(QWheelEvent *e)
{
    WheelEvent(e);
}

void SpinEdit::keyPressEvent( QKeyEvent *e )
{
    KeyEvent(e);
    //如果输入框内的字符为0，则清空该输入框（防止用户输入多个0）
    if( "0" == text() )
        clear();
}

void SpinEdit::focusInEvent(QFocusEvent *e)
{
    FocusInEvent();
    //如果某个输入框得到了输入焦点，则将它们设为全选状态
    selectAll();
}

void SpinEdit::focusOutEvent(QFocusEvent *e)
{
    FocusOutEvent();
}

void SpinEdit::ProcessMouseEvent(QMouseEvent* e)
{
    selectAll();
}

void SpinEdit::ProcessKeyEvent(QKeyEvent *e)
{
    QTimer::singleShot(500, this, SLOT(slot_StopSpeedup()));
    selectAll();
    if(e->key() == Qt::Key_Up)
    {
        mSpeedup++;
        quint32 tmpValue = value();
        tmpValue +=mSpeedup;
        if(tmpValue>=maximum())
            tmpValue = maximum();
        setValue(tmpValue);
    }
    else if(e->key() == Qt::Key_Down)
    {
        mSpeedup++;
        quint32 tmpValue = value();
        tmpValue -=mSpeedup;
        if(tmpValue<=minimum())
            tmpValue = minimum();
        setValue(tmpValue);
    }
}

void SpinEdit::ProcessWheelEvent(QWheelEvent *e)
{
    //>@滚轮一次滚动15度，一次得到delta为120，因此滚动的距离（也即滚轮滚动的单位数）为delta()/8/15
    int tmpDelta = e->delta();
    tmpDelta /= 8;
    tmpDelta /= 15;
    selectAll();
    if(tmpDelta > 0)
    {
        quint32 tmpValue = value();
        tmpValue ++;
        if(tmpValue <= maximum())
            setValue(tmpValue);
    }
    else if(tmpDelta < 0)
    {
        quint32 tmpValue = value();
        tmpValue --;
        if(tmpValue >= minimum())
            setValue(tmpValue);
    }
}

bool SpinEdit::isFocus()
{
    return hasFocus();
}

void SpinEdit::slot_StopSpeedup()
{
    mSpeedup = 0;
}

void SpinEdit::PaintBorder()
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

ENetAddrEdit::ENetAddrEdit(ENET_ADDR pIP, QWidget *parent)
    : QHBoxLayout(parent)
{
    for(int i=0; i<4; i++)
    {
        m_IPSpinbox[i] = new SpinEdit(pIP.mAddr[i], 0, 255);
        m_IPSpinbox[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
        m_IPSpinbox[i]->setAlignment(Qt::AlignCenter);
        m_IPSpinbox[i]->setFixedWidth(35);
        connect(m_IPSpinbox[i], SIGNAL(sChanged(QObject*)), this, SIGNAL(sChanged(QObject*)));
    }
    //四个输入框之间的分隔符
    for(int i=0; i<3; i++)
    {
        m_DotLabel[i] = new QLabel(QString("."));
        m_DotLabel[i]->setFixedWidth(10);
        m_DotLabel[i]->setAlignment(Qt::AlignCenter);
    }
    setSpacing(0);
    for(int i=0; i<3; i++)
    {
        addWidget(m_IPSpinbox[i]);
        addWidget(m_DotLabel[i]);
    }
    addWidget(m_IPSpinbox[3]);
}

ENetAddrEdit::~ENetAddrEdit()
{
    for(int i=0; i<4; i++)
        m_IPSpinbox[i]->deleteLater();
    for(int i=0; i<3; i++)
        m_DotLabel[i]->deleteLater();
}

void ENetAddrEdit::SetIP(ENET_ADDR pIP)
{
    for(int i=0; i<4; i++)
        m_IPSpinbox[i]->setValue(pIP.mAddr[i]);
}

ENET_ADDR ENetAddrEdit::GetIP()
{
    ENET_ADDR tmpIP;
    for(int i=0; i<3; i++)
    {
        if(m_IPSpinbox[i]->text().isEmpty())
        {
            return ENET_ADDR();
        }
        tmpIP.mAddr[i] = m_IPSpinbox[i]->value();
    }
    return tmpIP;
}

QString ENetAddrEdit::GetIPString()
{
    QString ipAddress;
    for(int i=0; i<3; i++)
    {
        if(m_IPSpinbox[i]->text().isEmpty())
        {
            ipAddress.clear();
            return ipAddress;
        }
        ipAddress += m_IPSpinbox[i]->text() + QString(".");
    }
    ipAddress += m_IPSpinbox[3]->text();
    return ipAddress;
}

DateTimeEdit::DateTimeEdit(QDateTime pDateTime, QWidget *parent)
    : QHBoxLayout(parent)
{
    m_Spinbox[0] = new SpinEdit(pDateTime.date().year(), 2000, 2064);
    m_Label[0] = new QLabel(QString("-"));
    m_Spinbox[1] = new SpinEdit(pDateTime.date().month(), 0, 12);
    m_Label[1] = new QLabel(QString("-"));
    m_Spinbox[2] = new SpinEdit(pDateTime.date().day(), 0, pDateTime.date().daysInMonth());
    m_Label[2] = new QLabel(QString(" "));
    m_Spinbox[3] = new SpinEdit(pDateTime.time().hour(), 0, 24);
    m_Label[3] = new QLabel(QString(":"));
    m_Spinbox[4] = new SpinEdit(pDateTime.time().minute(), 0, 60);
    m_Label[4] = new QLabel(QString(":"));
    m_Spinbox[5] = new SpinEdit(pDateTime.time().second(), 0, 60);
    for(int i=0; i<6; i++)
    {
        m_Spinbox[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
        m_Spinbox[i]->setAlignment(Qt::AlignCenter);
        m_Spinbox[i]->setFixedWidth(40);
        connect(m_Spinbox[i], SIGNAL(sChanged(QObject*)), this, SIGNAL(sChanged(QObject*)));
        addWidget(m_Spinbox[i]);
        if( i != 5 )
        {
            m_Label[i]->setFixedWidth(10);
            m_Label[i]->setAlignment(Qt::AlignCenter);
            addWidget(m_Label[i]);
        }
    }
    setSpacing(0);
}

void DateTimeEdit::SetDateTime(QDateTime pDateTime)
{
    m_Spinbox[0]->setValue(pDateTime.date().year());
    m_Spinbox[1]->setValue(pDateTime.date().month());
    m_Spinbox[2]->setValue(pDateTime.date().day());
    m_Spinbox[3]->setValue(pDateTime.time().hour());
    m_Spinbox[4]->setValue(pDateTime.time().minute());
    m_Spinbox[5]->setValue(pDateTime.time().second());
}

DateTimeEdit::~DateTimeEdit()
{
    for(int i=0; i<6; i++)
        m_Spinbox[i]->deleteLater();
    for(int i=0; i<5; i++)
        m_Label[i]->deleteLater();
}

QDateTime DateTimeEdit::GetDateTime()
{
    int time[6];
    for(int i=0; i<6; i++)
    {
        if(m_Spinbox[i]->text().isEmpty())
        {
            return QDateTime();
        }
        time[i] = m_Spinbox[i]->value();
    }
    return QDateTime(QDate(time[0], time[1], time[2]),
                     QTime(time[3], time[4], time[5]));
}
