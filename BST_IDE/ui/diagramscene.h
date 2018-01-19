#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include "global.h"
#include <QUndoCommand>
#include <QGraphicsScene>
#include "diagramframe.h"
#include "graphicsinc.h"
#include "diagramcom.h"
#include "zipfun.h"
#include "propertytree.h"
#include "diagramsetting.h"
#include "treeitemclass.h"

class DiagramScene;
class DiagramFrame;
class RegularThread;

class DiagramAddCommand : public QUndoCommand
{
public:
    DiagramAddCommand(COM_TYPE pComType, QDomElement pComElement, QPointF pPos, DiagramScene *pDiagramScene, QUndoCommand *parent = 0);

    void undo();
    void redo();
    bool IsValid() {return m_IsValid;}

private:
    COM_TYPE             m_ComType;
    QPointF              m_InitScenePos;
    GraphicsOperate     *m_Component;
    QDomElement          m_ComElement;
    DiagramScene        *m_DiagramScene;
    bool                 m_IsValid;
};

class DiagramDeleteCommand : public QUndoCommand
{
public:
    DiagramDeleteCommand(GraphicsOperate *pComPtr, QUndoCommand *parent = 0);
    ~DiagramDeleteCommand();

    void undo();
    void redo();
    bool IsValid() {return m_IsValid;}

private:
    COM_TYPE             m_ComType;
    GraphicsOperate     *m_Component;
    QDomElement          m_OldElement;
    DiagramScene        *m_DiagramScene;
    bool                 m_IsValid;
};

class DiagramMoveCommand : public QUndoCommand
{
public:
    DiagramMoveCommand(GraphicsOperate *pComPtr, QRectF pNewSceneRect, QUndoCommand *parent = 0);

    void undo();
    void redo();
    bool IsValid() {return m_IsValid;}

private:
    GraphicsOperate     *m_Component;
    QRectF               m_NewRect, m_OldRect;
    bool                 m_IsValid;
};

class DiagramChangeEffectCommand : public QUndoCommand
{
public:
    DiagramChangeEffectCommand(GraphicsOperate *pComPtr, QString pStateName, QString pEffectName, QUndoCommand *parent = 0);

    void undo();
    void redo();
    bool IsValid() {return m_IsValid;}

private:
    GraphicsOperate     *m_Component;
    QString              m_StateName;
    QString              m_NewEffectName, m_OldEffectName;
    bool                 m_IsValid;
};

class DiagramSetEffectParaCommand : public QUndoCommand
{
public:
    DiagramSetEffectParaCommand(GraphicsOperate *pComPtr, QString pStateName, QString pParaName, QString pParaValue, QUndoCommand *parent = 0);

    void undo();
    void redo();
    bool IsValid() {return m_IsValid;}

private:
    GraphicsOperate     *m_Component;
    QString              m_StateName;
    QString              m_ParaName;
    QString              m_NewParaValue, m_OldParaValue;
    bool                 m_IsValid;
};

class DiagramModifyRcCommand : public QUndoCommand
{
public:
    DiagramModifyRcCommand(GraphicsOperate *pComPtr, QString pStateName, QString pRcName, QString pRcFile, QUndoCommand *parent = 0);

    void undo();
    void redo();
    bool IsValid() {return m_IsValid;}

private:
    GraphicsOperate     *m_Component;
    QString              m_StateName;
    QString              m_RcName;
    QString              m_NewRcFile, m_OldRcFile;
    bool                 m_IsValid;
};

class DiagramDelRcCommand : public QUndoCommand
{
public:
    DiagramDelRcCommand(GraphicsOperate *pComPtr, QString pStateName, QString pRcName, QUndoCommand *parent = 0);

    void undo();
    void redo();
    bool IsValid() {return m_IsValid;}

private:
    GraphicsOperate     *m_Component;
    QString              m_StateName;
    QString              m_RcName;
    QString              m_OldRcFile;
    bool                 m_IsValid;
};

class DiagramScene : public GraphicsScene
{
    Q_OBJECT
public:
    DiagramScene(QObject *parent = 0);
    ~DiagramScene();

    void SetUndoStack(QUndoStack *pUndoStack) {m_UndoStack = pUndoStack;}

    void CreateMenu();
    void ReleaseMenu();

    bool AddComponent(COM_TYPE pComType, QDomElement pComElement, QPointF pScenePos=QPointF());

    //>@界面函数
    bool InitUI();

    void showFrame(GraphicsOperate *pComItem);
    void hideFrame(bool pDelFlag = false);

    bool StackMoveComponent(GraphicsOperate *pComPtr, QRectF pNewSceneRect);
    bool StackChangeComEffect(GraphicsOperate *pComPtr, QString pStateName, QString pEffectName);
    bool StackSetComEffectPara(GraphicsOperate *pComPtr, QString pStateName, QString pParaName, QString pParaValue);
    bool StackModifyComRc(GraphicsOperate *pComPtr, QString pStateName, QString pRcName, QString pRcFile);
    bool StackDelComRc(GraphicsOperate *pComPtr, QString pStateName, QString pRcName);

    void UpdateScene()  { hideFrame(); update(); }
    bool CollisionDetecting(QGraphicsItem *item);

    //>@将资源保存至UI所在目录中，并自修改资源路径为相对路径
    void SaveRelativeUI();
    //>@规整COM_INFO到一个新的uiManager中，比如将Resource文件夹中的控件压缩至Output，并自修改目标ui文件中的资源路径
    bool RegularComToManager(GraphicsOperate *pComponent, uiManager *pUiManager);
    //>@将COM_INFO中的资源规整到目标ui中
    void RegularUIToOutputFloder();
    //>@规整主题
    void RegularUI();

    void InitRegularThread();
    void ReleaseRegularThread();

    void ReleasePropertyTree();

public slots:
    void slot_TimeoutEvent();
    void SaveUI();

    void updateFrame();
    void updatePropertyTree();

    void slot_DeleteCom();
    void slot_CopyCom();
    void slot_PasteCom();
    void slot_TestDialog();

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
    bool                 m_InitializeOK;
    QUndoStack          *m_UndoStack;   //>@从uipage继承而来

    QMenu               *m_ContexMenu;
    QAction             *m_CopyComRC,
                        *m_PasteComRC,
                        *m_SoftTestCom,
                        *m_DelCom;
    QMenu               *m_SimulatorCom;

    FileTreeItem        *m_TreeItem;    //>@在项目管理窗口中的TreeItem

    QTimer              *m_CollisionTimer;//>@用于碰撞检测

    int                  m_UiRcNickLabel;   //>@进行UI设计时，UI资源的别名序号，也即可以支持多个别名
    DiagramFrame        *m_FrameItem;
    RegularThread       *m_RegularThread;

    int                  m_ProgressBarValue;
    QString              m_InsertComName;

    QString              m_OutputPath;
    QString              m_OutputDir;
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
