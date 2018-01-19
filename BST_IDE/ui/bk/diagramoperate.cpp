#include "diagramscene.h"

DiagramAddCommand::DiagramAddCommand(QString pComPath, QPointF pPoint, DiagramScene *pDiagramScene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    m_IsValid = true;
    m_ComInfo = 0;
    m_DiagramScene = 0;
    m_DiagramScene = pDiagramScene;
    if(m_DiagramScene == 0)
    {
        m_IsValid = false;
        return;
    }
    if(CreateControl(pComPath, pPoint) == false)
    {
        m_IsValid = false;
        return;
    }
    setText(QString("Add %1").arg(pComPath));
}

bool DiagramAddCommand::CreateControl(QString pComPath, QPointF pPoint)
{
    if(!m_IsValid)
        return false;
    QList<QGraphicsItem *> SelectItems = m_DiagramScene->items(pPoint);
    if(SelectItems.count()>0)
    {
        GraphicsBase *tmp = 0;
        for(int i=0;i<SelectItems.count();i++)
        {
            tmp = VAR_CAST<GraphicsBase*>(SelectItems.at(i));
            if(tmp == 0)  //>@是否为QGraphicsWidget的type
                continue;
            break;
        }
        if(tmp == 0)
        {
            IDE_TRACE();
            return false;
        }
        QString tmpParString = tmp->data(COMNAME).toString();
        COM_INFO *tmpParComInfo = m_DiagramScene->m_ComDiagramGroup.value(tmpParString);
        if(tmpParComInfo == 0)
        {
            IDE_TRACE_STR(tmpParString);
            return false;
        }
        //>@判断是否在其父窗口上创建
        tmpParString = getParentComPath(pComPath);
        if(tmpParComInfo->ComPath.compare(tmpParString))
        {
            IDE_TRACE_STR(tmpParString);
            return false;
        }
        m_ComInfo = new COM_INFO;
        m_ComInfo->ComPath = pComPath;
        //>@计算此控件的大小，保证子控件不超出父控件范围
        QRectF rect = tmpParComInfo->ComponentPtr->geometry();
        int right = rect.right();
        int bottom = rect.bottom();
        QPointF lefttop = pPoint - QPointF(StartPosX, StartPosY);
        int Width = (right - lefttop.x() > 100)?(100):(right - lefttop.x());
        int Height = (bottom - lefttop.y() > 100)?(100):(bottom - lefttop.y());
        QPointF tmpPoint = lefttop - rect.topLeft();
        m_ComInfo->ComRect = QRectF(tmpPoint, QSizeF(Width, Height));
        if(m_DiagramScene->OperateComDiagram(m_ComInfo, CodeInsert) == false)
            return false;
        return true;
    }
    return false;
}

void DiagramAddCommand::undo()
{
    if(!m_IsValid)
        return;
    m_DiagramScene->OperateComDiagram(m_ComInfo, CodeDelete);
}

void DiagramAddCommand::redo()
{
    if(!m_IsValid)
        return;
    m_DiagramScene->OperateComDiagram(m_ComInfo, CodeInsert);
}

DiagramDeleteCommand::DiagramDeleteCommand(COM_INFO *pComInfo, DiagramScene *pDiagramScene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    m_IsValid = true;
    m_ComInfo = 0;
    m_DiagramScene = 0;
    m_ComInfo = pComInfo;
    m_DiagramScene = pDiagramScene;
    if(m_DiagramScene == 0 || m_ComInfo == 0)
    {
        m_IsValid = false;
        return;
    }
    if(m_DiagramScene->OperateComDiagram(m_ComInfo, CodeDelete) == false)
    {
        m_IsValid = false;
        return;
    }
    setText(QObject::tr("Delete %1").arg(m_ComInfo->ComPath));
}

void DiagramDeleteCommand::undo()
{
    if(!m_IsValid)
        return;
    m_DiagramScene->OperateComDiagram(m_ComInfo, CodeInsert, true);
}

void DiagramDeleteCommand::redo()
{
    if(!m_IsValid)
        return;
    m_DiagramScene->OperateComDiagram(m_ComInfo, CodeDelete);
    m_DiagramScene->OperateComDiagram(m_DiagramScene->m_Desktop, CodeSelect);
}

