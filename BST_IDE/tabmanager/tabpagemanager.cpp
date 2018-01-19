#include "tabpagemanager.h"
#include "toolbox.h"

TabPageManager::TabPageManager(QWidget *parent) :
    QTabWidget(parent)
{
    m_CurPageInfo = 0;
    fileToolBar = 0;
    editToolBar = 0;
    textToolBar = 0;
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(on_tabWidget_currentChanged(int)));
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(on_tabWidget_closetab(int)));
}

TabPageManager::~TabPageManager()
{
    fileToolBar->deleteLater();
    editToolBar->deleteLater();
    textToolBar->deleteLater();
    m_CurPageInfo = 0;
    for(int i=count()-1;i>=0;i--)
    {
        DelTabPage(i);
    }
}

bool TabPageManager::isPageOpen(QString pFileName)
{
    PAGE_INFO *tmpPage = m_TabPageGroup.value(pFileName);
    if(tmpPage)
    {
        return true;
    }
    return false;
}

PAGE_INFO *TabPageManager::getActivePage()
{
#if 0
    int index = currentIndex();
    QString activeTabText = tabToolTip(index);
    if(activeTabText.isEmpty())
    {
        IDE_TRACE();
        return (PAGE_INFO*)0;
    }
    return m_TabPageGroup.value(activeTabText);
#else
    return m_CurPageInfo;
#endif
}

bool TabPageManager::setActivePage(QString pFileName)
{
    for(int i=0;i<count();i++)
    {
        QString tmpString = tabToolTip(i);
        if(tmpString == pFileName)
            return setActivePage(i);
    }
    return false;
}

bool TabPageManager::setActivePage(int pIndex)
{
    setCurrentIndex(pIndex);
    return true;
}

void TabPageManager::on_tabWidget_currentChanged(int index)
{
    OperatePageItem(m_CurPageInfo, CodeUnActive);
    QString activeTabText = tabToolTip(index);
    PAGE_INFO *tmpFileInfo = 0;
    if(!activeTabText.isEmpty())
    {
        tmpFileInfo = m_TabPageGroup.value(activeTabText);
        if(tmpFileInfo)
            OperatePageItem(tmpFileInfo, CodeActive);
    }
    m_CurPageInfo = tmpFileInfo;
    emit sTabPageChanged();//>@删除最后一个页面时无法执行发送信号的程序。
}

void TabPageManager::on_tabWidget_closetab(int tabNum)
{
    QString closeTabText = tabToolTip(tabNum);
    if(closeTabText.isEmpty())
    {
        IDE_TRACE();
        return;
    }
    PAGE_INFO *tmpFileInfo = m_TabPageGroup.value(closeTabText);
    if(!tmpFileInfo)
    {
        IDE_TRACE();
        return;
    }
    if(tmpFileInfo == m_CurPageInfo)
    {
        int newTabNum = 0;
        if(tabNum > 0)
            newTabNum = tabNum -1;
        else
            newTabNum = 1;
        on_tabWidget_currentChanged(newTabNum);
    }
    m_TabPageGroup.remove(closeTabText);
    OperatePageItem(tmpFileInfo, CodeDelete);
    removeTab(tabNum);
}

void TabPageManager::newFile()
{
    newFileForm FileForm;
    FileForm.setWindowTitle(tr("new"));
    FileForm.setWindowIcon(QIcon(":/Menu/rc/Menu/new.png"));
    int ret = FileForm.exec();
    if(ret == QDialog::Accepted)
    {
        FILE_TYPE tmpFileType = getFileType(&(FileForm.m_FileInfo));
        QString fileName;
        QString tmpParDir;
        ProjectBar* tmpProjectBar = VAR_CAST<ProjectBar*>(GlobalPara.m_ProjectBar);
        if(tmpProjectBar && tmpProjectBar->mProjectManager &&
           tmpProjectBar->mProjectManager->m_DevToolBar &&
           tmpProjectBar->mProjectManager->m_DevToolBar->mDevTreeItem &&
           tmpProjectBar->mProjectManager->m_DevToolBar->mDevTreeItem->mInterfaceDevice)
        {
            tmpParDir = tmpProjectBar->mProjectManager->m_DevToolBar->mDevTreeItem->mInterfaceDevice->mBufferPath;
        }
        else
        {
            tmpParDir = QDir::currentPath();
        }
        if(tmpFileType >= TYPE_UI)
        {
            //>@判断是新建何种类型文件
            fileName = QFileDialog::getSaveFileName(0, tr("New File"),
                                                    tmpParDir,
                                                    tr("file (*%1)").arg(FileForm.m_FileInfo.ItemType));
            if (fileName.isEmpty())
                return;
            AddTabPage(fileName, tmpFileType, FileForm.m_FileInfo.IconName);
        }
    }
}

