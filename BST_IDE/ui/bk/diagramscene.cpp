#include "diagramscene.h"
#include "mainwindow.h"

DiagramScene::DiagramScene(QObject *parent) :
    QGraphicsScene(parent)
{
    m_UndoStack = 0;
    m_RegularThread = 0;
    myNoteColor = Qt::black;
    myFrameColor = Qt::red;
    m_TreeItem = 0;

    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(TimeoutEvent()));

    MainWindow* tmpMain = VAR_CAST<MainWindow*>(GlobalPara.m_MainWindow);
    connect(this, SIGNAL(updatePropertyTree(COM_INFO *)), tmpMain, SLOT(updatePropertyTree(COM_INFO *)));
    ProjectBar* tmpProjectBar = VAR_CAST<ProjectBar*>(GlobalPara.m_ProjectBar);
    connect((UiPropertyWidget*)(tmpProjectBar->mPropertyWidget), SIGNAL(sUpdate(COM_INFO*)), this, SLOT(updateUi(COM_INFO*)));
    connect((UiPropertyWidget*)(tmpProjectBar->mPropertyWidget), SIGNAL(sEffectSetting()), this, SLOT(slot_SettingDialog()));

    connect(this,SIGNAL(sMessage(QString, int)), &GlobalPara, SLOT(updateProgress(QString, int)));

    CreateMenuAction();
}

DiagramScene::~DiagramScene()
{
    ReleaseRegularThread();
    DeleteAllCom();
    ReleaseMenuAction();
}

void DiagramScene::CreateMenuAction()
{
    m_ContexMenu = new QMenu;
    m_CopyComRC = m_ContexMenu->addAction(QIcon(":/Menu/rc/Menu/copy.png"), tr("Copy"),
                                                       this, SLOT(SLOT_ComCopyRC()));
    m_PasteComRC = m_ContexMenu->addAction(QIcon(":/Menu/rc/Menu/paste.png"), tr("Paste"),
                                                        this, SLOT(SLOT_ComPasteRC()));
    m_ContexMenu->addSeparator();
    m_ModifyCom = m_ContexMenu->addAction(QIcon(":/Menu/rc/Menu/toolView.png"), tr("Modify"),
                                                       this, SLOT(slot_SettingDialog()));
    m_DelCom = m_ContexMenu->addAction(QIcon(":/Menu/rc/Menu/minus.png"), tr("Delete"),
                                                    this, SLOT(SLOT_ComDel()));
    m_ContexMenu->addSeparator();
    m_SimulatorCom = m_ContexMenu->addMenu(QIcon(":/Menu/rc/Menu/debug.png"), tr("Simulator"));
}

void DiagramScene::ReleaseMenuAction()
{
    m_CopyComRC->deleteLater();
    m_PasteComRC->deleteLater();
    m_ModifyCom->deleteLater();
    m_DelCom->deleteLater();
    m_SimulatorCom->deleteLater();
    m_ContexMenu->deleteLater();
}

void DiagramScene::DeleteAllCom()
{
    mySelectCom = 0;
    //>@删除scene下的所有item
    QList<QGraphicsItem *> tmpList = this->items();
    for(int i=0;i<tmpList.count();i++)
    {
        QGraphicsItem *tmpItem = tmpList.at(i);
        this->removeItem(tmpItem);
    }
    QStringList tmpDiagramList = m_ComDiagramGroup.keys();
    for(int i=tmpDiagramList.count()-1; i>=0; i--)
    {
        COM_INFO* tmpComInfo = m_ComDiagramGroup.value(tmpDiagramList.at(i));
        if(tmpComInfo && tmpComInfo->ComponentPtr)
        {
            tmpComInfo->ComponentPtr->deleteLater();
        }
    }
    m_ComDiagramGroup.clear();
}