DiagramMoveCommand::DiagramMoveCommand(COM_INFO *pComInfo, DiagramScene *pDiagramScene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    m_IsValid = true;
    m_ComInfo = 0;
    m_DiagramScene = 0;
    m_ComInfo = pComInfo;
    m_DiagramScene = pDiagramScene;
    if(!(m_DiagramScene && m_ComInfo && m_ComInfo->ComponentPtr))
    {
        m_IsValid = false;
        return;
    }
    m_NewRect = m_ComInfo->ComRect;
    m_OldRect = m_ComInfo->OldRect;
    m_DragOperate = m_ComInfo->DragOperate;

    switch(m_DragOperate)
    {
        case DRAGALL:
        {
            if(m_DiagramScene->OperateComDiagram(m_ComInfo, CodeMove) == false)
            {
                m_IsValid = false;
                return;
            }
            break;
        }
        case DRAGRIGHT:
        case DRAGBOTTOM:
        case DRAGRIGHTBOTTOM:
        {
            if(m_DiagramScene->OperateComDiagram(m_ComInfo, CodeMove) == false)
            {
                m_IsValid = false;
                return;
            }
            break;
        }
        case DRAGLEFT:
        case DRAGTOP:
        case DRAGLEFTTOP:
        case DRAGRIGHTTOP:
        case DRAGLEFTBOTTOM:
        {
            if(!(m_ComInfo->ParentComPtr && m_ComInfo->ParentComPtr->ComponentPtr))
            {
                m_IsValid = false;
                return;
            }
            m_ComInfo->ReleaseParent(m_ComInfo->ParentComPtr->ComponentPtr);
            if(m_DiagramScene->OperateComDiagram(m_ComInfo, CodeMove) == false)
            {
                m_ComInfo->RestoreParent();
                m_IsValid = false;
                return;
            }
            m_ComInfo->RestoreParent();
            break;
        }
        default:
        {
            m_IsValid = false;
            return;
        }
    }
    setText(QObject::tr("Move %1").arg(m_ComInfo->ComPath));
}

void DiagramMoveCommand::undo()
{
    if(!m_IsValid)
        return;
    switch(m_DragOperate)
    {
        case DRAGALL:
        {
            m_ComInfo->ComRect = m_OldRect;
            m_DiagramScene->OperateComDiagram(m_ComInfo, CodeMove);
            break;
        }
        case DRAGRIGHT:
        case DRAGBOTTOM:
        case DRAGRIGHTBOTTOM:
        {
            m_ComInfo->ComRect = m_OldRect;
            m_DiagramScene->OperateComDiagram(m_ComInfo, CodeMove);
            break;
        }
        case DRAGLEFT:
        case DRAGTOP:
        case DRAGLEFTTOP:
        case DRAGRIGHTTOP:
        case DRAGLEFTBOTTOM:
        {
            m_ComInfo->ReleaseParent(m_ComInfo->ParentComPtr->ComponentPtr);
            m_ComInfo->ComRect = m_OldRect;
            m_DiagramScene->OperateComDiagram(m_ComInfo, CodeMove);
            m_ComInfo->RestoreParent();
            break;
        }
        default:
        {
            m_IsValid = false;
            return;
        }
    }
    m_DiagramScene->OperateComDiagram(m_DiagramScene->m_Desktop, CodeSelect);

}

void DiagramMoveCommand::redo()
{
    if(!m_IsValid)
        return;
    switch(m_DragOperate)
    {
        case DRAGALL:
        {
            m_ComInfo->ComRect = m_NewRect;
            m_DiagramScene->OperateComDiagram(m_ComInfo, CodeMove);
            break;
        }
        case DRAGRIGHT:
        case DRAGBOTTOM:
        case DRAGRIGHTBOTTOM:
        {
            m_ComInfo->ComRect = m_NewRect;
            m_DiagramScene->OperateComDiagram(m_ComInfo, CodeMove);
            break;
        }
        case DRAGLEFT:
        case DRAGTOP:
        case DRAGLEFTTOP:
        case DRAGRIGHTTOP:
        case DRAGLEFTBOTTOM:
        {
            m_ComInfo->ReleaseParent(m_ComInfo->ParentComPtr->ComponentPtr);
            m_ComInfo->ComRect = m_NewRect;
            m_DiagramScene->OperateComDiagram(m_ComInfo, CodeMove);
            m_ComInfo->RestoreParent();
            break;
        }
        default:
        {
            m_IsValid = false;
            return;
        }
    }
}

