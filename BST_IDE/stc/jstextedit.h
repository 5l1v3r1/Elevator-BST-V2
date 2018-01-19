#ifndef JSTEXTEDIT_H
#define JSTEXTEDIT_H

#include "global.h"
#include <QTextEdit>
#include "highlighter.h"

class JsTextEdit : public QTextEdit
{
public:
    JsTextEdit(QWidget *parent = 0);

    Highlighter *jsHighlighter;

    QStringList jsKeywordPatterns;

    bool ShowContents(QString FileName);
};

#endif // JSTEXTEDIT_H