void TabPageManager::OpenFile(PAGE_INFO *pFileInfo)
{
    OperatePageItem(pFileInfo, CodeInsert);
}

void TabPageManager::open()
{
    static QString sCurrentPath;
    QString fileName = QFileDialog::getOpenFileName(0, tr("Open Solution or File"),
                                         sCurrentPath,
                                         tr("All file (*.*)"));
    if (fileName.isEmpty())
        return;
    sCurrentPath = getFileDirectory(fileName);
    FILE_TYPE tmpType = getFileType(fileName);
    QString tmpIcon;
    if(tmpType == TYPE_PRO)
    {
        tmpIcon = ":/Menu/rc/Menu/solution.png";
    }
    else
    {
        QString tmpSuffix = getFileSuffix(fileName).remove(".");
        FileCategory tmpFileCategory = GlobalPara.FileCategoryGroup.value(tmpSuffix);
        tmpIcon = tmpFileCategory.Icon;
    }
    AddTabPage(fileName, tmpType, tmpIcon);
}

void TabPageManager::save()
{
    int tabNum = currentIndex();
    QString closeTabText = tabText(tabNum);
    PAGE_INFO *tmpFileInfo = m_TabPageGroup.value(closeTabText);
    if(tmpFileInfo)
        OperatePageItem(tmpFileInfo, CodeSave);
}

void TabPageManager::saveAs()
{
    int tabNum = currentIndex();
    QString closeTabText = tabText(tabNum);
    PAGE_INFO *tmpFileInfo = m_TabPageGroup.value(closeTabText);
    if(!(tmpFileInfo && tmpFileInfo->FileType >= TYPE_UI))
    {
        return;
    }

    QString tmpSuffix = getFileSuffix(tmpFileInfo->FileType);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                     QDir::currentPath(),
                                                     tr("file (*.%1)").arg(tmpSuffix));
    if (fileName.isEmpty())
        return;
    tmpFileInfo->FileDir = getFileDirectory(fileName);
    tmpFileInfo->FileName = getFileName(fileName);
    OperatePageItem(tmpFileInfo, CodeSave);
}

void TabPageManager::close()
{

}

void TabPageManager::print()
{

}
void TabPageManager::quit()
{

}

void TabPageManager::undo()
{

}
void TabPageManager::redo()
{

}
void TabPageManager::cut()
{

}
void TabPageManager::copy()
{

}
void TabPageManager::paste()
{

}
void TabPageManager::selectAll()
{

}
void TabPageManager::find()
{

}
void TabPageManager::replace()
{

}
void TabPageManager::findPrevious()
{

}
void TabPageManager::findNext()
{

}
void TabPageManager::gotoSomeWhere()
{

}

void TabPageManager::docCurPosition( int line, int col )
{
    QString tmp = "Line: ";
    if ( line < 0 )
        line = 0;
    tmp += QString::number( line + 1 );
    tmp += "  Column: ";
    tmp += QString::number( col + 1 );
    GlobalPara.docEditStatus->setText(tmp);
}
void TabPageManager::textBold()
{

}
void TabPageManager::textUnderline()
{

}
void TabPageManager::textItalic()
{

}
void TabPageManager::textFont()
{
    bool ok = true;
    QFont font = QFontDialog::getFont(&ok, this);
}
void TabPageManager::alignLeft()
{

}
void TabPageManager::alignCenter()
{

}
void TabPageManager::alignRight()
{

}
void TabPageManager::alignJustify()
{

}

bool TabPageManager::AddTabPage(QString pFile, FILE_TYPE pType, QString pIconName)
{
    PAGE_INFO *tmpFileInfo = new PAGE_INFO;
    tmpFileInfo->PageDialogPtr = 0;
    tmpFileInfo->FileDir = getParentDirectory(pFile);
    tmpFileInfo->FileName = getFileName(pFile);
    tmpFileInfo->FileType = pType;
    tmpFileInfo->IconName = pIconName;
    return AddTabPage(tmpFileInfo);
}

bool TabPageManager::AddTabPage(PAGE_INFO *pFileInfo)
{
    if(!pFileInfo)
        return false;
    if(!OperatePageItem(pFileInfo, CodeInsert))
    {
        IDE_TRACE();
        return false;
    }
    return true;
}

