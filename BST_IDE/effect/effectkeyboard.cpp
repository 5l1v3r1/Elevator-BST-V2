#include "effectkeyboard.h"

QString gKeyboardLetter[2][KEYCOUNT]={
{
"Esc",   "`",   "1",    "2",  "3",  "4",   "5",    "6",    "7",    "8",    "9",  "0",    "-",    "=",    "Back",  //>@15
"Tab",   "q",   "w",    "e",  "r",  "t",   "y",    "u",    "i",    "o",    "p",  "[",    "]",    "\\",    "Del",       //>@15
"Caps",  "a",   "s",    "d",  "f",  "g",   "h",    "j",    "k",    "l",    ";",  "\'",   "Enter",                      //>@13
"Shift", "z",   "x",    "c",  "v",  "b",   "n",    "m",    ",",    ".",    "/",  "Up",   "Home", "End",                //>@14
"Ctrl",  "Win", "Alt",  " ",  "EN", "Left","Down", "Right","Hide"                                                      //>@9
},
{
"ESC",   "`",   "!",   "@",  "#",    "$",    "%",    "^",    "&",    "*",     "(",   ")",    "_",    "+",  "BACK",
"TAB",   "Q",   "W",   "E",  "R",    "T",    "Y",    "U",    "I",     "O",     "P",   "{",   "}",    "|",  "DEL",
"CAPS",  "A",   "S",   "D",  "F",    "G",    "H",    "J",    "K",     "L",     ":",   "\"",  "ENTER",
"SHIFT", "Z",   "X",   "C",  "V",    "B",    "N",    "M",    "<",     ">",     "?",   "UP",  "HOME", "END",
"CTRL",  "WIN", "ALT", " ",  "EN"    "LEFT", "DWON", "RIGHT","HIDE"
}
};

EffectKeyboard::EffectKeyboard(QWidget *parent, QRect pRect) :
    EffectBase(parent, pRect)
{
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setupUi(this);
}

