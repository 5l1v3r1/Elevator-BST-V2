#include "global.h"
#include "logicthememanager.h"
#ifndef IDE
#include "devicemanager.h"
#endif
#include "logicnetmanager.h"

LogicThemeManager::LogicThemeManager(QObject *parent)
    :LogicDevice(parent)
{
    m_UiView = 0;
    mDirection = Horizontal;
}

LogicThemeManager::~LogicThemeManager()
{
    ReleaseTheme();
}

//>@查找某种类型的文件
QStringList LogicThemeManager::SearchThemes(QString pDirPath)
{
    if(pDirPath.endsWith('/')==false)
        pDirPath.append('/');
    QDir dir = QDir(pDirPath);
    QStringList findList;
    QStringList folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < folders.size(); ++i)
    {
        QString tmpThemeName = folders[i];
        if(!tmpThemeName.endsWith('/'))
            tmpThemeName.append('/');
        if(QFile::exists(pDirPath + tmpThemeName + D_UINAME))
            findList.append(folders[i]);
    }
    return findList;
}

quint32 LogicThemeManager::InitPara()
{
    QDomNodeList tmpParaNodeList = m_ParaElement.childNodes();
    if(tmpParaNodeList.count() < 1)
        return 0;
    quint32 validcount = 0;
    QHBoxLayout* tmpHBoxLayout;
    for(int i=0;i<tmpParaNodeList.count();i++)
    {
        QDomElement tmpElement = tmpParaNodeList.at(i).toElement();
        if(tmpElement.isNull())
            continue;
        QString tmpName = tmpElement.tagName();
        if(!tmpName.compare("Location"))
        {
            QString tmpValue = tmpElement.text();
            mLocation = tmpValue;
            tmpHBoxLayout = CreateTextEdit(tmpName, tmpValue);
        }
        else if(!tmpName.compare("Direction"))
        {
            QString tmpValue = tmpElement.text();
            mDirection = GetDirection(tmpValue);
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), tmpValue);
        }
        else if(!tmpName.compare("RenderDev"))
        {
            QString tmpValue = tmpElement.text();
            mRenderDev = GetRenderDev(tmpValue);
            tmpHBoxLayout = CreateEnum(tmpName, tmpElement.attribute("List").split(';'), tmpValue);
        }
        else if(!tmpName.compare("ActiveTheme"))
        {
            mActiveTheme = tmpElement.text();
            continue;
        }
        else if(!tmpName.compare("Device"))
        {
            QStringList tmpDevList = tmpElement.text().split(';');
            AppendPHDevices(tmpDevList);
            tmpHBoxLayout = CreateCheck(tmpName, tmpElement.attribute("List").split(';'), tmpDevList);
        }
        else
        {
            continue;
        }
        validcount++;
        m_SettingGroup->m_VLayout->addWidget(CreateLine());
        m_SettingGroup->m_VLayout->addLayout(tmpHBoxLayout);
    }
    validcount++;

    UpdateThemeList();  //>@刷新当前主题列表
    m_SettingGroup->m_VLayout->addLayout(CreateEnum(QString("ActiveTheme"), mThemeNameList, mActiveTheme));
#ifdef DISPLAY
    //>@启动界面
    InitTheme();
#endif
    return validcount;
}

