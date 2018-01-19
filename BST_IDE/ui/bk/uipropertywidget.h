#ifndef UIPROPERTYWIDGET_H
#define UIPROPERTYWIDGET_H

#include "global.h"
#include "mutiitem.h"
#include <QWidget>

namespace Ui {
class UiPropertyWidget;
}

class UiPropertyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UiPropertyWidget(QWidget *parent = 0);
    ~UiPropertyWidget();
    
    bool RefreshTree(COM_INFO *pComInfo);
    bool InitStateTree();

signals:
    void sUpdate(COM_INFO*);
    void sEffectSetting();

public slots:
    bool slot_UpdateState(QTreeWidgetItem *pItem, STATE_INFO* pStateInfo_B);
    bool slot_UpdateComInfo();
    void on_PropertyTree_itemPressed(QTreeWidgetItem *item, int column);

private slots:
    void slot_PropertyTreeDragEnter(QDragEnterEvent* event);
    void slot_PropertyTreeDrop(QDropEvent* event);
    void slot_PropertyTreeStartDrag(Qt::DropActions actions);
    void on_BtnConfigure_clicked();

public:
    QTreeWidgetItem*            mPreTreeItem;
    ComCategory*                mComInfo_A;
    COM_INFO*                   mComInfo_B;
    Ui::UiPropertyWidget *ui;
};

#endif // UIPROPERTYWIDGET_H
