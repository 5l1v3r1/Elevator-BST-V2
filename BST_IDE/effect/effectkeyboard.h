#ifndef EFFECTKEYBOARD_H
#define EFFECTKEYBOARD_H

#include "effectbase.h"
#include "ui_Keyboard.h"

typedef enum{
    EN=0,
    CH
} LANGUAGE;

typedef enum{
    LOWER=0,
    UPPER
} TEXTCASE;

#define KEYCOUNT        66

#define KEYEN           61  //>@LANGUAGE
#define KEYCAPS         30  //>@Textcase

class EffectKeyboard : public EffectBase ,public Ui::Keyboard
{
    Q_OBJECT
public:
    EffectKeyboard(QWidget *parent, QRect pRect);
    bool InitEffectRc(STATE_INFO* pEffect);
    bool Start();
    bool analysisEffectParaItem(AREA_ANIMATE* pAreaEffect, QString pLabel, QString pContent);
    bool PaintEffect(QPainter &p);
    LANGUAGE CurLanguage();
    bool SwitchLanguage();
    TEXTCASE CurTextcase();
    bool SwitchTextcase();
signals:
    void InputMethodChanged(LANGUAGE pLanguage);
public slots:
    void updateEffect(AREA_OPERATE pOperate, QVariant pPara);
    void buttonClickResponse(int gemfield);
public:
    QButtonGroup *m_KeyButtonGroup;
};

#endif // EFFECTKEYBOARD_H