DiagramUpdateCommand::DiagramUpdateCommand(COM_INFO *pComInfo, QList<STATE_INFO*> pSMList, DiagramScene *pDiagramScene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    m_IsValid = true;
    m_ComInfo =0;
    m_DiagramScene = 0;
    m_ComInfo = pComInfo;
    m_DiagramScene = pDiagramScene;
    if(!(m_DiagramScene && m_ComInfo && m_ComInfo->ComponentPtr))
    {
        m_IsValid = false;
        return;
    }
    //>@创建回滚的结构体
    m_NewSMList = m_ComInfo->StateMachine;
    m_OldSMList = pSMList;

    m_DiagramScene->OperateComDiagram(m_ComInfo, CodeUpdate);
    setText(QObject::tr("Update %1").arg(m_ComInfo->ComPath));
}

void DiagramUpdateCommand::undo()
{
    if(!m_IsValid)
        return;
    m_ComInfo->StateMachine = m_OldSMList;
    m_DiagramScene->OperateComDiagram(m_ComInfo, CodeUpdate);
    m_DiagramScene->updatePropertyTree(m_ComInfo);
}

void DiagramUpdateCommand::redo()
{
    if(!m_IsValid)
        return;
    m_ComInfo->StateMachine = m_NewSMList;
    m_DiagramScene->OperateComDiagram(m_ComInfo, CodeUpdate);
    m_DiagramScene->updatePropertyTree(m_ComInfo);
}