void LogicThemeManager::UpdateThemeList()
{
    //>@处理可用主题，先查看是否有用于执行的移动存储，最后本地存储
    PhysicalDevice *tmpPHDev = mPHDevices.value(P_MDISK);
    PhysicalMobileDisk *tmpMDiskDev = VAR_CAST<PhysicalMobileDisk *>(tmpPHDev);
    if(!(tmpMDiskDev && tmpMDiskDev->mExecutable && tmpMDiskDev->mDevState == S_PLUGGED))
    {
        tmpPHDev = mPHDevices.value(P_DISK);
    }
    mThemeNameList.clear();
    IDE_TRACE_INT(tmpPHDev);
    if(tmpPHDev)
    {
        mThemesPath = tmpPHDev->mDevPath + D_THEMEDIR;
        IDE_TRACE_STR(mThemesPath);
        //mThemesPath = QString("/THEME/");
        mThemeNameList = SearchThemes(mThemesPath);
        if(mThemeNameList.isEmpty())
            mActiveTheme.clear();
        else if(!mThemeNameList.contains(mActiveTheme))
            mActiveTheme = mThemeNameList.first();
        if(!mActiveTheme.isEmpty())
            mActiveThemePath = mThemesPath + mActiveTheme + QString("/") + D_UINAME;
    }
    else
    {
        mThemesPath.clear();
        mActiveTheme.clear();
        mActiveThemePath.clear();
    }
    IDE_TRACE_STR(mActiveThemePath);
}

void LogicThemeManager::UpdatePara(QObject* pObject)
{
    QString tmpObjectName = pObject->objectName();
    QDomElement tmpElement = m_ParaElement.firstChildElement(tmpObjectName);
    if(tmpElement.isNull())
        return;
    QString tmpName = tmpElement.tagName();
    if(!tmpName.compare("Location"))
    {
        TextEdit *tmpComponent = VAR_CAST<TextEdit *>(pObject);
        if(tmpComponent)
        {
            QString tmpLocation;
            if(!ModifyTextEdit(tmpComponent, tmpElement, tmpLocation, tmpComponent->text()))
            {
            }
            else
            {
                mLocation = tmpLocation;
            }
        }
    }
    else if(!tmpName.compare("Direction"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpDirection;
            if(!ModifyEnum(tmpComponent, tmpElement, tmpDirection, tmpComponent->currentText()))
            {
            }
            else
            {
                mDirection = GetDirection(tmpDirection);
            }
        }
    }
    else if(!tmpName.compare("RenderDev"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            QString tmpRenderDev;
            if(!ModifyEnum(tmpComponent, tmpElement, tmpRenderDev, tmpComponent->currentText()))
            {
            }
            else
            {
                mRenderDev = GetRenderDev(tmpRenderDev);;
            }
        }
    }
    else if(!tmpName.compare("ActiveTheme"))
    {
        ComboBox *tmpComponent = VAR_CAST<ComboBox *>(pObject);
        if(tmpComponent)
        {
            if(!ModifyEnum(tmpComponent, tmpElement, mActiveTheme, tmpComponent->currentText()))
            {
            }
        }
    }
    if(!tmpName.compare("Device"))
    {
        CheckList *tmpComponent = VAR_CAST<CheckList *>(pObject);
        if(tmpComponent)
        {
            QStringList tmpDevList = tmpComponent->GetSelectList();
            QStringList tmpList = PHDevicesAdded(tmpDevList, tmpComponent);
            if(!ModifyCheck(tmpComponent, tmpElement, tmpComponent->m_SelectList, tmpList))
            {
            }
        }
    }
}

void LogicThemeManager::DeviceStateChanged()
{
    PhysicalMobileDisk *tmpPHDev = VAR_CAST<PhysicalMobileDisk*>(sender());
    if(!tmpPHDev)
        return;
    //>@如果存储设备插上，并且当前没有正在运行的主题，则重启程序
    if(tmpPHDev->mExecutable && tmpPHDev->mDevState == S_PLUGGED && !mActiveThemePath.isEmpty())
    {
        UpdateThemeList();
        StartTheme();
    }
    //>@如果存储设备拔下，并且当前正在运行的主题在此设备中，则重启程序
    else if(tmpPHDev->mDevState == S_UNPLUGGED && mActiveThemePath.indexOf(tmpPHDev->mDevPath) >= 0)
    {
        UpdateThemeList();
        StartTheme();
    }
}

