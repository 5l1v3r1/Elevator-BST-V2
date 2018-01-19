#ifndef STCPAGE_H
#define STCPAGE_H

#include "tabpage.h"
#include "jstextedit.h"
#include "QSpacerItem"
#include "treeitemclass.h"

#define QT_NO_SCRIPTTOOLS
#ifndef QT_NO_SCRIPTTOOLS
    #include <QScriptEngineDebugger>
#endif

namespace Ui {
class stcPage;
}

class stcPage : public TabPage
{
    Q_OBJECT  
public:
    explicit stcPage(QString fileName=QString(), QWidget *parent=0);

    void SyncTreeItem(FileTreeItem *pTreeItem)
    {
        m_TreeItem = pTreeItem;
    }

public:
    Ui::stcPage             *ui;
    stcManager              *m_StcManager;
    FileTreeItem            *m_TreeItem;    //>@在项目管理窗口中的TreeItem
};

#endif // STCPAGE_H