void DiagramScene::InitUI(uiManager *pUiManager)
{
    m_ProgressBarValue = 0;

    frameItem = 0;
    mySelectCom = 0;
    m_Desktop = 0;

    m_ComDiagramGroup.clear();   //>@xx.ui页面中的组件（subui和control）的COM_INFO指针

    m_ProgressBarValue = 0;
    m_InsertComName = QString("");

    DeleteAllCom();

    //>@初始化主题编辑类
    m_UiManager = pUiManager;
    if(m_UiManager == 0)
    {
        IDE_TRACE();
        return;
    }

    QStringList tmpStringList = m_UiManager->m_ComElementGroup.keys();

    //初始化进度条
    emit sMessage(QString("Loading UI module..."), m_ProgressBarValue += 10);
    int count = tmpStringList.count();
    int diff = (int)(90.0/((qreal)count));

    //>@首先添加Desktop
    if(m_UiManager->m_ComElementGroup.contains("/") && AddControl(QString("/")))
    {
        for(int i=0;i<count;i++)
        {
            QString tmpComPath = tmpStringList.at(i);
            COM_TYPE tmpType = getParentComType(tmpComPath);
            if(tmpType != COM_INVALID || tmpType != COM_NULL)
            {
                //>@先添加其父控件
                QString tmpParentComPath = getParentComPath(tmpComPath);
                QDomElement tmpElement = m_UiManager->m_ComElementGroup.value(tmpParentComPath);
                if(tmpElement.isNull())
                    continue;
                //>@添加父控件失败
                if(!AddControl(tmpParentComPath))
                    continue;
                //>@再添加控件
                tmpElement = m_UiManager->m_ComElementGroup.value(tmpParentComPath);
                if(tmpElement.isNull())
                    continue;
                AddControl(tmpComPath);
            }
            emit sMessage(QString("Initing UI ..."), m_ProgressBarValue += diff);//进度条
        }
    }
    emit sMessage(QString(""), m_ProgressBarValue = 0);//进度条
    emit sInitUI();

    deleteFrame();
}

void DiagramScene::slot_SettingDialog()
{
    if(!mySelectCom)
        return;
    DiagramSetting SettingDialog(m_UiManager, mySelectCom);
    int ret = SettingDialog.exec();
    if(ret == QDialog::Accepted)
    {
        mySelectCom->SetUpdateLevel(COM_INFO::UPDATESTATE);
        updateUi(mySelectCom);
    }
}

void DiagramScene::updateUi(COM_INFO *pComInfo)
{
    if(!pComInfo)
        return;
    hideFrame();

    switch(mySelectCom->GetUpdateLevel())
    {
        case COM_INFO::UPDATEGEOMETRY:
        {
            if(m_UndoStack)
                m_UndoStack->push(new DiagramMoveCommand(pComInfo, this));
            break;
        }
        case COM_INFO::UPDATESTATE:
        {
            if(m_UndoStack)
                m_UndoStack->push(new DiagramUpdateCommand(pComInfo, m_bkSMList, this));
            break;
        }
        case COM_INFO::UPDATEALL:
        {
            if(m_UndoStack)
            {
                m_UndoStack->push(new DiagramMoveCommand(pComInfo, this));
                m_UndoStack->push(new DiagramUpdateCommand(pComInfo, m_bkSMList, this));
            }
            break;
        }
        default:
            break;
    }
}

//>@用于边界检测
void DiagramScene::TimeoutEvent()
{
    //>@区域检测
    QStringList ItemList;
    ItemList << QString("/FuncInfo") <<
                QString("/MultiMedia") <<
                QString("/TextBulletin") <<
                QString("/IntegratedInfo") <<
                QString("/ButtonOperate");
    //>@所有区域
    COM_INFO *tmpComInfo = 0;
    for(int i=0;i<ItemList.count();i++)
    {
        tmpComInfo = m_ComDiagramGroup.value(ItemList.at(i));
        if(tmpComInfo != 0)
        {
            if(CollisionDetecting(tmpComInfo->ComponentPtr))
            {
                Effect_color(tmpComInfo->ComponentPtr, 150, 0, 0);
            }
            else
            {
                Effect_color(tmpComInfo->ComponentPtr, -1, -1, -1);
            }
        }
    }
}