bool DiagramScene::OperateComDiagram(COM_INFO* ComInfo, ITEM_OPERATECODE Code,
                                     bool pSubControl, uiManager *pUiManager)
{
    if(!(ComInfo && (ComInfo->ComPath.isEmpty()==false)))
        return false;
    switch(Code)
    {
        case CodeInsert:
        {
            //>@如果存在此控件，则不再重复创建
            COM_INFO *tmpComInfo = m_ComDiagramGroup.value(ComInfo->ComPath);
            if(tmpComInfo)
            {
                IDE_TRACE_STR(tmpComInfo->ComPath);
                return false;
            }
            //>@如果添加桌面，则会删除所有控件
            if(ComInfo->ComPath.compare(QString("/"))==0)
            {
                //OperateComDiagram(m_ComDiagramGroup.value(ComInfo->ComPath), CodeDelete);
                m_ComDiagramGroup.clear();
                COM_TYPE tmpType = getCurComType(ComInfo->ComPath);
                if(OperateComponent(CodeInsert, tmpType, ComInfo) == false)
                {
                    IDE_TRACE();
                    return false;
                }
                //>@只要在创建桌面添加到Scene即可，其他的都以指定父窗口的方式添加到Scene中
                addItem(ComInfo->ComponentPtr);
            }
            else   //>@如果是添加控件
            {
                QString tmpString = getParentComPath(ComInfo->ComPath);
                if(tmpString.isNull())
                {
                    IDE_TRACE();
                    return false;
                }
                if(tmpString.isEmpty() == false)//>@如果需要父控件,则需要查看当前场景是否存在父控件
                {
                    ComInfo->ParentComPtr = m_ComDiagramGroup.value(tmpString);
                    if(ComInfo->ParentComPtr == 0)
                    {
                        IDE_TRACE_STR(tmpString);
                        return false;
                    }
                }
                COM_TYPE tmpType = getCurComType(ComInfo->ComPath);
                if(OperateComponent(CodeInsert, tmpType, ComInfo) == false)
                {
                    IDE_TRACE_STR(ComInfo->ComPath);
                    return false;
                }
            }
            m_ComDiagramGroup.insert(ComInfo->ComPath, ComInfo);
            //>@添加子控件
            if(pSubControl)
            {
                for(int i=0;i<ComInfo->ChildComListPtr.count();i++)
                {
                    COM_INFO* tmpCom = ComInfo->ChildComListPtr.at(i);
                    OperateComDiagram(tmpCom, CodeInsert, pSubControl);
                }
            }
            m_UiManager->AddComDom(ComInfo, pSubControl);
            OperateComDiagram(ComInfo, CodeUpdate, pSubControl);
            OperateComDiagram(ComInfo, CodeSelect);
            IDE_TRACE_STR(ComInfo->ComPath);
            break;
        }
        case CodeDelete:
        {
            //>@删除某个控件时主要：1、断开与其父的连接、2、在Scene/m_UiManager/m_ComDiagramGroup中删掉自己以及子节点；但COM_INFO以及子控件的COM_INFO都不进行改变，而且ChildComListPtr也不改变，否则回滚操作会找不不到子节点
            if(ComInfo->ParentComPtr)
                ComInfo->ParentComPtr->ChildComListPtr.removeOne(ComInfo);
            for(int i=ComInfo->ChildComListPtr.count()-1;i>=0;i--)
            {
                COM_INFO *tmpCom = ComInfo->ChildComListPtr.at(i);
                if(tmpCom == 0)
                    continue;
                m_UiManager->DeleteComDom(tmpCom);
                m_ComDiagramGroup.remove(tmpCom->ComPath);
                removeItem(tmpCom->ComponentPtr);
                COM_TYPE tmpType = getCurComType(tmpCom->ComPath);
                OperateComponent(CodeDelete, tmpType, tmpCom);
            }
            m_ComDiagramGroup.remove(ComInfo->ComPath);
            m_UiManager->DeleteComDom(ComInfo);
            if(ComInfo->ComponentPtr == 0)
                return false;
            removeItem(ComInfo->ComponentPtr);
            COM_TYPE tmpType = getCurComType(ComInfo->ComPath);
            OperateComponent(CodeDelete, tmpType, ComInfo);
            OperateComDiagram(m_Desktop, CodeSelect);
            break;
        }
        case CodeUpdate:  //>@更新高级特效部分（图片背景）
        {
            if(ComInfo->ComponentPtr == 0)
            {
                IDE_TRACE_STR(ComInfo->ComPath);
                return false;
            }
            GraphicsBase *tmpDesignBase = VAR_CAST<GraphicsBase *>(ComInfo->ComponentPtr);
            if(tmpDesignBase)
                return tmpDesignBase->InitEffect(ComInfo);
            return false;
        }
        case CodeRefresh:  //>@更新高级特效部分（图片背景）
        {
            if(ComInfo->ComponentPtr == 0)
            {
                IDE_TRACE_STR(ComInfo->ComPath);
                return false;
            }
            GraphicsBase *tmpDesignBase = VAR_CAST<GraphicsBase *>(ComInfo->ComponentPtr);
            if(tmpDesignBase)
                return tmpDesignBase->Refresh();
            return false;
        }
        case CodeMove:
        {
            if(ComInfo->ComPath.compare(QString("/")) == 0)  //>@Desktop不可移动
                return false;
            if(ComInfo->ComponentPtr == 0)
                return false;
            //>@判断ComInfo->ComRect是否有效
            if(ComInfo->ComRect.x()<0 || ComInfo->ComRect.y()<0 || !(ComInfo->ComRect.isValid()))
                return false;
            ComInfo->ComponentPtr->setGeometry(ComInfo->ComRect);
            OperateComDiagram(ComInfo, CodeRefresh);
            break;
        }
        case CodeSelect:
        {
            if(mySelectCom && mySelectCom->ComponentPtr)
            {
                GraphicsBase *tmpDesignBase = VAR_CAST<GraphicsBase *>(mySelectCom->ComponentPtr);
                if(tmpDesignBase)
                    tmpDesignBase->StopAnimate();
            }
            if(ComInfo->ComPath == QString("/"))
            {
                mySelectCom = m_Desktop;
                emit updatePropertyTree(m_Desktop);
                hideFrame();
                return true;
            }
            if(mySelectCom != ComInfo)
            {
                mySelectCom = ComInfo; //>@保存当前选中的控件
                updateFrame(ComInfo);  //>@将Frame更新为当前COMINFO的框
            }
            break;
        }
        default:
            break;
    }
    return true;
}

