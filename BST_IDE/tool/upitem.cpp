#include "upitem.h"
/**
//>@UpQTreeWidget
*/
UpQTreeWidget::UpQTreeWidget(QWidget *parent):
    QTreeWidget(parent)
{
}

UpQTreeWidget::~UpQTreeWidget()
{
}

void UpQTreeWidget::ClearAllItem()
{
    if(topLevelItemCount() <= 0)
        return;
    QTreeWidgetItem* tmpItem;
    for(int i=topLevelItemCount()-1;i>=0;i--)
    {
        tmpItem = takeTopLevelItem(i);
        if(tmpItem)
        {
            ClearTreeItem(tmpItem);
            delete tmpItem;
        }
    }
}

//>@删除parentItem的所有子节点
void UpQTreeWidget::ClearTreeItem(QTreeWidgetItem *parentItem)
{
    if(parentItem == 0)
        return;
    QList<QTreeWidgetItem*> ItemList = parentItem->takeChildren();
    if(ItemList.count() <= 0)
        return;
    QTreeWidgetItem* tmpItem;
    for(int i=ItemList.count()-1;i>=0;i--)
    {
        tmpItem = ItemList.takeAt(i);
        if(tmpItem)
        {
            ClearTreeItem(tmpItem);
            delete tmpItem;
        }
    }
}

//>@返回父item
QTreeWidgetItem *UpQTreeWidget::DelTreeItem(QTreeWidgetItem *pItem)
{
    if(pItem == 0)
        return 0;
    QTreeWidgetItem* tmpParentItem = pItem->parent();
    if(tmpParentItem == 0)
        return 0;
    ClearTreeItem(pItem);
    tmpParentItem->removeChild(pItem);
    delete pItem;
    return tmpParentItem;
}

void UpQTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    emit sDragEnter(event);
}

void UpQTreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    emit sDragLeave(event);
}

void UpQTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    emit sDragMove(event);
}

void UpQTreeWidget::dropEvent(QDropEvent *event)
{
    emit sDrop(event);
}

void UpQTreeWidget::startDrag(Qt::DropActions actions)
{
    emit sStartDrag(actions);
}
//>@--UpMouseQTreeWidget
UpMouseQTreeWidget::UpMouseQTreeWidget(QWidget *parent):
    UpQTreeWidget(parent)
{
}

UpMouseQTreeWidget::~UpMouseQTreeWidget()
{

}

void UpMouseQTreeWidget::mousePressEvent(QMouseEvent *event)
{
    emit sMousePressEvent(event);
}
/**
//>@UpQTableWidget
*/
UpQTableWidget::UpQTableWidget(QWidget *parent):
    QTableWidget(parent)
{
}

UpQTableWidget::~UpQTableWidget()
{
}

void UpQTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    emit sDragEnter(event);
}

void UpQTableWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    emit sDragLeave(event);
}

void UpQTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
    emit sDragMove(event);
}

void UpQTableWidget::dropEvent(QDropEvent *event)
{
    emit sDrop(event);
}

void UpQTableWidget::startDrag(Qt::DropActions actions)
{
    emit sStartDrag(actions);
}

/**
//>@UpQListWidget
*/
UpQListWidget::UpQListWidget(QWidget *parent):
    QListWidget(parent)
{
}

UpQListWidget::~UpQListWidget()
{
}

void UpQListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    emit sDragEnter(event);
}

void UpQListWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    emit sDragLeave(event);
}

void UpQListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    emit sDragMove(event);
}

void UpQListWidget::dropEvent(QDropEvent *event)
{
    emit sDrop(event);
}

void UpQListWidget::startDrag(Qt::DropActions actions)
{
    emit sStartDrag(actions);
}
//>@--UpSelecQListWidget
UpSelecQListWidget::UpSelecQListWidget(QWidget *parent):
    UpQListWidget(parent)
{
    connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_ItemClicked(QListWidgetItem*)));
}

UpSelecQListWidget::~UpSelecQListWidget()
{
    disconnect(this, SIGNAL(itemClicked(QListWidgetItem*)), 0, 0);
}

void UpSelecQListWidget::slot_ItemClicked(QListWidgetItem* pItem)
{
    QAbstractItemView::SelectionMode mode = this->selectionMode();
    switch(mode)
    {
        case SingleSelection:
        {
            if(pItem->checkState() == Qt::Checked)
            {
                for(int i=0;i<this->count();i++)
                {
                    QListWidgetItem* item = this->item(i);
                    item->setCheckState(Qt::Unchecked);
                }
                pItem->setCheckState(Qt::Checked);
            }
            break;
        }
        default:break;
    }
}
/**
//>@UpQGraphicsView******************************************************
*/
UpQGraphicsView::UpQGraphicsView(QWidget *parent):
    QGraphicsView(parent)
{
    m_FitInView = true;
}

UpQGraphicsView::~UpQGraphicsView()
{

}

void UpQGraphicsView::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawBackground(painter, rect);
}

void UpQGraphicsView::resizeEvent(QResizeEvent *event)
{
//    QGraphicsView::resizeEvent(event);
    if(m_FitInView)
    {
        fitInView(sceneRect(), Qt::IgnoreAspectRatio);
        sceneRect().setSize(size());
    }
}

void UpQGraphicsView::dropEvent(QDropEvent *event)
{
    emit sDropEvent(event);
}