//>@从UI文件中加载控件
bool DiagramScene::AddControl(QString Name)
{
    if(Name == QString("/"))
    {
        if(m_Desktop)
            return true;
        m_Desktop = new COM_INFO;
        m_Desktop->ParentComPtr = 0;
        m_Desktop->ComponentPtr = 0;
        m_Desktop->ChildComListPtr.clear();
        if(m_UiManager->CreateComDom(QString("/"), m_Desktop))
        {
            //>@根据桌面大小设置Scene大小
            setSceneRect(m_Desktop->ComRect.adjusted(0,0,2*StartPosX,2*StartPosX));
            return OperateComDiagram(m_Desktop, CodeInsert);
        }
        else
        {
            m_Desktop->deleteLater();
            m_Desktop = 0;
        }
    }
    else
    {
        COM_INFO *tmpCom = new COM_INFO;
        tmpCom->ParentComPtr = 0;
        tmpCom->ComponentPtr = 0;
        tmpCom->ChildComListPtr.clear();
        if(m_UiManager->CreateComDom(Name, tmpCom))
        {
            return OperateComDiagram(tmpCom, CodeInsert);
        }
        else
        {
            tmpCom->deleteLater();
        }
    }
    return false;
}

void DiagramScene::setFrameColor(const QColor &color)
{
    myFrameColor = color;
    if (COM_FRAME == selectedItems().first()->type())
    {
        DiagramFrame *item = qgraphicsitem_cast<DiagramFrame *>(selectedItems().first());
        item->setColor(myFrameColor);
        update();
    }
    else if(frameItem!=0)
    {
        frameItem->setColor(myFrameColor);
        update();
    }
}

void DiagramScene::setNoteColor(const QColor &color)
{
    myNoteColor = color;
    if (COM_NOTE == selectedItems().first()->type())
    {
        DiagramNoteItem *item = qgraphicsitem_cast<DiagramNoteItem *>(selectedItems().first());
        item->setDefaultTextColor(myNoteColor);
    }
}

void DiagramScene::setFont(const QFont &font)
{
    myFont = font;
    if (COM_NOTE == selectedItems().first()->type())
    {
        DiagramNoteItem *item = qgraphicsitem_cast<DiagramNoteItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramNoteItem
        if (item)
            item->setFont(myFont);
    }
}

bool DiagramScene::CollisionDetecting(QGraphicsItem *item)
{
    if(item == 0)
        return false;
    //>@只有区域控件做碰撞检测
    QString tmpString = item->data(COMNAME).toString();
    if( tmpString != QString("/FuncInfo") &&
        tmpString != QString("/MultiMedia") &&
        tmpString != QString("/TextBulletin") &&
        tmpString != QString("/IntegratedInfo") &&
        tmpString != QString("/ButtonOperate"))
    {
        IDE_TRACE();
        return false;
    }

    QList<QGraphicsItem *> CollisionList = collidingItems(item);
    for(int i=0; i<CollisionList.count(); i++)
    {
        QGraphicsItem *tmpItem = CollisionList.at(i);
        if(tmpItem->type() == 11)
        {
            QString tmpSubString = tmpItem->data(COMNAME).toString();
            if( tmpSubString == QString("/FuncInfo") ||
                tmpSubString == QString("/MultiMedia") ||
                tmpSubString == QString("/TextBulletin") ||
                tmpSubString == QString("/IntegratedInfo") ||
                tmpSubString == QString("/ButtonOperate"))
            {
                return true;
            }
        }
    }
    return false;
}

void DiagramScene::deleteFrame()
{
    if(frameItem == 0)
        return;
    disconnect(frameItem, SIGNAL(updateControl()), 0, 0);
    removeItem(frameItem);
    frameItem->deleteLater();
    frameItem = 0;
    //>@注意要讲mySelectCom清零，否则下次点击此控件时会出现因相同而不创建Frame的情况，也即必须先点击其他控件后才能点击此控件。
    mySelectCom = 0;
}

void DiagramScene::createFrame()
{
    if(frameItem == 0)
    {
        frameItem = new DiagramFrame;
        addItem(frameItem);
        connect(frameItem, SIGNAL(updateControl()), this, SLOT(updateCom()));
    }
}

