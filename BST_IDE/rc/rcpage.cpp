#include "rcpage.h"
#include "ui_rcpage.h"

rcPage::rcPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rcPage)
{
    ui->setupUi(this);
    InitRCUi();
}

rcPage::rcPage(QWidget *parent, QString fileName) :
    QWidget(parent),
    ui(new Ui::rcPage)
{
    ui->setupUi(this);
    InitRCUi();
}

rcPage::~rcPage()
{
    delete ui;
}

void rcPage::InitRCUi()
{
    connect(ui->BtnSearch, SIGNAL(clicked()), this, SLOT(on_BtnSearch_clicked()));
    connect(ui->Goto, SIGNAL(clicked()), this, SLOT(on_Goto_clicked()));
    connect(ui->Back, SIGNAL(clicked()), this, SLOT(on_Back_clicked()));
    connect(ui->Forward, SIGNAL(clicked()), this, SLOT(on_Forward_clicked()));
    connect(ui->UpFlorder, SIGNAL(clicked()), this, SLOT(on_UpFlorder_clicked()));
    connect(ui->refresh, SIGNAL(clicked()), this, SLOT(on_refresh_clicked()));
    connect(ui->ExplorerList, SIGNAL(clicked(QModelIndex)), this, SLOT(on_ExplorerList_clicked(QModelIndex index)));
    connect(ui->ExplorerList,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(changeIndex(QModelIndex)));
    connect(ui->Source_Add, SIGNAL(clicked()), this, SLOT(on_Source_Add_clicked()));
    connect(ui->Source_Del, SIGNAL(clicked()), this, SLOT(on_Source_Del_clicked()));

    QHeaderView* pHeader =ui->treeView->header();
    pHeader->setResizeMode(QHeaderView::ResizeToContents);
    pHeader->setStretchLastSection(false);

    pos=0;
    QStringList filter;
    dirModel=new QDirModel(filter,QDir::Dirs|QDir::NoDotAndDotDot,QDir::DirsFirst|QDir::Name|QDir::Type);

    ui->treeView->setModel(dirModel);
    ui->treeView->hideColumn(1); //TreeView.Column(1,2,3)
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);

    ui->ExplorerList->setViewMode(QListView::IconMode);  //Display Mode
    ui->ExplorerList->setMovement(QListView::Snap);
    ui->ExplorerList->setGridSize(QSize(60,60));
    //QToolButton
    ui->BtnGridScene->setDown(false);
    menu = new QMenu;
    IconAction = new QAction(QIcon("rc/menu/grid.png"),tr("Icon"),0);
    menu->addAction(IconAction);

    LineAction = new QAction(QIcon("rc/menu/grid.png"),tr("Line"),0);
    menu->addAction(LineAction);

    ui->BtnGridScene->setMenu(menu);
    //QToolButton
    connect(ui->BtnGridScene,SIGNAL(triggered(QAction*)),this,SLOT(View_Mode_Changed(QAction*)));

    selectionModel=ui->treeView->selectionModel();
    connect(ui->ExplorerList,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(changeIndex(QModelIndex)));
    connect(selectionModel,SIGNAL(currentChanged(const QModelIndex&,const QModelIndex&)),this,SLOT(isSelected(const QModelIndex&,const QModelIndex&)));

    action = new QAction(QIcon("rc/menu/grid.png"),"Property",0);
    ui->ExplorerList->addAction(action);
}

void rcPage::changeIndex(QModelIndex index)
{
    QFileInfo *info=new QFileInfo(dirModel1->filePath(index));
    if(info->isDir())
    {
        this->pathList.append(dirModel1->filePath(index));
        pos++;
        ui->ExplorerList->setRootIndex(index);
        ui->treeView->setCurrentIndex(dirModel->index(dirModel1->filePath(index)));
        GlobalPara.searchStatus->setText("");
        CurrentIndex = dirModel->index(dirModel1->filePath(index));
    }
}

void rcPage::isSelected(const QModelIndex& current,const QModelIndex& previous)
{
    if(current==previous)
    return;
    QString selectedPath=dirModel->filePath(current);
    OpenFlorder(selectedPath);
    ui->Address->setText(selectedPath);
}

