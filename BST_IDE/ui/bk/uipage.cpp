#include "uipage.h"
#include "ui_uipage.h"
#include "toolbar.h"

uiPage::uiPage(QWidget *parent, QString fileName) :
    TabPage(parent),
    ui(new Ui::uiPage)
{
    ui->setupUi(this);
    m_UiManager = 0;
    installEventFilter(this);

    connect(ui->graphicsView->scene, SIGNAL(sInitUI()), this, SLOT(slot_InitUI()));
    connect(ui->graphicsView->scene, SIGNAL(sEnableUI()), this, SLOT(slot_EnableUI()));
    connect(ui->graphicsView->scene, SIGNAL(sDisableUI()), this, SLOT(slot_DisableUI()));
    //>@
    ui->graphicsView->scene->SetUndoStack(m_UndoStack);
    //>@
    InitScene(fileName);

    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(slot_ComNumberUpdate()));
    m_Timer.start(1000);
}

uiPage::~uiPage()
{
    m_Timer.stop();
    ReleaseScene();
    delete ui;
}

void uiPage::slot_ComNumberUpdate()
{
    if(m_UiManager)
    {
        ui->ComNumber->display(m_UiManager->m_ComElementGroup.count());
    }
}

void uiPage::slot_InitUI()
{
    ui->BtnShowSetbar->setChecked(false);
    ui->BtnShowMsg->setChecked(false);
    ui->BtnShowPrgs->setChecked(false);
    on_BtnShowSetbar_clicked(false);
    on_BtnShowPrgs_clicked(false);
    on_BtnShowMsg_clicked(false);
}

void uiPage::slot_EnableUI()
{
    if(ui->graphicsView->isEnabled() == false)
        ui->graphicsView->setEnabled(true);
}

void uiPage::slot_DisableUI()
{
    if(ui->graphicsView->isEnabled())
        ui->graphicsView->setDisabled(true);
}

//>@初始化UI
void uiPage::InitScene(QString pFilePath)
{
    if(m_UiManager)
        delete m_UiManager;
    m_UiManager = new uiManager(pFilePath);
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
    ui->graphicsView->scene->InitUI(m_UiManager);
}

void uiPage::ReleaseScene()
{
    if(m_UiManager && m_UiManager->m_Valid)
    {
        //>@此处不做任何保存文件的提醒
        delete m_UiManager;
        m_UiManager = 0;
    }
}

bool uiPage::eventFilter(QObject *target, QEvent *event)
{
    if(target == this)
    {
        if(event->type() == QEvent::KeyPress)
        {
             QKeyEvent *k = static_cast<QKeyEvent *>(event);
             if(k->key() == Qt::Key_F5)
             {
                 slot_Refresh();
             }
             else if(k->modifiers() == Qt::CTRL && k->key() == Qt::Key_Z)
             {
                 m_UndoAction->trigger();
             }
             else if(k->modifiers() == Qt::CTRL && k->key() == Qt::Key_Y)
             {
                 m_RedoAction->trigger();
             }
             else if(k->modifiers() == Qt::CTRL && k->key() == Qt::Key_S)
             {
                 slot_Save();
             }
        }
    }
    return QWidget::eventFilter(target,event);
}

void uiPage::SyncTreeItem(FileTreeItem *pTreeItem)
{
    ui->graphicsView->scene->m_TreeItem = pTreeItem;
}

void uiPage::on_ZoomInBtn_clicked()
{
    int pos = ui->ZoomSlider->value();
    if(pos<100)
        ui->ZoomSlider->setValue(pos+1);
    else
        ui->ZoomSlider->setValue(99);
}

void uiPage::on_ZoomOutBtn_clicked()
{
    int pos = ui->ZoomSlider->value();
    if(pos>=10)
        ui->ZoomSlider->setValue(pos-1);
    else
        ui->ZoomSlider->setValue(10);
}

void uiPage::on_ZoomSlider_valueChanged(int value)
{
    if(!ui->graphicsView || value < 10 || value >= 100)
        return;
    double newScale = (double)(value+1) * 2.0 / 100.0;
    QMatrix oldMatrix = ui->graphicsView->matrix();
    ui->graphicsView->resetMatrix();
    ui->graphicsView->translate(oldMatrix.dx(), oldMatrix.dy());
    ui->graphicsView->scale(newScale, newScale);
}