bool TabPageManager::UpdateTabPage(QString pFile, QVariant pPara)
{
    PAGE_INFO *tmpFileInfo = m_TabPageGroup.value(pFile);
    if(!tmpFileInfo)
    {
        IDE_TRACE();
        return false;
    }
    tmpFileInfo->Parameter = pPara;
    return OperatePageItem(tmpFileInfo, CodeUpdate);
}

bool TabPageManager::DelTabPage(QString pFile)
{
    QString closeTabText;
    for(int i=0;i<count();i++)
    {
        closeTabText = tabToolTip(i);
        if(closeTabText.compare(pFile, Qt::CaseInsensitive) == 0)
        {
            int ret = QMessageBox::question(this, tr("Question"),
                                           tr("%1 already open.\n"
                                              "Do you want to close?").arg(pFile),
                                           QMessageBox::Yes | QMessageBox::No,
                                           QMessageBox::No);
            if(ret == QMessageBox::No)
            {
                return false;
            }
            PAGE_INFO *tmpFileInfo = m_TabPageGroup.value(closeTabText);
            if(tmpFileInfo)
            {
                OperatePageItem(tmpFileInfo, CodeDelete);
                m_TabPageGroup.remove(closeTabText);
                delete tmpFileInfo;
            }
            removeTab(i);
            return true;
        }
    }
    return true;
}

bool TabPageManager::DelTabPage(PAGE_INFO *pPageInfo)
{
    if(!pPageInfo)
    {
        IDE_TRACE();
        return false;
    }
    if(pPageInfo->PageDialogPtr)
    {
        int tabNum = indexOf(pPageInfo->PageDialogPtr);
        OperatePageItem(pPageInfo, CodeDelete);
        removeTab(tabNum);
    }
    m_TabPageGroup.remove(pPageInfo->FileDir+pPageInfo->FileName);
    //>@此处不删除PageInfo
    return true;
}

bool TabPageManager::DelTabPage(int tabNum)
{
    QString closeTabText = tabToolTip(tabNum);
    if(closeTabText.isEmpty())
    {
        IDE_TRACE();
        return false;
    }
    PAGE_INFO *tmpFileInfo = m_TabPageGroup.value(closeTabText);
    if(!tmpFileInfo)
    {
        IDE_TRACE();
        return false;
    }
    OperatePageItem(tmpFileInfo, CodeDelete);
    m_TabPageGroup.remove(closeTabText);
    removeTab(tabNum);
    delete tmpFileInfo;
    return true;
}