void rcPage::on_BtnSearch_clicked()
{
    //遍历当前文件目录下面所有的文件
    QString serachpath = ui->SerachPath->text();
    QString serachcontent = ui->SearchContent->text();
    if(serachpath.isEmpty()||serachcontent.isEmpty())
        return;
    QStringList SearchFileList = findFilesInFloder( serachpath,serachcontent,true);
    serach(SearchFileList);
    GlobalPara.searchStatus->setText("");
}

void rcPage::serach(QStringList SearchFileList)
{
    QStringList tmplist = SplitString(SearchFileList.at(SerachNum),"/");
    QString DstPath = tmplist.at(0);
    int count = tmplist.count()-1;
    for(int i=1;i<count;i++)
    {
        DstPath =DstPath + "/" + tmplist.at(i) ;
    }
    OpenFlorder(DstPath);
    ui->ExplorerList->selectAll();
    SerachNum++;
    if(SerachNum == tmplist.count())
    {
        QMessageBox *tmpbox = new QMessageBox(0);
        tmpbox->setText("Serach Over");
        SerachNum = 0;
    }
    ui->ExplorerList->keyboardSearch(tmplist.at(count));
}

void rcPage::on_Goto_clicked()
{
    QString selectedPath = ui->Address->text();
    OpenFlorder(selectedPath);
    GlobalPara.searchStatus->setText("");
}

void rcPage::OpenFlorder(QString selectedPath)
{
    QStringList nameFilter;
    dirModel1=new QDirModel(nameFilter,QDir::AllEntries|QDir::NoDotAndDotDot,QDir::Name|QDir::DirsFirst|QDir::Type);
    dirModel1->setReadOnly(false);
    ui->ExplorerList->setModel(dirModel1);
    ui->ExplorerList->setRootIndex(dirModel1->index(selectedPath));

    QString path=selectedPath;
    QFileInfo *info=new QFileInfo(path);
    if(info->isDir())
    {
        int a,j=0;
        for(a=pathList.count()-pos;a>0;a--)
        {
            pathList.removeAt(pos+j);
        }
        this->pathList.append(selectedPath);
        pos++;
    }
    CurrentPath = selectedPath;
}

void rcPage::on_Back_clicked()
{
    if(pathList.count()<=0)
        return ;

    QString string;
    if(pos >= 2)
    {
        --pos;
        string = this->pathList.at(pos-1);
    }
    else
    {
        string = this->pathList.at(0);
    }
    ui->ExplorerList->setRootIndex(dirModel1->index(string));
    ui->Address->setText(string);
    CurrentPath = ui->Address->text();
    GlobalPara.searchStatus->setText("");
}

void rcPage::on_Forward_clicked()
{
    if(pos>pathList.count()-1&&pathList.count()>=0)
    {
        return;
    }
    QString string;
    if(pos >= 1)
    {
        ++pos;
        string=this->pathList.at(pos-1);
    }
    else
    {
        string=this->pathList.at(0);
    }
    ui->ExplorerList->setRootIndex(dirModel1->index(string));
    ui->Address->setText(string);
    CurrentPath = ui->Address->text();
    GlobalPara.searchStatus->setText("");
}

void rcPage::on_UpFlorder_clicked()
{
    QFileInfo *info=new QFileInfo(ui->Address->text());
    QDir dir=info->dir();
    QString string=dir.path();
    ui->ExplorerList->setRootIndex(dirModel1->index(string));
    ui->Address->setText(string);
    CurrentPath = ui->Address->text();
    GlobalPara.searchStatus->setText("");
}

void rcPage::on_refresh_clicked()
{
    OpenFlorder(CurrentPath);
    GlobalPara.searchStatus->setText("");
}

