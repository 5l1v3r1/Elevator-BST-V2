#ifndef RCTREE_H
#define RCTREE_H

#include "global.h"
#include <QTreeWidget>

class RcTree : public QTreeWidget
{
    Q_OBJECT
public:
    enum RCLOCATION {DATABASE, LOCALDISK} ;
    explicit RcTree(QWidget *parent = 0);
    
    void InitDataBaseList(QString pTableName, QString pColumns, QString pCondition);
    void ReleaseDataBaseList();

    bool PrepareActiveFile(QString pBufferPath, QString &pFilePath);

    bool SetActive(QTreeWidgetItem *pTreeItem, bool flag)
    {
        if(pTreeItem == 0)
            return false;
        if(flag)
            pTreeItem->setFont(0, QFont("Arial", 9, QFont::Bold));
        else
            pTreeItem->setFont(0, QFont("Arial", 9, QFont::Normal));
        return true;
    }
    bool IsActive(QTreeWidgetItem *pTreeItem)
    {
        if(pTreeItem == 0)
            return false;
        QFont tmpFont = pTreeItem->font(0);
        return tmpFont.bold();
    }
    QTreeWidgetItem *GetActive(RCLOCATION &pLocation);
    void DelTreeAllItem(QTreeWidgetItem *parentItem);
    QTreeWidgetItem *DelTreeItem(QTreeWidgetItem *pItem);
    QTreeWidgetItem *AddTreeItem(QTreeWidgetItem *parentItem, QStringList contentList, bool Editable);

public slots:
    void TreeItemPressed(QTreeWidgetItem *item, int column);
    void slot_Add();
    void slot_Del();
    void slot_SetActive();
public:
    QAction             *m_AddAct,
                        *m_DelAct,
                        *m_SetActivAct;
    QTreeWidgetItem     *m_CurSelectItem;
};

#endif // RCTREE_H