void LogicThemeManager::slot_ProcessConnect()
{
    //>@建立与网络的信号连接
#ifdef DISPLAY
    GuestNetManager *tmpNetManagerDev = VAR_CAST<GuestNetManager*>(m_DevManager->m_LGDevList.value(L_NetManager));
#elif IDE
    GuestNetManager *tmpNetManagerDev = 0;
#endif
    if(tmpNetManagerDev && m_UiView && m_UiView->mScene)
    {
        connect(tmpNetManagerDev, SIGNAL(sInform(QString)), m_UiView->mScene, SLOT(ShowMessage(QString)));
        connect(tmpNetManagerDev, SIGNAL(sMessage(QString)), m_UiView->mScene, SLOT(ShowMessage(QString)));
    }

    //>@建立与电梯标准协议的信号连接
    LogicElevator *tmpElevatorDev = VAR_CAST<LogicElevator*>(m_DevManager->m_LGDevList.value(L_Elevator));
    if(tmpElevatorDev && m_UiView && m_UiView->mScene && m_UiView->mScene->m_EventMap)
    {
        connect(tmpElevatorDev, SIGNAL(sArrowEvent(ARROWSTATE)), m_UiView->mScene->m_EventMap, SLOT(slot_ArrowEvent(ARROWSTATE)));
        connect(tmpElevatorDev, SIGNAL(sFloorEvent(quint32)), m_UiView->mScene->m_EventMap, SLOT(slot_FloorEvent(quint32)));
        connect(tmpElevatorDev, SIGNAL(sFunctionEvent(quint32)), m_UiView->mScene->m_EventMap, SLOT(slot_FunctionEvent(quint32)));
        connect(tmpElevatorDev, SIGNAL(sAudioEvent(quint32)), m_UiView->mScene->m_EventMap, SLOT(slot_AudioEvent(quint32)));
    }
}

void LogicThemeManager::InitTheme()
{
    ReleaseTheme();
    m_UiView = new UiView(this, mRenderDev);
#ifdef DISPLAY
    QTimer::singleShot(1000, this, SLOT(StartTheme()));
#endif
}

void LogicThemeManager::ReleaseTheme()
{
    if(!m_UiView)
        return;
    m_UiView->deleteLater();
    m_UiView = 0;
}

void LogicThemeManager::StartTheme()
{
    if(m_UiView)
    {
        if(!m_UiView->mScene->m_UiManager)  //>@如果主题已经打开，则不再重新打开。
        {
            IDE_TRACE_STR(mActiveThemePath);
            m_UiView->mScene->Initialize(mActiveThemePath);
        }
    }
}

void LogicThemeManager::PauseTheme()
{
    if(m_UiView != 0)
        m_UiView->mScene->PauseUI();
}

void LogicThemeManager::ResumeTheme()
{
    if(m_UiView != 0)
        m_UiView->mScene->PlayUI();
}

void LogicThemeManager::ShowTheme()
{
#ifdef DISPLAY
    QGridLayout *tmpLayout = VAR_CAST<QGridLayout*>(m_DevManager->layout());
    if(tmpLayout)
        tmpLayout->addWidget(m_UiView, mLocation.mRow, mLocation.mColumn);
#endif
}

/**********************************************************
//>@
**********************************************************/

UiView::UiView(LogicThemeManager *pLogicDev, RenderDev pRenderDev, QWidget *parent) :
    QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setDragMode(QGraphicsView::NoDrag);
    setWindowFlags(Qt::FramelessWindowHint);
    SetRenderDev(pRenderDev);
    mScene = new UiScene(pLogicDev);
    setScene(mScene);
}

UiView::~UiView()
{
    if(mScene)
        mScene->deleteLater();
}

void UiView::SetRenderDev(RenderDev pRenderDev)
{
    switch(pRenderDev)
    {
        case RenderQPainter:break;
        case RenderOpenGL:break;
        case RenderGDI:break;
        case RenderDriect2D:break;
        case RenderXV:break;
        default:break;
    }
}

