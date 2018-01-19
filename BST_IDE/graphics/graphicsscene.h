#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include "global.h"
#include "graphicsoperate.h"

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    GraphicsScene(QObject *parent = 0);
    ~GraphicsScene();

    virtual bool AddComponent(COM_TYPE pComType, QDomElement pComElement, QPointF pScenePos);
    bool DelComponent(GraphicsOperate *&pComPtr);
    bool SelectComponent(COM_TYPE pComType);
    bool SelectComponent(GraphicsOperate *pComPtr);

    virtual bool Initialize(QString pFileName);
    //>@界面函数
    virtual bool InitUI();     //>@初始化UI文件
    void ReleaseUI();  //>@释放UI文件
    void DeleteAllCom();  //>@释放Scene中的所有组件

    virtual void showFrame(GraphicsOperate *pComItem) {}
    virtual void hideFrame(bool pDelFlag = false) {}
    virtual void updateFrame() {}

public:
    DIRECTION            m_UiDirection;
    uiManager           *m_UiManager;
    GraphicsOperate     *m_SelectComponet;   //>@当前选中的控件数据
    QHash<COM_TYPE, GraphicsOperate*> m_ComDiagramGroup;   //>@xx.ui页面中的组件（subui和control）的GraphicsBase指针
};

#endif // GRAPHICSSCENE_H
