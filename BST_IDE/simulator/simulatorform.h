#ifndef SIMULATORFORM_H
#define SIMULATORFORM_H

#include <QTimer>
#include "logicthememanager.h"

namespace Ui {
class SimulatorForm;
}

class SimulatorView : public QGraphicsView
{
    Q_OBJECT
public:
    SimulatorView(QWidget *parent=0);
    void resizeEvent(QResizeEvent *);

    bool SetUiPath(QString pUiPath, bool pForce=false);
    QString GetUiPath() { return m_UiPath; }
    bool SetUiRotate(qreal pRotate);

    void InitUi();
    void StartUi();
    void PauseUi();
    void ResumeUi();
    void StopUI();
    void KeyProcess(KEY_OPERATE pKey);

public slots:
    void slot_EnergySave(bool pEnable);
    void slot_UDPInform(QString pMessage);

public:
    UiScene*            m_Scene;
    QString             m_UiPath;
    bool                m_FitInViewEnable;
    qreal               m_ScreenRotate;
};

class SimulatorForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit SimulatorForm(QWidget *parent = 0);
    ~SimulatorForm();

    void StartSimulator(QString pUiPath);

private slots:
    void on_UiPathSelect_clicked();
    void on_BtnStart_clicked();
    void on_BtnPause_clicked();
    void on_BtnStop_clicked();
    void on_FitInViewEnable_toggled(bool checked);

public:
    SimulatorView  *m_SimulatorView;
    Ui::SimulatorForm *ui;
};

#endif // SIMULATORFORM_H
