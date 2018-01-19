#include "uipage.h"
#include "ui_uipage.h"
#include "toolbar.h"

uiPage::uiPage(QWidget *parent, QString fileName) :
    TabPage(parent),
    ui(new Ui::uiPage)
{
    ui->setupUi(this);
    m_GraphicsView = ui->graphicsView;
    installEventFilter(this);

    connect(ui->graphicsView->scene, SIGNAL(sInitUI()), this, SLOT(slot_InitUI()));
    connect(ui->graphicsView->scene, SIGNAL(sEnableUI()), this, SLOT(slot_EnableUI()));
    connect(ui->graphicsView->scene, SIGNAL(sDisableUI()), this, SLOT(slot_DisableUI()));
    //>@
    ui->graphicsView->scene->SetUndoStack(m_UndoStack);
    //>@
    m_UiFilePath = fileName;
    ui->graphicsView->scene->Initialize(fileName);

    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(slot_ComNumberUpdate()));
    m_Timer.start(1000);
}

uiPage::~uiPage()
{
    if(ui->graphicsView->scene->m_TreeItem)
    {
        ui->graphicsView->scene->m_TreeItem->mFileInfo->PageDialogPtr = 0;
    }
    m_Timer.stop();
    delete ui;
}

void uiPage::slot_ComNumberUpdate()
{
    if(ui->graphicsView->scene->m_UiManager)
    {
        ui->ComNumber->display(ui->graphicsView->scene->m_UiManager->m_ComElementGroup.count());
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
        else if(event->type() == QEvent::Wheel)
        {
             QKeyEvent *k = static_cast<QKeyEvent *>(event);
             QWheelEvent *w = static_cast<QWheelEvent *>(event);
             if(k->modifiers() == Qt::CTRL)
             {
                 //>@滚轮一次滚动15度，一次得到delta为120，因此滚动的距离（也即滚轮滚动的单位数）为delta()/8/15
                 int tmpDelta = w->delta();
                 tmpDelta /= 8;
                 tmpDelta /= 15;
                 if(tmpDelta > 0)
                 {
                     for(int i=0;i<tmpDelta;i++)
                     {
                         on_ZoomInBtn_clicked();
                     }
                 }
                 else if(tmpDelta < 0)
                 {
                     tmpDelta = -tmpDelta;
                     for(int i=0;i<tmpDelta;i++)
                     {
                         on_ZoomOutBtn_clicked();
                     }
                 }
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
    GraphicsOperate* tmpMessage = ui->graphicsView->scene->m_ComDiagramGroup.value(COM_MESSAGEBAR);
    if(checked)
    {
        if(!tmpMessage)
        {
            QMessageBox::warning(this, tr("Warning"),
                                       tr("You haven't create the MessageBar.\n"
                                          "Plesase create it firstly!"),
                                       QMessageBox::Ok,
                                       QMessageBox::Ok);
            ui->BtnShowMsg->setChecked(false);
            return;
        }
        tmpMessage->show();
        tmpMessage->setZValue(D_PRGCTRL_ZVALUE);
    }
    else
    {
        if(tmpMessage)
        {
            tmpMessage->hide();
            tmpMessage->setZValue(-D_PRGCTRL_ZVALUE);
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
    ui->graphicsView->scene->Initialize(m_UiFilePath);
}

void uiPage::on_CollisionBtn_clicked(bool checked)
{
    if(checked)
    {
        ui->graphicsView->scene->m_CollisionTimer->start();
    }
    else
    {
        ui->graphicsView->scene->m_CollisionTimer->stop();
        //>@区域检测
        for(int i=COM_AREA+1;i<COM_COMPONENT;i++)
        {
            GraphicsOperate *tmpCom = ui->graphicsView->scene->m_ComDiagramGroup.value((COM_TYPE)i);
            if(tmpCom)
            {
                Effect_color(tmpCom, -1, -1, -1);
            }
        }
    }
}

void uiPage::on_SetComFront_clicked(bool checked)
{

}

void uiPage::on_BtnShowSetbar_clicked(bool checked)
{
    GraphicsOperate* tmpSetBar = ui->graphicsView->scene->m_ComDiagramGroup.value(COM_SETBAR);
    if(checked)
    {
        if(!tmpSetBar)
        {
            QMessageBox::warning(this, tr("Warning"),
                                       tr("You haven't create the SetBar.\n"
                                          "Plesase create it firstly!"),
                                       QMessageBox::Ok,
                                       QMessageBox::Ok);
            ui->BtnShowSetbar->setChecked(false);
            return;
        }
        tmpSetBar->show();
        tmpSetBar->setZValue(D_SETCTRL_ZVALUE);
    }
    else
    {
        if(tmpSetBar)
        {
            tmpSetBar->hide();
            tmpSetBar->setZValue(-D_SETCTRL_ZVALUE);
        }
        ui->graphicsView->scene->hideFrame();
    }
}

void uiPage::on_BtnShowPrgs_clicked(bool checked)
{
    GraphicsOperate* tmpProgressBar = ui->graphicsView->scene->m_ComDiagramGroup.value(COM_PROGRESSBAR);
    if(checked)
    {
        if(!tmpProgressBar)
        {
            QMessageBox::warning(this, tr("Warning"),
                                       tr("You haven't create the ProgressBar.\n"
                                          "Plesase create it firstly!"),
                                       QMessageBox::Ok,
                                       QMessageBox::Ok);
            ui->BtnShowPrgs->setChecked(false);
            return;
        }
        tmpProgressBar->show();
        tmpProgressBar->setZValue(D_SETCTRL_ZVALUE);
    }
    else
    {
        if(tmpProgressBar)
        {
            tmpProgressBar->hide();
            tmpProgressBar->setZValue(-D_SETCTRL_ZVALUE);
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
    if(tmpProjectBar && tmpProjectBar->mSimulatorView)
    {
        tmpProjectBar->mSimulatorView->StartSimulator(m_UiFilePath);
    }
}


void uiPage::on_RotateBtn_clicked(bool checked)
{
    if(!ui->graphicsView->scene->m_UiManager->canSwitch())
    {
        QMessageBox::warning(0, tr("Warn"),
                                   tr("The Current Theme can't support Switch!"),
                                   QMessageBox::Yes);
        return;
    }
    if(checked)
        ui->graphicsView->scene->m_UiDirection = Vertical;
    else
        ui->graphicsView->scene->m_UiDirection = Horizontal;
    ui->graphicsView->scene->InitUI();
}
