#include <QtGui>
#include "syszuxpinyin.h"

QString syszux_lower_letter[67]={
    "en",
    "Esc", "`",    "1",    "2",    "3",    "4",    "5",    "6",    "7",    "8",    "9",  "0",    "-",    "=",    "Backspace",
    "Tab",    "q",    "w",    "e",    "r",    "t", "y",    "u",    "i",    "o",    "p",    "[",    "]",    "\\",    "Del",
    "Caps",  "a",    "s",    "d",    "f",    "g",    "h",    "j",    "k",    "l",    ";",  "\'",    "Enter",
    "Shift",    "z",    "x",    "c",    "v",    "b",    "n",    "m",  ",",    ".",    "/",    "Up",    "Shift",
    "Ctrl",    "Win",    "Alt",    " ",    "Alt",  "Ctrl",    "Left",    "Down",    "Right",    "Fn"
};
QString syszux_upper_letter[67]={
    "EN",
    "Esc","`",    "!",    "@",    "#",    "$",    "%",    "^",    "&&",    "*",    "(",     ")",    "_",    "+",    "Backspace",
    "Tab",    "Q",    "W",    "E",    "R",    "T",  "Y",    "U",    "I",    "O",    "P",    "{",    "}",    "\\",    "Del",
    "Caps",  "A",    "S",    "D",    "F",    "G",    "H",    "J",    "K",    "L",    ":",  "\"",    "Enter",
    "SHIFT",    "Z",    "X",    "C",    "V",    "B",    "N",    "M",  "<",    ">",    "?",    "Up",    "SHIFT",
    "Ctrl",    "Win",    "Alt",    " ",    "Alt",   "Ctrl",    "Left",    "Down",    "Right",    "Fn"
};
SyszuxPinyin::SyszuxPinyin(QWSInputMethod* im) :EffectKeyboard(0, QRect(0,0,480, 272)),button_group(new QButtonGroup(this)),input_method(0),lower_upper(0),page_count(0)
{
    int ret=0;
    upper_letter = 0;
    initGb();
    pinyin_file.setFileName(":/syszux/syszuxpinyin");

    if( !pinyin_file.open(QIODevice::ReadOnly) )
        QMessageBox::warning(0,tr("syszuxpinyin"),tr("can't load"));

    regExp.setCaseSensitivity(Qt::CaseSensitive);
    regExp.setPattern(QString("([a-z]+)"));

    while(!pinyin_file.atEnd())
    {
        QByteArray data = pinyin_file.readLine();
        ret = regExp.indexIn(QString(data.data()),0,QRegExp::CaretAtZero);
        pinyin_map.insert(regExp.cap(1),data.left(ret));
    }
}
SyszuxPinyin::~SyszuxPinyin()
{
}
void SyszuxPinyin::initGb()
{
    QPushButton *pushButton=new QPushButton(this);
    pushButton->hide();
    pushButton = ChangeInputMethod;
    for(int i=1;i<=78;i++)
    {
        button_vector.push_back(pushButton);
        button_group->addButton(pushButton,i);
        pushButton=VAR_CAST<QPushButton *>(pushButton->nextInFocusChain());
    }
    connect(button_group,SIGNAL(buttonClicked(int)),SLOT(buttonClickResponse(int)));
}
void SyszuxPinyin::buttonClickResponse(int gemfield)
{
    if(gemfield == 1)
    {
        if(input_method == 0)
        {
            input_method = 1;
        }
        else
        {
            input_method = 0;
        }
        changeInputMethod();
        return;
    }
    else if(gemfield <= 9 && gemfield >=2)     //匹配汉字
    {
        emit sendPinyin(button_vector.at(gemfield-1)->text());
        clearString();
        ChC_Previous->setDisabled(true);
        ChC_Next->setDisabled(true);
        return;
    }
    else if(gemfield == 10)  //  <--
    {
        selectHanziPre();
        return;
    }
    else if(gemfield == 11) //  -->
    {
        selectHanziNext();
        return;
    }
    else if(gemfield == 12) //  Hide
    {
        clearString();
        lineEdit->clear();
        sendPinyin("Hide");
        focusWidget()->clearFocus();
        this->hide();
        return;
    }
    else if(gemfield == 13) //  Esc
    {
        clearString();
        lineEdit->clear();
        return;
    }
    else if(gemfield == 28) //  Tab
    {
        emit sendPinyin("    ");
        return;
    }
    else if(gemfield == 72) //  " "
    {
        emit sendPinyin(" ");
        return;
    }

    if(input_method)
    {
        lineEdit->setText(lineEdit->text()+button_vector.at(gemfield-1)->text());
        matching(lineEdit->text());
    }
    else
    {
        qDebug()<<button_vector.at(gemfield-1)->text();
        emit sendPinyin(button_vector.at(gemfield-1)->text());
    }
}

void SyszuxPinyin::matching(QString gemfield)
{
    pinyin_list = pinyin_map.values(gemfield);
    changePage(0);
    page_count=0;
}

void SyszuxPinyin::changePage(int index)
{
    int i=index*8,j=0;
    while(++j != 8 )
    {
        button_vector.at(j)->setText(pinyin_list.value(++i));
    }
    if(index==0)
        ChC_Previous->setEnabled(false);
    else
        ChC_Previous->setEnabled(true);

    if(pinyin_list.size()>(index*8+8))
        ChC_Next->setEnabled(true);
    else
        ChC_Next->setEnabled(false);
}

void SyszuxPinyin::selectHanziPre()
{
    changePage(--page_count);
}

void SyszuxPinyin::selectHanziNext()
{
    changePage(++page_count);
}
void SyszuxPinyin::clearString()
{
    int i=0;
    while(++i!=8)
        button_vector.at(i)->setText("");
    lineEdit->setText("");
}

void SyszuxPinyin::changeInputMethod()
{
    if(input_method == 1)
    {
        if(lower_upper == 1)
        {
            changeLowerUpper();
        }
        ChangeInputMethod->setText("ch");
    }
    else
    {
        ChangeInputMethod->setText("en");
    }
}
void SyszuxPinyin::changeLowerUpper()
{
    if(upper_letter)
    {
        lower_upper = 0;
        button_vector.at(0)->setText(syszux_upper_letter[0]);
        int i=13;
        while(++i!=78)
            button_vector.at(i)->setText(syszux_upper_letter[i-11]);
    }
    else
    {
        lower_upper = 1;
        button_vector.at(0)->setText(syszux_lower_letter[0]);
        int i=13;
        while(++i!=78)
            button_vector.at(i)->setText(syszux_lower_letter[i-11]);
    }
}
