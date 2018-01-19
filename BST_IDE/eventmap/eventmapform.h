#ifndef EVENTMAPFORM_H
#define EVENTMAPFORM_H

#include "tabpage.h"
#include "upitem.h"
#include <QMultiHash>
#include <QTreeWidgetItem>
#include "treeitemclass.h"

namespace Ui {
class EventMapForm;
}

class EventMapForm : public TabPage
{
    Q_OBJECT
    enum EventType{
        EventFloor,
        EventArrow,
        EventFunction,
        EventAudio,
        EventTimer,
        EventAlarm,
        EventInvalid
    };
    enum EventParaType{
        EventParaInvalid=0,
        Num,
        Str,
        Enum,
        Check
    };
    enum ActionType{
        ActionInvalid=0,
        ActionArrow,
        ActionFloor,
        ActionFunction,
        ActionDesktop,
        ActionVideo,
        ActionAudio,
        ActionImage,
        ActionTxt,
        ActionLogo,
        ActionNamePlate,
        ActionWeather
    };
public:
    explicit EventMapForm(QString pMapPath, QWidget *parent = 0);
    ~EventMapForm();

    void InitConnect();
    void ReleaseConnect();
    void ShowPixmap(QString pPath, QIcon pIcon = QIcon());

    bool SycncActionTreeFromTheme(QString pPath);
    bool SycncEventTableFromMapAndTheme(QString pPath);
    void SycncMapFromUI();

    bool AddEventToMapForm(QIcon pIcon, QString pText, QString pToolTip, QPoint pPoint);
    bool AddActionToMapForm(QIcon pIcon, QString pText, QString pToolTip, QPoint pPoint);

    QIcon GenerateFloorIcon(QSize pSize, float pDivide, QImage pImage0, QImage pImage1);
    EventType GetEventType(QString pKey);
    EventParaType GetEventParaType(QString pKey);
    QString GetEventInfo(QWidget* pPage);

    bool MapToTable(QTableWidget* pTableWidget, QPoint pPoint, int& pRow, int& pColumn);

    ActionType GetActionType(COM_TYPE pKey);
    bool GetRcNum(int& pNum, QString pKey);

    bool OpenThemeUI(QString pPath);
    void CloseThemeUI();

    bool CreatMap(QString pPath);
    bool OpenMap(QString pPath);
    void CloseMap();
    bool SaveMap();

    void SyncTreeItem(FileTreeItem *pTreeItem);

private slots:
    void slot_EventTreeDragEnter(QDragEnterEvent* event);
    void slot_EventTreeDrop(QDropEvent* event);
    void slot_EventTreeStartDrag(Qt::DropActions);

    void slot_EventMapTableDragEnter(QDragEnterEvent* event);
    void slot_EventMapTableDrop(QDropEvent* event);
    void slot_EventMapTableStartDrag(Qt::DropActions actions);

    void slot_ActionTreeDragEnter(QDragEnterEvent* event);
    void slot_ActionTreeDrop(QDropEvent* event);
    void slot_ActionTreeStartDrag(Qt::DropActions);
    void slot_MousePressEvent(QMouseEvent*);

    void on_EventTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_ActionTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_Timer_TriggerTime_Para1_Num_valueChanged(int value);
    void on_SelectUIBtn_clicked();
    void on_EventMapTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_BtnSave_Schadule_clicked();
    void on_BtnAddRow_Schadule_clicked();
    void on_BtnDelRow_Schadule_clicked();
    void on_BtnRefresh_Schadule_clicked();
    void on_BtnAscending_Schadule_clicked();
    void on_BtnDescendingOrder_Schadule_clicked();
    void on_EventTree_activated(const QModelIndex &index);

public:
    Ui::EventMapForm   *ui;
    FileTreeItem       *m_TreeItem;    //>@在项目管理窗口中的TreeItem
    QMutex*             mMutex;
    uiManager          *mUIManager;
    QString             mUIPAth;
    mapManager         *mMapManager;
    QString             mMapPAth;
};

#endif // EVENTMAPFORM_H
