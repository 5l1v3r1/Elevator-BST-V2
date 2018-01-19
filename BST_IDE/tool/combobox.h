#ifndef COMBOBOX_H
#define COMBOBOX_H

#include "toolbase.h"
#include <QComboBox>

class ComboBox : public QComboBox, public ToolBase
{
    Q_OBJECT
public:
    explicit ComboBox(QStringList pParaList, QString pSelectPara, QWidget *parent = 0);

    void InitEnumList(QStringList pList, QString pValue);
    bool setCurrentText(QString pText);

    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent *e);

    void contextMenuEvent(QContextMenuEvent * e);

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
    void sChanged(QObject*);
public slots:
    void currentIndexChanged(int index);
public:
    bool m_PressFlag;
};

#endif // COMBOBOX_H
