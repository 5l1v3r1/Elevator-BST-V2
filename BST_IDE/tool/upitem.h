#ifndef UPITEM_H
#define UPITEM_H

#include <QDebug>
#include <QTreeWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QScrollBar>
#include <QEvent>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
class QDragMoveEvent;
QT_END_NAMESPACE
/**
//>@UpQTreeWidget******************************************************
*/
class UpQTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    UpQTreeWidget(QWidget *parent = 0);
    ~UpQTreeWidget();

    void ClearAllItem();
    void ClearTreeItem(QTreeWidgetItem *parentItem);
    QTreeWidgetItem *DelTreeItem(QTreeWidgetItem *pItem);

    void UpDragEnterEvent(QDragEnterEvent *event)
    {
        QTreeWidget::dragEnterEvent(event);
    }
    void UpDragLeaveEvent(QDragLeaveEvent *event)
    {
        QTreeWidget::dragLeaveEvent(event);
    }
    void UpDragMoveEvent(QDragMoveEvent *event)
    {
        QTreeWidget::dragMoveEvent(event);
    }
    void UpDropEvent(QDropEvent *event)
    {
        QTreeWidget::dropEvent(event);
    }
    void UpStartDrag(Qt::DropActions actions)
    {
        QTreeWidget::startDrag(actions);
    }
private:
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void startDrag(Qt::DropActions actions);
signals:
    void sDragEnter(QDragEnterEvent *event);
    void sDragLeave(QDragLeaveEvent *event);
    void sDragMove(QDragMoveEvent *event);
    void sDrop(QDropEvent *event);
    void sStartDrag(Qt::DropActions actions);
};
//>@--UpMouseQTreeWidget
class UpMouseQTreeWidget : public UpQTreeWidget
{
    Q_OBJECT
public:
    UpMouseQTreeWidget(QWidget *parent = 0);
    ~UpMouseQTreeWidget();

    void downMousePressEvent(QMouseEvent *event)
    {
        UpQTreeWidget::mousePressEvent(event);
    }
private:
protected:
    void mousePressEvent(QMouseEvent *event);
signals:
    void sMousePressEvent(QMouseEvent *event);
private slots:
};
/**
//>@UpQTableWidget******************************************************
*/
class UpQTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    UpQTableWidget(QWidget *parent = 0);
    ~UpQTableWidget();

    void UpDragEnterEvent(QDragEnterEvent *event)
    {
        QTableWidget::dragEnterEvent(event);
    }
    void UpDragLeaveEvent(QDragLeaveEvent *event)
    {
        QTableWidget::dragLeaveEvent(event);
    }
    void UpDragMoveEvent(QDragMoveEvent *event)
    {
        QTableWidget::dragMoveEvent(event);
    }
    void UpDropEvent(QDropEvent *event)
    {
        QTableWidget::dropEvent(event);
    }
    void UpStartDrag(Qt::DropActions actions)
    {
        QTableWidget::startDrag(actions);
    }
private:
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void startDrag(Qt::DropActions actions);
signals:
    void sDragEnter(QDragEnterEvent *event);
    void sDragLeave(QDragLeaveEvent *event);
    void sDragMove(QDragMoveEvent *event);
    void sDrop(QDropEvent *event);
    void sStartDrag(Qt::DropActions actions);
};
/**
//>@UpQListWidget******************************************************
*/
class UpQListWidget : public QListWidget
{
    Q_OBJECT
public:
    UpQListWidget(QWidget *parent = 0);
    ~UpQListWidget();

    void UpDragEnterEvent(QDragEnterEvent *event)
    {
        QListWidget::dragEnterEvent(event);
    }
    void UpDragLeaveEvent(QDragLeaveEvent *event)
    {
        QListWidget::dragLeaveEvent(event);
    }
    void UpDragMoveEvent(QDragMoveEvent *event)
    {
        QListWidget::dragMoveEvent(event);
    }
    void UpDropEvent(QDropEvent *event)
    {
        QListWidget::dropEvent(event);
    }
    void UpStartDrag(Qt::DropActions actions)
    {
        QListWidget::startDrag(actions);
    }
private:
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void startDrag(Qt::DropActions actions);
signals:
    void sDragEnter(QDragEnterEvent *event);
    void sDragLeave(QDragLeaveEvent *event);
    void sDragMove(QDragMoveEvent *event);
    void sDrop(QDropEvent *event);
    void sStartDrag(Qt::DropActions actions);
};
//>@--UpSelecQListWidget
class UpSelecQListWidget : public UpQListWidget
{
    Q_OBJECT
public:
    UpSelecQListWidget(QWidget *parent = 0);
    ~UpSelecQListWidget();
private:
protected:
signals:
private slots:
    void slot_ItemClicked(QListWidgetItem* pItem);
};
/**
//>@UpQGraphicsView******************************************************
*/
class UpQGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    UpQGraphicsView(QWidget *parent = 0);
    ~UpQGraphicsView();

    void DownDropEvent(QDropEvent *event)
    {
        QGraphicsView::dropEvent(event);
    }
    virtual void drawBackground(QPainter *painter, const QRectF &rect);
protected:
    void resizeEvent(QResizeEvent *event);
    void dropEvent(QDropEvent *event);
signals:
    void sDropEvent(QDropEvent*);

public:
    bool        m_FitInView;
};

#endif // UPITEM_H
