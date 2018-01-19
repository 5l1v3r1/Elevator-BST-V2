#include "jstextedit.h"

JsTextEdit::JsTextEdit(QWidget *parent) :
    QTextEdit(parent)
{ 
    jsHighlighter = 0;
    jsKeywordPatterns   << "\\bbreak\\b"
                        << "\\bdelete\\b"
                        << "\\bfunction\\b"
                        << "\\breturn\\b"
                        << "\\btypeof\\b"
                        << "\\bcase\\b"
                        << "\\bdo\\b"
                        << "\\bif\\b"
                        << "\\bswitch\\b"
                        << "\\bvar\\b"
                        << "\\bcatch\\b"
                        << "\\belse\\b"
                        << "\\bin\\b"
                        << "\\bthis\\b"
                        << "\\bcontinue\\b"
                        << "\\bfalse\\b"
                        << "\\binstanceof\\b"
                        << "\\bthrow\\b"
                        << "\\bwhile\\b"
                        << "\\bdebugger\\b"
                        << "\\bfinally\\b"
                        << "\\bnew\\b"
                        << "\\bwith\\b"
                        << "\\bdefault\\b"
                        << "\\bfor\\b"
                        << "\\bnull\\b"
                        << "\\btry\\b"
                        << "\\bvoid\\b";
}

bool JsTextEdit::ShowContents(QString FileName)
{
    QFile file(FileName);
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&file);
        setText(in.readAll());
        document()->setModified(false);
        if(jsHighlighter)
        {
            delete jsHighlighter;
            jsHighlighter = 0;
        }
        jsHighlighter = new Highlighter(document());
        jsHighlighter->SetKeywordPatterns(jsKeywordPatterns);
        return true;
    }
    return false;
}
