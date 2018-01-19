#include "rctree.h"
#include "toolbar.h"
#include "devicequeryform.h"

RcTree::RcTree(QWidget *parent) :
    QTreeWidget(parent)
{
    connect(this, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(TreeItemPressed(QTreeWidgetItem*,int)));

    m_AddAct = new QAction(QIcon(":/Menu/rc/Menu/plus.png"), tr("&Add Local File"), this);
    m_AddAct->setStatusTip(tr("Add Local File to Tree"));
    connect(m_AddAct, SIGNAL(triggered()), this, SLOT(slot_Add()));

    m_DelAct = new QAction(QIcon(":/Menu/rc/Menu/minus.png"), tr("&Del File"), this);
    m_DelAct->setStatusTip(tr("Delete form Tree"));
    connect(m_DelAct, SIGNAL(triggered()), this, SLOT(slot_Del()));

    m_SetActivAct = new QAction(QIcon(":/Menu/rc/Menu/build.png"), tr("&Set as Active"), this);
    m_SetActivAct->setStatusTip(tr("Set item as Active"));
    connect(m_SetActivAct, SIGNAL(triggered()), this, SLOT(slot_SetActive()));
}

void RcTree::TreeItemPressed(QTreeWidgetItem *item, int column)
{
    if (QApplication::mouseButtons() == Qt::RightButton)
    {
        m_CurSelectItem = item;
        if(item)
        {
            QTreeWidgetItem *tmpParItem = item->parent(); //>@确定不是顶级ITEM
            if(tmpParItem)
            {
                QString tmpParName = tmpParItem->text(0);
                if(tmpParName.compare("DataBase", Qt::CaseInsensitive) == 0)
                {
                    QMenu menu(tr("&Menu"),this);
                    menu.addAction(m_SetActivAct);
                    menu.exec(QCursor::pos());
                }
                else if(tmpParName.compare("LocalDisk", Qt::CaseInsensitive) == 0)
                {
                    QMenu menu(tr("&Menu"),this);
                    menu.addAction(m_SetActivAct);
                    menu.addSeparator();
                    menu.addAction(m_DelAct);
                    menu.exec(QCursor::pos());
                }
            }
            else                                          //>@确定是顶级ITEM
            {
                QString tmpName = item->text(0);
                if(tmpName.compare("LocalDisk", Qt::CaseInsensitive) == 0)
                {
                    QMenu menu(tr("&Menu"),this);
                    menu.addAction(m_AddAct);
                    menu.exec(QCursor::pos());
                }
            }
        }
    }
}

void RcTree::slot_Add()
{
    static QString sSelectPath = QDir::currentPath();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    sSelectPath,
                                                    tr("Kernel files (*.hex *.bin);;"
                                                       "Media files (**.bmp *.wav);;"
                                                       "All files (*.*)"));
    if (fileName.isEmpty())
        return;
    sSelectPath = getParentDirectory(fileName);
    QStringList tmpList;
    tmpList.append(getFileName(fileName));
    tmpList.append(fileName);
    QTreeWidgetItem *tmpTreeItem = AddTreeItem(m_CurSelectItem, tmpList, false);
    SetActive(tmpTreeItem, true);
    expandAll();
}

void RcTree::slot_Del()
{
    if(m_CurSelectItem == 0)
        return;
    QTreeWidgetItem *tmpParItem = m_CurSelectItem->parent();
    if(tmpParItem)
    {
        tmpParItem->removeChild(m_CurSelectItem);
        m_CurSelectItem = 0;
    }
}

void RcTree::slot_SetActive()
{
    for(int i=0;i<topLevelItemCount();i++)
    {
        QTreeWidgetItem *tmpTopItem = topLevelItem(i);
        if(tmpTopItem == 0)
            continue;
        for(int j=0;j<tmpTopItem->childCount();j++)
        {
            QTreeWidgetItem *tmpItem = tmpTopItem->child(j);
            if(tmpItem == 0)
                continue;
            if(tmpItem == m_CurSelectItem)
                SetActive(tmpItem, true);
            else
                SetActive(tmpItem, false);
        }
    }
}

//QString("资源管理"),QString("资源版本,存储位置"),QString("资源类型='Bootloader'")
//QString("资源管理"),QString("资源版本,存储位置"),QString("资源类型='Kernel'")
//QString("资源管理"),QString("资源版本,存储位置"),QString("资源类型='FileSystem'")
//QString("资源管理"),QString("资源版本,存储位置"),QString("资源类型='Application'")
void RcTree::InitDataBaseList(QString pTableName, QString pColumns, QString pCondition)
{
    SQLToolBar* tmpSqlBar = (SQLToolBar*)(GlobalPara.m_SqlToolBar);
    if(!(tmpSqlBar && tmpSqlBar->mDevQueryFrom && tmpSqlBar->mDevQueryFrom->m_SqlQuery))
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("SQLServer is disconnected, please correct!"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        ReleaseDataBaseList();
        return;
    }
    SqlQuery *tmpSqlQuery = tmpSqlBar->mDevQueryFrom->m_SqlQuery;
    ReleaseDataBaseList();
    QTreeWidgetItem *tmpTreeItem = topLevelItem(0);
    QList<QVariant> tmpRev = tmpSqlQuery->QueryDatas(pTableName, pColumns, pCondition);
    for(int i=0; i<tmpRev.count(); i++)
    {
        QVariant tmpVariant = tmpRev.at(i);
        QStringList tmpList = tmpVariant.toStringList();
        AddTreeItem(tmpTreeItem, tmpList, false);
    }
    expandAll();
}

