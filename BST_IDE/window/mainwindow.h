#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QHostInfo>

#include "global.h"
#include "toolbox.h"
#include "tabpage.h"
#include "ui_mainwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void InitUI();

    void CreateSysTrayIcon();
    void showTrayIconMessage();

public slots:
    void OpenStartPage();
public slots:
    void context();
    void index();
    void technicalSupport();
    void reportBug();
    void checkUpdate();
    void aboutIDE();
    void aboutBST();
public slots:
    void OpenImageEditor();
    void OpenAudioEditor();
    void OpenVideoEditor();
    void OpenFontEditor();
    void OpenFaceDetectTool();
    void OpenRcManagerTool();
    void OpenFtpTool();
    void OpenMapsForm();
    void OpenChatForm();
    void OpenSmsForm();
    void OpenSQLServerForm();
    void OpenMonitorForm();
    void OpenManufactureTool();
    void OpenUpgradeTool();
    void OpenNetworkManagerTool();
    void OpenAsciiCodecTool();
    void addNewTool();

public slots:
    void TrayIconMessageClicked();
    void iconActivated(QSystemTrayIcon::ActivationReason pReason);

signals:
    void setTrayIconVisible(bool);

public:
    //◊¿√Ê”“œ¬Ω«Õº±Í
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    ExtendToolBar       *extendToolBar;
    HelpToolBar         *helpToolBar;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