void rcPage::on_ExplorerList_clicked(const QModelIndex &index)
{
    QFileInfo *info=new QFileInfo(dirModel1->filePath(index));
    QString FileSize;
    qint64 tmpsize;
    tmpsize = info->size();

    int count = 0;
    while(tmpsize>1024)
    {
        tmpsize = tmpsize/1024;
        count ++;
    }
    if(count == 0)
    {
        FileSize = QString("FileSize:%1 Byte").arg(tmpsize);
    }
    if(count == 1)
    {
        FileSize = QString("FileSize:%1 K").arg(tmpsize);
    }
    if(count == 2)
    {
        FileSize = QString("FileSize:%1 M").arg(tmpsize);
    }
    if(count == 3)
    {
        FileSize = QString("FileSize:%1 G").arg(tmpsize);
    }

    QString FilePath = "Path:"+ info->absoluteFilePath();
    Current_File_Path = info->absoluteFilePath();
    QString property;
    if(info->isFile())
    {
        QStringList tmplist = SplitString(info->fileName(),".");
        int count = tmplist.count();
        property = "Type:" + tmplist.at(count-1)+" File:" + "    "+FileSize+"    "+FilePath;
    }
    if(info->isDir())
    {
        property = "Selected a Object";
    }

    GlobalPara.searchStatus->setText(property);
}

void rcPage::on_Source_Add_clicked()
{
    QStringList fileNames;
    fileNames = QFileDialog::getOpenFileNames(0,tr("Select one or more files to open"),
                                        QDir::currentPath()+"/Movies",
                                        tr( "Text (*.txt);;"
                                            "Images (*.svg *.png *.gif *.jpg *.bmp);;"
                                            "Video (*.avi *.mp4 *.swf *.rmvb *.flv *.mp3)"));
    //文件格式转换
    for(int i=0;i<fileNames.count();i++)
    {
        QStringList filetype = SplitString(fileNames.at(i),".");
        QString Tmp_File_Type = filetype.at(filetype.count()-1);
        int svg = Tmp_File_Type.compare("svg");
        int png = Tmp_File_Type.compare("png");
        int gif = Tmp_File_Type.compare("gif");
        int jpg = Tmp_File_Type.compare("jpg");
        int bmp = Tmp_File_Type.compare("bmp");
        int avi = Tmp_File_Type.compare("avi");
        int mp4 = Tmp_File_Type.compare("mp4");
        int rmvb= Tmp_File_Type.compare("rmvb");
        int flv = Tmp_File_Type.compare("flv");

        if(!(svg && png && gif && jpg && bmp))
        {
            IDE_TRACE();
            //picture convert
        }
        else if(!(avi && mp4 && rmvb &&flv))
        {
            //video convert
        }
    }
}

void rcPage::on_Source_Del_clicked()
{
    QMessageBox *msgBox=new QMessageBox(this);
    msgBox->setWindowTitle(tr("Delete"));
    msgBox->setText(tr("Delete %1 ?").arg(Current_File_Path));
    QPushButton *ok=new QPushButton(tr("Yes"));
    msgBox->addButton(ok,QMessageBox::YesRole);
    QPushButton *cancel=new QPushButton(tr("No"));
    msgBox->addButton(cancel,QMessageBox::NoRole);
    msgBox->exec();
    if(msgBox->clickedButton()==ok)
    {
        QFileInfo *info=new QFileInfo(Current_File_Path);
        if(info->isFile())
        {
            QFile::remove(Current_File_Path);
            GlobalPara.searchStatus->setText("");
        }
        else if(info->isDir())
        {
            removedirectory(Current_File_Path);
            GlobalPara.searchStatus->setText("");
        }
        QStringList tmppathlist = SplitString(Current_File_Path,"/");
        Current_File_Path = tmppathlist.at(0);
        for(int i=1;i<tmppathlist.count()-1;i++)
        {
            CurrentPath = CurrentPath + "/" + tmppathlist.at(i);
        }
        dirModel1->refresh();
    }
    dirModel->refresh(CurrentIndex);
}

void rcPage::removedirectory(QString tmp_del_dir)
{
    QDir dir(tmp_del_dir);
    QString tmpdir = "";
    if(!dir.exists())
    {
        return;
    }
    QFileInfoList fileInfoList = dir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;
        if(fileInfo.isDir())
        {
            tmpdir = tmp_del_dir + ("/") + fileInfo.fileName();
            removedirectory(tmpdir);
        }
        else if(fileInfo.isFile())
        {
            QFile tmpFile(fileInfo.fileName());
            dir.remove(tmpFile.fileName()); /**< 删除临时文件 */

        }
    }
    dir.cdUp();            /**< 返回上级目录，因为只有返回上级目录，才可以删除这个目录 */
    if(dir.exists(tmp_del_dir))
    {
        if(!dir.rmdir(tmp_del_dir))
        {
            return;
        }
    }

}