void RcTree::ReleaseDataBaseList()
{
    DelTreeAllItem(topLevelItem(0));
}

bool RcTree::PrepareActiveFile(QString pBufferPath, QString &pFilePath)
{
    RCLOCATION tmpLocation;
    QTreeWidgetItem *tmpItem = GetActive(tmpLocation);
    if(tmpItem == 0)
    {
        QMessageBox::critical(this, tr("Error"),
                                         tr("Hasn't set any active item!"),
                                         QMessageBox::Yes);
        return false;
    }
    QString tmpFileName = tmpItem->text(0);
    QString tmpFilePath = tmpItem->text(1);
    QString tmpBufferFile = pBufferPath + tmpFileName;
    if(isFileExist(tmpBufferFile))
    {
        int ret = QMessageBox::warning(this, tr("Warnning"),
                                             tr("The current selected file has exist in buffer direction,\ndo you want to overwrite!"),
                                             QMessageBox::Yes | QMessageBox::No,
                                             QMessageBox::No);
        if(ret == QMessageBox::Yes)
            delFile(tmpBufferFile);
        else
            return false;
    }
    //>@拷贝文件到缓冲目录
    bool flag = false;
    if(tmpLocation == DATABASE)
    {
        //>@从SQL中下载文件
        SQLToolBar* tmpSqlBar = (SQLToolBar*)(GlobalPara.m_SqlToolBar);
        if(!(tmpSqlBar && tmpSqlBar->mDevQueryFrom && tmpSqlBar->mDevQueryFrom->m_SqlQuery))
        {
            QMessageBox::warning(this, tr("Error"),
                                 tr("SQLServer is disconnected, please correct!"),
                                 QMessageBox::Ok,
                                 QMessageBox::Ok);
            return false;
        }
        SqlQuery *tmpSqlQuery = tmpSqlBar->mDevQueryFrom->m_SqlQuery;
        if(tmpSqlQuery)
            flag = tmpSqlQuery->DownLoadFile(tmpFilePath, tmpBufferFile, 60000); //>@60s
    }
    else if(tmpLocation == LOCALDISK)
    {
        QFile tmpFile(tmpFilePath);
        flag = tmpFile.copy(tmpFilePath, tmpBufferFile);
    }
    if(flag)
        pFilePath = tmpBufferFile;
    return flag;
}

QTreeWidgetItem *RcTree::GetActive(RCLOCATION &pLocation)
{
    for(int i=0;i<topLevelItemCount();i++)
    {
        QTreeWidgetItem *tmpTopItem = topLevelItem(i);
        if(tmpTopItem == 0)
            continue;
        for(int j=0;j<tmpTopItem->childCount();j++)
        {
            QTreeWidgetItem *tmpItem = tmpTopItem->child(j);
            if(tmpItem == 0)
                continue;
            if(IsActive(tmpItem))
            {
                if(tmpTopItem->text(0).compare("DataBase", Qt::CaseInsensitive) == 0)
                {
                    pLocation = DATABASE;
                }
                else if(tmpTopItem->text(0).compare("LocalDisk", Qt::CaseInsensitive) == 0)
                {
                    pLocation = LOCALDISK;
                }
                return tmpItem;
            }
        }
    }
    return (QTreeWidgetItem *)0;
}

void RcTree::DelTreeAllItem(QTreeWidgetItem *parentItem)
{
    QList<QTreeWidgetItem*> ItemList = parentItem->takeChildren();
    QTreeWidgetItem* tmpItem;
    for(int i=0;i<ItemList.count();i++)
    {
        tmpItem = ItemList.at(i);
        parentItem->removeChild(tmpItem);
    }
}

//>@返回父item
QTreeWidgetItem *RcTree::DelTreeItem(QTreeWidgetItem *pItem)
{
    QTreeWidgetItem* tmpParentItem = pItem->parent();
    if(tmpParentItem == 0)
        return 0;
    tmpParentItem->removeChild(pItem);
    return tmpParentItem;
}

//>@返回添加的Item
QTreeWidgetItem *RcTree::AddTreeItem(QTreeWidgetItem *parentItem, QStringList contentList, bool Editable)
{
    static int i = 0;
    QTreeWidgetItem *pNewItem = new QTreeWidgetItem(parentItem, contentList);
    pNewItem->setTextAlignment(0, Qt::AlignLeft || Qt::AlignVCenter);
    pNewItem->setTextAlignment(1, Qt::AlignLeft || Qt::AlignVCenter);
    pNewItem->setBackgroundColor(0, (i%2)?QColor(254,235,208):QColor(255,250,179));
    pNewItem->setBackgroundColor(1, (i%2)?QColor(254,235,208):QColor(255,250,179));
    pNewItem->setFont(0, QFont());
    if(Editable)
        pNewItem->setFlags(Qt::ItemIsEditable |Qt::ItemIsEnabled);
    parentItem->addChild(pNewItem);
    i++;
    return pNewItem;
}