bool TabPageManager::OperatePageItem(PAGE_INFO *&FileInfo, ITEM_OPERATECODE OperateCode)
{
    //>@页面处理
    if(FileInfo == 0)
        return false;
    int num = 0;
    switch(OperateCode)
    {
        case CodeInsert:
        {
            //>@预处理，防止相同页面
            if(!FileInfo->FileDir.endsWith('/'))
                FileInfo->FileDir.append('/');
            if(isPageOpen(FileInfo->FileDir + FileInfo->FileName))
            {
                setActivePage(FileInfo->FileDir + FileInfo->FileName);
                return false;
            }
            switch(FileInfo->FileType)
            {
                case TYPE_UI:
                {
                    uiPage* tmpDlg = new uiPage(0, FileInfo->FileDir + FileInfo->FileName);
                    if(tmpDlg == NULL)
                        return false;
                    tmpDlg->SyncTreeItem(FileInfo->TreeItem);
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_MDP:
                {
                    DeviceManageForm* tmpDlg = new DeviceManageForm(FileInfo->FileDir + FileInfo->FileName, 0);
                    if(tmpDlg == NULL)
                        return false;
                    tmpDlg->SyncTreeItem(FileInfo->TreeItem);
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_STC:
                {
                    stcPage* tmpDlg = new stcPage(FileInfo->FileDir + FileInfo->FileName);
                    if(tmpDlg == NULL)
                        return false;
                    tmpDlg->SyncTreeItem(FileInfo->TreeItem);
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_MAP:
                {
                    EventMapForm* tmpDlg = new EventMapForm(FileInfo->FileDir + FileInfo->FileName, 0);
                    if(tmpDlg == NULL)
                        return false;
                    tmpDlg->SyncTreeItem(FileInfo->TreeItem);
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_START:
                {
                    MiddleTopTabInit* tmpDlg = new MiddleTopTabInit();
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_MANUFACTURE:
                {
                    ManufacturePage* tmpDlg = new ManufacturePage();
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_UPGRADE:
                {
                    UpgradeForm* tmpDlg = new UpgradeForm();
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_NETWORKMANAGER:
                {
                    NetWorkManagerForm* tmpDlg = new NetWorkManagerForm;
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_ASCIICODEC:
                {
                    AsciiConvertForm* tmpDlg = new AsciiConvertForm;
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_VIDEOEDITOR:
                {
                    VideoEditor* tmpDlg = new VideoEditor;
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_IMAGEEDITOR:
                {
                    ImageEditor* tmpDlg = new ImageEditor;
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_AUDIOEDITOR:
                {
                    AudioEditor* tmpDlg = new AudioEditor();
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_FONTEDITOR:
                {
                    FontEditor* tmpDlg = new FontEditor();
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_CHAT:
                {
                    ChatForm* tmpDlg = new ChatForm(FileInfo->FileDir + FileInfo->FileName);
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_MAPS:
                {
                    MapsForm* tmpDlg = new MapsForm();
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_XML:
                {
                    MiddleTopTabXML* tmpDlg = new MiddleTopTabXML(0, FileInfo->FileDir + FileInfo->FileName);
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_FACEDETECT:
                {
                    FaceDetectForm* tmpDlg = new FaceDetectForm(0);
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_FTP:
                {
                    FtpForm* tmpDlg = new FtpForm(FileInfo->FileName);
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_SMS:
                {
                    StreamMediaServer* tmpDlg = new StreamMediaServer;
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_SQLSERVER:
                {
                    SQLServerForm* tmpDlg = new SQLServerForm;
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case Type_MONITOR:
                {
                    MonitorForm* tmpDlg = new MonitorForm;
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                case TYPE_LOG:
                {
                    logQueryPage* tmpDlg = new logQueryPage(FileInfo->FileDir);
                    if(tmpDlg == NULL)
                        return false;
                    FileInfo->PageDialogPtr = tmpDlg;
                    break;
                }
                default:
                {
                    return false;
                }
            }
            num = addTab(FileInfo->PageDialogPtr, QIcon(FileInfo->IconName), FileInfo->FileName);
            //>@要注意，先设置ToolTip再设置活动窗口，否则在添加新页面时on_tabWidget_currentChanged会出现找不到ToolTip的bug。
            setTabToolTip(num, FileInfo->FileDir + FileInfo->FileName);
            m_TabPageGroup.insert(FileInfo->FileDir + FileInfo->FileName, FileInfo);
            //>@用于打开新页后将新页作为活动窗口
            setActivePage(num);
            //>@随机性出现addTab后立即回调进入on_tabWidget_currentChanged的情况，此时还没有进行setTabToolTip，因此会无效退出，下面的代码是为了确保一定会正确执行on_tabWidget_currentChanged。
            on_tabWidget_currentChanged(num);
            break;
        }
        case CodeDelete:
            //>@提醒是否保存
            if(FileInfo->PageDialogPtr)
            {
                if(!FileInfo->PageDialogPtr->isSaved())
                {
                    int ret = QMessageBox::question(this, tr("Question"),
                                                   tr("The file has changed,\n"
                                                      "Do you want to save firstly?"),
                                                   QMessageBox::Yes | QMessageBox::No,
                                                   QMessageBox::No);
                    if(ret == QMessageBox::Yes)
                    {
                        OperatePageItem(FileInfo, CodeSave);
                    }
                }
                FileInfo->PageDialogPtr->deleteLater();
                FileInfo->PageDialogPtr = 0;
            }
            break;
        case CodeActive:
            if(FileInfo->PageDialogPtr)
            {
                connect(fileToolBar->saveAct, SIGNAL(triggered()),
                        FileInfo->PageDialogPtr, SLOT(slot_Save()));
                connect(editToolBar->undoAct, SIGNAL(triggered()),
                        FileInfo->PageDialogPtr->m_UndoAction, SIGNAL(triggered()));
                connect(editToolBar->redoAct, SIGNAL(triggered()),
                        FileInfo->PageDialogPtr->m_RedoAction, SIGNAL(triggered()));
            }
            break;
        case CodeUnActive:
            if(FileInfo->PageDialogPtr)
            {
                disconnect(fileToolBar->saveAct, SIGNAL(triggered()),0,0);
                disconnect(editToolBar->undoAct, SIGNAL(triggered()),0,0);
                disconnect(editToolBar->redoAct, SIGNAL(triggered()),0,0);
            }
            break;
        case CodeSave:
        {
            if(FileInfo->PageDialogPtr)
                FileInfo->PageDialogPtr->slot_Save();
            break;
        }
        case CodeUpdate:
            break;
        default:
            return false;
    }
    return true;
}