bool EffectKeyboard::InitEffectRc(STATE_INFO* pEffect)
{
    QString tmpString = pEffect->StateName;
    if(tmpString.isEmpty())
    {
        return false;
    }
    if(tmpString.compare(QString("Common"), Qt::CaseInsensitive) == 0)
    {
        RESOURCE_INFO tmpRc;
        char index;
        int count = pEffect->Effect.RcFiles.count();
        for(int i=0;i<count;i++)
        {
            tmpRc = pEffect->Effect.RcFiles.at(i);
            index = GetKey(tmpRc.RcName);
            IDE_TRACE_INT(index);
            if(LoadPixmap(index, tmpRc.RcFile) == false)
            {
                IDE_TRACE_STR(tmpRc.RcFile);
            }
        }
    }
    else if(tmpString.compare(QString("Press"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Transparent"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeTransparent;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_KEYPRESS, tmpAreaAnimate);
    }
    else if(tmpString.compare(QString("Release"), Qt::CaseInsensitive) == 0)
    {
        AREA_ANIMATE *tmpAreaAnimate = new  AREA_ANIMATE;
        if(pEffect->Effect.EffectType.compare(QString("Transparent"), Qt::CaseInsensitive) == 0)
        {
            tmpAreaAnimate->mEffectType = EffectTypeTransparent;
        }
        else
        {
            delete tmpAreaAnimate;
            return false;
        }
        analysisEffectPara(tmpAreaAnimate, pEffect->Effect.EffectPara);
        m_EffectGroup.insert(OPERATE_KEYRELEASE, tmpAreaAnimate);
    }
    else
    {
        return false;
    }
    return true;
}

bool EffectKeyboard::analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent)
{
    return EffectBase::analysisEffectParaItem(pAreaEffect, pLabel, pContent);
}

bool EffectKeyboard::Start()
{
    EffectBase::Start();
    m_KeyButtonGroup = new QButtonGroup(this);
    QPixmap tmpNormalPixmap, tmpHightlightPixmap;
    tmpNormalPixmap = m_EffectImage.value(0);
    tmpHightlightPixmap = m_EffectImage.value(1);
    //>@³õÊ¼»¯°´¼ü
    KeyButton *tmpKeyButton = qobject_cast<KeyButton *>(Esc);
    for(int i=1;i<=KEYCOUNT;i++)
    {
        if(tmpKeyButton == 0)
        {
            IDE_TRACE();
            break;
        }
        tmpKeyButton->InitRc(tmpNormalPixmap, tmpHightlightPixmap);
        tmpKeyButton->InitText(gKeyboardLetter[LOWER][i-1]);
        m_KeyButtonGroup->addButton(tmpKeyButton, i-1);
        tmpKeyButton = qobject_cast<KeyButton *>(tmpKeyButton->nextInFocusChain());
    }
    connect(m_KeyButtonGroup, SIGNAL(buttonClicked(int)), SLOT(buttonClickResponse(int)));
    hide();
    return true;
}

bool EffectKeyboard::PaintEffect(QPainter &p)
{
    QPixmap tmpPixmap = m_EffectImage.value(BGIMG);
    if(tmpPixmap.isNull() == false)
        p.drawPixmap(rect(), tmpPixmap);
    return true;
}

LANGUAGE EffectKeyboard::CurLanguage()
{
    KeyButton *tmpKeyButton = qobject_cast<KeyButton *>(m_KeyButtonGroup->button(KEYEN));
    if(tmpKeyButton == 0)
        return EN;
    QString tmpString = tmpKeyButton->text();
    if(tmpString.compare("EN", Qt::CaseInsensitive) == 0)
        return EN;
    else if(tmpString.compare("CH", Qt::CaseInsensitive) == 0)
        return CH;
    return EN;
}

bool EffectKeyboard::SwitchLanguage()
{
    KeyButton *tmpKeyButton = qobject_cast<KeyButton *>(m_KeyButtonGroup->button(KEYEN));
    if(tmpKeyButton == 0)
        return false;
    LANGUAGE tmpLanguage = CurLanguage();
    switch(tmpLanguage)
    {
        case EN:
            tmpKeyButton->setText("CH");
            emit InputMethodChanged(CH);
            break;
        case CH:
            tmpKeyButton->setText("EN");
            emit InputMethodChanged(EN);
            break;
        default:
            return false;
    }
    return true;
}

TEXTCASE EffectKeyboard::CurTextcase()
{
    KeyButton *tmpKeyButton = qobject_cast<KeyButton *>(m_KeyButtonGroup->button(KEYCAPS));
    if(tmpKeyButton == 0)
        return LOWER;
    QString tmpString = tmpKeyButton->text();
    if(tmpString.compare(gKeyboardLetter[LOWER][KEYCAPS]) == 0)
        return UPPER;
    else if(tmpString.compare(gKeyboardLetter[UPPER][KEYCAPS]) == 0)
        return LOWER;
    return LOWER;
}

bool EffectKeyboard::SwitchTextcase()
{
    TEXTCASE tmpTextcase = CurTextcase();
    KeyButton *tmpKeyButton = qobject_cast<KeyButton *>(Esc);;
    for(int i=1;i<=KEYCOUNT;i++)
    {
        if(tmpKeyButton == 0)
            break;
        tmpKeyButton->InitText(gKeyboardLetter[tmpTextcase][i-1]);
        tmpKeyButton = qobject_cast<KeyButton *>(tmpKeyButton->nextInFocusChain());
    }
}

void EffectKeyboard::updateEffect(AREA_OPERATE pOperate, QVariant pPara)
{
    static AREA_OPERATE sOPerate = OPERATE_NONE;
    static QVariant sPara = QVariant();
    if(sOPerate != pOperate && pOperate != OPERATE_PLAY && pOperate != OPERATE_STOP)
    {
        sOPerate = pOperate;
        sPara = pPara;
    }
    switch(pOperate)
    {
        case OPERATE_PLAY:
        {
            updateEffect(sOPerate, sPara);
            break;
        }
        case OPERATE_STOP:
        {
            m_Animator.stop();
            break;
        }
        default:
            break;
    }
}

void EffectKeyboard::buttonClickResponse(int gemfield)
{
    TEXTCASE tmpTextcase = CurTextcase();
    QString tmpLetter = gKeyboardLetter[tmpTextcase][gemfield];
    switch(gemfield)
    {
    case 0: //Esc
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Escape,Qt::NoModifier,tmpLetter));
        break;
    case 1: //`
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_At,Qt::NoModifier,tmpLetter));
        break;
    case 2:  //>@1
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress, (!tmpTextcase)?(Qt::Key_1):(Qt::Key_Exclam), Qt::NoModifier, tmpLetter));
        break;
    case 3:
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress, Qt::Key_2, Qt::NoModifier, tmpLetter));
        break;
    case 4:
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress, Qt::Key_3, Qt::NoModifier, tmpLetter));
        break;
    case 5:
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress, Qt::Key_4, Qt::NoModifier, tmpLetter));
        break;
    case 6:
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress, Qt::Key_5, Qt::NoModifier, tmpLetter));
        break;
    case 7:
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress, Qt::Key_6, Qt::NoModifier, tmpLetter));
        break;
    case 8:
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress, Qt::Key_7, Qt::NoModifier, tmpLetter));
        break;
    case 9:
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress, Qt::Key_8, Qt::NoModifier, tmpLetter));
        break;
    case 10:  //>@9
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress, Qt::Key_9, Qt::NoModifier, tmpLetter));
        break;
    case 11:  //>@0
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress, Qt::Key_0, Qt::NoModifier, tmpLetter));
        break;
    case 12://-
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Minus,Qt::NoModifier,tmpLetter));
        break;
    case 13://=
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Equal,Qt::NoModifier,tmpLetter));
        break;
    case 14://Backspace
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Backspace,Qt::NoModifier));
        break;
    case 15://Tab
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Tab,Qt::NoModifier));
        break;
    case 16://q
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Q,Qt::NoModifier,tmpLetter));
        break;
    case 17://w
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_W,Qt::NoModifier,tmpLetter));
        break;
    case 18://e
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_E,Qt::NoModifier,tmpLetter));
        break;
    case 19://r
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_R,Qt::NoModifier,tmpLetter));
        break;
    case 20://t
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_T,Qt::NoModifier,tmpLetter));
        break;
    case 21://y
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Y,Qt::NoModifier,tmpLetter));
        break;
    case 22://u
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_U,Qt::NoModifier,tmpLetter));
        break;
    case 23://i
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_I,Qt::NoModifier,tmpLetter));
        break;
    case 24://o
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_O,Qt::NoModifier,tmpLetter));
        break;
    case 25://p
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_P,Qt::NoModifier,tmpLetter));
        break;
    case 26://  [   {
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_BracketLeft,Qt::NoModifier,tmpLetter));
        break;
    case 27://  ]    }
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_BracketRight,Qt::NoModifier,tmpLetter));
        break;
    case 28:// \  |
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Hyper_L,Qt::NoModifier,tmpLetter));
        break;
    case 29://Del
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Delete,Qt::NoModifier));
        break;
    case KEYCAPS:  //Caps
        SwitchTextcase();
        break;
    case 31://a
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_A,Qt::NoModifier,tmpLetter));
        break;
    case 32://s
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_S,Qt::NoModifier,tmpLetter));
        break;
    case 33://d
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_D,Qt::NoModifier,tmpLetter));
        break;
    case 34://f
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_F,Qt::NoModifier,tmpLetter));
        break;
    case 35://g
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_G,Qt::NoModifier,tmpLetter));
        break;
    case 36://h
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_H,Qt::NoModifier,tmpLetter));
        break;
    case 37://j
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_J,Qt::NoModifier,tmpLetter));
        break;
    case 38://k
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_K,Qt::NoModifier,tmpLetter));
        break;
    case 39://l
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_L,Qt::NoModifier,tmpLetter));
        break;
    case 40:// ";"
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Semicolon,Qt::NoModifier,tmpLetter));
        break;
    case 41://"\'"
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_QuoteLeft,Qt::NoModifier,tmpLetter));
        break;
    case 42: //Enter
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Enter,Qt::NoModifier));
        break;
    case 43://Shift
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Shift,Qt::NoModifier));
        break;
    case 44://z
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Z,Qt::NoModifier,tmpLetter));
        break;
    case 45://x
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_X,Qt::NoModifier,tmpLetter));
        break;
    case 46://c
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_C,Qt::NoModifier,tmpLetter));
        break;
    case 47://v
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_V,Qt::NoModifier,tmpLetter));
        break;
    case 48://b
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_B,Qt::NoModifier,tmpLetter));
        break;
    case 49://n
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_N,Qt::NoModifier,tmpLetter));
        break;
    case 50://m
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_M,Qt::NoModifier,tmpLetter));
        break;
    case 51://  ,
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Comma,Qt::NoModifier,tmpLetter));
        break;
    case 52://  .
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Period,Qt::NoModifier,tmpLetter));
        break;
    case 53://  /
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Hyper_R,Qt::NoModifier,tmpLetter));
        break;
    case 54://UP
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Up,Qt::NoModifier));
        break;
    case 55://Home
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Home,Qt::NoModifier));
        break;
    case 56://End
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_End,Qt::NoModifier));
        break;
    case 57://Ctrl
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Control,Qt::NoModifier));
        break;
    case 58://Win
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Meta,Qt::NoModifier));
        break;
    case 59://Alt
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Alt,Qt::NoModifier));
        break;
    case 60://Space
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Space,Qt::NoModifier));
        break;
    case KEYEN://EN
        SwitchLanguage();
        break;
    case 62://Left
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Left,Qt::NoModifier));
        break;
    case 63://Down
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Down,Qt::NoModifier));
        break;
    case 64://Right
        QApplication::sendEvent(QApplication::focusWidget(),
                                new QKeyEvent(QEvent::KeyPress,Qt::Key_Right,Qt::NoModifier));
        break;
    case 65: //Hide
        hide();
        break;
    default:
        break;
    }
 }