void uiPage::on_BtnShowMsg_clicked(bool checked)
{
    COM_INFO* tmpMessage = ui->graphicsView->scene->m_ComDiagramGroup.value(QString("/MessageBar"));
    if(checked)
    {
        if(!(tmpMessage && tmpMessage->ComponentPtr))
        {
            QMessageBox::warning(this, tr("Warning"),
                                       tr("You haven't create the MessageBar.\n"
                                          "Plesase create it firstly!"),
                                       QMessageBox::Ok,
                                       QMessageBox::Ok);
            ui->BtnShowMsg->setChecked(false);
            return;
        }
        tmpMessage->ComponentPtr->show();
        tmpMessage->ComponentPtr->setZValue(D_PRGCTRL_ZVALUE);
    }
    else
    {
        if(tmpMessage && tmpMessage->ComponentPtr)
        {
            tmpMessage->ComponentPtr->hide();
            tmpMessage->ComponentPtr->setZValue(-D_PRGCTRL_ZVALUE);
        }
        ui->graphicsView->scene->hideFrame();
    }
}

void uiPage::slot_Save()
{
    ui->graphicsView->scene->SaveRelativeUI();
}

void uiPage::slot_Refresh()
{
    ui->graphicsView->scene->SaveUI();
    InitScene(m_UiManager->m_FileDir+m_UiManager->m_FileName);
}

void uiPage::on_CollisionBtn_clicked(bool checked)
{
    if(checked)
    {
        ui->graphicsView->scene->m_timer->start();
    }
    else
    {
        ui->graphicsView->scene->m_timer->stop();
        //>@区域检测
        QStringList ItemList;
        ItemList << QString("/FuncInfo") <<
                    QString("/MultiMedia") <<
                    QString("/TextBulletin") <<
                    QString("/IntegratedInfo") <<
                    QString("/ButtonOperate");
        COM_INFO *tmpComInfo = 0;
        for(int i=0;i<ItemList.count();i++)
        {
            tmpComInfo = ui->graphicsView->scene->m_ComDiagramGroup.value(ItemList.at(i));
            if(tmpComInfo != 0)
            {
                Effect_color(tmpComInfo->ComponentPtr, -1, -1, -1);
            }
        }
    }
}

void uiPage::on_SetComFront_clicked(bool checked)
{

}

void uiPage::on_BtnShowSetbar_clicked(bool checked)
{
    COM_INFO* tmpSetBar = ui->graphicsView->scene->m_ComDiagramGroup.value(QString("/SetBar"));
    if(checked)
    {
        if(!(tmpSetBar && tmpSetBar->ComponentPtr))
        {
            QMessageBox::warning(this, tr("Warning"),
                                       tr("You haven't create the SetBar.\n"
                                          "Plesase create it firstly!"),
                                       QMessageBox::Ok,
                                       QMessageBox::Ok);
            ui->BtnShowSetbar->setChecked(false);
            return;
        }
        tmpSetBar->ComponentPtr->show();
        tmpSetBar->ComponentPtr->setZValue(D_SETCTRL_ZVALUE);
    }
    else
    {
        if(tmpSetBar && tmpSetBar->ComponentPtr)
        {
            tmpSetBar->ComponentPtr->hide();
            tmpSetBar->ComponentPtr->setZValue(-D_SETCTRL_ZVALUE);
        }
        ui->graphicsView->scene->hideFrame();
    }
}

void uiPage::on_BtnShowPrgs_clicked(bool checked)
{
    COM_INFO* tmpProgressBar = ui->graphicsView->scene->m_ComDiagramGroup.value(QString("/ProgressBar"));
    if(checked)
    {
        if(!(tmpProgressBar && tmpProgressBar->ComponentPtr))
        {
            QMessageBox::warning(this, tr("Warning"),
                                       tr("You haven't create the ProgressBar.\n"
                                          "Plesase create it firstly!"),
                                       QMessageBox::Ok,
                                       QMessageBox::Ok);
            ui->BtnShowPrgs->setChecked(false);
            return;
        }
        tmpProgressBar->ComponentPtr->show();
        tmpProgressBar->ComponentPtr->setZValue(D_SETCTRL_ZVALUE);
    }
    else
    {
        if(tmpProgressBar && tmpProgressBar->ComponentPtr)
        {
            tmpProgressBar->ComponentPtr->hide();
            tmpProgressBar->ComponentPtr->setZValue(-D_SETCTRL_ZVALUE);
        }
        ui->graphicsView->scene->hideFrame();
    }
}

void uiPage::slot_Generate()
{
    ui->graphicsView->scene->RegularUI();
}

void uiPage::slot_Simulate()
{
    ProjectBar *tmpProjectBar = VAR_CAST<ProjectBar*>(GlobalPara.m_ProjectBar);
    if(m_UiManager && tmpProjectBar && tmpProjectBar->mSimulatorView)
    {
        tmpProjectBar->mSimulatorView->StartSimulator(m_UiManager->m_FileDir+m_UiManager->m_FileName);
    }
}


void uiPage::on_RotateBtn_clicked(bool checked)
{

}
