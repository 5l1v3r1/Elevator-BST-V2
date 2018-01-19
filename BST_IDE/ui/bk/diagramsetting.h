#ifndef DIAGRAMSETTING_H
#define DIAGRAMSETTING_H

#include "global.h"
#include "diagramcom.h"

namespace Ui {
class DiagramSetting;
}

class EffectSetting : public QDialog
{
    Q_OBJECT
public:
    EffectSetting(QWidget *parent, QString pControl, QString pOldControl);
    bool CreateUi(QWidget *pParentWidget, QLayout *pLayout, QStringList pUiPara, QString pInitValue);

    QVBoxLayout *m_MainLayout;
    QString m_EffectPara;
    QStringList m_StringList;

    QDialogButtonBox *m_ButtonBox;
private slots:
    void accept();
    void reject();
};

class DiagramSetting : public QDialog
{
    Q_OBJECT
    
public:
    explicit DiagramSetting(uiManager *pManager, COM_INFO *pComInfo, QWidget *parent = 0);
    ~DiagramSetting();
    
    void DelTreeAllItem(QTreeWidgetItem *parentItem);
    QTreeWidgetItem *DelTreeItem(QTreeWidgetItem *pItem);
    QTreeWidgetItem *AddTreeItem(QTreeWidgetItem *parentItem, QStringList contentList, bool Editable);
    QTreeWidgetItem *AddTreeItem(QTreeWidgetItem *parentItem, STATE_INFO* pState);

    void InitUI();
    void UpdateTree(COM_INFO *pComInfo);

    void updateRcNameCombo();
    void SetRcNameCurrentIndex(int pIndex);

    void PreviewRC(QString pFile);

private slots:
    void on_EffectEditTree_itemSelectionChanged();  //>@点击资源树中的某项

    void on_StateMachineType_currentIndexChanged(const QString &arg1);  //>@状态机组合框改变
    void on_EffectType_currentIndexChanged(const QString &arg1);  //>@特效组合框改变

    void on_BtnRcBrowser_clicked();
    void on_BtnAdd_clicked();
    void on_BtnModify_clicked();
    void on_BtnDelete_clicked();
    void on_BtnClear_clicked();
    void on_BtnEffectParaEdit_clicked();

    void on_BtnRefresh_clicked();

    void on_RcName_currentIndexChanged(int index);

public:
    uiManager       *m_Manager;
    COM_INFO        *m_ComInfo;
    bool             m_IsValid;

    ComCategory     *m_ComCategory;

    QTreeWidgetItem* m_RootTreeItem;
    QTreeWidgetItem* m_CurSelectItem;

    STATEMACHINE    *m_CurState;
    QString          m_CurEffectPara;

    int              m_CurStateIndex;   //>@状态机组合框当前显示的项序号
    int              m_CurEffectIndex;  //>@特效组合框当前显示的项序号
    QString          m_CurRcDir;    //>@当前打开的文件所在的目录

private:
    Ui::DiagramSetting *ui;
};

#endif // DIAGRAMSETTING_H
