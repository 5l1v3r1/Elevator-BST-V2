#ifndef PROPERTYWIDGET_H
#define PROPERTYWIDGET_H

#include "global.h"
#include "mutiitem.h"
#include "upitem.h"

class DiagramScene;
class GraphicsOperate;

#include <QTreeWidgetItem>
#include <QDoubleSpinBox>
#include <QPushButton>

class TreeComboBox : public QComboBox
{
    Q_OBJECT
public:
    TreeComboBox(QTreeWidgetItem* pItem, QStringList pEnumValue, QString pValue = QString(), QWidget *parent = 0);
    ~TreeComboBox() {}

signals:
    void sValueChanged(QString pValue);
public slots:
    void slot_ValueChanged(QString pValue);
public:
    QString             mValue;
    QTreeWidgetItem*    mTreeItem;
};

class TreeSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    TreeSpinBox(QTreeWidgetItem* pItem, QString pValue, QWidget *parent = 0);
    ~TreeSpinBox() {}

public slots:
    void slot_ValueChanged();
signals:
    void sValueChanged(QString pValue);
public:
    QTreeWidgetItem*    mTreeItem;
    int                 mValue;
};

class TreeButtons : public QWidget
{
    Q_OBJECT
public:
    TreeButtons(QTreeWidgetItem* pItem, QString pFilePath, QStringList pFileType, QWidget *parent = 0);
    ~TreeButtons();

    void focusOutEvent(QFocusEvent *event);

signals:
    void sValueChanged(QString pValue);
public slots:
    void slot_ExplorerFile();
    void slot_ClearFile();
public:
    QString             mFileFormat;
    QString             mFilePath;
    QTreeWidgetItem*    mTreeItem;
};

#define COMTREEITEM_TYPE        10000

#define RECTTREEITEM_TYPE       10001
#define X_TREEITEM_TYPE         10002
#define Y_TREEITEM_TYPE         10003
#define W_TREEITEM_TYPE         10004
#define H_TREEITEM_TYPE         10005

#define STATETREEITEM_TYPE      10011

#define PARATREEITEM_TYPE       10012
#define SUBPARATREEITEM_TYPE    10013
#define RCTREEITEM_TYPE         10015
#define SUBRCTREEITEM_TYPE      10016

#define EFFECT_NAME      20000

#define RC_NICKNAME      30000
#define RC_FILEPATH      30001
#define RC_FILETYPE      30002

#define EDIT_TYPE        10000  //>@1:COMBOX, 2:SPINBOX
#define EDIT_DATA        10001  //>@COMBOX的可选项

/*
    |--Componet
    |--Rect：横竖显         --没有右键菜单
    |----X
    |----Y
    |----W
    |----H
    |--Common:类型          --没有右键菜单
    |----Parameter
    |------xx
    |------xx
    |----Resource
    |------xx              --Add Del
    |------xx
*/

class PropertyTree : public UpQTreeWidget
{
    Q_OBJECT
public:
    explicit PropertyTree(QWidget *parent = 0);
    ~PropertyTree();
    
    void CreateMenu();
    void ReleaseMenu();

    void Connect();
    void Disconnect();

    bool RefreshTree(GraphicsOperate *pComponent);  //>@针对pComItem进行初始化tree

    QRect GetCurRect(QTreeWidgetItem *pGeometryTree);
    bool ChangeRect(QTreeWidgetItem *pGeometryTree, QRect pRect);
    bool ChangeEffect(QTreeWidgetItem *pItem, QString pEffectName);

    QTreeWidgetItem* AddEffectSubParaItem(QTreeWidgetItem* pParent, QString pParaName, QString pInitValue, QString pUiValue);
    QTreeWidgetItem* AddEffectSubRcItem(QTreeWidgetItem* pParent, QStringList pRcNames, QStringList pRcTypes, int pRcLabel, QString pRcFile);
    QTreeWidgetItem* AddEffectSubRcItem(QTreeWidgetItem* pParent, QString pRcName, QString pRcFile);

    void InitEffectPara(QTreeWidgetItem *pItem, EFFECT *pDefaultEffect, QDomElement pStateElement);
    void InitSubRcList(QTreeWidgetItem *pItem, EFFECT *pDefaultEffect, QDomElement pStateElement);

    void EditNumValue(QTreeWidgetItem *pItem, QString pValue);
    void EditFileValue(QTreeWidgetItem *pItem, QString pFilePath, QStringList pFileType);
    void EditEnumValue(QTreeWidgetItem *pItem, QStringList pEnumValue, QString pValue);

    void AddRcItem(QTreeWidgetItem *tmpParItem, QString pRcName, QString pRcFile);
    void ModifyRcItem(QTreeWidgetItem *pItem, QString pValue);
    void DelRcItem(QTreeWidgetItem *pItem);

signals:

public slots:
    void slot_ItemDoubleClicked(QTreeWidgetItem *item, int column);
    void slot_ItemPressed(QTreeWidgetItem *pItem, int column);

    //>@响应从其它地方发来的改变坐标的信号
    void slot_SceneRectChanged(QRectF pRect);

    void slot_NumValueChanged(QString pValue);
    void slot_FileValueChanged(QString pValue);
    void slot_EnumValueChanged(QString pValue);

    void slot_NewRC();  //>@添加一个新RC名，自动递增号码
    void slot_AddRCs(); //>@一次添加多个资源
    void slot_DelRC();
    void slot_ClearRCs();

public:
    GraphicsOperate            *mComponet;
    QMenu                      *mRcMenu;
    QTreeWidgetItem            *mCurTreeItem;//>@主要用来记录最后一次右键点击的树项

    QString                     mSelectPath;
};

#endif // UIPROPERTYWIDGET_H