void DiagramScene::updateCom()
{
    updatePropertyTree(mySelectCom);
    if(m_UndoStack)
        m_UndoStack->push(new DiagramMoveCommand(mySelectCom, this));
    //>@同步到设备中
    //m_UiManager->CreateComDom()
    //tmpNetInfo->mNetManager->Set(mySelectCom->);
}

void DiagramScene::updateFrame(COM_INFO* ComInfo)
{
    if(ComInfo == 0)
        return;
    if(ComInfo->ComPath.isEmpty() || ComInfo->ComponentPtr == 0 || ComInfo->ComRect.isEmpty())
        return;
    createFrame();
    frameItem->InitFrame(ComInfo);
    frameItem->show();

    m_bkSMList = ComInfo->BackupStatemachine();

    updatePropertyTree(ComInfo);
}

void DiagramScene::updatePropertyTree(COM_INFO* ComInfo)
{
    if(ComInfo == 0)
        return;
    ProjectBar* tmpProjectBar = VAR_CAST<ProjectBar*>(GlobalPara.m_ProjectBar);
    if(tmpProjectBar && tmpProjectBar->mPropertyWidget)
        tmpProjectBar->mPropertyWidget->RefreshTree(ComInfo);
}

void DiagramScene::hideFrame()
{
    if(frameItem == 0)
        return;
    frameItem->hide();
}

void DiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton)
    {
        if(m_ContexMenu)
        {
            QList<QGraphicsItem *> SelectItems = items(mouseEvent->scenePos());
            if(SelectItems.count()>0)
            {
                GraphicsBase *tmpCom = 0;
                for(int i=0;i<SelectItems.count();i++)
                {
                    tmpCom = VAR_CAST<GraphicsBase *>(SelectItems.at(i));
                    if(tmpCom == 0)  //>@是否为GraphicsBase
                        continue;
                    break;
                }
                if(!tmpCom)
                {
                    IDE_TRACE();
                    return;
                }
                COM_TYPE tmpType = (COM_TYPE)(tmpCom->data(COMTYPE).toInt());
                if(tmpType < COM_DESKTOP)
                    return;
                if(tmpType == COM_DESKTOP)
                {
                    m_CopyComRC->setEnabled(false);
                    m_PasteComRC->setEnabled(false);
                    m_ModifyCom->setEnabled(true);
                    m_DelCom->setDisabled(false);
                }
                else if(tmpType >= COM_SETBAR)
                {
                    m_CopyComRC->setEnabled(true);
                    m_PasteComRC->setEnabled(true);
                    m_ModifyCom->setEnabled(true);
                    m_DelCom->setEnabled(true);
                }
                if(m_SimulatorCom)
                    m_SimulatorCom->clear();


                QList<QAction*> tmpActionList;
                tmpActionList = tmpCom->GetActionList();
                if(tmpActionList.count()>0)
                {
                    m_SimulatorCom->addActions(tmpActionList);
                }
                m_ContexMenu->exec(QCursor::pos());//在当前鼠标位置显示
            }
        }
        return;
    }
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::RightButton)
    {
        if(m_ContexMenu)
        {
            QList<QGraphicsItem *> SelectItems = items(mouseEvent->scenePos());
            if(SelectItems.count()>0)
            {
                DiagramFrame *tmpFrame = 0;
                for(int i=0;i<SelectItems.count();i++)
                {
                    tmpFrame = VAR_CAST<DiagramFrame *>(SelectItems.at(i));
                    if(tmpFrame == 0)  //>@是否为DiagramFrame
                        continue;
                    break;
                }
                if(!(tmpFrame && tmpFrame->m_SelectFlag))
                {
                    IDE_TRACE();
                    return;
                }
                if(tmpFrame->m_CurCom && tmpFrame->m_CurCom == mySelectCom)
                {
                    QGraphicsWidget *tmpWidget = mySelectCom->ComponentPtr;
                    if(tmpWidget == 0)
                        return;
                    COM_TYPE tmpType = (COM_TYPE)(tmpWidget->data(COMTYPE).toInt());
                    if(tmpType < COM_DESKTOP)
                        return;
                    if(tmpType == COM_DESKTOP)
                    {
                        m_CopyComRC->setEnabled(false);
                        m_PasteComRC->setEnabled(false);
                        m_ModifyCom->setEnabled(true);
                        m_DelCom->setDisabled(false);
                    }
                    else if(tmpType >= COM_SETBAR)
                    {
                        m_CopyComRC->setEnabled(true);
                        m_PasteComRC->setEnabled(true);
                        m_ModifyCom->setEnabled(true);
                        m_DelCom->setEnabled(true);
                    }
                    if(m_SimulatorCom)
                        m_SimulatorCom->clear();


                    QList<QAction*> tmpActionList;
                    GraphicsBase* tmpDesign = VAR_CAST<GraphicsBase*>(tmpWidget);
                    if((tmpDesign == 0)||(m_SimulatorCom == 0))
                        return;
                    tmpActionList = tmpDesign->GetActionList();
                    if(tmpActionList.count()>0)
                    {
                        m_SimulatorCom->addActions(tmpActionList);
                    }
                    m_ContexMenu->exec(QCursor::pos());//在当前鼠标位置显示
                }
            }
        }
        return;
    }
    else if(mouseEvent->button() != Qt::LeftButton)
    {
        IDE_TRACE();
        return;
    }
    QList<QGraphicsItem *> SelectItems = items(mouseEvent->scenePos());
    if(SelectItems.count()>0)
    {
        for(int i=0;i<SelectItems.count();i++)
        {
            GraphicsBase *tmp = VAR_CAST<GraphicsBase*>(SelectItems.at(i));
            if(tmp)  //>@是否为QGraphicsWidget的type
            {
                QString tmpString = tmp->data(COMNAME).toString();
                if(tmpString.isEmpty())  //>@一般来讲都是点击了此控件的子UI，主要指Button
                {
                    tmp = VAR_CAST<GraphicsBase*>(tmp->parentItem());
                    if(tmp == 0)
                        return;
                    tmpString = tmp->data(COMNAME).toString();
                }
                COM_INFO *tmpComInfo = m_ComDiagramGroup.value(tmpString);
                if(tmpComInfo == 0)
                {
                    IDE_TRACE_STR(tmpString);
                    return;
                }
                OperateComDiagram(tmpComInfo, CodeSelect);
                break;
            }
        }
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void DiagramScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}

void DiagramScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if(!event->mimeData()->hasFormat("COM"))
        return;
    QByteArray itemData = event->mimeData()->data("COM");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);
    dataStream>>m_InsertComName;
    IDE_TRACE_STR(m_InsertComName);
    if(m_UndoStack)
        m_UndoStack->push(new DiagramAddCommand(m_InsertComName, event->scenePos(), this));
}

void DiagramScene::slot_ComAdd()
{
    IDE_TRACE();
}

void DiagramScene::slot_ComDel()
{
    if(m_UndoStack)
        m_UndoStack->push(new DiagramDeleteCommand(mySelectCom, this));
    mySelectCom = 0;
}

void DiagramScene::SaveUI()
{
    QStringList tmpList = m_ComDiagramGroup.keys();

    //初始化进度条
    emit sMessage(tr("Saving UI module..."), m_ProgressBarValue += 10);
    int count = tmpList.count();
    int diff = (int)(90.0/((qreal)count));

    for(int i=count-1;i>=0;i--)
    {
        QString tmpString = tmpList.at(i);
        COM_INFO *tmpComInfo = m_ComDiagramGroup.value(tmpString);
        if(tmpComInfo == 0)
            continue;
        m_UiManager->ModifyComDom(tmpComInfo, false);
        emit sMessage(tr("Saving %1 ...").arg(tmpComInfo->ComPath), m_ProgressBarValue += diff);//进度条
    }
    m_UiManager->m_IsChanged = true;
    m_UiManager->SaveUI();

    emit sMessage(QString(""), m_ProgressBarValue = 0);//进度条
}

