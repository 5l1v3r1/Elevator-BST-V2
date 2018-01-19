#ifndef SPEEDINSTRUMENT_H
#define SPEEDINSTRUMENT_H

#include <QWidget>

class SpeedInstrument : public QWidget
{
    Q_OBJECT
public:
    explicit SpeedInstrument(QWidget *parent = 0);
    
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void focusOutEvent(QFocusEvent *e);

public slots:
    void setValue(int);
    void init(void);

protected:
    void paintEvent(QPaintEvent *event);
    int mValue;

    bool m_DragFlag;
};

#endif // SPEEDINSTRUMENT_H
