#include "simulatorform.h"
#include "ui_simulatorform.h"

SimulatorView::SimulatorView(QWidget *parent) :
    QGraphicsView(parent)
{
    m_Scene = 0;
    m_FitInViewEnable = true;
    setRenderHint(QPainter::Antialiasing);
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setDragMode(QGraphicsView::NoDrag);
#ifdef SUPPORT_3D
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif
    setBackgroundBrush(QPixmap(":/Page/rc/Page/background.png"));
}

void SimulatorView::resizeEvent(QResizeEvent *)
{
    if(m_FitInViewEnable)
        fitInView(sceneRect(), Qt::KeepAspectRatio);
}

bool SimulatorView::SetUiPath(QString pUiPath, bool pForce)
{
    if(pUiPath.isEmpty())
        return false;
    if(m_UiPath.isEmpty() || pForce)
    {
        m_UiPath = pUiPath;
        return true;
    }
    return false;
}

bool SimulatorView::SetUiRotate(qreal pRotate)
{
    m_ScreenRotate = pRotate;
    return false;
}

void SimulatorView::InitUi()
{
    StopUI();
    m_Scene = new UiScene(0);
    setScene(m_Scene);
}

void SimulatorView::StartUi()
{
    if(m_Scene != 0)
        m_Scene->Initialize(m_UiPath);
}

void SimulatorView::PauseUi()
{
    if(m_Scene != 0)
        m_Scene->PauseUI();
}
void SimulatorView::ResumeUi()
{
    if(m_Scene != 0)
        m_Scene->PlayUI();
}
void SimulatorView::StopUI()
{
    if(m_Scene != 0)
    {
        m_Scene->deleteLater();
        m_Scene = 0;
    }
}

void SimulatorView::KeyProcess(KEY_OPERATE pKey)
{
    if(m_Scene != 0)
        m_Scene->KeyProcess(pKey);
}

void SimulatorView::slot_EnergySave(bool pEnable)
{
    if(pEnable)
        PauseUi();
    else
        ResumeUi();
}

void SimulatorView::slot_UDPInform(QString pMessage)
{
    if((m_Scene == 0)||(pMessage.isEmpty()))
            return;
    QStringList paraList = pMessage.split(";", QString::SkipEmptyParts);
    quint8 paraCnt = paraList.count();
    if(paraCnt < 2)
        return;
    m_Scene->ShowMessage(paraList.at(0), paraList.at(1).toInt());
}

SimulatorForm::SimulatorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimulatorForm)
{
    ui->setupUi(this);
    m_SimulatorView = ui->UiSimulatorView;
    ui->BtnStart->setEnabled(true);
    ui->BtnPause->setEnabled(false);
    ui->BtnStop->setEnabled(false);
}

SimulatorForm::~SimulatorForm()
{
    delete ui;
}

void SimulatorForm::StartSimulator(QString pUiPath)
{
    ui->UiPathLineEdit->setText(pUiPath);
    on_BtnStart_clicked();
}

void SimulatorForm::on_UiPathSelect_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Ui File"),
                                         QDir::currentPath(),
                                         "*.ui");
    if (fileName.isEmpty())
        return;
    ui->UiPathLineEdit->setText(fileName);
}

void SimulatorForm::on_BtnStart_clicked()
{
    ui->BtnStart->setEnabled(false);
    ui->BtnPause->setEnabled(true);
    ui->BtnStop->setEnabled(true);
    if(ui->BtnPause->isEnabled())
    {
        QString tmpFileName = ui->UiPathLineEdit->text();
        qreal tmpRotate = ui->RotateSpinBox->value();
        if(isFileExist(tmpFileName) == false)
        {
            QMessageBox::warning(0, tr("Error"),
                                 tr("%1 not exist, \nPlease check it!")
                                 .arg(tmpFileName));
            return;
        }
        if(ui->UiSimulatorView->SetUiPath(tmpFileName, true) == false)
        {
            QMessageBox::warning(0, tr("Error"),
                                 tr("Can't set path to %1, \nPlease check it!")
                                 .arg(tmpFileName));
            return;
        }
        if(ui->UiSimulatorView->SetUiRotate(tmpRotate))
        {
            QMessageBox::warning(0, tr("Error"),
                                 tr("Can't set Rotate to %1, \nPlease check it!")
                                 .arg(tmpRotate));
            return;
        }
        ui->UiSimulatorView->InitUi();
        ui->UiSimulatorView->StartUi();
    }
    else
    {
        ui->UiSimulatorView->ResumeUi();
    }
}

void SimulatorForm::on_BtnPause_clicked()
{
    ui->BtnStart->setEnabled(true);
    ui->BtnPause->setEnabled(false);
    ui->BtnStop->setEnabled(true);
    ui->UiSimulatorView->PauseUi();
}

void SimulatorForm::on_BtnStop_clicked()
{
    ui->BtnStart->setEnabled(true);
    ui->BtnPause->setEnabled(false);
    ui->BtnStop->setEnabled(false);
    ui->UiSimulatorView->StopUI();
}

void SimulatorForm::on_FitInViewEnable_toggled(bool checked)
{
    ui->UiSimulatorView->m_FitInViewEnable = checked;
}
