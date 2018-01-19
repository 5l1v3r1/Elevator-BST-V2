#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QDebug>
#include "toolbase.h"
#include <QLineEdit>

class TextEdit : public QLineEdit, public ToolBase
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent = 0);
    TextEdit(QString pText, QWidget *parent = 0);
    
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent *e);

    void keyPressEvent(QKeyEvent *e);

    void contextMenuEvent(QContextMenuEvent * e);

    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);

    //>@虚函数，实现所有功能
    void ProcessMouseEvent(QMouseEvent* e);
    void ProcessKeyEvent(QKeyEvent *e);
    void ProcessWheelEvent(QWheelEvent *e);
    bool isFocus();
    void PaintBorder();

    void UpdateSelection(int pos);
    void UpdateSelectionData(Qt::Key pKey);

signals:
    void ShowKeyBoard(QPoint pos);
    void HideKeyBoard();
    void sChanged(QObject*);
public slots:
    void slot_TextChanged(QString);
};


#endif // TEXTEDIT_H
