#ifndef XMLTEXTEDIT_H
#define XMLTEXTEDIT_H

#include "global.h"
#include <QPlainTextEdit>
#include "xmlsyntaxhighlighter.h"

class LineNumberArea;

class XmlTextEdit : public QPlainTextEdit
{
public:
    XmlTextEdit(QWidget *parent = 0);
    bool ShowContents(QString FileName);

public:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(XmlTextEdit *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    XmlTextEdit *codeEditor;
};

#endif // XMLTEXTEDIT_H