//>@将资源保存至UI所在目录中，并自修改资源路径
void DiagramScene::SaveRelativeUI()
{
    if(m_UiManager->isValid() == false)
        return;

    QStringList tmpList = m_ComDiagramGroup.keys();
    //初始化进度条
    emit sMessage(tr("Saving UI module to Relative Floder..."), m_ProgressBarValue += 10);
    int count = tmpList.count();
    int diff = (int)(90.0/((qreal)count));
    //>@修改Desktop
    m_UiManager->SaveRelativeComDom(m_Desktop);
    //>@创建其他有父窗口的控件
    for(int i=0;i<count;i++)
    {
        QString tmpComPath = tmpList.at(i);
        COM_TYPE tmpType = getParentComType(tmpComPath);
        if(tmpType != COM_INVALID || tmpType != COM_NULL)
        {
            //>@先添加其父控件
            QString tmpParentComPath = getParentComPath(tmpComPath);
            COM_INFO *tmpComInfo = m_ComDiagramGroup.value(tmpParentComPath);
            if(tmpComInfo == 0)
                continue;
            m_UiManager->SaveRelativeComDom(tmpComInfo);
            //>@再添加控件
            tmpComInfo = m_ComDiagramGroup.value(tmpComPath);
            if(tmpComInfo == 0)
                continue;
            m_UiManager->SaveRelativeComDom(tmpComInfo);
        }
        emit sMessage(QString("Saving regular UI ..."), m_ProgressBarValue += diff);//进度条
    }

    m_UiManager->m_IsChanged = true;
    m_UiManager->SaveUI();

    emit sMessage(QString(""), m_ProgressBarValue = 0);//进度条
}

//>@规整COM_INFO到一个新的uiManager中，比如将Resource文件夹中的控件压缩至Output，并自修改目标ui文件中的资源路径
bool DiagramScene::RegularComToManager(COM_INFO *pComInfo, uiManager *pUiManager)
{
    if(!(m_UiManager && m_UiManager->isValid()))
        return false;
    if(!(pComInfo && pComInfo->ComPath.isEmpty()==false))
        return false;
    if(!(pUiManager && pUiManager->isValid()))
        return false;
    //>@如果存在此控件，则退出
    QDomElement tmpElement = pUiManager->m_ComElementGroup.value(pComInfo->ComPath);
    if(tmpElement.isNull() == false)
    {
        IDE_TRACE_STR(pComInfo->ComPath);
        return true;
    }
    COM_TYPE tmpComType = getCurComType(pComInfo->ComPath);
    //>@资源拷贝
    COM_INFO *tmpComInfo = new COM_INFO;
    tmpComInfo->ComPath = pComInfo->ComPath;
    tmpComInfo->ComRect = pComInfo->ComRect;
    GraphicsBase *tmpDesignBase = VAR_CAST<GraphicsBase *>(pComInfo->ComponentPtr);
    for(int i=0;i<pComInfo->StateMachine.count();i++)
    {
        STATE_INFO *tmpNewState = new STATE_INFO;
        STATE_INFO *tmpState = pComInfo->StateMachine.at(i);
        tmpNewState->StateName = tmpState->StateName;
        tmpNewState->Effect.EffectType = tmpState->Effect.EffectType;
        tmpNewState->Effect.EffectPara = tmpState->Effect.EffectPara;
        for(int j=0;j<tmpState->Effect.RcFiles.count();j++)
        {
            RESOURCE_INFO tmpRc = tmpState->Effect.RcFiles.at(j);
            //>@根据组件不同修改压缩的分辨率
            QSize tmpSize;
            if(tmpDesignBase)
                tmpSize = tmpDesignBase->GetRcSize(tmpRc.RcName);
            if(m_UiManager->RegularFile(tmpComType, tmpRc, pUiManager->m_FileDir, tmpSize) == false)
                continue;
            tmpNewState->Effect.RcFiles.append(tmpRc);
        }
        tmpComInfo->StateMachine.append(tmpNewState);
    }
    return pUiManager->ModifyComDom(tmpComInfo, false);
}

