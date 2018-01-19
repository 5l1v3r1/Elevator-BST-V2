#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include "toolbase.h"
#include <QPushButton>
#include "label.h"

class Button : public QPushButton, public ToolBase
{
    Q_OBJECT
public:
    explicit Button(QString pText, QWidget *parent = 0);

    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent *e);

    void keyPressEvent(QKeyEvent *e);

    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);

    void paintEvent(QPaintEvent *e);

    //>@虚函数，实现所有功能
    void ProcessMouseEvent(QMouseEvent* e);
    void ProcessKeyEvent(QKeyEvent *e);
    void ProcessWheelEvent(QWheelEvent* e);
    bool isFocus();
    void PaintBorder();

public:
    QPixmap     m_DownImage;
    QPixmap     m_NormalImage;
};

class ButtonList : public QHBoxLayout
{
    Q_OBJECT
public:
    explicit ButtonList(QStringList pParaList, QWidget *parent = 0);
    ~ButtonList();

    QStringList GetCheckedButton();
    void SetButtonCheckable(bool pEnable);

signals:
    void sChanged(QObject*);

public:
    QList<Button *>   m_ButtonList;
    QSignalMapper    *m_ButtonSigMapper;
};

#endif // TOOLBUTTON_H
