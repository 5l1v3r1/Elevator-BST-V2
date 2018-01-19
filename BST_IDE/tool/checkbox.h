#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "toolbase.h"
#include <QCheckBox>

class Checkbox : public QCheckBox, public ToolBase
{
    Q_OBJECT
public:
    Checkbox(QWidget *parent = 0);
    
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent *e);

    void keyPressEvent(QKeyEvent *e);

    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);

    //>@虚函数，实现所有功能
    void ProcessMouseEvent(QMouseEvent* e);
    void ProcessKeyEvent(QKeyEvent *e);
    void ProcessWheelEvent(QWheelEvent *e);
    bool isFocus();
    void PaintBorder();

signals:
    
public slots:
    
};

class CheckList : public QHBoxLayout
{
    Q_OBJECT
public:
    explicit CheckList(QStringList pParaList, QStringList pSelectList=QStringList(), QWidget *parent = 0);
    ~CheckList();
    void InitCheckList(QStringList pList);
    void RestoreCheckList();
    bool IsChanged();
    QStringList GetSelectList();
    QString GetSelectText(QStringList pList);
    QString GetSelectText();
signals:
    void sChanged(QObject*);
public slots:
    void slot_StateChanged(int);
public:
    QList<Checkbox *>   m_CheckboxList;
    QStringList         m_SelectList;
};

#endif // CHECKBOX_H