void DiagramScene::RegularUIToOutputFloder()
{
    //>@首先保存UI
    SaveRelativeUI();
    //>@获取工程名，压缩包的名字为工程名
    QString tmpProjectPath = m_UiManager->m_FileDir;
    if(tmpProjectPath.endsWith('/') == false)
        tmpProjectPath.append('/');
    QString tmpProjectName = GetDirectoryName(tmpProjectPath);
    m_OutputDir = tmpProjectPath + OUTPUTDIR + QString("/") + tmpProjectName + QString("/");

    //初始化进度条
    m_ProgressBarValue = 0;
    emit sMessage(QString("Regular UI module to Output Folder..."), m_ProgressBarValue += 5);

    //>@删除工程OUTPUT文件夹中资源输出目录中的内容
    emit sMessage(QString("Deleting %1...").arg(m_OutputDir), m_ProgressBarValue += 5);
    if(!ClearFolder(m_OutputDir))
    {
        emit sMessage(tr("Can't clear folder, go to check %1 !").arg(m_OutputDir), m_ProgressBarValue = 99);
        QString tmpPath = QString("file:///%1").arg(m_OutputDir);
        QDesktopServices::openUrl(QUrl(tmpPath, QUrl::TolerantMode));
        return;
    }

    //>@在待保存的UI文件的工程OUTPUT文件夹目录下的与工程名相同的文件夹中创建一个新的XML文件
    uiManager *tmpUiManager = new uiManager(m_OutputDir + m_UiManager->m_FileName);
    tmpUiManager->NewUI();

    QStringList tmpList = m_ComDiagramGroup.keys();
    emit sMessage(QString("Start regular resources..."), m_ProgressBarValue += 5);
    int count = tmpList.count();
    int diff = (int)(90.0/((qreal)count));

    //>@修改Desktop
    RegularComToManager(m_Desktop, tmpUiManager);
    //>@创建其他有父窗口的控件
    for(int i=0;i<count;i++)
    {
        QString tmpComPath = tmpList.at(i);
        COM_TYPE tmpType = getParentComType(tmpComPath);
        if(tmpType != COM_INVALID || tmpType != COM_NULL)
        {
            //>@先添加其父控件
            QString tmpParentComPath = getParentComPath(tmpComPath);
            COM_INFO *tmpComInfo = m_ComDiagramGroup.value(tmpParentComPath);
            if(tmpComInfo == 0)
                continue;
            RegularComToManager(tmpComInfo, tmpUiManager);
            //>@再添加控件
            tmpComInfo = m_ComDiagramGroup.value(tmpComPath);
            if(tmpComInfo == 0)
                continue;
            RegularComToManager(tmpComInfo, tmpUiManager);
        }
        emit sMessage(QString("Regular %1...").arg(tmpComPath), m_ProgressBarValue += diff);//进度条
    }
    tmpUiManager->m_IsChanged = true;
    delete tmpUiManager;

    emit sMessage(QString(""), m_ProgressBarValue = 0);//进度条
}

void DiagramScene::RegularUI()
{
    InitRegularThread();
}

void DiagramScene::InitRegularThread()
{
    ReleaseRegularThread();
    m_RegularThread = new RegularThread(this);
    connect(m_RegularThread, SIGNAL(finished()),this,SIGNAL(sEnableUI()));
    m_RegularThread->start(QThread::LowPriority);
}

void DiagramScene::ReleaseRegularThread()
{
    if(m_RegularThread != 0)
    {
        if(m_RegularThread != 0)
        {
            if (m_RegularThread->isRunning())
            {
                if (QThread::currentThread() != m_RegularThread)
                    m_RegularThread->wait();
            }
        }
        m_RegularThread->quit();
        m_RegularThread->deleteLater();
        m_RegularThread = 0;
    }
}

RegularThread::RegularThread(DiagramScene* pQesp):
    QThread()
{
    this->qesp = pQesp;
}

RegularThread::~RegularThread()
{

}

void RegularThread::run()
{
    qesp->RegularUIToOutputFloder();
}
