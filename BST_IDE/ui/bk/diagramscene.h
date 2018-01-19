#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include "global.h"
#include <QUndoCommand>
#include <QGraphicsScene>
#include "diagramframe.h"
#include "graphicsinc.h"
#include "diagramitem.h"
#include "zipfun.h"
#include "uipropertywidget.h"
#include "diagramsetting.h"
#include "treeitemclass.h"

class DiagramScene;
class DiagramFrame;
class RegularThread;

class DiagramAddCommand : public QUndoCommand
{
public:
    DiagramAddCommand(QString pComPath, QPointF pPoint, DiagramScene *pDiagramScene, QUndoCommand *parent = 0);

    bool CreateControl(QString pComPath, QPointF pPoint);
    void undo();
    void redo();
    bool IsValid() {return m_IsValid;}

private:
    QString       m_ComPath;
    COM_INFO     *m_ComInfo;
    DiagramScene *m_DiagramScene;
    bool          m_IsValid;
};

class DiagramDeleteCommand : public QUndoCommand
{
public:
    DiagramDeleteCommand(COM_INFO *pComInfo, DiagramScene *pDiagramScene, QUndoCommand *parent = 0);

    void undo();
    void redo();
    bool IsValid() {return m_IsValid;}

private:
    COM_INFO     *m_ComInfo;
    DiagramScene *m_DiagramScene;
    bool          m_IsValid;
};

class DiagramMoveCommand : public QUndoCommand
{
public:
    DiagramMoveCommand(COM_INFO *pComInfo, DiagramScene *pDiagramScene, QUndoCommand *parent = 0);

    void undo();
    void redo();
    bool IsValid() {return m_IsValid;}

private:
    COM_INFO     *m_ComInfo;
    COM_OPERTATE  m_DragOperate;
    QRectF        m_NewRect, m_OldRect;
    DiagramScene *m_DiagramScene;
    bool          m_IsValid;
};

class DiagramUpdateCommand : public QUndoCommand
{
public:
    DiagramUpdateCommand(COM_INFO *pComInfo, QList<STATE_INFO*> pSMList, DiagramScene *pDiagramScene, QUndoCommand *parent = 0);

    void undo();
    void redo();
    bool IsValid() {return m_IsValid;}

private:
    COM_INFO            *m_ComInfo;
    QList<STATE_INFO*>   m_NewSMList;
    QList<STATE_INFO*>   m_OldSMList;
    DiagramScene        *m_DiagramScene;
    bool                 m_IsValid;
};

class DiagramScene : public QGraphicsScene
{
    Q_OBJECT
public:
    DiagramScene(QObject *parent = 0);
    ~DiagramScene();

    void SetUndoStack(QUndoStack *pUndoStack) {m_UndoStack = pUndoStack;}

    void CreateMenuAction();
    void ReleaseMenuAction();

    QFont font() const { return myFont; }
    QColor noteColor() const  { return myNoteColor; }
    QColor frameColor() const { return myFrameColor; }
    void setFont(const QFont &font);
    void setFrameColor(const QColor &color);
    void setNoteColor(const QColor &color);
    bool CollisionDetecting(QGraphicsItem *item);

    void createFrame();
    void deleteFrame();
    void hideFrame();

    bool OperateComDiagram(COM_INFO* ComInfo, ITEM_OPERATECODE Code,
                           bool pSubControl=false, uiManager *pUiManager=0);
    bool OperateComponent(ITEM_OPERATECODE pCode, COM_TYPE pType, COM_INFO *pComInfo);

    void InitUI(uiManager *pUiManager);
    bool AddControl(QString Name);                          //>@从UI文件中加载控件
    void DeleteAllCom();

    void UpdateScene()  { deleteFrame(); update(); }

    //>@将资源保存至UI所在目录中，并自修改资源路径为相对路径
    void SaveRelativeUI();
    //>@将COM_INFO中的资源规整到目标ui中
    bool RegularComToManager(COM_INFO *pComInfo, uiManager *pUiManager);
    void RegularUIToOutputFloder();
    //>@规整主题
    void RegularUI();

    void InitRegularThread();
    void ReleaseRegularThread();

public slots:
    void slot_SettingDialog();
    void updateFrame(COM_INFO* ComInfo);
    void updatePropertyTree(COM_INFO *ComInfo);
    void updateUi(COM_INFO *pComInfo);
    void TimeoutEvent();
    void SaveUI();

    void updateCom();

    void slot_ComAdd();
    void slot_ComDel();

signals:
    void sMessage(QString pMsg, int pValue);
    void sInitUI();
    void sEnableUI();
    void sDisableUI();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);

public:
    QUndoStack      *m_UndoStack;   //>@从uipage继承而来
    QMenu           *m_SimulatorCom;
    QMenu           *m_ContexMenu;
    QAction         *m_CopyComRC,
                    *m_PasteComRC,
                    *m_ModifyCom,
                    *m_DelCom;

    FileTreeItem    *m_TreeItem;    //>@在项目管理窗口中的TreeItem

    QFont           myFont;
    QColor          myNoteColor;

    QTimer          *m_timer;  //>@用于碰撞检测

    uiManager      *m_UiManager;
    DiagramFrame    *frameItem;
    QColor          myFrameColor;
    COM_INFO        *m_Desktop;
    COM_INFO        *mySelectCom;   //>@当前选中的控件
    QList<STATE_INFO*> m_bkSMList;

    QHash<QString, COM_INFO*> m_ComDiagramGroup;   //>@xx.ui页面中的组件（subui和control）的COM_INFO指针

    int             m_ProgressBarValue;
    QString         m_InsertComName;

    QString         m_OutputPath;
    QString         m_OutputDir;

    RegularThread   *m_RegularThread;
};

class RegularThread: public QThread
{
    Q_OBJECT
public:
    RegularThread(DiagramScene * pQesp);
    ~RegularThread();
protected:
    virtual void run();
public:
    DiagramScene *  qesp;
};

#endif
