#ifndef TABPAGEMANAGER_H
#define TABPAGEMANAGER_H

#include "global.h"
#include "findfileform.h"
#include "timeinstrument.h"
#include "middletoptabinit.h"
#include "middletoptabxml.h"
#include "newfileform.h"
#include "diagramsetting.h"
#include "uipage.h"
#include "upgradeform.h"
#include "propertytree.h"
#include "devicemanageform.h"
#include "stcpage.h"
#include "stoform.h"
#include "rcpage.h"
#include "loginform.h"
#include "sqlserverform.h"
#include "manufacturepage.h"
#include "deviceinfo.h"
#include "logquerypage.h"
#include "videoeditor.h"
#include "imageeditor.h"
#include "audioeditor.h"
#include "ftpform.h"
#include "eventmapform.h"
#include "streammediaserver.h"
#include "monitorform.h"
#include "fonteditor.h"
#include "projectmanager.h"
#include "networkmanagerform.h"
#include "ChatForm.h"
#include "mapsform.h"
#include "asciiconvertform.h"
#include "facedetectform.h"

class TabPageManager : public QTabWidget
{
    Q_OBJECT
public:
    TabPageManager(QWidget *parent = 0);
    ~TabPageManager();

    void CreateToolbar();

    bool AddTabPage(QString pFile, FILE_TYPE pType, QString pIconName);
    bool AddTabPage(PAGE_INFO *pFileInfo);
    bool DelTabPage(QString pFile);
    bool DelTabPage(PAGE_INFO *pPageInfo);
    bool DelTabPage(int tabNum);
    bool UpdateTabPage(QString pFile, QVariant pPara);

    bool isPageOpen(QString pFileName);  //>@标签上的文件名
    bool setActivePage(QString pFileName);
    bool setActivePage(int pIndex);
    PAGE_INFO *getActivePage();

    bool OperatePageItem(PAGE_INFO *&FileInfo, ITEM_OPERATECODE OperateCode);

signals:
    void sTabPageChanged();

public slots:
    void on_tabWidget_currentChanged(int index);
    void on_tabWidget_closetab(int tabNum);
public slots:
    void newFile();  //>@仅仅新建文件
    void OpenFile(PAGE_INFO *pFileInfo);
    void open();
    void save();
    void saveAs();
    void close();
    void print();
    void quit();
public slots:
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void find();
    void replace();
    void findPrevious();
    void findNext();
    void gotoSomeWhere();
public slots:
    void docCurPosition( int line, int col );
    void textBold();
    void textUnderline();
    void textItalic();
    void textFont();
    void alignLeft();
    void alignCenter();
    void alignRight();
    void alignJustify();

public:
    //>@QString表示文件的全路径
    QHash<QString, PAGE_INFO*>  m_TabPageGroup;     //>@当前打开的页面集合
    PAGE_INFO                  *m_CurPageInfo;

    FileToolBar         *fileToolBar;
    EditToolBar         *editToolBar;
    TextToolBar         *textToolBar;
};
#endif // TABPAGE_H
