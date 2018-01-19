#include "baseform.h"
#include "ui_baseform.h"

BaseForm::BaseForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BaseForm)
{
    ui->setupUi(this);
    connect(ui->newItemView,SIGNAL(clicked(QModelIndex)),this,SLOT(newItemViewClicked(QModelIndex)));

    updateTemplateTableWidget();
}

BaseForm::~BaseForm()
{
    delete ui;
}

void BaseForm::addTableWidgetItem(QString ItemName, QString IconName)
{
    QTableWidgetItem *tmpTableWidgetItem = new QTableWidgetItem(ItemName);
    tmpTableWidgetItem->setIcon(QIcon(IconName));
    int row = ui->templateTableWidget->rowCount();
    ui->templateTableWidget->insertRow(row);
    ui->templateTableWidget->setItem(row,0,tmpTableWidgetItem);
}

//>@初始化左边栏
void BaseForm::updateTemplateTableWidget()
{
    ui->templateTableWidget->verticalHeader()->setVisible(false);
    ui->templateTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    int count = GlobalPara.NewCategoryGroup.count();
    if(count <= 0)
        return ;

    QList<QString> tmpStringList = GlobalPara.NewCategoryGroup.keys();
    for(int i=0;i<count;i++)
    {
        QString tmpString = tmpStringList.at(i);
        if(tmpString == QString("Solution"))
            addTableWidgetItem(tmpString, ":/Page/rc/Page/Solution.png");
        else if(tmpString == QString("File"))
            addTableWidgetItem(tmpString, ":/Page/rc/Page/File.png");
    }

    //>@初始化显示Solution中的内容
    m_FileInfo.CategoryType = QString("Solution");
    updateNewItemView(m_FileInfo.CategoryType);
}

//>@根据左边栏中点击的项 初始化右上边栏
void BaseForm::updateNewItemView(QString Name)
{
    QList<QString> tmpStringList;
    tmpStringList.clear();
    bool isSolution = false;
    if(Name.compare(QString("Solution")) == 0)
    {
        isSolution = true;
        ui->newItemView->setSpacing(0);
        ui->newItemView->setIconSize(QSize(30,30));
        tmpStringList = GlobalPara.SolutionCategoryGroup.keys();
    }
    else if(Name.compare(QString("File")) == 0)
    {
        isSolution = false;
        ui->newItemView->setSpacing(0);
        ui->newItemView->setIconSize(QSize(30,30));
        tmpStringList = GlobalPara.FileCategoryGroup.keys();
    }
    newListViewModel.clear();
    for(int i=0;i<tmpStringList.count();i++)
    {
        QString string = tmpStringList.at(i);
        QString IconName;
        if(isSolution)
        {
            SolutionCategory tmpSolutionCategory = GlobalPara.SolutionCategoryGroup.value(string);
            IconName = tmpSolutionCategory.Icon;
        }
        else
        {
            FileCategory tmpFileCategory = GlobalPara.FileCategoryGroup.value(string);
            IconName = tmpFileCategory.Icon;
        }
        QStandardItem *item = new QStandardItem(string);
        QIcon tmpIcon = QIcon(IconName);
        //>@如果是未知类型则使用默认图标
        if(tmpIcon.isNull())
        {
            tmpIcon = QIcon(QString(":/Menu/rc/Menu/question.png"));
        }
        item->setIcon(QIcon(tmpIcon));
        if(i % 2 == 1)
        {
            QLinearGradient linearGrad(QPointF(0, 0), QPointF(200, 200));
            linearGrad.setColorAt(0, Qt::lightGray);
            linearGrad.setColorAt(1, Qt::white);
            QBrush brush(linearGrad);
            item->setBackground(brush);
        }
        newListViewModel.appendRow(item);
    }
    ui->newItemView->setModel(&newListViewModel);
    /*
    ui->newItemView->setViewMode(QListView::ListMode);
    ui->newItemView->setMovement(QListView::Static);
    ui->newItemView->setFlow(QListView::TopToBottom);
    ui->newItemView->setWrapping(true);
    */
}

void BaseForm::newItemViewClicked(QModelIndex index)
{
    QString tmpString = index.data().toString();
    if(m_FileInfo.CategoryType == QString("Solution"))
    {
        SolutionCategory tmpSolutionCategory = GlobalPara.SolutionCategoryGroup.value(tmpString);
        m_FileInfo.ItemType = tmpString;
        tmpString = tmpSolutionCategory.Description;
        if(tmpString.isEmpty()==false)
        {
            ui->descTextEdit->setText(tmpString);
        }
    }
    else
    {
        FileCategory tmpFileCategory = GlobalPara.FileCategoryGroup.value(tmpString);
        m_FileInfo.ItemType = tmpFileCategory.Suffix;
        tmpString = tmpFileCategory.Description;
        if(tmpString.isEmpty() == false)
        {
            ui->descTextEdit->setText(tmpString);
        }
    }
}

void BaseForm::on_templateTableWidget_clicked(const QModelIndex &index)
{
    QString tmpString = index.data().toString();
    m_FileInfo.CategoryType = tmpString;
    updateNewItemView(tmpString);
}

void BaseForm::HideLeftFrame(bool flag)
{
    if(flag)
        ui->templateTableWidget->hide();
    else
        ui->templateTableWidget->show();
}

void BaseForm::ShowProjects()
{
    m_FileInfo.CategoryType = QString("Solution");
    updateNewItemView(QString("Solution"));
}

void BaseForm::ShowFiles()
{
    m_FileInfo.CategoryType = QString("File");
    updateNewItemView(QString("File"));
}

void BaseForm::accept()
{
    QDialog::accept();
}

void BaseForm::reject()
{
    QDialog::reject();
}
