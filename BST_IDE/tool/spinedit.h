#ifndef SPINEDIT_H
#define SPINEDIT_H

#include "toolbase.h"
#include <QSpinBox>

class SpinEdit : public QSpinBox, public ToolBase
{
    Q_OBJECT
public:
    SpinEdit(int pValue=0, int mimValue=0, int maxValue=100, QWidget *parent = 0);

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

public slots:
    void slot_StopSpeedup();
    void slot_ValueChanged(int pValue);
signals:
    void sChanged(QObject*);

public:
    quint8      mSpeedup;
};

class ENetAddrEdit : public QHBoxLayout
{
    Q_OBJECT
public:
    explicit ENetAddrEdit(ENET_ADDR pIP, QWidget *parent = 0);
    ~ENetAddrEdit();

    ENET_ADDR GetIP();
    QString GetIPString();
    void SetIP(ENET_ADDR pIP);

signals:
    void sChanged(QObject*);

public:
    SpinEdit*   m_IPSpinbox[4];
    QLabel*     m_DotLabel[3];
};

class DateTimeEdit : public QHBoxLayout
{
    Q_OBJECT
public:
    explicit DateTimeEdit(QDateTime pDateTime, QWidget *parent = 0);
    ~DateTimeEdit();

    void SetDateTime(QDateTime pDateTime);
    QDateTime GetDateTime();

signals:
    void sChanged(QObject*);

public:
    SpinEdit*   m_Spinbox[6];
    QLabel*     m_Label[5];
};

#endif // SPINEDIT_H
