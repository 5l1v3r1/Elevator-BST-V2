#include "graphicsscene.h"

GraphicsScene::GraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
    m_UiManager = 0;
    m_SelectComponet = 0;
    m_UiDirection = Horizontal;
}

GraphicsScene::~GraphicsScene()
{
    ReleaseUI();
}

bool GraphicsScene::Initialize(QString pFileName)
{
    if(m_UiManager)
        delete m_UiManager;
    m_UiManager = new uiManager(pFileName);
    if(isFileExist(m_UiManager->m_FileDir+m_UiManager->m_FileName) == false)
    {
        IDE_TRACE_STR(m_UiManager->m_FileName);
        m_UiManager->NewUI();
    }
    else
    {
        IDE_TRACE_STR(m_UiManager->m_FileName);
        m_UiManager->OpenUI();
    }
    return InitUI();
}

bool GraphicsScene::InitUI()
{
    DeleteAllCom();
    //>@初始化主题编辑类
    if(!m_UiManager)
        return false;
    return true;
}

void GraphicsScene::ReleaseUI()
{
    DeleteAllCom();
    if(m_UiManager && m_UiManager->m_Valid)
    {
        //>@此处不做任何保存文件的提醒
        delete m_UiManager;
        m_UiManager = 0;
    }
}


void GraphicsScene::DeleteAllCom()
{
    hideFrame(true);
    //>@删除scene下的所有item
    QList<COM_TYPE> tmpComList = m_ComDiagramGroup.keys();
    for(int i=tmpComList.count()-1; i>=0; i--)
    {
        GraphicsOperate* tmpComItem = m_ComDiagramGroup.take(tmpComList.at(i));
        if(tmpComItem)
        {
            IDE_TRACE_STR(tmpComItem->m_ComPath);
            removeItem(tmpComItem);
            tmpComItem->deleteLater();
        }
    }
    m_ComDiagramGroup.clear();
}

bool GraphicsScene::AddComponent(COM_TYPE pComType, QDomElement pComElement, QPointF pScenePos)
{
    return true;
}

//>@删除某个控件时主要：
//>@  1、断开与其父的连接、
//>@  2、在Scene/m_UiManager/m_ComDiagramGroup中删掉自己以及子节点
bool GraphicsScene::DelComponent(GraphicsOperate *&pComPtr)
{
    if(!pComPtr)
        return true;
    if(m_SelectComponet == pComPtr)
        hideFrame();
    QList<QGraphicsItem *> tmpChildItems = pComPtr->childItems();
    for(int i=tmpChildItems.count()-1;i>=0;i--)
    {
        GraphicsOperate *tmpChildItem = (GraphicsOperate *)tmpChildItems.at(i);
        if(!tmpChildItem)
            continue;
        DelComponent(tmpChildItem);
    }
    COM_TYPE tmpType = (COM_TYPE)(pComPtr->data(COMTYPE).toInt());
    m_ComDiagramGroup.remove(tmpType);
    removeItem(pComPtr);
    pComPtr->DeleteComponent();
    pComPtr->deleteLater();
    pComPtr = 0;
    return true;
}

bool GraphicsScene::SelectComponent(COM_TYPE pComType)
{
    GraphicsOperate *tmpComItem = m_ComDiagramGroup.value(pComType);
    return SelectComponent(tmpComItem);
}

bool GraphicsScene::SelectComponent(GraphicsOperate *pComPtr)
{
    if(!pComPtr)
        return false;
    if(pComPtr == m_SelectComponet)
        return true;
    //>@如果更换选择控件，则之前被选择的控件需要停止特效操作。
    if(m_SelectComponet)
        m_SelectComponet->StopAnimate();
    m_SelectComponet = pComPtr; //>@保存当前选中的控件
    m_SelectComponet->RefreshParameter();
    updateFrame();  //>@将Frame更新为当前COMINFO的框
    return true;
}