void UiView::keyPressEvent(QKeyEvent *event)
{
    if(mScene != 0)
    {
        switch(event->key())
        {
            case Qt::Key_Up:
                mScene->KeyProcess(KEY_SET);
                break;
            case Qt::Key_Right:
                mScene->KeyProcess(KEY_UP);
                break;
            case Qt::Key_Down:
                mScene->KeyProcess(KEY_DN);
                break;
            case Qt::Key_Left:
                mScene->KeyProcess(KEY_BACK);
                break;
        }
    }
    return QGraphicsView::keyPressEvent(event);
}

/**********************************************************
//>@
**********************************************************/
UiScene::UiScene(QObject *parent) :
    GraphicsScene(parent)
{
#ifdef DISPLAY
    m_LogicThemeManager = VAR_CAST<LogicThemeManager *>(parent);
#endif
    setItemIndexMethod((QGraphicsScene::NoIndex));
    m_EventMap = new EventMap;
    m_IniParse = 0;
    m_SetBar = 0;
    m_MessageBar = 0;
    m_ProgressBar = 0;
    m_ManualState = STATE_NONE;
}

UiScene::~UiScene()
{
    if(m_EventMap)
        m_EventMap->deleteLater();
    ReleaseUI();
}

bool UiScene::Initialize(QString pFileName)
{
    //>@确定Theme文件的位置
    if(!QFile::exists(pFileName))
    {
        UpdateProgress(QString("Theme [%1] Unexist, Searching Default Theme!").arg(pFileName), 60);//进度条
        pFileName = QString(D_FSRCPATH + D_THEMEDIR + D_DEFAULTTHEMEDIR + D_UINAME);
        if(!QFile::exists(pFileName))
        {
            UpdateProgress(tr("Default Theme [%1] unexist, Please contact BST!").arg(pFileName), 60);//进度条
            goto InitError;
        }
    }
    if(!pFileName.endsWith(QString(".ui"), Qt::CaseInsensitive))
    {
        ShowMessage(tr("The ui name has no suffix, Please correct!"), 1000);
        goto InitError;
    }
    m_UiManager = new uiManager(pFileName);
    if(!(m_UiManager->OpenUI()))
    {
        ShowMessage(tr("The ui file is invalid, Please correct!"), 1000);
        goto InitError;
    }

    //>@打开环境文件
    m_IniParse = new iniParse(m_UiManager->m_FileDir + D_ENVIRONMENT);
    m_IniParse->setProperty("/Time", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    IDE_TRACE();
    //>@初始化映射表
    m_EventMap->Initialize(m_UiManager->m_FileDir + D_MAPNAME);
    IDE_TRACE();
    //>@设置主题方向
#ifdef DISPLAY
    if(m_LogicThemeManager->mDirection > Horizontal)
    {
        ShowMessage(tr("Direction is invalid!"), 1000);
        goto InitError;
    }
    IDE_TRACE();
    if(m_UiManager->m_DirectionPermisson != m_LogicThemeManager->mDirection)
    {
        if(m_UiManager->m_DirectionPermisson != HVBoth)
        {
            ShowMessage(tr("Direction is invalid!"), 1000);
            goto InitError;
        }
    }
    IDE_TRACE();
    m_UiDirection = m_LogicThemeManager->mDirection;
#endif

    //>@初始化界面
    InitUI();

    IDE_TRACE();
    return true;

InitError:
    SystemAudio(AUDIO_ERROR);
    ReleaseUI();
    return false;
}

void UiScene::UpdateProgress(QString pMsg, int pValue)
{
#ifdef DISPLAY
    m_LogicThemeManager->UpdateProgress(pMsg, pValue);
#endif
}

bool UiScene::InitUI()
{
    IDE_TRACE();
    DeleteAllCom();

    //>@加载当前主题中已有的所有控件
    QList<COM_TYPE> tmpComTypeList = m_UiManager->m_ComElementGroup.keys();
    int count = tmpComTypeList.count();
    if(count < 0)
        return false;
    int initValue = 60;
    qreal diff = (100-initValue)/(qreal)count;
    UpdateProgress(QString("Initial Theme:[%1]").arg(m_UiManager->m_FileDir+m_UiManager->m_FileName), initValue);//进度条

    IDE_TRACE();
    //>@设定Scene大小与Desktop的大小相同
    QDomElement tmpDesktopElement = m_UiManager->m_ComElementGroup.value(COM_DESKTOP);
    if(tmpDesktopElement.isNull())
        return false;
    QRectF rect = m_UiManager->GetComRect(tmpDesktopElement, m_UiDirection);
    setSceneRect(rect);
    //>@固定住VIEW的大小
    m_LogicThemeManager->m_UiView->setFixedSize(rect.size().toSize());

    IDE_TRACE();
    //>@加载控件
    if(AddComponent(COM_DESKTOP, tmpDesktopElement))
    {
        QString tmpMessage;
        for(int i=0;i<count;i++)
        {
            COM_TYPE tmpType = tmpComTypeList.at(i);
            COM_TYPE tmpParType = getParentComType(tmpType);
            QDomElement tmpParElement = m_UiManager->m_ComElementGroup.value(tmpParType);
            if(!AddComponent(tmpParType, tmpParElement))
            {
                UpdateProgress(QString("Load:[%1] error").arg(tmpParElement.tagName()), initValue+diff*i);
                continue;
            }
            QDomElement tmpElement = m_UiManager->m_ComElementGroup.value(tmpType);
            if(!AddComponent(tmpType, tmpElement))
                tmpMessage = QString("Load:[%1] error").arg(tmpElement.tagName());
            else
                tmpMessage = QString("Load:[%1] success").arg(tmpElement.tagName());
            UpdateProgress(tmpMessage, initValue+diff*i);
            IDE_TRACE();
        }
    }
    UpdateProgress(QString("Theme Load Complete!"), 100);//进度条

    IDE_TRACE();
    //>@显示Theme
#ifdef DISPLAY
    m_LogicThemeManager->ShowTheme();
#endif
    return true;
}

bool UiScene::PlayUI()
{
    QList<COM_TYPE> tmpComList = m_ComDiagramGroup.keys();
    for(int i=0;i<tmpComList.count();i++)
    {
        GraphicsOperate *tmpCom = m_ComDiagramGroup.value(tmpComList.at(i));
        if(tmpCom)
            tmpCom->Play();
    }
    emit sBeep(D_StartUI);
    return true;
}

bool UiScene::PauseUI()
{
    QList<COM_TYPE> tmpComList = m_ComDiagramGroup.keys();
    for(int i=0;i<tmpComList.count();i++)
    {
        GraphicsOperate *tmpCom = m_ComDiagramGroup.value(tmpComList.at(i));
        if(tmpCom)
            tmpCom->Stop();
    }
    emit sBeep(D_PauseUI);
    return true;
}

void UiScene::ShowMessage(QString Key, int pDelay)
{
    emit sShowInfo(Key, pDelay);
}

void UiScene::UpdateProgress(quint32 pValue)
{
    emit sUpdateProgress(pValue);
}

void UiScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
}

void UiScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QList<QGraphicsItem *> SelectItems = items(event->scenePos());
    if(SelectItems.count()>0)
    {
        for(int i=0;i<SelectItems.count();i++)
        {
            GraphicsOperate *tmp = qgraphicsitem_cast<GraphicsOperate*>(SelectItems.at(i));
            if(tmp)  //>@是否为QGraphicsWidget的type
            {
                tmp->MoveEvent(event);
                break;
            }
        }
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void UiScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QList<QGraphicsItem *> SelectItems = items(event->scenePos());
    if(SelectItems.count()>0)
    {
        for(int i=0;i<SelectItems.count();i++)
        {
            GraphicsOperate *tmp = qgraphicsitem_cast<GraphicsOperate*>(SelectItems.at(i));
            if(tmp)  //>@是否为QGraphicsWidget的type
            {
                tmp->ReleaseEvent(event);
                break;
            }
        }
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void UiScene::KeyProcess(KEY_OPERATE pKey)
{
    if(m_SetBar->m_Animator.state() == QTimeLine::Running)
        return;
    emit sBeep(D_KeyPress);
    switch(m_ManualState)
    {
        case STATE_NONE:        //>@点击SET按钮进入一级菜单
        {
            if(pKey == KEY_SET)
            {
                m_ManualState = STATE_ITEMUI;
                m_SetBar->Initialize();
            }
            break;
        }
        case STATE_ITEMUI:      //>@点击左右按键左右滑动，点击SET按钮设置窗口获得输入焦点
        {
            if(pKey == KEY_BACK)
            {
                m_ManualState = STATE_NONE;
                m_SetBar->Release();
                //>@显示“正在应用配置”的消息框，然后根据需要重启BST_DAEMON程序
                if(m_SetBar->NeedReboot())
                {
                    ShowMessage(QString("Reboot ..."), 20000);
                    //StartBST_DAEMON();
                }
                else
                {
                    ShowMessage(QString("Applying Configuration Infomation ..."), 1000);
                }
            }
            else if(pKey == KEY_SET)
            {
                if(m_SetBar->updateEffect(true))
                {
                    while(m_SetBar->m_Animator.state() == QTimeLine::Running)
                        QtSleep(1);
                    m_SetBar->GotoPage();
                    m_ManualState = STATE_CTRLUI;
                }
            }
            else if(pKey == KEY_UP)
            {
                m_SetBar->MoveLeft();
            }
            else if(pKey == KEY_DN)
            {
                m_SetBar->MoveRight();
            }
            break;
        }
        case STATE_CTRLUI:      //>@点击SET按钮设置当前输入框修改区域,点击左右按键左右移动焦点
        {
            if(pKey == KEY_BACK)
            {
                if(m_SetBar->updateEffect(false))
                {
                    m_ManualState = STATE_ITEMUI;
                }
            }
            else if(pKey == KEY_SET)
            {
                QWidget *tmpWidget = m_SetBar->GetFocusSubctrl();
                if(tmpWidget == 0)
                    return;
                if(m_SetBar->MoveFocusIn())
                    m_ManualState = STATE_FOCUSUI;
            }
            else if(pKey == KEY_UP)
            {
                m_SetBar->MoveFocusUp();
            }
            else if(pKey == KEY_DN)
            {
                m_SetBar->MoveFocusDown();
            }
            break;
        }
        case STATE_FOCUSUI:      //>@点击SET按钮设置当前输入框修改区域,点击左右按键左右移动焦点
        {
            if(pKey == KEY_BACK)
            {

                if(m_SetBar->MoveFocusOut())
                {
                    m_SetBar->LeaveModify();
                    m_ManualState = STATE_CTRLUI;
                }
            }
            else if(pKey == KEY_UP)
            {
                m_SetBar->ModifyValueUp();
            }
            else if(pKey == KEY_DN)
            {
                m_SetBar->ModifyValueDown();
            }
            break;
        }
    }
}

bool UiScene::AddComponent(COM_TYPE pComType, QDomElement &pComElement, QPointF pScenePos)
{
    if(pComType < COM_DESKTOP)
        return false;
    //>@如果存在此控件，则不再重复创建
    if(m_ComDiagramGroup.contains(pComType))
        return true;
    if(pComElement.isNull())
    {
        IDE_TRACE();
        return false;
    }
    //>@如果需要父控件,则需要查看当前场景是否存在父控件，如果没有则退出
    GraphicsOperate *tmpParComPtr = 0;
    COM_TYPE tmpParComType = getParentComType(pComType);
    if(tmpParComType >= COM_DESKTOP)
    {
        tmpParComPtr = m_ComDiagramGroup.value(tmpParComType);
        if(!tmpParComPtr)
            return false;
    }
    GraphicsOperate *tmpComPtr = 0;
    switch(pComType)
    {
        case COM_DESKTOP:
            m_Desktop = new GraphicsDesktop(tmpParComPtr);
            tmpComPtr = m_Desktop;
            addItem(tmpComPtr);
            QObject::connect(m_EventMap, SIGNAL(sDesktopAction(AREA_OPERATE,QVariant)),
                             m_Desktop, SLOT(updateEffect(AREA_OPERATE, QVariant)));
            break;
        case COM_SETBAR:
            m_SetBar = new GraphicsSetBar(tmpParComPtr);
#ifdef DISPLAY
            m_SetBar->SyncDevManager(m_LogicThemeManager->m_DevManager);
#endif
            m_SetBar->setZValue(D_SETCTRL_ZVALUE);
            m_SetBar->Release();
            tmpComPtr = m_SetBar;
            break;
        case COM_MESSAGEBAR:
            m_MessageBar = new GraphicsMessage(tmpParComPtr);
            tmpComPtr = m_MessageBar;
            m_MessageBar->setZValue(D_MSGCTRL_ZVALUE);
            connect(this, SIGNAL(sShowInfo(QString, int)), m_MessageBar, SLOT(info(QString, int)));
            connect(this, SIGNAL(sShowWarn(QString, int)), m_MessageBar, SLOT(warn(QString,int)));
            connect(this, SIGNAL(sShowError(QString, int)), m_MessageBar, SLOT(error(QString, int)));
            break;
        case COM_PROGRESSBAR:
            m_ProgressBar = new GraphicsProgress(tmpParComPtr);
            tmpComPtr = m_ProgressBar;
            m_ProgressBar->setZValue(D_PRGCTRL_ZVALUE);
            connect(this, SIGNAL(sUpdateProgress(quint32)), m_ProgressBar, SLOT(UpdateValue(quint32)));
            break;
        case COM_ANIMAL:
            tmpComPtr = new GraphicsAnimal(tmpParComPtr);
            break;
        case COM_FLOOR:
            tmpComPtr = new GraphicsFloor(tmpParComPtr);
            QObject::connect(m_EventMap, SIGNAL(sFloorAction(AREA_OPERATE,QVariant)),
                             tmpComPtr, SLOT(updateEffect(AREA_OPERATE, QVariant)));
            break;
        case COM_ARROW:
            tmpComPtr = new GraphicsArrow(tmpParComPtr);
            QObject::connect(m_EventMap, SIGNAL(sArrowAction(AREA_OPERATE,QVariant)),
                             tmpComPtr, SLOT(updateEffect(AREA_OPERATE, QVariant)));
            break;
        case COM_FUNCTION:
            tmpComPtr = new GraphicsFunction(tmpParComPtr);
            QObject::connect(m_EventMap, SIGNAL(sFunctionAction(AREA_OPERATE,QVariant)),
                             tmpComPtr, SLOT(updateEffect(AREA_OPERATE, QVariant)));
            break;
        case COM_VIDEO:
        {
#ifdef DISPLAY
            qreal tmpRotate = 0;
            PhysicalLCD *tmpLCD = VAR_CAST<PhysicalLCD *>(m_LogicThemeManager->m_DevManager->m_PHDevList.value(P_LCD));
            if(tmpLCD)
                tmpRotate = (qreal)tmpLCD->mRotation;
            tmpComPtr = new GraphicsVideo(tmpParComPtr, tmpRotate);
#elif IDE
            tmpComPtr = new GraphicsVideo(tmpParComPtr);
#endif
            QObject::connect(m_EventMap, SIGNAL(sVideoAction(AREA_OPERATE,QVariant)),
                             tmpComPtr, SLOT(updateEffect(AREA_OPERATE, QVariant)));
            break;
        }
        case COM_WEBVIEW:
#ifdef SURRPORT_WEBKIT
            tmpComPtr = new GraphicsWebview(tmpParComPtr);
#endif
            break;
        case COM_QML:
#ifdef SURRPORT_QML
            tmpComPtr = new GraphicsQML(tmpParComPtr);
#endif
            break;
        case COM_OPENGL:
#ifdef SURRPORT_3D
            tmpComPtr = new GraphicsOpenGL(tmpParComPtr);
#endif
            break;
        case COM_IMAGE:
            tmpComPtr = new GraphicsImage(tmpParComPtr);
            QObject::connect(m_EventMap, SIGNAL(sImageAction(AREA_OPERATE,QVariant)),
                             tmpComPtr, SLOT(updateEffect(AREA_OPERATE, QVariant)));
            break;
        case COM_AUDIO:
            tmpComPtr = new GraphicsAudio(tmpParComPtr);
            QObject::connect(m_EventMap, SIGNAL(sAudioAction(AREA_OPERATE,QVariant)),
                             tmpComPtr, SLOT(updateEffect(AREA_OPERATE, QVariant)));
            break;
        case COM_TEXT:
            tmpComPtr = new GraphicsText(tmpParComPtr);
            QObject::connect(m_EventMap, SIGNAL(sTextAction(AREA_OPERATE,QVariant)),
                             tmpComPtr, SLOT(updateEffect(AREA_OPERATE, QVariant)));
            break;
        case COM_LOGO:
            tmpComPtr = new GraphicsLogo(tmpParComPtr);
            QObject::connect(m_EventMap, SIGNAL(sLogoAction(AREA_OPERATE,QVariant)),
                             tmpComPtr, SLOT(updateEffect(AREA_OPERATE, QVariant)));
            break;
        case COM_WEATHER:
            tmpComPtr = new GraphicsWeather(tmpParComPtr);
            QObject::connect(m_EventMap, SIGNAL(sWeatherAction(AREA_OPERATE,QVariant)),
                             tmpComPtr, SLOT(updateEffect(AREA_OPERATE, QVariant)));
            break;
        case COM_TIME:
            tmpComPtr = new GraphicsClock(tmpParComPtr);
            QObject::connect(m_EventMap, SIGNAL(sClockAction(AREA_OPERATE,QVariant)),
                             tmpComPtr, SLOT(updateEffect(AREA_OPERATE, QVariant)));
            break;
        case COM_NAMEPLATE:
            tmpComPtr = new GraphicsNamePlate(tmpParComPtr);
            break;
        case COM_FLOORBTN:
            tmpComPtr = new GraphicsFloorButton(tmpParComPtr);
            break;
        case COM_KEYBOARDBTN:
            tmpComPtr = new GraphicsKeyboard(tmpParComPtr);
            break;
        case COM_PHONEBTN:
        case COM_EMERGENCYBTN:
        case COM_OPENBTN:
        case COM_CLOSEBTN:
        case COM_DISABILITYBTN:
            tmpComPtr = new GraphicsButton(tmpParComPtr);
            break;
        default:
            return false;
    }
    if(tmpComPtr == 0)
        return false;
    if(tmpParComPtr == 0)
        tmpComPtr->setZValue(0);
    else
        tmpComPtr->setZValue(tmpParComPtr->zValue()+2);
    tmpComPtr->InitScene(this);
    tmpComPtr->InitComponent(pComElement, (pComType==COM_DESKTOP)?(true):(false));

    QString tmpEnvirString;
    if(m_IniParse && m_IniParse->getProperty(tmpComPtr->m_ComElement.tagName(), tmpEnvirString))
        tmpComPtr->SetEnvironment(tmpEnvirString);

    m_ComDiagramGroup.insert(pComType, tmpComPtr);
    return true;
}

bool UiScene::RemoveComponent(COM_TYPE pComType)
{
    GraphicsOperate* tmpComItem = m_ComDiagramGroup.take(pComType);
    if(tmpComItem)
    {
        QString tmpEnvirString;
        if(m_IniParse && tmpComItem->GetEnvironment(tmpEnvirString))
            m_IniParse->setProperty(tmpComItem->m_ComElement.tagName(), tmpEnvirString);
        removeItem(tmpComItem);
        tmpComItem->deleteLater();
    }
    return true;
}

