#include "toolbox.h"

ToolBox::ToolBox(QWidget *parent):
    UpQTreeWidget(parent)
{
    m_CurToolType = TYPE_INVALID;
    setHeaderLabel(tr("Tool Box"));
    setAcceptDrops(true);
    setDragDropMode(QTreeWidget::DragDrop);
    //setSortingEnabled(true);
    connect(this,SIGNAL(sStartDrag(Qt::DropActions)),this,SLOT(slot_ToolTreeStartDrag(Qt::DropActions)));
}

void ToolBox::AddTools(QString ToolName, QString ToolIcon)
{
    if(ToolName.isNull() || ToolName.isEmpty())
        return;
    if(ToolIcon.isEmpty())
        ToolIcon = ":/Menu/rc/Menu/property.png";
    if(ToolName == QString("/") || ToolName == QString("Desktop")) //>@×ÀÃæ
    {
        QTreeWidgetItem* cldItem = new QTreeWidgetItem;
        cldItem->setIcon(0, QIcon(ToolIcon));
        cldItem->setText(0, QString("Desktop"));
        cldItem->setToolTip(0, "/");
        insertTopLevelItem(0, cldItem);
        return;
    }
    QStringList listName = ToolName.split("/", QString::SkipEmptyParts);
    int cnt = listName.count();
    if(cnt == 0)
        return;
    QString parName = listName.at(0);
    QTreeWidgetItem* parItem;
    for(int i=0;i<topLevelItemCount();i++)
    {
        parItem = topLevelItem(i);
        QString itemName = parItem->text(0);
        if(!itemName.compare(parName, Qt::CaseInsensitive))
            goto GT_FindParItem;
    }
    parItem = new QTreeWidgetItem;
    parItem->setIcon(0, QIcon(ToolIcon));
    parItem->setText(0, parName);
    parItem->setToolTip(0, parName.insert(0, "/"));
    addTopLevelItem(parItem);
GT_FindParItem:
    if(cnt == 2)
    {
        QString cldName = listName.at(1);
        QTreeWidgetItem* cldItem = new QTreeWidgetItem;
        cldItem->setIcon(0, QIcon(ToolIcon));
        cldItem->setText(0, cldName);
        cldItem->setToolTip(0, ToolName);
        parItem->addChild(cldItem);
    }
}

void ToolBox::slot_ToolTreeStartDrag(Qt::DropActions pAction)
{
    QTreeWidgetItem* item = currentItem();
    if(m_CurToolType == TYPE_UI)
    {
        QPixmap pixmap = item->icon(0).pixmap(30,25);
        QString toolTip = item->toolTip(0);

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream<<toolTip;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("COM", itemData);

        QDrag *drag = new QDrag(this);
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
        drag->setMimeData(mimeData);
        drag->exec(Qt::MoveAction);
    }
    else if(m_CurToolType == TYPE_MDP)
    {
        QPixmap pixmap = item->icon(0).pixmap(30,25);
        QString eventInfo = item->toolTip(0);

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream<<eventInfo;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("Device", itemData);

        QDrag *drag = new QDrag(this);
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
        drag->setMimeData(mimeData);
        drag->exec(Qt::MoveAction);
    }
}

void ToolBox::InitToolBox(FILE_TYPE pType)
{
    if(m_CurToolType == pType)
        return;
    m_CurToolType = pType;
    ClearAllItem();
    if(pType == TYPE_UI)
    {
        QList<QString> tmpStringList = GlobalPara.m_ComCategoryGroup.keys();
        for(int i=0;i<tmpStringList.count();i++)
        {
            QString tmpString = tmpStringList.at(i);
            ComCategory* tmpComCategory = GlobalPara.m_ComCategoryGroup.value(tmpString);
            if(tmpString.isEmpty() || !tmpComCategory)
                continue;
            AddTools(tmpString, tmpComCategory->Icon);
        }
    }
    else if(pType == TYPE_MDP)
    {
        QList<QString> tmpStringList = GlobalPara.m_ConfigInfoGroup.keys();
        for(int i=0;i<tmpStringList.count();i++)
        {
            QString tmpString = tmpStringList.at(i);
            QDomElement tmpElement = GlobalPara.m_ConfigInfoGroup.value(tmpString);
            if(tmpElement.isNull())
                continue;
            AddTools(tmpString, tmpElement.attribute("img"));
        }
    }
}