bool DiagramScene::OperateComponent(ITEM_OPERATECODE pCode, COM_TYPE pType, COM_INFO *pComInfo)
{
    if(!(pComInfo && (pComInfo->ComPath.isEmpty()==false)))
        return false;
    switch(pType)
    {
        case COM_DESKTOP:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    pComInfo->ComponentPtr = new DesignDesktop(0, ToSceneRect(pComInfo->ComRect));
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setFlag(QGraphicsItem::ItemIsMovable,false);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignDesktop *tmpItem = VAR_CAST<DesignDesktop *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_SETBAR:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignSetBar(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignSetBar *tmpItem = VAR_CAST<DesignSetBar *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_MESSAGEBAR:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignMessage(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignMessage *tmpItem = VAR_CAST<DesignMessage *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_PROGRESSBAR:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignProgress(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignProgress *tmpItem = VAR_CAST<DesignProgress *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_ANIMAL:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignAnimal(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignFuncInfo *tmpItem = VAR_CAST<DesignFuncInfo *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_FUNCINFO:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignFuncInfo(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignFuncInfo *tmpItem = VAR_CAST<DesignFuncInfo *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_FLOOR:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignFloor(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignFloor *tmpItem = VAR_CAST<DesignFloor *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_ARROW:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignArrow(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignArrow *tmpItem = VAR_CAST<DesignArrow *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_FUNCTION:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignFunction(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignFunction *tmpItem = VAR_CAST<DesignFunction *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_MULTIMEDIA:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignMultimedia(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignMultimedia *tmpItem = VAR_CAST<DesignMultimedia *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_VIDEO:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignVideo(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignVideo *tmpItem = VAR_CAST<DesignVideo *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_WEBVIEW:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignWebview(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignWebview *tmpItem = VAR_CAST<DesignWebview *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_QML:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignQML(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignQML *tmpItem = VAR_CAST<DesignQML *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_OPENGL:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignOpenGL(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignOpenGL *tmpItem = VAR_CAST<DesignOpenGL *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_IMAGE:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignImage(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignImage *tmpItem = VAR_CAST<DesignImage *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_AUDIO:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignAudio(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignAudio *tmpItem = VAR_CAST<DesignAudio *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_TEXTBULLETIN:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignTextBulletin(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignTextBulletin *tmpItem = VAR_CAST<DesignTextBulletin *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_TEXT:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignText(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignText *tmpItem = VAR_CAST<DesignText *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_INTEGRATEDINFO:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignIntegratedInfo(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignIntegratedInfo *tmpItem = VAR_CAST<DesignIntegratedInfo *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_LOGO:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignLogo(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignLogo *tmpItem = VAR_CAST<DesignLogo *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_WEATHER:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignWeather(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignWeather *tmpItem = VAR_CAST<DesignWeather *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_TIME:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignClock(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignClock *tmpItem = VAR_CAST<DesignClock *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_NAMEPLATE:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignNamePlate(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignNamePlate *tmpItem = VAR_CAST<DesignNamePlate *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_BUTTONOPERATE:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignButtonOperate(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignButtonOperate *tmpItem = VAR_CAST<DesignButtonOperate *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_FLOORBTN:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignFloorButton(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignFloorButton *tmpItem = VAR_CAST<DesignFloorButton *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_KEYBOARDBTN:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignKeyboard(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignKeyboard *tmpItem = VAR_CAST<DesignKeyboard *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case COM_PHONEBTN:
        case COM_EMERGENCYBTN:
        case COM_OPENBTN:
        case COM_CLOSEBTN:
        case COM_DISABILITYBTN:
        {
            switch(pCode)
            {
                case CodeInsert:
                {
                    if(!(pComInfo->ParentComPtr && pComInfo->ParentComPtr->ComponentPtr))
                    {
                        IDE_TRACE();
                        return false;
                    }
                    pComInfo->ComponentPtr = new DesignButton(pComInfo->ParentComPtr->ComponentPtr, pComInfo->ComRect);
                    if(pComInfo->ComponentPtr == 0)
                        return false;
                    pComInfo->ComponentPtr->setZValue(pComInfo->ParentComPtr->ComponentPtr->zValue()+2);
                    pComInfo->ParentComPtr->ChildComListPtr.append(pComInfo);
                    break;
                }
                case CodeDelete:
                {
                    if(pComInfo->ComponentPtr == 0)
                        return true;
                    DesignButton *tmpItem = VAR_CAST<DesignButton *>(pComInfo->ComponentPtr);
                    if(tmpItem == 0)
                    {
                        pComInfo->ComponentPtr->deleteLater();
                        pComInfo->ComponentPtr = 0;
                        return true;
                    }
                    tmpItem->deleteLater();
                    pComInfo->ComponentPtr = 0;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            return false;
    }
    return true;
}
